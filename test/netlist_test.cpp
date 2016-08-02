#include <catch.hpp>
#include <ophidian/netlist/netlist.h>
#include <memory>

using namespace ophidian;

using Netlist = netlist::netlist;
using Cell = netlist::cell;
using Pin = netlist::pin;

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

