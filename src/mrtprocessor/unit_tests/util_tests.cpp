//
// Created by Rob Ballantyne on 2024-01-14.
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.hpp"

#include "../util.h"

TEST_SUITE("Utility Tests") {

    TEST_CASE("path to vector") {
        auto res1 = path_to_vector("1 2 3");
        CHECK( res1.size() == 3);
        bool ok = (res1[0] == 1) && (res1[1] == 2) && (res1[2] == 3);
        CHECK( ok );
    }

    TEST_CASE("path to vector (delim specified)") {
        auto res1 = path_to_vector("1:2:3", ':');
        CHECK( res1.size() == 3);
        bool ok = (res1[0] == 1) && (res1[1] == 2) && (res1[2] == 3);
        CHECK( ok );
    }

    TEST_CASE("str to nlri v4") {
        struct in_addr a { htonl(16909060)};
        auto expect = nlriv4(a,15);
        auto res1 = str_to_nlriv4("1.2.3.4/15");
        CHECK( res1 == expect );
    }

    TEST_CASE("str to vect nlri") {
        auto res1 = nlriv4list_to_vec("1.2.3.4/16,1.2.3.5/17,1.2.3.6/18,1.2.3.7/19");
        struct in_addr a[] { htonl(16909060), htonl(16909061), htonl(16909062), htonl(16909063)};
        int len[] { 16, 17, 18, 19 };
        std::vector<nlriv4> expecteds;
        for ( int i = 0; i < 4; i++ ) {
            expecteds.push_back(nlriv4(a[i], len[i]));
        }
        bool ok = res1[0] == expecteds[0]
                && res1[1] == expecteds[1]
                && res1[2] == expecteds[2]
                && res1[3] == expecteds[3];
        CHECK( ok );
    }

    TEST_CASE("edit distance") {
        std::vector<uint32_t> a = { 1, 2, 1, 2 };
        std::vector<uint32_t> b = { 1, 2, 1, 3 };
        std::vector<uint32_t> c = { 9, 9, 9, 9 };
        std::vector<uint32_t> d = { 1, 2, 1};
        std::vector<uint32_t> e = {};
        CHECK( edit_distance(a,a) == 0 );
        CHECK( edit_distance(a,b) == 1);
        CHECK( edit_distance(a,c) == 4);
        CHECK( edit_distance(b,c) == 4);
        CHECK( edit_distance(a,d) == 1);
        CHECK( edit_distance(c,e) == 4);
        CHECK( edit_distance(d,a) == 1);
    }

}