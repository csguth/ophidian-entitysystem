#ifndef OPHIDIAN_NETLIST_NETLIST_H
#define OPHIDIAN_NETLIST_NETLIST_H

#include <ophidian/entity_system/entity_system.h>
#include <ophidian/entity_system/property.h>
#include <cassert>

#include <boost/serialization/strong_typedef.hpp>

namespace ophidian {
namespace netlist {

class netlist;

template <class T> struct entity_type_offset {
    static const size_t value = -1;
};

BOOST_STRONG_TYPEDEF(entity_system::entity_system::entity, cell)
BOOST_STRONG_TYPEDEF(entity_system::entity_system::entity, pin)
BOOST_STRONG_TYPEDEF(entity_system::entity_system::entity, net)

template<> struct entity_type_offset<cell> {
    static const size_t value = 0;
};

template<> struct entity_type_offset<pin> {
    static const size_t value = 1;
};

template<> struct entity_type_offset<net> {
    static const size_t value = 2;
};

class netlist {
    entity_system::entity_system m_systems[3];

    template <typename T>
    inline void offset_check() const {
        static_assert(entity_type_offset<T>::value == entity_type_offset<cell>::value || entity_type_offset<T>::value == entity_type_offset<pin>::value || entity_type_offset<T>::value == entity_type_offset<net>::value, "The passed Entity is not handled by Netlist!!");
    }

public:
    template <typename T> T create() {
        offset_check<T>();
        return T(m_systems[entity_type_offset<T>::value].create());
    }

    template <typename T> void destroy(T entity) {
        offset_check<T>();
        return m_systems[entity_type_offset<T>::value].destroy(entity);
    }

    template <typename T> std::size_t size() const {
        offset_check<T>();
        return m_systems[entity_type_offset<T>::value].size();
    }

    template <typename T> bool valid(T entity) const {
        offset_check<T>();
        return m_systems[entity_type_offset<T>::value].valid(entity);
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
