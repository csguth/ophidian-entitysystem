#include <catch.hpp>

#include <ophidian/entity_system/composition.h>

#include "association_fixture.h"

using namespace ophidian::entity_system;

using whole_composed_of_three_parts_fixture = association_fixture::whole_composed_of_three_parts_fixture<composition>;

TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "composition: 'whole_composed_of_three_parts_fixture' test", "[composition]") {
    REQUIRE( composite().size() == 1 );
    REQUIRE( component().size() == 3 );
    REQUIRE( relation().components_size(whole()) == 3 );
}

TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "composition: destroy whole, destroy parts", "[composition]") {
    composite().destroy(whole());
    REQUIRE( !component().valid(part<0>()) );
    REQUIRE( !component().valid(part<1>()) );
    REQUIRE( !component().valid(part<2>()) );
    REQUIRE_THROWS( relation().components_bounds(whole()) );
    REQUIRE_THROWS( relation().components_size(whole()) );
    REQUIRE_THROWS( relation().composite_of(part<0>()) );
    REQUIRE_THROWS( relation().composite_of(part<1>()) );
    REQUIRE_THROWS( relation().composite_of(part<2>()) );
}

TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "composition: clear whole system, destroy parts", "[composition]") {
    composite().clear();
    REQUIRE( !component().valid(part<0>()) );
    REQUIRE( !component().valid(part<1>()) );
    REQUIRE( !component().valid(part<2>()) );
    REQUIRE_THROWS( relation().components_bounds(whole()) );
    REQUIRE_THROWS( relation().components_size(whole()) );
    REQUIRE_THROWS( relation().composite_of(part<0>()) );
    REQUIRE_THROWS( relation().composite_of(part<1>()) );
    REQUIRE_THROWS( relation().composite_of(part<2>()) );
}


TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "composition: destroy part, remove from whole", "[composition]") {
    component().destroy(part<0>());
    REQUIRE_THROWS( relation().is_component_of(whole(), part<0>()) );
    REQUIRE( relation().is_component_of(whole(), part<1>()) );
    REQUIRE( relation().is_component_of(whole(), part<2>()) );
    REQUIRE( relation().components_size(whole()) == 2 );
    REQUIRE( relation().composite_of(part<1>()) == whole() );
    REQUIRE( relation().composite_of(part<2>()) == whole() );
    REQUIRE_THROWS( relation().composite_of(part<0>()) );
}

TEST_CASE_METHOD(whole_composed_of_three_parts_fixture, "composition: clear parts, remove from wholes", "[composition]") {
    component().clear();
    REQUIRE_THROWS( relation().is_component_of(whole(), part<0>()) );
    REQUIRE_THROWS( relation().is_component_of(whole(), part<1>()) );
    REQUIRE_THROWS( relation().is_component_of(whole(), part<2>()) );
    REQUIRE( relation().components_size(whole()) == 0 );
    REQUIRE_THROWS( relation().composite_of(part<1>()) );
    REQUIRE_THROWS( relation().composite_of(part<2>()) );
    REQUIRE_THROWS( relation().composite_of(part<0>()) );
}


struct self_composition_with_root_and_child_fixture {
    entity_system sys;
    composition compo{sys, sys};
    entity_system::entity root;
    entity_system::entity child;

    self_composition_with_root_and_child_fixture() :
        root(sys.create()),
        child(sys.create())
    {
        compo.attach_component(root, child);
    }
};


TEST_CASE_METHOD(self_composition_with_root_and_child_fixture, "compostition:  'self_composition_with_root_and_child_fixture' test", "[composition]") {
    REQUIRE( compo.components_size(root) == 1 );
    REQUIRE( compo.composite_of(child) == root );
    REQUIRE( std::find(compo.components_bounds(root).begin(), compo.components_bounds(root).end(), child) != compo.components_bounds(root).end() );
}


TEST_CASE_METHOD(self_composition_with_root_and_child_fixture, "compostition: self composition, attach child, destroy parent", "[composition]") {
    sys.destroy(root);
    REQUIRE( !sys.valid(child) );
    REQUIRE_THROWS( compo.components_size(root) );
    REQUIRE_THROWS( compo.components_size(child) );
}

TEST_CASE_METHOD(self_composition_with_root_and_child_fixture, "compostition: self composition, attach child, destroy child", "[composition]") {
    sys.destroy(child);
    REQUIRE( compo.components_size(root) == 0 );
    REQUIRE_THROWS( compo.components_size(child) );
}

