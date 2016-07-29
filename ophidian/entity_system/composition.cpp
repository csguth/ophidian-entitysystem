#include "composition.h"
#include "entity_system.h"

#include <algorithm>
#include <functional>

namespace ophidian {
namespace entity_system {

composition::composition(const entity_system &composite, entity_system &component) :
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

composition::~composition()
{
    entity_system::null().notifier()->dettach(m_components);
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
    return (m_part.composite_of(component) == composite);
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
    m_part.composite_of(component, entity_system::entity::null());
}

void composition::destroy(entity_system::entity en)
{
    auto components = m_components[en];

    for(auto component : components)
        m_component.destroy(component);

    m_components.destroy(en);
}

void composition::clear()
{
    for(auto composite : m_composite) {
        auto components = m_components[composite];
        for(auto component : components)
            m_component.destroy(component);
    }
    m_components.clear();
}

part_of::part_of(composition &compo) :
    m_composition(compo),
    m_composites(make_property<entity_system::entity>(compo.component()))
{
    compo.component().notifier()->dettach(m_composites);
    entity_system::null().notifier()->attach(m_composites);
    system(compo.component());
    resize(compo.component().size());
}

part_of::~part_of()
{
    entity_system::null().notifier()->dettach(m_composites);
}

void part_of::composite_of(entity_system::entity component, entity_system::entity composite)
{
    m_composites[component] = composite;
}

void part_of::destroy(entity_system::entity en)
{
    if(is_attached(en))
        m_composition.dettach_component(m_composites[en], en);
    m_composites.destroy(en);
}

void part_of::clear()
{
    for(auto component : m_composition.component())
        m_composition.dettach_component(m_composites[component], component);
    m_composites.clear();
}

}
}
