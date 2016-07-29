#ifndef OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H
#define OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H

#include "property.h"

namespace ophidian {
namespace entity_system {

class association;
class part_of_association : public attached_property {
    association & m_association;
    property< entity_system::entity > m_composites;
public:
    part_of_association(association & compo);
    virtual ~part_of_association();

    void composite_of(entity_system::entity component, entity_system::entity composite);
    void destroy(entity_system::entity en);

    bool is_attached(entity_system::entity en) const {
        return m_composites[en] != entity_system::entity::null();
    }

    void resize(std::size_t new_size) {
        m_composites.resize(new_size);
    }

    void create(entity_system::entity en) {
        m_composites.create(en);
    }

    void clear();

    entity_system::entity composite_of(entity_system::entity en) const {
        return m_composites[en];
    }

};

class association : public attached_property
{
    const entity_system & m_composite;
    entity_system & m_component;
    part_of_association m_part;
protected:
    property< std::vector< entity_system::entity > > m_components;

    void destroy_component(entity_system::entity en);


public:
    association(const entity_system & composite, entity_system & component);
    virtual ~association();
    const entity_system& composite() const;
    const entity_system& component() const;
    bool is_component_of(entity_system::entity composite, entity_system::entity component) const;

    void attach_component(entity_system::entity composite, entity_system::entity component);
    void dettach_component(entity_system::entity composite, entity_system::entity component);

    bool is_attached(entity_system::entity component) const {
        return m_part.is_attached(component);
    }

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

    virtual void destroy(entity_system::entity en) = 0;

    virtual void clear() = 0;

};


template <class Iterator>
void attach_component(association& compo, const Iterator composite_begin, const Iterator composite_end, const Iterator component_begin, const Iterator component_end) {
    Iterator curr_composite(composite_begin);
    Iterator curr_component(component_begin);

    for(; curr_composite != composite_end && curr_component != component_end; ++curr_composite, ++curr_component)
        compo.attach_component(*curr_composite, *curr_component);
}

template <class Iterator>
void attach_component(association& compo, entity_system::entity composite, const Iterator component_begin, const Iterator component_end) {
    Iterator curr_component(component_begin);

    for(;  curr_component != component_end;  ++curr_component)
        compo.attach_component(composite, *curr_component);
}

}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H
