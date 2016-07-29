#include "composition.h"
#include "entity_system.h"

#include <algorithm>
#include <functional>

namespace ophidian {
namespace entity_system {

composition::composition(const entity_system &composite, entity_system &component) :
    association(composite, component)
{
}

composition::~composition()
{
}

void composition::destroy(entity_system::entity en)
{
    auto components = m_components[en];

    for(auto component : components)
        destroy_component(component);

    m_components.destroy(en);
}

void composition::clear()
{
    for(auto compo : composite()) {
        auto components = m_components[compo];
        for(auto component : components)
            destroy_component(component);
    }
    m_components.clear();
}

}
}
