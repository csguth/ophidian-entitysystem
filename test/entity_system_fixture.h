#ifndef ENTITY_SYSTEM_FIXTURE_H
#define ENTITY_SYSTEM_FIXTURE_H

#include <ophidian/entity_system/entity_system.h>

namespace entity_system_test {

using namespace ophidian::entity_system;

struct property_mock : public abstract_property {

    void resize(std::size_t new_size) {

    }

    void create(entity_system::entity e){
    }

    void destroy(entity_system::entity e){
    }

    void clear() {
    }
};


// FIXTURES
struct empty_system {
    entity_system sys;
    property_mock prop;
};


struct attatched_empty_system {
    empty_system fixture;
    entity_system & sys = fixture.sys;
    abstract_property & prop = fixture.prop;

    attatched_empty_system() {
        sys.notifier()->attach(prop);
    }
};

struct empty_system_with_int_property : public empty_system  {
    property<int> ints;
    empty_system_with_int_property() :
        ints(sys){
    }
};


template <int N>
struct system_with_N_entities {
    entity_system sys;
    std::array< entity_system::entity, N > en;
    system_with_N_entities() : en(make_entities<N>(sys)) {
    }


    template<int i>
    entity_system::entity entity() {
        return en[i];
    }

};

using system_with_1_entity = system_with_N_entities<1>;
using system_with_2_entities = system_with_N_entities<2>;
using system_with_3_entities = system_with_N_entities<3>;
using system_with_4_entities = system_with_N_entities<4>;


struct system_with_3_entities_and_a_property : public system_with_3_entities {
    property<int> prop;
    system_with_3_entities_and_a_property() : prop(sys) {
        prop[en[0]] = 1;
        prop[en[1]] = 2;
        prop[en[2]] = 3;
    }
};


template <class Fixture>
struct destroy_first_entity {
    Fixture fixture;
    destroy_first_entity() {
        fixture.sys.destroy(fixture.entity<0>());
    }

    entity_system & sys = fixture.sys;

    template<int i>
    entity_system::entity entity() {
        return fixture.entity<i>();
    }
};


}

#endif // ENTITY_SYSTEM_FIXTURE_H
