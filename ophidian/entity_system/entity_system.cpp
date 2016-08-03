#include "entity_system.h"
#include "property.h"

#include <limits>

namespace ophidian {
namespace entity_system {

const std::size_t entity_system::entity::INVALID{std::numeric_limits<std::size_t>::max()};

const entity_system entity_system::m_NULL;

entity_system::entity_system() :
    m_notifier(new notifier_)
{

}

entity_system::~entity_system()
{

}

entity_system::entity entity_system::create()
{
    entity new_entity(m_id2index.size());
    m_id2index.push_back(m_entities.size());
    m_entities.push_back(new_entity);

    m_notifier->create(new_entity);

    return new_entity;
}

void entity_system::destroy(entity_system::entity e)
{
    m_notifier->destroy(e);

    std::size_t index = m_id2index.at(e.id());

    m_id2index.at(m_entities.back().id()) = index;

    std::swap(m_entities.at(index), m_entities.back());
    m_entities.pop_back();
    m_id2index[e.id()] = entity_system::entity::INVALID;



}

bool entity_system::valid(entity_system::entity e) const
{
    return (e != entity::null()) && (m_id2index[e.id()] != entity_system::entity::INVALID);
}

void entity_system::clear()
{
    m_notifier->clear();
    m_entities.clear();
    std::fill(m_id2index.begin(), m_id2index.end(), entity::INVALID);
}

entity_system::entity::entity() : m_id(INVALID)
{

}

entity_system::entity::entity(const entity_system::entity &o)
{
    *this = o;
}

entity_system::entity &entity_system::entity::operator=(const entity_system::entity &o)
{
    m_id = o.m_id;
    return *this;
}

bool entity_system::entity::operator<(const entity_system::entity &o) const
{
    return m_id < o.m_id;
}

bool entity_system::entity::operator==(const entity_system::entity &o) const
{
    return m_id == o.m_id;
}

bool entity_system::entity::operator!=(const entity_system::entity &o) const
{
    return !((*this) == o);
}

entity_system::entity entity_system::entity::null()
{
    static entity null_entity;
    return null_entity;
}

void entity_system::notifier_::attach(abstract_property &prop)
{
    m_properties.insert(&prop);
}

void entity_system::notifier_::dettach(abstract_property &prop)
{
    std::size_t items_erased{m_properties.erase(&prop)};
    if(!items_erased)
        throw std::runtime_error("can't dettach unattached property");
}

void entity_system::notifier_::create(entity_system::entity en)
{
    for(auto prop : m_properties)
        prop->create(en);
}

void entity_system::notifier_::destroy(entity_system::entity en)
{
    for(auto prop : m_properties)
        prop->destroy(en);
}

void entity_system::notifier_::clear()
{
    for(auto prop : m_properties)
        prop->clear();
}


bool entity_system::notifier_::has_property(abstract_property &prop) const
{
    return m_properties.find(&prop) != m_properties.end();
}

}
}
