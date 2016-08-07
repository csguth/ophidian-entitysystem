#ifndef OPHIDIAN_NETLIST_NETLIST_H
#define OPHIDIAN_NETLIST_NETLIST_H

#include <ophidian/entity_system/entity_system.h>
#include <ophidian/entity_system/property.h>
#include <ophidian/entity_system/aggregation.h>
#include <ophidian/entity_system/composition.h>
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
public:

    class pin_iterator : public std::iterator<std::forward_iterator_tag, pin, std::ptrdiff_t> {
    public:
        friend class netlist;
        pin_iterator();
        ~pin_iterator();
        pin_iterator(const pin_iterator & o);
        pin_iterator& operator=(const pin_iterator & o);
        const pin_iterator& operator++(void);
        difference_type operator-(const pin_iterator & o) const;
        pin operator*() const;
        bool operator==(const pin_iterator & it) const;
        bool operator!=(const pin_iterator & it) const;
    private:
        struct impl;
        pin_iterator(const impl & i);
        std::unique_ptr<impl> this_;
    };

    netlist();
    ~netlist();

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



    inline bool is_connected(pin p) const {
        return m_nets_pins.is_attached(p);
    }

    inline net net_of(pin p) const {
        return static_cast<net>(m_nets_pins.composite_of(p));
    }

    inline cell cell_of(pin p) const {
        return static_cast<cell>(m_cells_pins.composite_of(p));
    }

    inline void connect(net n, pin p) {
        m_nets_pins.attach_component(n, p);
    }

    inline std::size_t pins_size(net n) const {
        return m_nets_pins.components_size(n);
    }

    inline std::size_t pins_size(cell c) const {
        return m_cells_pins.components_size(c);
    }

    inline void disconnect(pin p) {
        m_nets_pins.dettach_component(m_nets_pins.composite_of(p), p);
    }

    template <typename T, typename PropertyType>
    inline entity_system::opaque_property<T, PropertyType> make_property() {
        offset_check<T>();
        return entity_system::make_opaque_property<T, PropertyType>(m_systems[entity_type_offset<T>::value]);
    }

    inline void attach(cell c, pin p) {
        m_cells_pins.attach_component(c, p);
    }

    entity_system::bounds<pin_iterator> pins_of(net n) const;
    entity_system::bounds<pin_iterator> pins_of(cell n) const;

private:
    entity_system::entity_system m_systems[3];
    entity_system::aggregation m_nets_pins;
    entity_system::composition m_cells_pins;

    template <typename T>
    inline void offset_check() const {
        static_assert(entity_type_offset<T>::value == entity_type_offset<cell>::value || entity_type_offset<T>::value == entity_type_offset<pin>::value || entity_type_offset<T>::value == entity_type_offset<net>::value, "The passed Entity is not handled by Netlist!!");
    }

};




}
}

#endif // OPHIDIAN_NETLIST_NETLIST_H
