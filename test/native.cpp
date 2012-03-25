
// Copyright (C) 2006-2009, 2012 Alexander Nasonov
// Copyright (C) 2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.boost.org/LICENSE_1_0.txt)
// Home at http://www.boost.org/libs/scope_exit

#include <boost/scope_exit.hpp>
#include <boost/config.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/typeof/std/string.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <ostream>
#include <string>

using namespace boost::unit_test;

std::string g_str;

template<int Dummy = 0>
struct Holder {
    static long g_long;
};

template<int Dummy> long Holder<Dummy>::g_long;

void test_non_local(void) {
    // ... and one local variable as well:
    int i = 0;

    BOOST_SCOPE_EXIT(void) {
        BOOST_CHECK(Holder<>::g_long == 3);
    } BOOST_SCOPE_EXIT_END

    BOOST_SCOPE_EXIT( (i) ) {
        BOOST_CHECK(i == 0);
        BOOST_CHECK(Holder<>::g_long == 3);
        BOOST_CHECK(g_str == "try: g_str");
    } BOOST_SCOPE_EXIT_END

    BOOST_SCOPE_EXIT( (&i) ) {
        BOOST_CHECK(i == 3);
        BOOST_CHECK(Holder<>::g_long == 3);
        BOOST_CHECK(g_str == "try: g_str");
    } BOOST_SCOPE_EXIT_END

    {
        g_str = "";
        Holder<>::g_long = 1;

        BOOST_SCOPE_EXIT( (&i) ) {
            i = 1;
            g_str = "g_str";
        } BOOST_SCOPE_EXIT_END

        BOOST_SCOPE_EXIT( (&i) ) {
            try {
                i = 2;
                Holder<>::g_long = 2;
                throw 0;
            } catch(...) {}
        } BOOST_SCOPE_EXIT_END

        BOOST_CHECK(i == 0);
        BOOST_CHECK(g_str == "");
        BOOST_CHECK(Holder<>::g_long == 1);
    }

    BOOST_CHECK(Holder<>::g_long == 2);
    BOOST_CHECK(g_str == "g_str");
    BOOST_CHECK(i == 1); // Check that first declared is executed last.

    BOOST_SCOPE_EXIT( (&i) ) {
        BOOST_CHECK(i == 3);
        BOOST_CHECK(Holder<>::g_long == 3);
        BOOST_CHECK(g_str == "try: g_str");
    } BOOST_SCOPE_EXIT_END

    BOOST_SCOPE_EXIT( (i) ) {
        BOOST_CHECK(i == 1);
        BOOST_CHECK(Holder<>::g_long == 3);
        BOOST_CHECK(g_str == "try: g_str");
    } BOOST_SCOPE_EXIT_END

    try {
        BOOST_SCOPE_EXIT( (&i) ) {
            i = 3;
            g_str = "try: g_str";
        } BOOST_SCOPE_EXIT_END
        
        BOOST_SCOPE_EXIT( (&i) ) {
            i = 4;
            Holder<>::g_long = 3;
        } BOOST_SCOPE_EXIT_END

        BOOST_CHECK(i == 1);
        BOOST_CHECK(g_str == "g_str");
        BOOST_CHECK(Holder<>::g_long == 2);

        throw 0;
    } catch(int) {
        BOOST_CHECK(Holder<>::g_long == 3);
        BOOST_CHECK(g_str == "try: g_str");
        BOOST_CHECK(i == 3); // Check that first declared is executed last.
    }
}

bool foo(void) { return true; }

bool foo2(void) { return false; }

void test_types(void) {
    bool (*pf)(void) = 0;
    bool (&rf)(void) = foo;
    bool results[2] = {};

    {
        BOOST_SCOPE_EXIT( (&results) (&pf) (&rf) ) {
            results[0] = pf();
            results[1] = rf();
        }
        BOOST_SCOPE_EXIT_END

        pf = &foo;

        BOOST_CHECK(results[0] == false);
        BOOST_CHECK(results[1] == false);
    }

    BOOST_CHECK(results[0] == true);
    BOOST_CHECK(results[1] == true);

    {
        BOOST_SCOPE_EXIT( (&results) (pf) ) {
            results[0] = !pf();
            results[1] = !pf();
            pf = &foo2; // modify a copy
        }
        BOOST_SCOPE_EXIT_END

        pf = 0;

        BOOST_CHECK(results[0] == true);
        BOOST_CHECK(results[1] == true);
    }

    BOOST_CHECK(pf == 0);
    BOOST_CHECK(results[0] == false);
    BOOST_CHECK(results[1] == false);
}

void test_cpp11(void) {
#ifndef BOOST_NO_LAMBDAS
    int i = 0, j = 1;

    {
        BOOST_SCOPE_EXIT_ALL(=) {
            i = j = 1; // modify copies
        };
    }
    BOOST_CHECK(i == 0);
    BOOST_CHECK(j == 1);

    {
        BOOST_SCOPE_EXIT_ALL(&) {
            i = 1;
            j = 2;
        };
        BOOST_CHECK(i == 0);
        BOOST_CHECK(j == 1);
    }
    BOOST_CHECK(i == 1);
    BOOST_CHECK(j == 2);

    {
        BOOST_SCOPE_EXIT_ALL(=, &j) {
            i = 2; // modify a copy
            j = 3;
        };
        BOOST_CHECK(i == 1);
        BOOST_CHECK(j == 2);
    }
    BOOST_CHECK(i == 1);
    BOOST_CHECK(j == 3);
#endif // lambdas
}

test_suite* init_unit_test_suite(int, char* []) {
    framework::master_test_suite().p_name.value = "Unit test for ScopeExit";
    framework::master_test_suite().add(BOOST_TEST_CASE(&test_non_local));
    framework::master_test_suite().add(BOOST_TEST_CASE(&test_types));
    framework::master_test_suite().add(BOOST_TEST_CASE(&test_cpp11));
    return 0;
}

