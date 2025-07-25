cmake_minimum_required(VERSION 3.20)
project(tam VERSION 0.1.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS Yes)

add_subdirectory(src)
add_subdirectory(test)

find_package(Doxygen)
if(Doxygen_FOUND)
  set(DOXYGEN_JAVADOC_AUTOBRIEF Yes)
  doxygen_add_docs(docs ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/include)
endif()
