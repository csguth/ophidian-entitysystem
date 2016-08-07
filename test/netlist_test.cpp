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

using PI = netlist::primary_input;

TEST_CASE("netlist: PI", "[netlist]")
{
    netlist::netlist nl;

    REQUIRE( nl.size<PI>() == 0 );

    Pin inp1 = nl.create<Pin>();
    Pin u1_a = nl.create<Pin>();

    REQUIRE( !nl.is_PI(inp1) );
    REQUIRE( !nl.is_PI(u1_a) );

    PI inp1_PI = nl.make_PI(inp1);

    REQUIRE( nl.pin_of(inp1_PI) == inp1 );
    REQUIRE( nl.valid(inp1_PI) );
    REQUIRE( nl.is_PI(inp1) );
    REQUIRE( !nl.is_PI(u1_a) );
    REQUIRE( nl.size<PI>() == 1 );
    REQUIRE( nl.PI_of(inp1) == inp1_PI );

    nl.destroy(inp1_PI);

    REQUIRE( !nl.is_PI(inp1) );
    REQUIRE( nl.size<PI>() == 0 );
    REQUIRE( !nl.valid(inp1_PI) );

    REQUIRE( static_cast<entity_system::entity_system::entity>(nl.PI_of(inp1)) == NULL_entity );
    inp1_PI = nl.make_PI(inp1);

    nl.destroy(inp1);

    REQUIRE( !nl.valid(inp1) );
    REQUIRE( !nl.valid(inp1_PI) );

}


TEST_CASE("netlist: property for PIs, like input delay", "[netlist]")
{
    netlist::netlist nl;
    Pin inp1, inp2, u1_a;
    PI Inp1, Inp2;

    Inp1 = nl.make_PI(inp1 = nl.create<Pin>());
    Inp2 = nl.make_PI(inp2 = nl.create<Pin>());
    u1_a = nl.create<Pin>();

    auto input_delays = nl.make_property<PI, double>();

    input_delays[Inp1] = 0.0;
    input_delays[Inp2] = 2.0;

    REQUIRE( Approx(input_delays[Inp1]) == 0.0 );
    REQUIRE( Approx(input_delays[Inp2]) == 2.0 );

}


using PO = netlist::primary_output;

TEST_CASE("netlist: PO", "[netlist]")
{
    netlist::netlist nl;

    REQUIRE( nl.size<PO>() == 0 );

    Pin out1 = nl.create<Pin>();
    Pin u1_a = nl.create<Pin>();

    REQUIRE( !nl.is_PO(out1) );
    REQUIRE( !nl.is_PO(u1_a) );

    PO out1_PO = nl.make_PO(out1);

    REQUIRE( nl.pin_of(out1_PO) == out1 );
    REQUIRE( nl.valid(out1_PO) );
    REQUIRE( nl.is_PO(out1) );
    REQUIRE( !nl.is_PO(u1_a) );
    REQUIRE( nl.size<PO>() == 1 );
    REQUIRE( nl.PO_of(out1) == out1_PO );

    nl.destroy(out1_PO);

    REQUIRE( !nl.is_PO(out1) );
    REQUIRE( nl.size<PO>() == 0 );
    REQUIRE( !nl.valid(out1_PO) );

    REQUIRE( static_cast<entity_system::entity_system::entity>(nl.PO_of(out1)) == NULL_entity );
    out1_PO = nl.make_PO(out1);

    nl.destroy(out1);

    REQUIRE( !nl.valid(out1) );
    REQUIRE( !nl.valid(out1_PO) );

}

TEST_CASE("netlist: property for POs, like output loads", "[netlist]")
{
    netlist::netlist nl;
    Pin out1, out2, u1_a;
    PO Out1, Out2;

    Out1 = nl.make_PO(out1 = nl.create<Pin>());
    Out2 = nl.make_PO(out2 = nl.create<Pin>());
    u1_a = nl.create<Pin>();

    auto output_loads = nl.make_property<PO, double>();

    output_loads[Out1] = 0.0;
    output_loads[Out2] = 2.0;

    REQUIRE( Approx(output_loads[Out1]) == 0.0 );
    REQUIRE( Approx(output_loads[Out2]) == 2.0 );

}

TEST_CASE("netlist: make_PO from FF:d pin", "[netlist]")
{
    netlist::netlist nl;
    Cell f1 = nl.create<Cell>();
    Pin f1d, f1q, f1ck;
    nl.attach(f1, f1d = nl.create<Pin>());
    nl.attach(f1, f1q = nl.create<Pin>());
    nl.attach(f1, f1ck = nl.create<Pin>());

    Pin out = nl.create<Pin>();

    auto PO_slacks = nl.make_property<PO, double>();

    auto f1d_PO = nl.make_PO(f1d);
    auto out_PO = nl.make_PO(out);

    PO_slacks[f1d_PO] = -1.0;
    PO_slacks[out_PO] = -2.0;

    double TNS = std::accumulate(PO_slacks.begin(), PO_slacks.end(), 0.0);

    REQUIRE( Approx(TNS) == -3.0 );

}

TEST_CASE("netlist: bounds", "[netlist]")
{
    netlist::netlist nl; // create netlist
    Cell u1;
    Pin inp1, inp2, u1a, u1b, u1o, out;
    Net inp1_net, inp2_net, out_net;

    u1 = nl.create<Cell>(); // create cell u1
    nl.attach(u1, u1a = nl.create<Pin>()); // create pin u1:a, and attach it to cell u1
    nl.attach(u1, u1b = nl.create<Pin>()); // create pin u1:b, and attach it to cell u1
    nl.attach(u1, u1o = nl.create<Pin>()); // create pin u1:o, and attach it to cell u1

    nl.make_PI(inp1 = nl.create<Pin>()); // make inp1 a PI
    nl.make_PI(inp2 = nl.create<Pin>()); // make inp2 a PI
    nl.make_PO(out = nl.create<Pin>()); // make out a PO

    nl.connect(inp1_net = nl.create<Net>(), inp1); // create net inp1 and connect pin inp1 to it
    nl.connect(inp2_net = nl.create<Net>(), inp2); // create net inp2 and connect pin inp2 to it
    nl.connect(out_net = nl.create<Net>(), u1o); // create net out and connect pin out to it

    nl.connect(inp1_net, u1a); // connect pin u1:a to net inp1
    nl.connect(inp2_net, u1b); // connect pin u1:b to net inp2
    nl.connect(out_net, u1o); // connect pin u1:o to net out

    REQUIRE( nl.size<PI>() == 2 );
    REQUIRE( nl.size<PO>() == 1 );
    REQUIRE( nl.size<Cell>() == 1 );
    REQUIRE( nl.size<Net>() == 3 );
    REQUIRE( nl.size<Pin>() == 2/*PI*/ + 1/*PO*/ + 3/*cell u1*/ );

    REQUIRE( std::count(nl.bounds<PI>().begin(), nl.bounds<PI>().end(), nl.PI_of(inp1)) == 1 );
    REQUIRE( std::count(nl.bounds<PI>().begin(), nl.bounds<PI>().end(), nl.PI_of(inp2)) == 1 );
    REQUIRE( std::count(nl.bounds<PO>().begin(), nl.bounds<PO>().end(), nl.PO_of(out)) == 1 );
    REQUIRE( std::count(nl.bounds<Net>().begin(), nl.bounds<Net>().end(), inp1_net) == 1 );
    REQUIRE( std::count(nl.bounds<Net>().begin(), nl.bounds<Net>().end(), inp2_net) == 1 );
    REQUIRE( std::count(nl.bounds<Net>().begin(), nl.bounds<Net>().end(), out_net) == 1 );
    REQUIRE( std::count(nl.bounds<Cell>().begin(), nl.bounds<Cell>().end(), u1) == 1 );
    REQUIRE( std::count(nl.bounds<Pin>().begin(), nl.bounds<Pin>().end(), inp1) == 1 );
    REQUIRE( std::count(nl.bounds<Pin>().begin(), nl.bounds<Pin>().end(), inp2) == 1 );
    REQUIRE( std::count(nl.bounds<Pin>().begin(), nl.bounds<Pin>().end(), u1a) == 1 );
    REQUIRE( std::count(nl.bounds<Pin>().begin(), nl.bounds<Pin>().end(), u1b) == 1 );
    REQUIRE( std::count(nl.bounds<Pin>().begin(), nl.bounds<Pin>().end(), u1o) == 1 );
    REQUIRE( std::count(nl.bounds<Pin>().begin(), nl.bounds<Pin>().end(), out) == 1 );

}
