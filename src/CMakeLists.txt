add_library(tam STATIC tam.cc)
target_include_directories(tam PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(tam_exe main.cc)
target_include_directories(tam_exe PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(tam_exe tam)
