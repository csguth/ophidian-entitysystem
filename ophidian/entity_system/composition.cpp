#include "composition.h"
#include "entity_system.h"

#include <algorithm>
#include <functional>

namespace ophidian {
namespace entity_system {

composition::composition(const entity_system &composite, entity_system &component) :
    m_composite(composite),
    m_component(component),
    m_components(composite),
    m_part(*this)
{
    composite.notifier()->dettach(m_components);
    composite.notifier()->attach(*this);
    resize(composite.size());
}

const entity_system &composition::composite() const
{
    return m_composite;
}

const entity_system &composition::component() const
{
    return m_component;
}

bool composition::is_component_of(entity_system::entity composite, entity_system::entity component) const
{
    auto& parts = m_components.at(composite);
    return std::find(parts.begin(), parts.end(), component) != parts.end();
}

void composition::attach_component(entity_system::entity composite, entity_system::entity component)
{
    m_components[composite].push_back(component);
    m_part.composite_of(component, composite);
}

void composition::dettach_component(entity_system::entity composite, entity_system::entity component)
{
    auto& parts = m_components[composite];
    auto erased = std::remove(parts.begin(), parts.end(), component);
    if(erased == parts.end())
        throw std::runtime_error("dettaching unattached component!!");
    parts.erase(erased , parts.end() );
}

void composition::destroy(entity_system::entity en)
{
    auto components = m_components[en];

//    std::for_each(components.begin(), components.end(), std::bind(&entity_system::destroy, &m_component, std::placeholders::_1));
    for(auto component : components)
        m_component.destroy(component);

    m_components.destroy(en);
}

part_of::part_of(composition &compo) :
    m_composition(compo),
    m_composites(compo.component())
{
    compo.component().notifier()->dettach(m_composites);
    compo.component().notifier()->attach(*this);
}

void part_of::composite_of(entity_system::entity component, entity_system::entity composite)
{
    m_composites[component] = composite;
}

void part_of::destroy(entity_system::entity en)
{
    m_composition.dettach_component(m_composites[en], en);
    m_composites.destroy(en);
}


}
}
