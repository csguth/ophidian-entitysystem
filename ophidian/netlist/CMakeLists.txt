aux_source_directory(. SRC_LIST)
add_library(ophidian_netlist ${SRC_LIST})
target_link_libraries(ophidian_netlist ophidian_entitysystem)
