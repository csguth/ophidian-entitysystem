#include <catch.hpp>

#include <ophidian/entity_system/entity_system.h>
#include <ophidian/entity_system/composition.h>

using namespace ophidian::entity_system;


struct part_whole_fixture {
    entity_system composite, component;
    composition relation;
    part_whole_fixture() :
        relation(composite, component)
    {
    }
};

struct part_whole_fixture_with_one_part_and_one_whole_unassigned : public part_whole_fixture {
    entity_system::entity whole;
    entity_system::entity part;
    part_whole_fixture_with_one_part_and_one_whole_unassigned() : whole(composite.create()), part(component.create()) {
    }
};


struct part_whole_fixture_with_one_part_and_one_whole : public part_whole_fixture_with_one_part_and_one_whole_unassigned {

    part_whole_fixture_with_one_part_and_one_whole() {
        relation.attach_component(whole, part);
    }
};


struct whole_with_two_parts : public part_whole_fixture_with_one_part_and_one_whole {
    entity_system::entity part2;
    whole_with_two_parts() : part2(component.create())
    {
        relation.attach_component(whole, part2);
    }
};

struct whole_with_three_parts : public whole_with_two_parts {
    entity_system::entity part3;
    whole_with_three_parts() : part3(component.create()) {
        relation.attach_component(whole, part3);
    }
};

TEST_CASE("part of: destructor", "[composition][part_of]") {
    entity_system composite, component;
    composition compo(composite, component);
    const int NULL_PROPERTIES_BEFORE = 2;
    part_of * part;
    part = new part_of(compo);
    REQUIRE( entity_system::null().properties_size() == NULL_PROPERTIES_BEFORE + 1 );
    delete part;
    REQUIRE( entity_system::null().properties_size() == NULL_PROPERTIES_BEFORE);
}

TEST_CASE("part of: constructor", "[composition][part_of]") {
    entity_system composite, component;
    composition compo(composite, component);
    const int NULL_PROPERTIES_BEFORE = 2;
    part_of part(compo);
    REQUIRE(&part.system() == &component);
    REQUIRE(composite.has_property(compo));
    REQUIRE(composite.properties_size() == 1);
    REQUIRE(entity_system::null().properties_size() == NULL_PROPERTIES_BEFORE + 1);
}


TEST_CASE("composition: constructor & destructor","[composition]") {
    entity_system composite;
    entity_system component;

    const int NULL_PROPERTIES_BEFORE = entity_system::null().properties_size();

    composition * relation;
    REQUIRE_NOTHROW(relation = new composition(composite, component));
    // part-to-whole property, whole-to-part property
    REQUIRE(entity_system::null().properties_size() == NULL_PROPERTIES_BEFORE + 2);
    REQUIRE_NOTHROW(delete relation);

}

TEST_CASE("composition: test fixture","[composition]") {
    REQUIRE_NOTHROW( part_whole_fixture() );
}

TEST_CASE_METHOD(part_whole_fixture, "composition: part-whole", "[composition]") {
    REQUIRE( relation.composite() == composite );
    REQUIRE( relation.component() == component );
}

TEST_CASE_METHOD(part_whole_fixture_with_one_part_and_one_whole_unassigned, "composition: unassigned", "[composition]") {
    REQUIRE( !relation.is_component_of(whole, part) );
    REQUIRE( relation.components_size(whole) == 0 );
    REQUIRE( relation.composite_of(part) == entity_system::entity::null() );
}

TEST_CASE_METHOD(part_whole_fixture_with_one_part_and_one_whole, "composition: attach part to whole", "[composition]") {
    REQUIRE( relation.is_component_of(whole, part) );
    REQUIRE( relation.components_size(whole) == 1 );
    REQUIRE( relation.composite_of(part) == whole );
}

TEST_CASE_METHOD(part_whole_fixture_with_one_part_and_one_whole, "composition: dettach part from whole", "[composition]") {
    relation.dettach_component(whole, part);
    REQUIRE_THROWS( relation.dettach_component(whole, part) );
}


TEST_CASE_METHOD(whole_with_two_parts, "composition: get components bounds (two parts)", "[composition]") {
    auto bounds = relation.components_bounds(whole);
    REQUIRE( std::distance(bounds.begin(), bounds.end()) == 2 );
    REQUIRE( std::find(bounds.begin(), bounds.end(), part)  != bounds.end() );
    REQUIRE( std::find(bounds.begin(), bounds.end(), part2)  != bounds.end() );
}

#include <algorithm>

TEST_CASE_METHOD(whole_with_three_parts, "composition: get components bounds (three parts)", "[composition]") {
    auto bounds = relation.components_bounds(whole);
    REQUIRE( std::distance(bounds.begin(), bounds.end()) == 3 );
    REQUIRE( std::find(bounds.begin(), bounds.end(), part)  != bounds.end() );
    REQUIRE( std::find(bounds.begin(), bounds.end(), part2)  != bounds.end() );
    REQUIRE( std::find(bounds.begin(), bounds.end(), part3)  != bounds.end() );
}

TEST_CASE_METHOD(whole_with_three_parts, "composition: create whole, allocate links to parts", "[composition]") {
    auto new_whole = composite.create();
    REQUIRE_NOTHROW( relation.components_size(new_whole) );
    REQUIRE_NOTHROW( relation.components_bounds(new_whole) );
}

TEST_CASE_METHOD(whole_with_three_parts, "composition: create part, allocate link to whole", "[composition]") {
    auto new_part = component.create();
    REQUIRE_NOTHROW( relation.composite_of(new_part) == entity_system::entity::null() );
}


TEST_CASE_METHOD(whole_with_three_parts, "composition: destroy whole, destroy parts", "[composition]") {
    composite.destroy(whole);
    REQUIRE( !component.valid(part) );
    REQUIRE( !component.valid(part2) );
    REQUIRE( !component.valid(part3) );
    REQUIRE_THROWS( relation.components_bounds(whole) );
    REQUIRE_THROWS( relation.components_size(whole) );
    REQUIRE_THROWS( relation.composite_of(part) );
    REQUIRE_THROWS( relation.composite_of(part2) );
    REQUIRE_THROWS( relation.composite_of(part3) );
}

TEST_CASE_METHOD(whole_with_three_parts, "composition: clear whole system, destroy parts", "[composition]") {
    composite.clear();
    REQUIRE( !component.valid(part) );
    REQUIRE( !component.valid(part2) );
    REQUIRE( !component.valid(part3) );
    REQUIRE_THROWS( relation.components_bounds(whole) );
    REQUIRE_THROWS( relation.components_size(whole) );
    REQUIRE_THROWS( relation.composite_of(part) );
    REQUIRE_THROWS( relation.composite_of(part2) );
    REQUIRE_THROWS( relation.composite_of(part3) );
}


TEST_CASE_METHOD(whole_with_three_parts, "composition: destroy part, remove from whole", "[composition]") {
    component.destroy(part);
    REQUIRE( !relation.is_component_of(whole, part) );
    REQUIRE( relation.is_component_of(whole, part2) );
    REQUIRE( relation.is_component_of(whole, part3) );
    REQUIRE( relation.components_size(whole) == 2 );
    REQUIRE( relation.composite_of(part2) == whole );
    REQUIRE( relation.composite_of(part3) == whole );
    REQUIRE_THROWS( relation.composite_of(part) );
}

TEST_CASE_METHOD(whole_with_three_parts, "composition: clear parts, remove from wholes", "[composition]") {
    component.clear();
    REQUIRE( !relation.is_component_of(whole, part) );
    REQUIRE( !relation.is_component_of(whole, part2) );
    REQUIRE( !relation.is_component_of(whole, part3) );
    REQUIRE( relation.components_size(whole) == 0 );
    REQUIRE_THROWS( relation.composite_of(part2) );
    REQUIRE_THROWS( relation.composite_of(part3) );
    REQUIRE_THROWS( relation.composite_of(part) );
}

#include "entity_system_fixture.h"

struct whole_system_with_1_entity_and_part_system_with_4_entities {

    entity_system_test::system_with_1_entity composite;
    entity_system_test::system_with_4_entities component;
    composition relation;
    whole_system_with_1_entity_and_part_system_with_4_entities() : relation(composite.sys, component.sys ){
    }
};

TEST_CASE_METHOD(whole_system_with_1_entity_and_part_system_with_4_entities, "composition: helper attach N components", "[composition]") {
    attach_component(relation, composite.entity<0>(), component.en.begin(), component.en.end());
    REQUIRE( relation.components_size(composite.entity<0>()) == 4 );
    REQUIRE( std::equal(component.en.begin(), component.en.end(), relation.components_bounds(composite.entity<0>()).begin()) );
}

