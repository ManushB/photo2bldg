cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_CXX_STANDARD 17)

project(photo2bldg)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Auto-run MOC and UIC when needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# Set OpenGL legacy preference
set(OpenGL_GL_PREFERENCE GLVND)

# Find libraries
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenCV REQUIRED)
find_package(CUDA 10.0 REQUIRED)
find_package(Caffe REQUIRED)
find_package(dlib REQUIRED)
find_package(CGAL REQUIRED COMPONENTS Core)
include_directories(${Caffe_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

# Get sources
file(GLOB SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Gui)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::OpenGL)
target_link_libraries(${PROJECT_NAME} Qt5::Xml)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} ${Caffe_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${CUDA_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${DLIB_LIBRARIES} dlib)
target_link_libraries(${PROJECT_NAME} CGAL::CGAL CGAL::CGAL_Core)
