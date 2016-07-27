#include <catch.hpp>

#include <ophidian/entity_system/system.h>
#include <ophidian/entity_system/property.h>


using namespace ophidian;

struct property_mock : public entity_system::abstract_property {
    void create(entity_system::system::entity e){

    }

    void destroy(entity_system::system::entity e){

    }

    void clear() {

    }
};


// FIXTURES
struct brand_new_entity_system_fixture {
    entity_system::system sys;
    property_mock prop;
};

struct system_with_one_entity {
    entity_system::system sys;
    entity_system::system::entity en;
    system_with_one_entity() : en(sys.create()){
    }
};

struct system_with_two_entities : public system_with_one_entity{
    entity_system::system::entity en2;
    system_with_two_entities() : system_with_one_entity(), en2(sys.create()){
    }
};

struct system_with_three_entities : public system_with_two_entities{
    entity_system::system::entity en3;
    system_with_three_entities() : system_with_two_entities(), en3(sys.create()){
    }
};

struct entity_system_and_property_int : public system_with_three_entities {
    entity_system::property<int> prop;
    entity_system_and_property_int() : prop(sys) {
        prop[en] = 1;
        prop[en2] = 2;
        prop[en3] = 3;
    }
};


TEST_CASE_METHOD(brand_new_entity_system_fixture, "entity_system: entity system has a constructor", "[entity_system]") {
    REQUIRE(sys.size() == 0);
}

TEST_CASE_METHOD(brand_new_entity_system_fixture, "entity_system: can't destroy null", "[entity_system]") {
    REQUIRE_THROWS(sys.destroy(entity_system::system::entity::null()));
}

TEST_CASE_METHOD(system_with_one_entity, "entity_system: add entity", "[entity_system]") {
    REQUIRE(en.id() == 0);
    REQUIRE(sys.size() == 1);
}

TEST_CASE_METHOD(system_with_one_entity, "entity_system: check entity", "[entity_system]") {
    REQUIRE(sys.valid(en));
    REQUIRE(!sys.valid(entity_system::system::entity::null()));
}

TEST_CASE_METHOD(system_with_one_entity, "entity_system: destroy entity", "[entity_system]") {
    sys.destroy(en);
    REQUIRE(sys.size() == 0);
    REQUIRE(!sys.valid(en));
}

TEST_CASE_METHOD(system_with_one_entity, "entity_system: can't destroy twice", "[entity_system]") {
    sys.destroy(en);
    REQUIRE_THROWS(sys.destroy(en));
}

TEST_CASE_METHOD(system_with_one_entity, "entity_system: lookup", "[entity_system]") {
    REQUIRE( sys.lookup(en) == 0 );
    REQUIRE_THROWS(sys.lookup(entity_system::system::entity::null()));
}

TEST_CASE_METHOD(system_with_two_entities, "entity_system: system with two entities", "[entity_system]") {
    REQUIRE(sys.size() == 2);
    REQUIRE( sys.lookup(en) == 0 );
    REQUIRE( sys.lookup(en2) == 1 );

}

TEST_CASE_METHOD(system_with_two_entities, "entity_system: crescent ids", "[entity_system]") {
    REQUIRE(en.id() == 0);
    REQUIRE(en2.id() == 1);
}

TEST_CASE_METHOD(system_with_two_entities, "entity_system: destroy first, keep second", "[entity_system]") {
    sys.destroy(en);
    REQUIRE(!sys.valid(en));
    REQUIRE(sys.valid(en2));
    REQUIRE(sys.size() == 1);
    REQUIRE_THROWS( sys.lookup(en) );
    REQUIRE( sys.lookup(en2) == 0 );
}


TEST_CASE_METHOD(system_with_two_entities, "entity_system: clear", "[entity_system]") {
    sys.clear();
    REQUIRE(sys.size() == 0);
}

TEST_CASE_METHOD(system_with_three_entities, "entity_system: system with three entities", "[entity_system]") {
    REQUIRE(sys.size() == 3);
}

TEST_CASE_METHOD(system_with_three_entities, "entity_system: destroy first, keep second and third", "[entity_system]") {
    sys.destroy(en);
    REQUIRE(!sys.valid(en));
    REQUIRE(sys.valid(en2));
    REQUIRE(sys.valid(en3));
    REQUIRE(sys.size() == 2);
}


TEST_CASE_METHOD(brand_new_entity_system_fixture, "entity_system: notifier withouth prop", "[entity_system]") {
    REQUIRE( !sys.notifier()->has_property(prop) );
}

TEST_CASE_METHOD(brand_new_entity_system_fixture, "entity_system: notifier attach", "[entity_system]") {
    sys.notifier()->attach(prop);
    REQUIRE( sys.notifier()->has_property(prop) );
}

TEST_CASE_METHOD(brand_new_entity_system_fixture, "entity_system: notifier detach", "[entity_system]") {
    sys.notifier()->attach(prop);
    sys.notifier()->dettach(prop);
    REQUIRE(!sys.notifier()->has_property(prop));
}


TEST_CASE_METHOD(brand_new_entity_system_fixture, "entity_system: property<int>", "[entity_system]") {
    entity_system::property<int> ints(sys);
    REQUIRE(ints.size() == 0);
    REQUIRE(sys.has_property(ints));
}

TEST_CASE_METHOD(brand_new_entity_system_fixture, "entity_system: add entity, add property", "[entity_system]") {
    entity_system::property<int> ints(sys);
    entity_system::system::entity created = sys.create();
    REQUIRE(ints.size() == 1);
}

TEST_CASE_METHOD(entity_system_and_property_int, "entity_system: create property<int> after create entities", "[entity_system]") {
    REQUIRE(prop.size() == 3);
}


TEST_CASE_METHOD(entity_system_and_property_int, "entity_system: get property value", "[entity_system]") {
    REQUIRE( prop[en] == 1 );
    REQUIRE( prop[en2] == 2 );
    REQUIRE( prop[en3] == 3 );
}


TEST_CASE_METHOD(entity_system_and_property_int, "entity_system: assign property value", "[entity_system]") {
    prop[en2] = 6;
    REQUIRE( prop[en] == 1 );
    REQUIRE( prop[en2] == 6 );
    REQUIRE( prop[en3] == 3 );
}

TEST_CASE_METHOD(entity_system_and_property_int, "entity_system: keep property values of others when destroying", "[entity_system]") {
    sys.destroy(en);
    REQUIRE( prop[en2] == 2 );
    REQUIRE( prop[en3] == 3 );
}

TEST_CASE_METHOD(entity_system_and_property_int, "entity_system: try to retrieve property from destroyed entity", "[entity_system]") {
    sys.destroy(en);
    REQUIRE_THROWS( prop[en] );
    REQUIRE(prop.size() == 2);
}

TEST_CASE_METHOD(entity_system_and_property_int, "entity_system: clear property after clearing system", "[entity_system]") {
    sys.clear();
    REQUIRE( prop.size() == 0 );
}

struct car_and_wheels_fixture {
    entity_system::system car;
    entity_system::system wheels;

    entity_system::system::entity whole;
    entity_system::system::entity part;

    car_and_wheels_fixture() : whole(car.create()), part(wheels.create()) {
        car.composed_of(wheels);
    }
};

TEST_CASE_METHOD(car_and_wheels_fixture, "entity_system: composition 1-1", "[entity_system]") {
    REQUIRE( wheels.is_part_of(car) );
    REQUIRE( car.is_composed_of(wheels) );
    REQUIRE( !car.is_part_of(wheels) );
    REQUIRE( !wheels.is_composed_of(car) );
}

TEST_CASE_METHOD(car_and_wheels_fixture, "entity_system: assign part to whole", "[entity_system]") {

}



