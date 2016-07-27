#ifndef OPHIDIAN_ENTITY_SYSTEM_SYSTEM_H
#define OPHIDIAN_ENTITY_SYSTEM_SYSTEM_H

#include <cstdint>
#include <vector>
#include <set>
#include <memory>

namespace ophidian {
namespace entity_system {

class abstract_property;




class system
{
public:
    class entity {

        const std::uint32_t INVALID{-1};

        friend class system;
        std::uint32_t m_id;

        explicit entity(std::uint32_t id) : m_id(id) {};
    public:
        entity();
        entity(const entity& o);
        entity& operator=(const entity&o);

        bool operator==(const entity&o);
        bool operator!=(const entity&o);
        std::uint32_t id() const {
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

private:

    std::vector<std::size_t> m_id2index;
    std::vector<entity> m_entities;


    std::set<const system*> m_parts;

    std::unique_ptr<notifier_> m_notifier;
public:
    system();
    virtual ~system();

    std::uint32_t size() const { return m_entities.size(); }
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

    void composed_of(const system & o);
    bool is_part_of(const system & o) const { return o.is_composed_of(*this); }
    bool is_composed_of(const system & o) const { return m_parts.find(&o) != m_parts.end(); }
    bool has_property(abstract_property& prop) const { return m_notifier->has_property(prop); }


};

}
}



#endif // OPHIDIAN_ENTITY_SYSTEM_SYSTEM_H
