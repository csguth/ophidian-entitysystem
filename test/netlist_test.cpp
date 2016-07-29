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
    Netlist_With_One_Cell_Fixture() : m_cell(netlist().create<Cell>()) {

    }

    netlist::cell cell() const {
        return m_cell;
    }

};

class Netlist_With_One_Pin_Fixture : public BrandNewNetlist_Fixture {
    Pin m_pin;
public:
    Netlist_With_One_Pin_Fixture() : m_pin(netlist().create<Pin>()) {

    }

    netlist::pin pin() const {
        return m_pin;
    }

};

TEST_CASE_METHOD(BrandNewNetlist_Fixture, "netlist: creation", "[netlist]") {
    REQUIRE(netlist().size<Cell>() == 0);
    REQUIRE(netlist().size<Pin>() == 0);
}

//TEST_CASE_METHOD(Netlist_With_One_Cell_Fixture, "netlist: create cell", "[netlist]") {
//    REQUIRE(netlist().valid(cell()));
//    REQUIRE(netlist().size<Cell>() == 1);
//}

//TEST_CASE_METHOD(Netlist_With_One_Cell_Fixture, "netlist: destroy cell", "[netlist]") {
//    netlist().cell_destroy(cell());
//    REQUIRE(netlist().size<Cell>() == 0);
//}


//TEST_CASE_METHOD(Netlist_With_One_Pin_Fixture, "netlist: create pin", "[netlist]") {
//    REQUIRE(netlist().valid(pin()));
//    REQUIRE(netlist().size<Pin>() == 1);
//}

//TEST_CASE_METHOD(Netlist_With_One_Pin_Fixture, "netlist: destroy pin", "[netlist]") {
//    netlist().pin_destroy(pin());
//    REQUIRE(netlist().size<Pin>() == 0);
//}




