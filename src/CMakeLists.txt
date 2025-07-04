add_library(tam STATIC tam.cc primitives.cc)
target_include_directories(tam PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(tam_exe main.cc)
target_link_libraries(tam_exe tam CLI11::CLI11)
set_property(TARGET tam_exe PROPERTY OUTPUT_NAME tam)
