#include <catch.hpp>
#include <ophidian/netlist/netlist.h>
#include <memory>

using namespace ophidian;

using Netlist = netlist::netlist;

class BrandNewNetlist_Fixture {
    Netlist m_netlist;

public:
    const Netlist & netlist() const {
        return m_netlist;
    }
};

TEST_CASE_METHOD(BrandNewNetlist_Fixture, "netlist: creation", "[netlist]") {
//    REQUIRE(netlist().cells_size() == 0);
//    REQUIRE(netlist().pins_size() == 0);
//    REQUIRE(netlist().nets_size() == 0);
}



