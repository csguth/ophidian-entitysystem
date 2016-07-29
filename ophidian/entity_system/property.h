#ifndef OPHIDIAN_ENTITY_SYSTEM_PROPERTY_H
#define OPHIDIAN_ENTITY_SYSTEM_PROPERTY_H

#include <vector>
#include "entity_system.h"
#include <cassert>

#ifndef NDEBUG
#include <iostream>
#endif

namespace ophidian {
namespace entity_system {

class abstract_property {
public:
    virtual ~abstract_property() { }
    virtual void resize(std::size_t new_size) = 0;
    virtual void create(entity_system::entity en) = 0;
    virtual void destroy(entity_system::entity en) = 0;
    virtual void clear() = 0;
    virtual void system(const entity_system & system) = 0;
    virtual const entity_system& system() const = 0;
};

class attached_property : public abstract_property {
    const entity_system * m_system;
public:
    attached_property() :
        m_system(&entity_system::null()) {
        m_system->notifier()->attach(*this);
    }

    virtual ~attached_property() {
        if(m_system->notifier()->has_property(*this))
            m_system->notifier()->dettach(*this);
    }
    void system(const entity_system & system) {
        m_system->notifier()->dettach(*this);
        m_system = &system;
        m_system->notifier()->attach(*this);
    }

    const entity_system& system() const {
        return *m_system;
    }
};







template <class PropertyType>
class property : public attached_property
{
public:
    class helper {
    public:
        static property<PropertyType> make_property(const entity_system & sys) {
            return std::move(property<PropertyType>(sys));
        }
    };

    friend class helper;

    using ContainerType = std::vector<PropertyType>;
    using IteratorType = typename ContainerType::iterator;
    using ConstIteratorType = typename ContainerType::const_iterator;

private:

    ContainerType m_container;

    void init(const entity_system & sys) {
        system(sys);
        resize(system().size());
    }

    property(const entity_system & sys)
    {
#ifndef NDEBUG
        std::cout << "property::property(const entity_system & sys)" << std::endl;
#endif
        init(sys);
    }
public:

    property(const property & prop) :
        property(prop.system())
    {
#ifndef NDEBUG
        std::cout << "property::property(const property & prop)" << std::endl;
#endif
    }

    property(property &&prop) {
#ifndef NDEBUG
        std::cout << "property::property(property &&prop)" << std::endl;
#endif
        init(prop.system());
        prop.system().notifier()->dettach(prop);
    }

    property& operator=(const property& prop) {
#ifndef NDEBUG
        std::cout << "property::operator=(const property& prop)" << std::endl;
#endif
        init(prop.system());
    }


    property()
    {
    }

    std::size_t lookup(entity_system::entity en) const {
        return system().lookup(en);
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
        std::swap(m_container.at(lookup(en)), m_container.back());
        m_container.pop_back();
    }

    void clear() {
        m_container.clear();
    }

    void resize(std::size_t new_size) {
        m_container.resize(new_size);
    }


    PropertyType operator[](entity_system::entity_system::entity en) const {
        return m_container[lookup(en)];
    }

    typename ContainerType::reference operator[](entity_system::entity_system::entity en) {
        return m_container[lookup(en)];
    }

    typename ContainerType::const_reference at(entity_system::entity_system::entity en) const {
        return m_container.at(lookup(en));
    }


};

template<class PropertyType>
property<PropertyType> make_property(const entity_system & sys) {
    return property<PropertyType>::helper::make_property(sys);
}


}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_PROPERTY_H
