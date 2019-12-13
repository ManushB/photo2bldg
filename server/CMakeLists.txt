# the `pkg_check_modules` function is created with this call
find_package(PkgConfig REQUIRED)

# these calls create special `PkgConfig::<MODULE>` variables
pkg_check_modules(hiredis REQUIRED IMPORTED_TARGET hiredis)

# Find libraries
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(EGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(CUDA 9.0 REQUIRED)
find_package(Caffe REQUIRED)
find_package(dlib 19.17 REQUIRED)
find_package(CGAL REQUIRED COMPONENTS Core)
find_package(glm 0.9.9 REQUIRED)
find_package(cJSON REQUIRED)
include_directories(${Caffe_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-DGLM_FORCE_CTOR_INIT)

# Get sources
file(GLOB SOURCES src/*.cpp)

add_executable(${PROJECT_NAME}_server ${SOURCES})
target_link_libraries(${PROJECT_NAME}_server Qt5::Core)
target_link_libraries(${PROJECT_NAME}_server Qt5::Widgets)
target_link_libraries(${PROJECT_NAME}_server Qt5::OpenGL)
target_link_libraries(${PROJECT_NAME}_server Qt5::Xml)
target_link_libraries(${PROJECT_NAME}_server ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_server ${GLEW_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_server ${EGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_server ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME}_server ${Caffe_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_server ${CUDA_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_server ${DLIB_LIBRARIES} dlib)
target_link_libraries(${PROJECT_NAME}_server CGAL::CGAL CGAL::CGAL_Core)
target_link_libraries(${PROJECT_NAME}_server PkgConfig::hiredis)
target_link_libraries(${PROJECT_NAME}_server ${CJSON_LIBRARIES})
