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
BOOST_STRONG_TYPEDEF(entity_system::entity_system::entity, primary_input)
BOOST_STRONG_TYPEDEF(entity_system::entity_system::entity, primary_output)

template<> struct entity_type_offset<cell> {
    static const size_t value = 0;
};

template<> struct entity_type_offset<pin> {
    static const size_t value = 1;
};

template<> struct entity_type_offset<net> {
    static const size_t value = 2;
};

template<> struct entity_type_offset<primary_input> {
    static const size_t value = 3;
};

template<> struct entity_type_offset<primary_output> {
    static const size_t value = 4;
};

class netlist {
public:

    template <typename ValueType>
    class iterator : public std::iterator<std::forward_iterator_tag, ValueType, std::ptrdiff_t> {
    public:
        friend class netlist;
        iterator(){}
        ~iterator(){}
        iterator(const iterator & o){
            m_it = o.m_it;
        }
        iterator& operator=(const iterator & o) {
            m_it = o.m_it;
        }
        const iterator& operator++(void) {
            ++m_it;
            return *this;
        }
        std::ptrdiff_t operator-(const iterator & o) const {
            return m_it - o.m_it;
        }
        ValueType operator*() const {
            return static_cast<ValueType>(*m_it);
        }
        bool operator==(const iterator & it) const {
            return m_it == it.m_it;
        }
        bool operator!=(const iterator & it) const {
            return m_it != it.m_it;
        }
    private:
        iterator(entity_system::entity_system::const_iterator it) :
            m_it(it){}
        entity_system::entity_system::const_iterator m_it;
    };

    template <typename ValueType>
    class part_iterator : public std::iterator<std::forward_iterator_tag, ValueType, std::ptrdiff_t> {
    public:
        friend class netlist;
        part_iterator(){}
        ~part_iterator(){}
        part_iterator(const part_iterator & o){
            m_it = o.m_it;
        }
        part_iterator& operator=(const part_iterator & o) {
            m_it = o.m_it;
        }
        const part_iterator& operator++(void) {
            ++m_it;
            return *this;
        }
        std::ptrdiff_t operator-(const part_iterator & o) const {
            return m_it - o.m_it;
        }
        ValueType operator*() const {
            return static_cast<ValueType>(*m_it);
        }
        bool operator==(const part_iterator & it) const {
            return m_it == it.m_it;
        }
        bool operator!=(const part_iterator & it) const {
            return m_it != it.m_it;
        }
    private:
        part_iterator(std::vector<entity_system::entity_system::entity>::const_iterator it) :
            m_it(it){}
        std::vector<entity_system::entity_system::entity>::const_iterator m_it;
    };

    netlist();
    ~netlist();

    template <typename T> T create() {
        offset_check<T>();
        return T(m_systems[entity_type_offset<T>::value].create());
    }

    template <typename T> void destroy(T entity) {
        offset_check_for_size<T>();
        return m_systems[entity_type_offset<T>::value].destroy(entity);
    }

    template <typename T> std::size_t size() const {
        offset_check_for_size<T>();
        return m_systems[entity_type_offset<T>::value].size();
    }

    template <typename T> bool valid(T entity) const {
        offset_check_for_size<T>();
        return m_systems[entity_type_offset<T>::value].valid(entity);
    }

    template <typename T> entity_system::bounds< iterator<T> > bounds() const {
        offset_check_for_size<T>();
        return entity_system::bounds< iterator<T> >(
                    iterator<T>(m_systems[entity_type_offset<T>::value].begin()),
                    iterator<T>(m_systems[entity_type_offset<T>::value].end()));
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
        offset_check_for_size<T>();
        return entity_system::make_opaque_property<T, PropertyType>(m_systems[entity_type_offset<T>::value]);
    }

    inline void attach(cell c, pin p) {
        m_cells_pins.attach_component(c, p);
    }

    entity_system::bounds<part_iterator<pin> > pins_of(net n) const;
    entity_system::bounds<part_iterator<pin> > pins_of(cell n) const;

    inline primary_input make_PI(pin p) {
        primary_input PI;
        m_pins_PI.attach_component(p, PI = m_systems[entity_type_offset<primary_input>::value].create());
        return PI;
    }

    bool is_PI(pin p) const {
        return (m_pins_PI.components_size(p) == 1);
    }

    pin pin_of(primary_input PI) const {
        return static_cast<pin>(m_pins_PI.composite_of(PI));
    }

    primary_input PI_of(pin p) const {
        for(entity_system::entity_system::entity component : m_pins_PI.components_bounds(p))
            return static_cast<primary_input>(component);
        return primary_input();
    }

    inline primary_output make_PO(pin p) {
        primary_output PO;
        m_pins_PO.attach_component(p, PO = m_systems[entity_type_offset<primary_output>::value].create());
        return PO;
    }

    bool is_PO(pin p) const {
        return (m_pins_PO.components_size(p) == 1);
    }

    pin pin_of(primary_output PO) const {
        return static_cast<pin>(m_pins_PO.composite_of(PO));
    }

    primary_output PO_of(pin p) const {
        for(entity_system::entity_system::entity component : m_pins_PO.components_bounds(p))
            return static_cast<primary_output>(component);
        return primary_output();
    }

private:
    entity_system::entity_system m_systems[5];
    entity_system::aggregation m_nets_pins;
    entity_system::composition m_cells_pins;
    entity_system::composition m_pins_PI;
    entity_system::composition m_pins_PO;


    template <typename T>
    inline void offset_check() const {
        static_assert(
                    entity_type_offset<T>::value == entity_type_offset<cell>::value ||
                    entity_type_offset<T>::value == entity_type_offset<pin>::value ||
                    entity_type_offset<T>::value == entity_type_offset<net>::value,
                    "The passed Entity is not handled by Netlist!!");
    }

    template <typename T>
    inline void offset_check_for_size() const {
        static_assert(
                    entity_type_offset<T>::value == entity_type_offset<cell>::value ||
                    entity_type_offset<T>::value == entity_type_offset<pin>::value ||
                    entity_type_offset<T>::value == entity_type_offset<net>::value ||
                    entity_type_offset<T>::value == entity_type_offset<primary_input>::value ||
                    entity_type_offset<T>::value == entity_type_offset<primary_output>::value,
                    "The passed Entity is not handled by Netlist!!");
    }


};




}
}

#endif // OPHIDIAN_NETLIST_NETLIST_H
