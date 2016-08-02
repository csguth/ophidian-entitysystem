#ifndef OPHIDIAN_NETLIST_NETLIST_H
#define OPHIDIAN_NETLIST_NETLIST_H

#include <ophidian/entity_system/entity_system.h>
#include <ophidian/entity_system/property.h>
#include <cassert>

namespace ophidian {
namespace netlist {

class netlist;

#define CREATE_OPAQUE_ENTITY(NAME, id) \
    class NAME {\
        entity_system::entity_system::entity m_entity;\
        friend class netlist;\
        explicit NAME(entity_system::entity_system::entity entity) : m_entity(entity) {}\
    public:\
        NAME(void) {}\
        entity_system::entity_system::entity entity() const { return m_entity; }\
    };\
    template <> struct entity_type_offset <NAME> {\
        static const size_t value = id;\
    };\



//\
//    template <> void netlist::destroy<NAME>(NAME en){ SYSTEM.destroy(en.m_entity); }\
//    template <> bool netlist::valid<NAME>(NAME en) const { return SYSTEM.valid(en.m_entity); }\
//    template <> std::size_t netlist::size<NAME>(void) const { return SYSTEM.size(); }

struct NetlistEntityIDs {

enum IDs {
    CELL = 0,
    PIN,
    NET
};

};

template <class T> struct entity_type_offset {
    static const size_t value = -1;
};

CREATE_OPAQUE_ENTITY(cell, NetlistEntityIDs::CELL)
CREATE_OPAQUE_ENTITY(pin, NetlistEntityIDs::PIN)
CREATE_OPAQUE_ENTITY(net, NetlistEntityIDs::NET)

class netlist {
    entity_system::entity_system m_systems[3];

    template <typename T>
    inline void offset_check() const {
        static_assert(entity_type_offset<T>::value >= NetlistEntityIDs::CELL && entity_type_offset<T>::value <= NetlistEntityIDs::NET, "invalid entity");
    }

public:
    template <typename T> T create() {
        offset_check<T>();
        return T(m_systems[entity_type_offset<T>::value].create());
    }

    template <typename T> void destroy(T entity) {
        offset_check<T>();
        return m_systems[entity_type_offset<T>::value].destroy(entity.m_entity);
    }

    template <typename T> std::size_t size() const {
        offset_check<T>();
        return m_systems[entity_type_offset<T>::value].size();
    }

    template <typename T> bool valid(T entity) const {
        offset_check<T>();
        return m_systems[entity_type_offset<T>::value].valid(entity.m_entity);
    }

    template <typename T, typename PropertyType>
    entity_system::opaque_property<T, PropertyType> make_property() {
        offset_check<T>();
        return entity_system::make_opaque_property<T, PropertyType>(m_systems[entity_type_offset<T>::value]);
    }
};




}
}

#endif // OPHIDIAN_NETLIST_NETLIST_H
