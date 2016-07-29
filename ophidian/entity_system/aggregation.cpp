#include "aggregation.h"

namespace ophidian {
namespace entity_system {

aggregation::aggregation(const entity_system &composite, entity_system &component) :
    association(composite, component)
{

}

aggregation::~aggregation()
{

}

void aggregation::destroy(entity_system::entity en)
{
    auto components = m_components[en];

    for(auto component : components)
        dettach_component(en, component);

    m_components.destroy(en);
}

void aggregation::clear()
{
    for(auto compo : composite()) {
        auto components = m_components[compo];
        for(auto component : components)
            dettach_component(compo, component);
    }
    m_components.clear();
}

}
}
