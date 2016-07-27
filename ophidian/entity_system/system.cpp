#include "system.h"
#include "property.h"

#include <limits>

namespace ophidian {
namespace entity_system {

system::system() :
    m_notifier(new notifier_)
{

}

system::~system()
{

}

system::entity system::create()
{
    entity new_entity(m_id2index.size());
    m_id2index.push_back(m_entities.size());
    m_entities.push_back(new_entity);

    m_notifier->create(new_entity);

    return new_entity;
}

void system::destroy(system::entity e)
{
    m_notifier->destroy(e);

    std::size_t index = m_id2index.at(e.id());

    m_id2index[m_entities.back().id()] = index;

    std::swap(m_entities.at(index), m_entities.back());
    m_entities.pop_back();
    m_id2index[e.id()] = std::numeric_limits<std::size_t>::max();



}

bool system::valid(system::entity e) const
{
    return (e != entity::null()) && (m_id2index[e.id()] != std::numeric_limits<std::size_t>::max());
}

void system::clear()
{
    m_notifier->clear();
    m_entities.clear();
}

void system::composed_of(const system &o)
{
    m_parts.insert(&o);
}

system::entity::entity() : m_id(INVALID)
{

}

system::entity::entity(const system::entity &o)
{
    *this = o;
}

system::entity &system::entity::operator=(const system::entity &o)
{
    m_id = o.m_id;
    return *this;
}

bool system::entity::operator==(const system::entity &o)
{
    return m_id == o.m_id;
}

bool system::entity::operator!=(const system::entity &o)
{
    return !((*this) == o);
}

system::entity system::entity::null()
{
    return entity();
}

void system::notifier_::attach(abstract_property &prop)
{
    m_properties.insert(&prop);
}

void system::notifier_::dettach(abstract_property &prop)
{
    m_properties.erase(&prop);
}

void system::notifier_::create(system::entity en)
{
    for(auto prop : m_properties)
        prop->create(en);
}

void system::notifier_::destroy(system::entity en)
{
    for(auto prop : m_properties)
        prop->destroy(en);
}

void system::notifier_::clear()
{
    for(auto prop : m_properties)
        prop->clear();
}


bool system::notifier_::has_property(abstract_property &prop) const
{

}

}
}
