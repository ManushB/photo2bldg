#include "OBJReader.h"

// 3d render
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "util.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void build_rotmatrix(float m[4][4], const float q[4]) {
    m[0][0] = 1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]);
    m[0][1] = 2.0 * (q[0] * q[1] - q[2] * q[3]);
    m[0][2] = 2.0 * (q[2] * q[0] + q[1] * q[3]);
    m[0][3] = 0.0;

    m[1][0] = 2.0 * (q[0] * q[1] + q[2] * q[3]);
    m[1][1] = 1.0 - 2.0 * (q[2] * q[2] + q[0] * q[0]);
    m[1][2] = 2.0 * (q[1] * q[2] - q[0] * q[3]);
    m[1][3] = 0.0;

    m[2][0] = 2.0 * (q[2] * q[0] - q[1] * q[3]);
    m[2][1] = 2.0 * (q[1] * q[2] + q[0] * q[3]);
    m[2][2] = 1.0 - 2.0 * (q[1] * q[1] + q[0] * q[0]);
    m[2][3] = 0.0;

    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = 1.0;
}

static std::string GetBaseDir(const std::string &filepath) {
    if (filepath.find_last_of("/\\") != std::string::npos)
        return filepath.substr(0, filepath.find_last_of("/\\"));
    return "";
}

static bool FileExists(const std::string &abs_filename) {
    bool ret;
    FILE *fp = fopen(abs_filename.c_str(), "rb");
    if (fp) {
        ret = true;
        fclose(fp);
    } else {
        ret = false;
    }

    return ret;
}

static void CheckErrors(std::string desc) {
    GLenum e = glGetError();
    if (e != GL_NO_ERROR) {
        fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", desc.c_str(), e, e);
        exit(20);
    }
}

static void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]) {
    float v10[3];
    v10[0] = v1[0] - v0[0];
    v10[1] = v1[1] - v0[1];
    v10[2] = v1[2] - v0[2];

    float v20[3];
    v20[0] = v2[0] - v0[0];
    v20[1] = v2[1] - v0[1];
    v20[2] = v2[2] - v0[2];

    N[0] = v20[1] * v10[2] - v20[2] * v10[1];
    N[1] = v20[2] * v10[0] - v20[0] * v10[2];
    N[2] = v20[0] * v10[1] - v20[1] * v10[0];

    float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
    if (len2 > 0.0f) {
        float len = sqrtf(len2);

        N[0] /= len;
        N[1] /= len;
        N[2] /= len;
    }
}

namespace  // Local utility functions
{
    struct vec3 {
        float v[3];
        vec3() {
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
        }
    };

    void normalizeVector(vec3 &v) {
        float len2 = v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2];
        if (len2 > 0.0f) {
            float len = sqrtf(len2);

            v.v[0] /= len;
            v.v[1] /= len;
            v.v[2] /= len;
        }
    }

// Check if `mesh_t` contains smoothing group id.
    bool hasSmoothingGroup(const tinyobj::shape_t& shape)
    {
        for (size_t i = 0; i < shape.mesh.smoothing_group_ids.size(); i++) {
            if (shape.mesh.smoothing_group_ids[i] > 0) {
                return true;
            }
        }
        return false;
    }

    void computeSmoothingNormals(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape,
                                 std::map<int, vec3>& smoothVertexNormals)
    {
        smoothVertexNormals.clear();
        std::map<int, vec3>::iterator iter;

        for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++) {
            // Get the three indexes of the face (all faces are triangular)
            tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
            tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
            tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

            // Get the three vertex indexes and coordinates
            int vi[3];      // indexes
            float v[3][3];  // coordinates

            for (int k = 0; k < 3; k++) {
                vi[0] = idx0.vertex_index;
                vi[1] = idx1.vertex_index;
                vi[2] = idx2.vertex_index;
                assert(vi[0] >= 0);
                assert(vi[1] >= 0);
                assert(vi[2] >= 0);

                v[0][k] = attrib.vertices[3 * vi[0] + k];
                v[1][k] = attrib.vertices[3 * vi[1] + k];
                v[2][k] = attrib.vertices[3 * vi[2] + k];
            }

            // Compute the normal of the face
            float normal[3];
            CalcNormal(normal, v[0], v[1], v[2]);

            // Add the normal to the three vertexes
            for (size_t i = 0; i < 3; ++i) {
                iter = smoothVertexNormals.find(vi[i]);
                if (iter != smoothVertexNormals.end()) {
                    // add
                    iter->second.v[0] += normal[0];
                    iter->second.v[1] += normal[1];
                    iter->second.v[2] += normal[2];
                } else {
                    smoothVertexNormals[vi[i]].v[0] = normal[0];
                    smoothVertexNormals[vi[i]].v[1] = normal[1];
                    smoothVertexNormals[vi[i]].v[2] = normal[2];
                }
            }

        }  // f

        // Normalize the normals, that is, make them unit vectors
        for (iter = smoothVertexNormals.begin(); iter != smoothVertexNormals.end();
             iter++) {
            normalizeVector(iter->second);
        }

    }  // computeSmoothingNormals
}  // namespace


bool OBJReader::LoadObjAndConvert(const char* filename) {

    std::vector<tinyobj::material_t> materials;
    std::vector<tinyobj::shape_t> shapes;
    tinyobj::attrib_t attrib;

    std::string base_dir = GetBaseDir(filename);
    if (base_dir.empty()) {
        base_dir = ".";
    }

    base_dir += "/";

    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename,
                                base_dir.c_str());
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load " << filename << std::endl;
        return false;
    }


    printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
    printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
    printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
    printf("# of materials = %d\n", (int)materials.size());
    printf("# of shapes    = %d\n", (int)shapes.size());

    // Append `default` material
    materials.push_back(tinyobj::material_t());

    for (size_t i = 0; i < materials.size(); i++) {
        printf("material[%d].diffuse_texname = %s\n", int(i),
               materials[i].diffuse_texname.c_str());
    }
    {
        for (size_t m = 0; m < materials.size(); m++) {
            tinyobj::material_t* mp = &materials[m];

            if (mp->diffuse_texname.length() > 0) {
                // Only load the texture if it is not already loaded
                if (textures.find(mp->diffuse_texname) == textures.end()) {
                    GLuint texture_id;
                    int w, h;
                    int comp;

                    std::string texture_filename = mp->diffuse_texname;
                    if (!FileExists(texture_filename)) {
                        // Append base dir.
                        texture_filename = base_dir + mp->diffuse_texname;
                        if (!FileExists(texture_filename)) {
                            std::cerr << "Unable to find file: " << mp->diffuse_texname
                                      << std::endl;
                            exit(1);
                        }
                    }

                    unsigned char* image =
                            stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
                    if (!image) {
                        std::cerr << "Unable to load texture: " << texture_filename
                                  << std::endl;
                        exit(1);
                    }
                    std::cout << "Loaded texture: " << texture_filename << ", w = " << w
                              << ", h = " << h << ", comp = " << comp << std::endl;

                    glGenTextures(1, &texture_id);
                    glBindTexture(GL_TEXTURE_2D, texture_id);
////////////////////////////////////////////////////////////////////////////////
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
///////////////////////////////////////////////////////////////////////////////////
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    // qDebug() << "Image has " << comp << " number of components";
                    if (comp == 3) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
                                     GL_UNSIGNED_BYTE, image);
                    } else if (comp == 4) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                                     GL_UNSIGNED_BYTE, image);
                    } else {
                        assert(0);  // TODO
                    }
////////////////////////////////////////////////////////////////////////////////
                    glGenerateMipmap(GL_TEXTURE_2D);
////////////////////////////////////////////////////////////////////////////////
                    glBindTexture(GL_TEXTURE_2D, 0);//release current binded texture, ready for the next new generated.
                    stbi_image_free(image);
                    textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
                    //next step is to extract textures from this vector and render them with multiple vao1...vaon for different classes.
                }
            }
        }
    }

    bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
    bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();

    {
        for (size_t s = 0; s < shapes.size(); s++) {
            DrawObject o;
            std::vector<float> buffer;  // pos(3float), normal(3float), color(3float)

            std::cout<<"Num of faces vertex: "<<shapes[s].mesh.num_face_vertices.size()<<std::endl;

            // Check for smoothing group and compute smoothing normals
            std::map<int, vec3> smoothVertexNormals;
            if (hasSmoothingGroup(shapes[s]) > 0) {
                std::cout << "Compute smoothingNormal for shape [" << s << "]" << std::endl;
                computeSmoothingNormals(attrib, shapes[s], smoothVertexNormals);
            }

            for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {
                tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
                tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
                tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

                int current_material_id = shapes[s].mesh.material_ids[f];

                if ((current_material_id < 0) ||
                    (current_material_id >= static_cast<int>(materials.size()))) {
                    // Invaid material ID. Use default material.
                    current_material_id =
                            materials.size() -
                            1;  // Default material is added to the last item in `materials`.
                }
                float diffuse[3];
                for (size_t i = 0; i < 3; i++) {
                    diffuse[i] = materials[current_material_id].diffuse[i];
                }
                float tc[3][2];
                if (attrib.texcoords.size() > 0) {
                    if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) ||
                        (idx2.texcoord_index < 0)) {
                        // face does not contain valid uv index.
                        tc[0][0] = 0.0f;
                        tc[0][1] = 0.0f;
                        tc[1][0] = 0.0f;
                        tc[1][1] = 0.0f;
                        tc[2][0] = 0.0f;
                        tc[2][1] = 0.0f;
                    } else {
                        assert(attrib.texcoords.size() >
                               size_t(2 * idx0.texcoord_index + 1));
                        assert(attrib.texcoords.size() >
                               size_t(2 * idx1.texcoord_index + 1));
                        assert(attrib.texcoords.size() >
                               size_t(2 * idx2.texcoord_index + 1));

                        // Flip Y coord.
                        tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
                        tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
                        tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
                        tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
                        tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
                        tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
                    }
                } else {
                    tc[0][0] = 0.0f;
                    tc[0][1] = 0.0f;
                    tc[1][0] = 0.0f;
                    tc[1][1] = 0.0f;
                    tc[2][0] = 0.0f;
                    tc[2][1] = 0.0f;
                }

                float v[3][3];
                for (int k = 0; k < 3; k++) {
                    int f0 = idx0.vertex_index;
                    int f1 = idx1.vertex_index;
                    int f2 = idx2.vertex_index;
                    assert(f0 >= 0);
                    assert(f1 >= 0);
                    assert(f2 >= 0);

                    v[0][k] = attrib.vertices[3 * f0 + k];
                    v[1][k] = attrib.vertices[3 * f1 + k];
                    v[2][k] = attrib.vertices[3 * f2 + k];
                    bmin[k] = std::min(v[0][k], bmin[k]);
                    bmin[k] = std::min(v[1][k], bmin[k]);
                    bmin[k] = std::min(v[2][k], bmin[k]);
                    bmax[k] = std::max(v[0][k], bmax[k]);
                    bmax[k] = std::max(v[1][k], bmax[k]);
                    bmax[k] = std::max(v[2][k], bmax[k]);
                }

                float n[3][3];
                {
                    bool invalid_normal_index = false;
                    if (attrib.normals.size() > 0) {
                        int nf0 = idx0.normal_index;
                        int nf1 = idx1.normal_index;
                        int nf2 = idx2.normal_index;

                        if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0)) {
                            // normal index is missing from this face.
                            invalid_normal_index = true;
                        } else {
                            for (int k = 0; k < 3; k++) {
                                assert(size_t(3 * nf0 + k) < attrib.normals.size());
                                assert(size_t(3 * nf1 + k) < attrib.normals.size());
                                assert(size_t(3 * nf2 + k) < attrib.normals.size());
                                n[0][k] = attrib.normals[3 * nf0 + k];
                                n[1][k] = attrib.normals[3 * nf1 + k];
                                n[2][k] = attrib.normals[3 * nf2 + k];
                            }
                        }
                    } else {
                        invalid_normal_index = true;
                    }

                    if (invalid_normal_index && !smoothVertexNormals.empty()) {
                        // Use smoothing normals
                        int f0 = idx0.vertex_index;
                        int f1 = idx1.vertex_index;
                        int f2 = idx2.vertex_index;

                        if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
                            n[0][0] = smoothVertexNormals[f0].v[0];
                            n[0][1] = smoothVertexNormals[f0].v[1];
                            n[0][2] = smoothVertexNormals[f0].v[2];

                            n[1][0] = smoothVertexNormals[f1].v[0];
                            n[1][1] = smoothVertexNormals[f1].v[1];
                            n[1][2] = smoothVertexNormals[f1].v[2];

                            n[2][0] = smoothVertexNormals[f2].v[0];
                            n[2][1] = smoothVertexNormals[f2].v[1];
                            n[2][2] = smoothVertexNormals[f2].v[2];
                            invalid_normal_index = false;
                        }
                    }

                    if (invalid_normal_index) {
                        // compute geometric normal
                        CalcNormal(n[0], v[0], v[1], v[2]);
                        n[1][0] = n[0][0];
                        n[1][1] = n[0][1];
                        n[1][2] = n[0][2];
                        n[2][0] = n[0][0];
                        n[2][1] = n[0][1];
                        n[2][2] = n[0][2];
                    }
                }

                for (int k = 0; k < 3; k++) {
                    buffer.push_back(v[k][0]);
                    buffer.push_back(v[k][1]);
                    buffer.push_back(v[k][2]);
                    buffer.push_back(n[k][0]);
                    buffer.push_back(n[k][1]);
                    buffer.push_back(n[k][2]);
                    // Combine normal and diffuse to get color.
                    float normal_factor = 0.2;
                    float diffuse_factor = 1 - normal_factor;
                    float c[3] = {n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
                                  n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
                                  n[k][2] * normal_factor + diffuse[2] * diffuse_factor};
                    float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
                    if (len2 > 0.0f) {
                        float len = sqrtf(len2);

                        c[0] /= len;
                        c[1] /= len;
                        c[2] /= len;
                    }
                    buffer.push_back(c[0] * 0.5 + 0.5);
                    buffer.push_back(c[1] * 0.5 + 0.5);
                    buffer.push_back(c[2] * 0.5 + 0.5);

                    buffer.push_back(tc[k][0]);
                    buffer.push_back(tc[k][1]);
                }
            }

            o.vb_id = 0;
            o.numTriangles = 0;
            // OpenGL viewer does not support texturing with per-face material.
            if (shapes[s].mesh.material_ids.size() > 0 &&
                shapes[s].mesh.material_ids.size() > s) {
                o.material_id = shapes[s].mesh.material_ids[0];  // use the material ID
                // of the first face.
            } else {
                o.material_id = materials.size() - 1;  // = ID for default material.
            }
            printf("shape[%d] material_id %d\n", int(s), int(o.material_id));

            if (buffer.size() > 0) {

                ///////////////////////////////////////////////////////////////////////////////////////////////////
                for (int i = 0; i < shapes[0].mesh.material_ids.size(); ++i)
                {
                    if(shapes[0].mesh.material_ids[i]==33)
                    {
                        indices_roof.push_back(i*3);
                        indices_roof.push_back(i*3+1);
                        indices_roof.push_back(i*3+2);
                    }
                }


                for (int i = 0; i < shapes[0].mesh.material_ids.size(); ++i)
                {
                    if( (shapes[0].mesh.material_ids[i]==42) || (shapes[0].mesh.material_ids[i]==36) || (shapes[0].mesh.material_ids[i]==38) ||(shapes[0].mesh.material_ids[i]==40))
                    {
                        indices_window.push_back(i*3);
                        indices_window.push_back(i*3+1);
                        indices_window.push_back(i*3+2);
                    }
                }


                for (int i = 0; i < shapes[0].mesh.material_ids.size(); ++i)
                {
                    if( (shapes[0].mesh.material_ids[i]!= 33) && (shapes[0].mesh.material_ids[i]!=42) && (shapes[0].mesh.material_ids[i]!=36) && (shapes[0].mesh.material_ids[i]!=38) &&(shapes[0].mesh.material_ids[i]!=40) )
                    {
                        indices_wall.push_back(i*3);
                        indices_wall.push_back(i*3+1);
                        indices_wall.push_back(i*3+2);
                    }
                }

                glGenVertexArrays(1, &vao);
                glBindVertexArray(vao);

                glGenBuffers(1, &vbuf);
                glBindBuffer(GL_ARRAY_BUFFER, vbuf);
                std::cout<<buffer.size()<<std::endl;
                glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float),
                             &buffer.at(0), GL_STATIC_DRAW);
                o.numTriangles = buffer.size() / (3 + 3 + 3 + 2) /
                                 3;  // 3:vtx, 3:normal, 3:col, 2:texcoord

                printf("shape[%d] # of triangles = %d\n", static_cast<int>(s),
                       o.numTriangles);

                glGenBuffers(1, &IBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_roof.size()*4, indices_roof.data(),GL_STATIC_DRAW);

                //////////////////////////////////////////////////////////////////////////////////////////////////
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), NULL);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*) (9*sizeof(float)) );
                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                //////////////////////////////////////////////////////////////////////////////////////////////////

                glGenVertexArrays(1, &vao1);
                glBindVertexArray(vao1);

                glGenBuffers(1, &vbuf1);
                glBindBuffer(GL_ARRAY_BUFFER, vbuf1);
                std::cout<<buffer.size()<<std::endl;
                glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float),
                             &buffer.at(0), GL_STATIC_DRAW);
                o.numTriangles = buffer.size() / (3 + 3 + 3 + 2) /
                                 3;  // 3:vtx, 3:normal, 3:col, 2:texcoord

                printf("shape[%d] # of triangles = %d\n", static_cast<int>(s),
                       o.numTriangles);


                glGenBuffers(1, &IBO1);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO1);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_window.size()*4, indices_window.data(),GL_STATIC_DRAW);

                //////////////////////////////////////////////////////////////////////////////////////////////////
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), NULL);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*) (9*sizeof(float)) );
                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                //////////////////////////////////////////////////////////////////////////////////////////////////

                glGenVertexArrays(1, &vao2);
                glBindVertexArray(vao2);

                glGenBuffers(1, &vbuf2);
                glBindBuffer(GL_ARRAY_BUFFER, vbuf2);
                std::cout<<buffer.size()<<std::endl;
                glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float),
                             &buffer.at(0), GL_STATIC_DRAW);
                o.numTriangles = buffer.size() / (3 + 3 + 3 + 2) /
                                 3;  // 3:vtx, 3:normal, 3:col, 2:texcoord

                printf("shape[%d] # of triangles = %d\n", static_cast<int>(s),
                       o.numTriangles);


                glGenBuffers(1, &IBO2);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO2);
                //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_wall), indices_wall.data(),GL_STATIC_DRAW);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_wall.size()*4, indices_wall.data(),GL_STATIC_DRAW);


                //////////////////////////////////////////////////////////////////////////////////////////////////
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), NULL);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (GLvoid*) (9*sizeof(float)) );
                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                //////////////////////////////////////////////////////////////////////////////////////////////////

            }
//            qDebug() << "buffer has " << buffer.size() << " numbers.";
            if (buffer.size() % 11 != 0) {
//                qDebug() << "buffer size is not divisible by 11. 11 numbers needed for each vertex: 3 pos + 3 normal + 3 color + 2 tex uv";
            }

            drawObjects.push_back(o);
        }
    }

    printf("bmin = %f, %f, %f\n", bmin[0], bmin[1], bmin[2]);
    printf("bmax = %f, %f, %f\n", bmax[0], bmax[1], bmax[2]);

    maxExtent = 0.5f * (bmax[0] - bmin[0]);
    if (maxExtent < 0.5f * (bmax[1] - bmin[1])) {
        maxExtent = 0.5f * (bmax[1] - bmin[1]);
    }
    if (maxExtent < 0.5f * (bmax[2] - bmin[2])) {
        maxExtent = 0.5f * (bmax[2] - bmin[2]);
    }
    std::cout<<maxExtent<<std::endl;
    assert(glGetError() == GL_NO_ERROR);

    return true;
}

void OBJReader::initShader(){
    // Compile and link shader program
    std::vector<GLuint> shaders;
    shaders.push_back(compileShader(GL_VERTEX_SHADER, "sh_v.glsl"));
    shaders.push_back(compileShader(GL_FRAGMENT_SHADER, "sh_f.glsl"));
    shader = linkProgram(shaders);
    // Release shader sources
    for (auto s = shaders.begin(); s != shaders.end(); ++s)
        glDeleteShader(*s);
    shaders.clear();
    // Locate uniforms
    uniXform = glGetUniformLocation(shader, "xform");
    /////////////////////////////////////////////////////////////////////

    //reshapeFunc(window, width, height);
//    qDebug() << "GL version = " << reinterpret_cast<const char *>(glGetString(GL_VERSION));
    assert(glGetError() == GL_NO_ERROR);
}

void OBJReader::render(Camera camera, int width, int height){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    assert(glGetError() == GL_NO_ERROR);

    glUseProgram(shader);

    // Create view and projection matrices
    glm::mat4 proj = glm::perspective(45.0f, (float)width/ (float)height, 0.01f, 1000.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -camera.z));
    glm::mat4 rt = glm::make_mat4(camera.rt);
    view = view * rt;
    // Scale and center mesh using bounding box
    //std::pair<glm::vec3, glm::vec3> meshBB = mesh->boundingBox();
    glm::mat4 fixBB = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / (4 * maxExtent) ) );
    fixBB = glm::translate(fixBB, glm::vec3(-0.5 * (bmax[0] + bmin[0]),
                                            -0.5 * (bmax[1] + bmin[1]), -0.5 * (bmax[2] + bmin[2]) ) );
    // Concatenate all transformations and upload to shader
    glm::mat4 xform = proj * view * fixBB;
    glUniformMatrix4fv(uniXform, 1, GL_FALSE, value_ptr(xform));

    assert(glGetError() == GL_NO_ERROR);
    ////////////////////////////////////////////////////////////////////////////////////////////
    //draw the texture
    glActiveTexture(GL_TEXTURE0 + 0);
    tex_unit_location = glGetUniformLocation(shader, "tex_unit");

    glBindTexture(GL_TEXTURE_2D, 2);
    glUniform1i(tex_unit_location, 0);

    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES, indices_roof.size(), GL_UNSIGNED_INT,0);

    glBindVertexArray(0);

///////////////////////////////////////////////////////////////////////////////
    glActiveTexture(GL_TEXTURE0 + 0);
    tex_unit_location = glGetUniformLocation(shader, "tex_unit");

    glBindTexture(GL_TEXTURE_2D, 1);
    glUniform1i(tex_unit_location, 0);

    glBindVertexArray(vao2);
    glDrawElements(GL_TRIANGLES, indices_wall.size(), GL_UNSIGNED_INT,0);

    glBindVertexArray(0);

    assert(glGetError() == GL_NO_ERROR);

//////////////////////////////////////////////////////////////////////////////////////////////
    glActiveTexture(GL_TEXTURE0 + 0);
    tex_unit_location = glGetUniformLocation(shader, "tex_unit");

    glBindTexture(GL_TEXTURE_2D, 3);
    glUniform1i(tex_unit_location, 0);

    glBindVertexArray(vao1);
    glDrawElements(GL_TRIANGLES, indices_window.size(), GL_UNSIGNED_INT,0);

    glBindVertexArray(0);

    glUseProgram(0);
}
