aux_source_directory(. SRC_LIST)
add_library(ophidian_netlist ${SRC_LIST})
target_link_libraries(ophidian_netlist ophidian_entitysystem)
include_directories(${CMAKE_SOURCE_DIR})


install(TARGETS ophidian_netlist DESTINATION lib)
install(FILES netlist.h DESTINATION include/ophidian/netlist)
