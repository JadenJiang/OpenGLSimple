cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)

project (OpenGLSimple)


if (WIN32)
    set(SYSTEM_PLATFORM "Windows")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")

elseif(UNIX AND NOT APPLE)
    set(SYSTEM_PLATFORM "Unix")

elseif(APPLE)
    set(SYSTEM_PLATFORM "Apple")

else(${CMAKE_SYSTEM_NAME} STREQUAL "XXXX")

endif(WIN32)


set(3RDPART_PATH "${CMAKE_SOURCE_DIR}/3rdparty/${SYSTEM_PLATFORM}")

set(GLFW_PATH "${3RDPART_PATH}/GLFW")
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${GLFW_PATH}/include)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${GLFW_PATH}/lib )
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} glfw3)


set(LearnOpenGL_PATH "${3RDPART_PATH}/LearnOpenGL")
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${LearnOpenGL_PATH}/include)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${LearnOpenGL_PATH}/lib )
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} assimp)


#complie for stb_image lib.
add_definitions(-DSTB_IMAGE_IMPLEMENTATION)

set(GLAD_MODULE libglad)
add_library(${GLAD_MODULE} STATIC "deps/glad.c")
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} ${GLAD_MODULE})

set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} opengl32 )
#RelWithDebInfo\STB_IMAGE.lib
include_directories(${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})


list(APPEND CMAKE_CXX_FLAGS "-std=c++11")
  

set(PROJECTS
    textures
    triangle
    transformation
    coordinate
    camera
    depth_testing
    stencil_testing
    blending
    face_culling
    framebuffers
)


foreach(PROJECT  ${PROJECTS})
    add_subdirectory(source/${PROJECT})
endforeach(PROJECT)

#MESSAGE(">>>,"  "${CMAKE_SOURCE_DIR}," "${CMAKE_CURRENT_BINARY_DIR}," "${SUFFIX},")
