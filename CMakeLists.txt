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
include_directories(${GLFW_PATH}/include)
link_directories(${GLFW_PATH}/lib )
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} glfw3)


set(LearnOpenGL_PATH "${3RDPART_PATH}/LearnOpenGL")
include_directories(${LearnOpenGL_PATH}/include)
link_directories(${LearnOpenGL_PATH}/lib )
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} assimp)


set(GLM_PATH "${3RDPART_PATH}/glm")
include_directories(${GLM_PATH}/include)


set(GLAD_PATH "${3RDPART_PATH}/glad")
set(GLAD_MODULE libglad)
add_library(${GLAD_MODULE} STATIC "deps/glad.c")
target_include_directories(${GLAD_MODULE} PRIVATE 
                                ${GLAD_PATH}/include/glad
                                ${GLAD_PATH}/include/KHR)
#target_link_libraries(${GLAD_MODULE} xxxx)
include_directories(${GLAD_PATH}/include)
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} ${GLAD_MODULE})


set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} opengl32 )
add_compile_options(-std=c++11)
#complie for stb_image lib.
add_definitions(-DSTB_IMAGE_IMPLEMENTATION)

#####build libcommon#####
set(COMMON_PATH "source/common")
add_subdirectory(${COMMON_PATH})
include_directories(${COMMON_PATH}/include)
set(CMAKE_LIBRARYS ${CMAKE_LIBRARYS} libcommon)


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
    glsl_uniform
    instancing
    filters
    pixel_buffer_object
)


foreach(PROJECT  ${PROJECTS})
    add_subdirectory(source/${PROJECT})
endforeach(PROJECT)

#MESSAGE(">>>,"  "${CMAKE_SOURCE_DIR}," "${CMAKE_CURRENT_BINARY_DIR}," "${SUFFIX},")
