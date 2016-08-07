#include "netlist.h"

namespace ophidian {
namespace netlist {

netlist::netlist() :
    m_nets_pins(m_systems[entity_type_offset<net>::value], m_systems[entity_type_offset<pin>::value]),
    m_cells_pins(m_systems[entity_type_offset<cell>::value], m_systems[entity_type_offset<pin>::value])
{

}

netlist::~netlist()
{

}



struct netlist::pin_iterator::impl {
    std::vector< entity_system::entity_system::entity >::const_iterator it;
};

entity_system::bounds<netlist::pin_iterator> netlist::pins_of(net n) const {
    auto bounds = m_nets_pins.components_bounds(n);
    return entity_system::bounds<pin_iterator>(
                pin_iterator(pin_iterator::impl{bounds.begin()}),
                pin_iterator(pin_iterator::impl{bounds.end()}))
            ;
}

entity_system::bounds<netlist::pin_iterator> netlist::pins_of(cell n) const
{
    auto bounds = m_cells_pins.components_bounds(n);
    return entity_system::bounds<pin_iterator>(
                pin_iterator(pin_iterator::impl{bounds.begin()}),
                pin_iterator(pin_iterator::impl{bounds.end()}))
            ;
}

netlist::pin_iterator::pin_iterator() :
    this_(new impl)
{

}

netlist::pin_iterator::~pin_iterator()
{

}

netlist::pin_iterator::pin_iterator(const netlist::pin_iterator &o) :
    this_(new impl)
{
    this_->it = o.this_->it;
}

netlist::pin_iterator &netlist::pin_iterator::operator=(const netlist::pin_iterator &o)
{
    this_->it = o.this_->it;
}

const netlist::pin_iterator &netlist::pin_iterator::operator++()
{
    ++this_->it;
    return *this;
}

netlist::pin_iterator::difference_type netlist::pin_iterator::operator-(const netlist::pin_iterator &o) const
{
    return this_->it - o.this_->it;
}

pin netlist::pin_iterator::operator*() const
{
    return static_cast<pin>(*this_->it);
}

bool netlist::pin_iterator::operator==(const netlist::pin_iterator &it) const
{
    return this_->it == it.this_->it;
}

bool netlist::pin_iterator::operator!=(const netlist::pin_iterator &it) const
{
    return this_->it != it.this_->it;
}

netlist::pin_iterator::pin_iterator(const netlist::pin_iterator::impl &i)
    : this_(new impl(i))
{

}

}
}
