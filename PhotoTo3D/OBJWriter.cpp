#include "OBJWriter.h"
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

bool Material::equals(const Material& other) {
	if (type != other.type) return false;
	if (type == 1) {
		if (color != other.color) return false;
	}
	else if (type == 2) {
		if (texture != other.texture) return false;
	}

	return true;
}

std::string Material::to_string() {
	std::stringstream ss;

	if (type == 1) {
		ss << "Ka " << color.r << " " << color.g << " " << color.b << std::endl;
		ss << "Kd " << color.r << " " << color.g << " " << color.b << std::endl;
		ss << "Ks " << color.r << " " << color.g << " " << color.b << std::endl;
		ss << "Ke " << color.r << " " << color.g << " " << color.b << std::endl;
		ss << "d 1" << std::endl;
		ss << "Ns 20" << std::endl;
		ss << "illum 2" << std::endl;
	}
	else if (type == 2) {
		ss << "Ka 1 1 1" << std::endl;
		ss << "Kd 1 1 1" << std::endl;
		ss << "Ks 1 1 1" << std::endl;
		ss << "Ke 1 1 1" << std::endl;
		ss << "d 1" << std::endl;
		ss << "Ns 20" << std::endl;
		ss << "illum 2" << std::endl;
		//ss << "map_Ka " << texture << std::endl;
		ss << "map_Kd " << texture << std::endl;
		//ss << "map_Ks " << texture << std::endl;
		//ss << "map_Ns " << texture << std::endl;
		//ss << "map_d " << texture << std::endl;
	}

	return ss.str();
}

/*
void OBJWriter::write(const std::vector<sc::SceneObject>& objects, const std::string& filename) {
	std::ofstream file(filename);
	std::ofstream mat_file(filename + ".mtl");

	file << "mtllib " << filename + ".mtl" << std::endl;
	file << std::endl;

	file << "# List of geometric vertices" << std::endl;
	for (int i = 0; i < objects.size(); ++i) {
		for (int j = 0; j < objects[i].faces.size(); ++j) {
			for (int k = 0; k < objects[i].faces[j]->vertices.size(); ++k) {
				file << "v " << objects[i].faces[j]->vertices[k].position.x << " " << objects[i].faces[j]->vertices[k].position.y << " " << objects[i].faces[j]->vertices[k].position.z << std::endl;
			}
		}
	}
	file << std::endl;

	file << "# List of texture coordinates" << std::endl;
	for (int i = 0; i < objects.size(); ++i) {
		for (int j = 0; j < objects[i].faces.size(); ++j) {
			for (int k = 0; k < objects[i].faces[j]->vertices.size(); ++k) {
				if (objects[i].faces[j]->vertices[k].texCoord.x == 0 && objects[i].faces[j]->vertices[k].texCoord.y == 0 && objects[i].faces[j]->vertices[k + 1].texCoord.x == 0 && objects[i].faces[j]->vertices[k + 1].texCoord.y == 0 && objects[i].faces[j]->vertices[k + 2].texCoord.x == 0 && objects[i].faces[j]->vertices[k + 2].texCoord.y == 0) continue;

				file << "vt " << objects[i].faces[j]->vertices[k].texCoord.x << " " << objects[i].faces[j]->vertices[k].texCoord.y << std::endl;
			}
		}
	}
	file << std::endl;

	file << "# List of vertex normals" << std::endl;
	for (int i = 0; i < objects.size(); ++i) {
		for (int j = 0; j < objects[i].faces.size(); ++j) {
			for (int k = 0; k < objects[i].faces[j]->vertices.size(); ++k) {
				file << "vn " << objects[i].faces[j]->vertices[k].normal.x << " " << objects[i].faces[j]->vertices[k].normal.y << " " << objects[i].faces[j]->vertices[k].normal.z << std::endl;
			}
		}
	}
	file << std::endl;


	int positionId = 1;
	int texCoordId = 1;
	int materialId = 1;
	Material material;
	for (int i = 0; i < objects.size(); ++i) {
		for (int j = 0; j < objects[i].faces.size(); ++j) {
			bool textureEnabled = true;
			if (objects[i].faces[j]->texture.empty()) {
				textureEnabled = false;
			}
			if (objects[i].faces[j]->vertices[0].texCoord.x == 0 && objects[i].faces[j]->vertices[0].texCoord.y == 0 && objects[i].faces[j]->vertices[1].texCoord.x == 0 && objects[i].faces[j]->vertices[1].texCoord.y == 0 && objects[i].faces[j]->vertices[2].texCoord.x == 0 && objects[i].faces[j]->vertices[2].texCoord.y == 0) {
				textureEnabled = false;
			}

			Material new_material;
			if (textureEnabled) {
				new_material = Material(objects[i].faces[j]->texture);
			}
			else {
				new_material = Material(objects[i].faces[j]->vertices[0].color);
			}

			if (!new_material.equals(material)) {
				material = new_material;

				mat_file << "newmtl Material" << materialId << std::endl;
				mat_file << material.to_string() << std::endl;

				file << std::endl;
				file << "usemtl Material" << materialId << std::endl;
				materialId++;
			}

			for (int k = 0; k < objects[i].faces[j]->vertices.size() / 3; ++k) {
				file << "f ";
				for (int l = 0; l < 3; ++l) {
					if (l > 0) {
						file << " ";
					}
					file << positionId;

					file << "/";
					if (objects[i].faces[j]->vertices[k].texCoord.x != 0 || objects[i].faces[j]->vertices[k].texCoord.y != 0 || objects[i].faces[j]->vertices[k + 1].texCoord.x != 0 || objects[i].faces[j]->vertices[k + 1].texCoord.y != 0 || objects[i].faces[j]->vertices[k + 2].texCoord.x != 0 || objects[i].faces[j]->vertices[k + 2].texCoord.y != 0) {
						file << texCoordId++;
					}

					file << "/" << positionId++;
				}
				file << std::endl;
			}
		}
	}

	file.close();
}*/

void OBJWriter::write(const std::vector<boost::shared_ptr<glutils::Face> >& faces, const std::string& filename) {
	std::ofstream file(filename);
	std::ofstream mat_file(filename + ".mtl");

	boost::filesystem::path p(filename + ".mtl");

	file << "mtllib " << p.filename().string() << std::endl;
	file << std::endl;

	file << "# List of geometric vertices" << std::endl;
	for (int j = 0; j < faces.size(); ++j) {
		for (int k = 0; k < faces[j]->vertices.size(); ++k) {
			file << "v " << faces[j]->vertices[k].position.x << " " << faces[j]->vertices[k].position.y << " " << faces[j]->vertices[k].position.z << std::endl;
		}
	}
	file << std::endl;

	file << "# List of texture coordinates" << std::endl;
	for (int j = 0; j < faces.size(); ++j) {
		if (faces[j]->vertices.size() < 3) continue;
		if (faces[j]->vertices[0].texCoord.x == 0 && faces[j]->vertices[0].texCoord.y == 0 && faces[j]->vertices[1].texCoord.x == 0 && faces[j]->vertices[1].texCoord.y == 0 && faces[j]->vertices[2].texCoord.x == 0 && faces[j]->vertices[2].texCoord.y == 0) continue;

		for (int k = 0; k < faces[j]->vertices.size(); ++k) {
			file << "vt " << faces[j]->vertices[k].texCoord.x << " " << faces[j]->vertices[k].texCoord.y << std::endl;
		}
	}
	file << std::endl;

	file << "# List of vertex normals" << std::endl;
	for (int j = 0; j < faces.size(); ++j) {
		for (int k = 0; k < faces[j]->vertices.size(); ++k) {
			file << "vn " << faces[j]->vertices[k].normal.x << " " << faces[j]->vertices[k].normal.y << " " << faces[j]->vertices[k].normal.z << std::endl;
		}
	}
	file << std::endl;


	int positionId = 1;
	int texCoordId = 1;
	int materialId = 1;
	Material material;
	for (int j = 0; j < faces.size(); ++j) {
		if (faces[j]->vertices.size() < 3) continue;

		bool textureEnabled = true;
		if (faces[j]->texture.empty()) {
			textureEnabled = false;
		}
		if (faces[j]->vertices[0].texCoord.x == 0 && faces[j]->vertices[0].texCoord.y == 0 && faces[j]->vertices[1].texCoord.x == 0 && faces[j]->vertices[1].texCoord.y == 0 && faces[j]->vertices[2].texCoord.x == 0 && faces[j]->vertices[2].texCoord.y == 0) {
			textureEnabled = false;
		}

		Material new_material;
		if (textureEnabled) {
			new_material = Material(faces[j]->texture);
		}
		else {
			new_material = Material(faces[j]->vertices[0].color);
		}

		if (!new_material.equals(material)) {
			material = new_material;

			mat_file << "newmtl Material" << materialId << std::endl;
			mat_file << material.to_string() << std::endl;

			file << std::endl;
			file << "usemtl Material" << materialId << std::endl;
			materialId++;
		}

		for (int k = 0; k < faces[j]->vertices.size() / 3; ++k) {
			file << "f ";
			for (int l = 0; l < 3; ++l) {
				if (l > 0) {
					file << " ";
				}
				file << positionId;

				file << "/";
				if (faces[j]->vertices[k].texCoord.x != 0 || faces[j]->vertices[k].texCoord.y != 0 || faces[j]->vertices[k + 1].texCoord.x != 0 || faces[j]->vertices[k + 1].texCoord.y != 0 || faces[j]->vertices[k + 2].texCoord.x != 0 || faces[j]->vertices[k + 2].texCoord.y != 0) {
					file << texCoordId++;
				}

				file << "/" << positionId++;
			}
			file << std::endl;
		}
	}

	file.close();
}