#include "association.h"
#include "entity_system.h"

#include <algorithm>
#include <functional>

namespace ophidian {
namespace entity_system {

void association::destroy_component(entity_system::entity en)
{
    m_component.destroy(en);
}

association::association(const entity_system &composite, entity_system &component) :
    m_composite(composite),
    m_component(component),
    m_components(make_property<std::vector< entity_system::entity > >(composite)),
    m_part(*this)
{
    composite.notifier()->dettach(m_components);
    entity_system::null().notifier()->attach(m_components);
    system(composite);
    resize(composite.size());
}

association::~association()
{
    entity_system::null().notifier()->dettach(m_components);
}

const entity_system &association::composite() const
{
    return m_composite;
}

const entity_system &association::component() const
{
    return m_component;
}

bool association::is_component_of(entity_system::entity composite, entity_system::entity component) const
{
    return (m_part.composite_of(component) == composite);
}

void association::attach_component(entity_system::entity composite, entity_system::entity component)
{
    m_components[composite].push_back(component);
    m_part.composite_of(component, composite);
}

void association::dettach_component(entity_system::entity composite, entity_system::entity component)
{
    auto& parts = m_components[composite];
    auto erased = std::remove(parts.begin(), parts.end(), component);
    if(erased == parts.end())
        throw std::runtime_error("dettaching unattached component!!");
    parts.erase(erased , parts.end() );
    m_part.composite_of(component, entity_system::entity::null());
}

void association::destroy(entity_system::entity en)
{
    auto components = m_components[en];

    for(auto component : components)
        m_component.destroy(component);

    m_components.destroy(en);
}


part_of_association::part_of_association(association &compo) :
    m_association(compo),
    m_composites(make_property<entity_system::entity>(compo.component()))
{
    compo.component().notifier()->dettach(m_composites);
    entity_system::null().notifier()->attach(m_composites);
    system(compo.component());
    resize(compo.component().size());
}

part_of_association::~part_of_association()
{
    entity_system::null().notifier()->dettach(m_composites);
}

void part_of_association::composite_of(entity_system::entity component, entity_system::entity composite)
{
    m_composites[component] = composite;
}

void part_of_association::destroy(entity_system::entity en)
{
    if(is_attached(en))
        m_association.dettach_component(m_composites[en], en);
    m_composites.destroy(en);
}

void part_of_association::clear()
{
    for(auto component : m_association.component())
        m_association.dettach_component(m_composites[component], component);
    m_composites.clear();
}

}
}
