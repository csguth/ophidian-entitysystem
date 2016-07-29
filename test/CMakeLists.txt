add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE .)

aux_source_directory(. SRC_LIST)
include_directories(${CMAKE_SOURCE_DIR})

add_executable(run_tests ${SRC_LIST})
target_link_libraries(run_tests Catch ophidian_entitysystem ophidian_netlist)


add_test(NAME entity_system COMMAND run_tests "[entity_system]")
