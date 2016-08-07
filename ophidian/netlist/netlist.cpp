#include "netlist.h"

namespace ophidian {
namespace netlist {

netlist::netlist() :
    m_nets_pins(m_systems[entity_type_offset<net>::value], m_systems[entity_type_offset<pin>::value]),
    m_cells_pins(m_systems[entity_type_offset<cell>::value], m_systems[entity_type_offset<pin>::value]),
    m_pins_PI(m_systems[entity_type_offset<pin>::value], m_systems[entity_type_offset<primary_input>::value]),
    m_pins_PO(m_systems[entity_type_offset<pin>::value], m_systems[entity_type_offset<primary_output>::value])
{

}

netlist::~netlist()
{

}


entity_system::bounds<netlist::part_iterator<pin> > netlist::pins_of(net n) const {
    auto bounds = m_nets_pins.components_bounds(n);
    return entity_system::bounds<netlist::part_iterator<pin> >(
                netlist::part_iterator<pin>(bounds.begin()),
                netlist::part_iterator<pin>(bounds.end())
                );
}

entity_system::bounds<netlist::part_iterator<pin> > netlist::pins_of(cell n) const
{
    auto bounds = m_cells_pins.components_bounds(n);
    return entity_system::bounds<netlist::part_iterator<pin> >(
                netlist::part_iterator<pin>(bounds.begin()),
                netlist::part_iterator<pin>(bounds.end())
                );
}

}
}
