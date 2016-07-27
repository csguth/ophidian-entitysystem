#ifndef OPHIDIAN_ENTITY_SYSTEM_PROPERTY_H
#define OPHIDIAN_ENTITY_SYSTEM_PROPERTY_H

#include <vector>
#include "entity_system.h"
namespace ophidian {
namespace entity_system {


class abstract_property {

public:
    virtual void resize(std::size_t new_size) = 0;
    virtual void create(entity_system::entity en) = 0;
    virtual void destroy(entity_system::entity en) = 0;
    virtual void clear() = 0;
};



template <class PropertyType>
class property : public abstract_property
{
public:
    using ContainerType = std::vector<PropertyType>;
    using IteratorType = typename ContainerType::iterator;
    using ConstIteratorType = typename ContainerType::const_iterator;

private:
    const entity_system & m_system;
    ContainerType m_container;


public:
    property(const entity_system & sys) : m_system(sys) {
        sys.notifier()->attach(*this);
        m_container.resize(m_system.size());
    }

    virtual ~property() {

    }

    ConstIteratorType begin() const {
        return m_container.cbegin();
    }

    ConstIteratorType end() const {
        return m_container.cend();
    }

    std::size_t size() const {
        return m_container.size();
    }


    void create(entity_system::entity en) {
        m_container.resize(m_container.size()+1);
    }

    void destroy(entity_system::entity en) {
        std::swap(m_container.at(m_system.lookup(en)), m_container.back());
        m_container.pop_back();
    }

    void clear() {
        m_container.clear();
    }

    void resize(std::size_t new_size) {
        m_container.resize(new_size);
    }


    PropertyType operator[](entity_system::entity_system::entity en) const {
        return m_container[m_system.lookup(en)];
    }

    typename ContainerType::reference operator[](entity_system::entity_system::entity en) {
        return m_container[m_system.lookup(en)];
    }

    typename ContainerType::const_reference at(entity_system::entity_system::entity en) const {
        return m_container.at(m_system.lookup(en));
    }
};

}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_PROPERTY_H
