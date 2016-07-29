#ifndef OPHIDIAN_NETLIST_NETLIST_H
#define OPHIDIAN_NETLIST_NETLIST_H

#include <ophidian/entity_system/entity_system.h>

namespace ophidian {
namespace netlist {

class netlist;

#define CREATE_OPAQUE_ENTITY(NAME) \
    class NAME {\
        entity_system::entity_system::entity m_entity;\
        friend class netlist;\
        NAME(entity_system::entity_system::entity entity) : m_entity(entity) {}\
    public:\
        NAME(void) {}\
    };


#define CREATE_ENTITY_HANDLING_METHODS(NAME, SYSTEM) \
    template <> NAME create(void){ return NAME(SYSTEM.create()); }
//\
//    template <> void netlist::destroy<NAME>(NAME en){ SYSTEM.destroy(en.m_entity); }\
//    template <> bool netlist::valid<NAME>(NAME en) const { return SYSTEM.valid(en.m_entity); }\
//    template <> std::size_t netlist::size<NAME>(void) const { return SYSTEM.size(); }


CREATE_OPAQUE_ENTITY(cell)
CREATE_OPAQUE_ENTITY(pin)
CREATE_OPAQUE_ENTITY(net)


class netlist
{
    entity_system::entity_system m_cells;
    entity_system::entity_system m_pins;
    entity_system::entity_system m_nets;
public:
    netlist();
    CREATE_ENTITY_HANDLING_METHODS(cell, m_cells)
    CREATE_ENTITY_HANDLING_METHODS(pin, m_pins)


//    CREATE_ENTITY_HANDLING_METHODS(net, m_nets)


};



}
}

#endif // OPHIDIAN_NETLIST_NETLIST_H
