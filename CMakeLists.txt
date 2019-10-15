cmake_minimum_required(VERSION 3.14.0)
set(CMAKE_CXX_STANDARD 17)

project(photo2bldg)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Auto-run MOC and UIC when needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# Set OpenGL legacy preference
#set(OpenGL_GL_PREFERENCE GLVND)

#if(IS_CLIENT)
    add_subdirectory(client)
#else()
#    add_subdirectory(server)
#endif()

#
## Get sources
#file(GLOB SOURCES src/*.cpp)
#
#list(REMOVE_ITEM SOURCES src/Server*)
#list(REMOVE_ITEM SOURCES src/MainWindow*)
#
#add_executable(${PROJECT_NAME} ${SOURCES})
#target_link_libraries(${PROJECT_NAME} Qt5::Core)
#target_link_libraries(${PROJECT_NAME} Qt5::Gui)
#target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
#target_link_libraries(${PROJECT_NAME} Qt5::OpenGL)
#target_link_libraries(${PROJECT_NAME} Qt5::Xml)
#target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
#target_link_libraries(${PROJECT_NAME} ${Caffe_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} ${CUDA_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} ${DLIB_LIBRARIES} dlib)
#target_link_libraries(${PROJECT_NAME} CGAL::CGAL CGAL::CGAL_Core)
