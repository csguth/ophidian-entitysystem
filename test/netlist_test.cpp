#include <catch.hpp>
#include <ophidian/netlist/netlist.h>
#include <memory>

using namespace ophidian;

using Netlist = netlist::netlist;
using Cell = netlist::cell;
using Pin = netlist::pin;
using Net = netlist::net;

auto NULL_entity = entity_system::entity_system::entity::null();


class BrandNewNetlist_Fixture {
    Netlist m_netlist;
public:
    Netlist & netlist() {
        return m_netlist;
    }
};

class Netlist_With_One_Cell_Fixture : public BrandNewNetlist_Fixture {
    Cell m_cell;
public:
    Netlist_With_One_Cell_Fixture() : m_cell(netlist().create<Cell>()){

    }
    netlist::cell cell() const {
        return m_cell;
    }
};

class Netlist_With_One_Pin_Fixture : public BrandNewNetlist_Fixture {
    Pin m_pin;
public:
    Netlist_With_One_Pin_Fixture() : m_pin(netlist().create<Pin>()){
    }
    netlist::pin pin() const {
        return m_pin;
    }
};

TEST_CASE_METHOD(BrandNewNetlist_Fixture, "netlist: creation", "[netlist]") {
    REQUIRE(netlist().size<Cell>() == 0);
    REQUIRE(netlist().size<Pin>() == 0);
}

TEST_CASE_METHOD(Netlist_With_One_Cell_Fixture, "netlist: create cell", "[netlist]") {
    REQUIRE(netlist().valid(cell()));
    REQUIRE(netlist().size<Cell>() == 1);
}

TEST_CASE_METHOD(Netlist_With_One_Cell_Fixture, "netlist: destroy cell", "[netlist]") {
    netlist().destroy(cell());
    REQUIRE(netlist().size<Cell>() == 0);
}


TEST_CASE_METHOD(Netlist_With_One_Pin_Fixture, "netlist: create pin", "[netlist]") {
    REQUIRE(netlist().valid(pin()));
    REQUIRE(netlist().size<Pin>() == 1);
}

TEST_CASE_METHOD(Netlist_With_One_Pin_Fixture, "netlist: destroy pin", "[netlist]") {
    netlist().destroy(pin());
    REQUIRE(netlist().size<Pin>() == 0);
}


TEST_CASE_METHOD(Netlist_With_One_Cell_Fixture, "netlist: make cell property", "[netlist]") {
    auto property = netlist().make_property<Cell, int>();
    property[cell()] = 10;
    REQUIRE(property[cell()] == 10);
}

TEST_CASE_METHOD(Netlist_With_One_Cell_Fixture, "netlist: cell names", "[netlist]") {
    auto names = netlist().make_property<Cell, std::string>();
    names[cell()] = "u1";
    REQUIRE(names[cell()] == "u1");
}

struct myscopedentity {
    entity_system::entity_system::entity entity() const {
        return entity_system::entity_system::entity{};
    }
};

TEST_CASE_METHOD(Netlist_With_One_Cell_Fixture, "netlist: cell names 2 ", "[netlist]") {
    auto names = netlist().make_property<Cell, std::string>();
    //   DON'T COMPILE :
    //   auto ints = netlist().make_property<myscopedentity, int>();
    names[cell()] = "u1";
    REQUIRE(names[cell()] == "u1");
}

TEST_CASE_METHOD(Netlist_With_One_Cell_Fixture, "netlist: trying to access a cell property with a pin", "[netlist]") {
    auto names = netlist().make_property<Cell, std::string>();
    auto cell = netlist().create<Cell>();
    auto pin = netlist().create<Pin>();
    names[cell]; // OK, compiles
    //   names[pin]; // Invalid, does not compile
}


TEST_CASE("netlist: connect net and pin", "[netlist]")
{

    netlist::netlist nl;
    auto net = nl.create<Net>();
    auto pin = nl.create<Pin>();

    REQUIRE( nl.pins_size(net) == 0 );
    REQUIRE( !nl.is_connected(pin) );
    REQUIRE( static_cast<entity_system::entity_system::entity>(nl.net_of(pin)) == NULL_entity );

    nl.connect(net, pin);

    REQUIRE( nl.pins_size(net) == 1 );
    REQUIRE( nl.is_connected(pin) );
    REQUIRE( nl.net_of(pin) == net );

    nl.disconnect(pin);

    REQUIRE( nl.pins_size(net) == 0 );
    REQUIRE( !nl.is_connected(pin) );
    REQUIRE( static_cast<entity_system::entity_system::entity>(nl.net_of(pin)) == NULL_entity );

}

TEST_CASE("netlist: net pin iterator", "[netlist]")
{

    netlist::netlist nl;
    auto net = nl.create<Net>();

    Pin p1, p2, p3;

    nl.connect(net, p1 = nl.create<Pin>());
    nl.connect(net, p2 = nl.create<Pin>());
    nl.connect(net, p3 = nl.create<Pin>());

    REQUIRE( std::count(nl.pins_of(net).begin(), nl.pins_of(net).end(), p1) == 1 );
    REQUIRE( std::count(nl.pins_of(net).begin(), nl.pins_of(net).end(), p2) == 1 );
    REQUIRE( std::count(nl.pins_of(net).begin(), nl.pins_of(net).end(), p3) == 1 );

    REQUIRE( nl.net_of(p1) == net );
    REQUIRE( nl.net_of(p2) == net );
    REQUIRE( nl.net_of(p3) == net );

    nl.disconnect(p2);

    REQUIRE( std::count(nl.pins_of(net).begin(), nl.pins_of(net).end(), p1) == 1 );
    REQUIRE( std::count(nl.pins_of(net).begin(), nl.pins_of(net).end(), p2) == 0 );
    REQUIRE( std::count(nl.pins_of(net).begin(), nl.pins_of(net).end(), p3) == 1 );


    REQUIRE( static_cast<entity_system::entity_system::entity>(nl.net_of(p2)) == NULL_entity  );

}


TEST_CASE("netlist: cell & pins composition", "[netlist]")
{

    netlist::netlist nl;
    auto cell = nl.create<Cell>();

    Pin p1, p2, p3, p4;

    REQUIRE( nl.pins_size(cell) == 0 );

    nl.attach(cell, p1 = nl.create<Pin>());
    nl.attach(cell, p2 = nl.create<Pin>());
    nl.attach(cell, p3 = nl.create<Pin>());

    p4 = nl.create<Pin>(); // unnassigned pin

    REQUIRE( nl.size<Pin>() == 4 );

    REQUIRE( nl.pins_size(cell) == 3 );
    REQUIRE( std::count(nl.pins_of(cell).begin(), nl.pins_of(cell).end(), p1) == 1 );
    REQUIRE( std::count(nl.pins_of(cell).begin(), nl.pins_of(cell).end(), p2) == 1 );
    REQUIRE( std::count(nl.pins_of(cell).begin(), nl.pins_of(cell).end(), p3) == 1 );
    REQUIRE( std::count(nl.pins_of(cell).begin(), nl.pins_of(cell).end(), p4) == 0 ); // must not contain p4

    nl.destroy(p2);

    REQUIRE( nl.pins_size(cell) == 2 );
    REQUIRE( std::count(nl.pins_of(cell).begin(), nl.pins_of(cell).end(), p1) == 1 );
    REQUIRE( std::count(nl.pins_of(cell).begin(), nl.pins_of(cell).end(), p2) == 0 );
    REQUIRE( std::count(nl.pins_of(cell).begin(), nl.pins_of(cell).end(), p3) == 1 );
    REQUIRE( std::count(nl.pins_of(cell).begin(), nl.pins_of(cell).end(), p4) == 0 ); // must not contain p4

    nl.destroy(cell);

    REQUIRE( !nl.valid(p1) );
    REQUIRE( !nl.valid(p2) );
    REQUIRE( !nl.valid(p3) );

    REQUIRE( nl.valid(p4) );
    REQUIRE( nl.size<Pin>() == 1 ); // must remain p4
}
