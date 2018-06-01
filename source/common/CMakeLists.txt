#file(RELATIVE_PATH MODULE "${CMAKE_CURRENT_SOURCE_DIR}/.." "${CMAKE_CURRENT_SOURCE_DIR}")
set(MODULE libcommon)
set(SUFFIX  *.cpp *.h)

file(GLOB_RECURSE SRCS ${SUFFIX})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(${MODULE} ${SRCS})
target_link_libraries(${MODULE} ${CMAKE_LIBRARYS})
