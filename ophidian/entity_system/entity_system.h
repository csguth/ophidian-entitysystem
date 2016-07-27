#ifndef OPHIDIAN_ENTITY_SYSTEM_ENTITY_SYSTEM_H
#define OPHIDIAN_ENTITY_SYSTEM_ENTITY_SYSTEM_H

#include <vector>
#include <set>
#include <memory>

namespace ophidian {
namespace entity_system {

class abstract_property;

class entity_system
{
public:
    class entity {

        static const std::size_t INVALID;

        friend class entity_system;
        std::size_t m_id;

        explicit entity(std::size_t id) : m_id(id) {};
    public:
        entity();
        entity(const entity& o);
        entity& operator=(const entity&o);

        bool operator==(const entity&o) const;
        bool operator!=(const entity&o) const;
        std::size_t id() const {
            return m_id;
        }
        static entity null();
    };

    class notifier_ {

        std::set<abstract_property*> m_properties;


    public:

        void attach(abstract_property& prop);
        void dettach(abstract_property& prop);
        bool has_property(abstract_property &prop) const;

        void create(entity en);
        void destroy(entity en);
        void clear();


    };

    using StorageType = std::vector<entity>;

private:

    std::vector<std::size_t> m_id2index;
    StorageType m_entities;


    std::set<const entity_system*> m_parts;

    std::unique_ptr<notifier_> m_notifier;
public:
    entity_system();
    virtual ~entity_system();

    std::size_t size() const { return m_entities.size(); }
    std::size_t lookup(entity e) const {
        std::size_t value = m_id2index.at(e.id());
        if(value == -1)
            throw std::runtime_error("error, deleted entity id = " + std::to_string(e.id()));
        return value;
    }

    entity create();
    void destroy(entity e);
    bool valid(entity e) const;
    void clear();


    notifier_ * notifier() const {
        return const_cast<notifier_*>(m_notifier.get());
    }


    bool has_property(abstract_property& prop) const { return m_notifier->has_property(prop); }

    bool operator==(const entity_system& other) const {
        return this == &other;
    }


};

// helper functs
template <int NumberOfElements>
std::array<entity_system::entity, NumberOfElements> make_entities(entity_system& e) {
    std::array<entity_system::entity, NumberOfElements> elements;
    for(auto & el : elements)
        el = e.create();
    return elements;
}


}
}



#endif // OPHIDIAN_ENTITY_SYSTEM_ENTITY_SYSTEM_H
