//
// Created by Chong Peng on 10/19/15.
//

#include <catch.hpp>

#include "../expression/orbital_index.h"

TEST_CASE("Orbital Index", "[orbital_index]"){

    SECTION("single letter case"){
        mpqc::OrbitalIndex i("i");
        REQUIRE( i.index() == mpqc::OrbitalIndex::Index::occ);

        mpqc::OrbitalIndex l("i");
        REQUIRE( l.index() == mpqc::OrbitalIndex::Index::occ);

        mpqc::OrbitalIndex a("a");
        REQUIRE( a.index() == mpqc::OrbitalIndex::Index::virt);

        mpqc::OrbitalIndex d("d");
        REQUIRE( d.index() == mpqc::OrbitalIndex::Index::virt);

        mpqc::OrbitalIndex p("p");
        REQUIRE( p.index() == mpqc::OrbitalIndex::Index::any);

        mpqc::OrbitalIndex s("p");
        REQUIRE( s.index() == mpqc::OrbitalIndex::Index::any);
    }

    SECTION("letter with number"){

        mpqc::OrbitalIndex i("i1");
        REQUIRE( i.index() == mpqc::OrbitalIndex::Index::occ);

        mpqc::OrbitalIndex l("l2");
        REQUIRE( l.index() == mpqc::OrbitalIndex::Index::occ);

        mpqc::OrbitalIndex a("a4");
        REQUIRE( a.index() == mpqc::OrbitalIndex::Index::virt);

        mpqc::OrbitalIndex d("d5");
        REQUIRE( d.index() == mpqc::OrbitalIndex::Index::virt);

        mpqc::OrbitalIndex p("p6");
        REQUIRE( p.index() == mpqc::OrbitalIndex::Index::any);

        mpqc::OrbitalIndex s("s7");
        REQUIRE( s.index() == mpqc::OrbitalIndex::Index::any);
    }

    SECTION("letter with prime"){
        mpqc::OrbitalIndex P("P'");
        REQUIRE( P.index() == mpqc::OrbitalIndex::Index::allany);

        mpqc::OrbitalIndex S("S'");
        REQUIRE( S.index() == mpqc::OrbitalIndex::Index::allany);

        mpqc::OrbitalIndex a("a'");
        REQUIRE( a.index() == mpqc::OrbitalIndex::Index::othervirt);

        mpqc::OrbitalIndex d("d'");
        REQUIRE( d.index() == mpqc::OrbitalIndex::Index::othervirt);

        mpqc::OrbitalIndex A("A'");
        REQUIRE( A.index() == mpqc::OrbitalIndex::Index::allvirt);

        mpqc::OrbitalIndex D("D'");
        REQUIRE( D.index() == mpqc::OrbitalIndex::Index::allvirt);
    }


    SECTION("letter with number and prime"){
        mpqc::OrbitalIndex P("P1'");
        REQUIRE( P.index() == mpqc::OrbitalIndex::Index::allany);

        mpqc::OrbitalIndex S("S2'");
        REQUIRE( S.index() == mpqc::OrbitalIndex::Index::allany);

        mpqc::OrbitalIndex a("a4'");
        REQUIRE( a.index() == mpqc::OrbitalIndex::Index::othervirt);

        mpqc::OrbitalIndex d("d5'");
        REQUIRE( d.index() == mpqc::OrbitalIndex::Index::othervirt);

        mpqc::OrbitalIndex A("A6'");
        REQUIRE( A.index() == mpqc::OrbitalIndex::Index::allvirt);

        mpqc::OrbitalIndex D("D7'");
        REQUIRE( D.index() == mpqc::OrbitalIndex::Index::allvirt);
    }

    SECTION("error handling"){
        // key not allowed
        REQUIRE_THROWS(mpqc::OrbitalIndex("m"));
        REQUIRE_THROWS(mpqc::OrbitalIndex("I"));
        REQUIRE_THROWS(mpqc::OrbitalIndex("I'"));
        REQUIRE_THROWS(mpqc::OrbitalIndex("I1"));

        //wrong length
        REQUIRE_THROWS(mpqc::OrbitalIndex("i123"));

        // wrong format
        REQUIRE_THROWS(mpqc::OrbitalIndex("a13"));
        REQUIRE_THROWS(mpqc::OrbitalIndex("a'4"));
    }

}