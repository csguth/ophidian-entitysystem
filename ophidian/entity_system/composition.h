#ifndef OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H
#define OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H

#include "entity_system.h"
#include "property.h"


namespace ophidian {
namespace entity_system {

template <class IteratorType>
class bounds {

    const std::vector< entity_system::entity > & m_storage;

public:
    bounds(const std::vector< entity_system::entity > & storage) :
        m_storage(storage){
    }

    std::vector< entity_system::entity >::const_iterator begin() const {
        return m_storage.begin();
    }

    std::vector< entity_system::entity >::const_iterator end() const {
        return m_storage.end();
    }
};


class entity_system;
class composition;

class part_of : public abstract_property {
    composition & m_composition;
    property< entity_system::entity > m_composites;
public:
    part_of(composition & compo);

    void composite_of(entity_system::entity component, entity_system::entity composite);
    void destroy(entity_system::entity en);

    void resize(std::size_t new_size) {
        m_composites.resize(new_size);
    }

    void create(entity_system::entity en) {
        m_composites.create(en);
    }

    void clear() {
        m_composites.clear();
    }

    entity_system::entity composite_of(entity_system::entity en) const {
        return m_composites[en];
    }

};

class composition : public abstract_property
{
    const entity_system & m_composite;
    entity_system & m_component;

    property< std::vector< entity_system::entity > > m_components;
    part_of m_part;


public:
    composition(const entity_system & composite, entity_system & component);

    const entity_system& composite() const;
    const entity_system& component() const;
    bool is_component_of(entity_system::entity composite, entity_system::entity component) const;

    void attach_component(entity_system::entity composite, entity_system::entity component);
    void dettach_component(entity_system::entity composite, entity_system::entity component);


    bounds<std::vector< entity_system::entity >::const_iterator> components_bounds(entity_system::entity composite) const
    {
        return bounds<std::vector< entity_system::entity >::const_iterator>(m_components.at(composite));
    }

    std::size_t components_size(entity_system::entity composite) const {
        return m_components.at(composite).size();
    }
    entity_system::entity composite_of(entity_system::entity component) const {
        return m_part.composite_of(component);
    }
    void resize(std::size_t new_size) {
        m_components.resize(new_size);
    }

    void create(entity_system::entity en) {
        m_components.create(en);
    }

    void destroy(entity_system::entity en);

    void clear() {
    }

};

template <class Iterator>
void attach_component(composition& compo, const Iterator composite_begin, const Iterator composite_end, const Iterator component_begin, const Iterator component_end) {
    Iterator curr_composite(composite_begin);
    Iterator curr_component(component_begin);

    for(; curr_composite != composite_end && curr_component != component_end; ++curr_composite, ++curr_component)
        compo.attach_component(*curr_composite, *curr_component);
}

template <class Iterator>
void attach_component(composition& compo, entity_system::entity composite, const Iterator component_begin, const Iterator component_end) {
    Iterator curr_component(component_begin);

    for(;  curr_component != component_end;  ++curr_component)
        compo.attach_component(composite, *curr_component);
}

}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H
