
cmake_minimum_required(VERSION 2.8)

project(voxelterrain)



#OpenGL
find_package(OpenGL REQUIRED)

#GLUT
set(GLUT_INCLUDE_DIRS)
set(GLUT_LIBRARY glut64)
set(GLEW_LIBRARY glew64)



add_executable(main main.cc HeightMap.cc VoxelMap.cc MarchingCuber.cc Chunk.cc Texture.cc lodepng.cc)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
target_link_libraries(main ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY} )