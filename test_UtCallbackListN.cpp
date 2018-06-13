
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_CallbackList

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <cmath>
#include "UtCallbackListN.hpp"

BOOST_AUTO_TEST_SUITE(CallbackListN_Tests)

    BOOST_AUTO_TEST_CASE(test_global_connect) {
        CallbackListN<void(int), int> list;

        CallbackHolder holder;
        int changedVal = 0;
        holder.emplace(list.connect([&](int aVal) { changedVal = aVal; }));
        BOOST_REQUIRE(!list.Empty());
        for (int changeTo:{1, 7, 90, -2}) {
            list(changeTo);
            BOOST_CHECK_EQUAL(changedVal, changeTo);
        }

        holder.clear();
        BOOST_CHECK(list.Empty());
    }

    BOOST_AUTO_TEST_CASE(test_enum_connect) {
        CallbackListN<void(int), char> list;
        CallbackHolder holder;
        int a = 0;
        int b = 0;

        holder.emplace(list['a'].connect([&](int aVal) { a = aVal; }));
        holder.emplace(list['b'].connect([&](int aVal) { b = aVal; }));
        BOOST_REQUIRE(!list.Empty());

        list['a'](2);
        BOOST_CHECK_EQUAL(a, 2);
        BOOST_CHECK_EQUAL(b, 0); // no change to b

        list['b'](-9);
        BOOST_CHECK_EQUAL(a, 2); //no change to a
        BOOST_CHECK_EQUAL(b, -9);

        list(10); // no enumeration provided means calling all enumeration lists
        BOOST_CHECK_EQUAL(a, 2);
        BOOST_CHECK_EQUAL(b, -9);

        list['c'](100); // 'c' has nothing connected to it
        BOOST_CHECK_EQUAL(a, 2);
        BOOST_CHECK_EQUAL(b, -9);

        holder.clear();
        BOOST_CHECK(list.Empty());
    }

    BOOST_AUTO_TEST_CASE(test_enum_calling_global){
        CallbackListN<void(int), char> list;
        CallbackHolder holder;
        int a = 0;
        int b = 0;
        int glob = 0;

        holder.emplace(list['a'].connect([&](int aVal) { a = aVal; }));
        holder.emplace(list['b'].connect([&](int aVal) { b = aVal; }));
        holder.emplace(list.connect([&](int aVal) { glob = aVal; }));
        BOOST_REQUIRE(!list.Empty());

        list['a'](2);
        BOOST_CHECK_EQUAL(a, 2);
        BOOST_CHECK_EQUAL(b, 0); // no change to b
        BOOST_CHECK_EQUAL(glob, 2);

        list['b'](-9);
        BOOST_CHECK_EQUAL(a, 2); //no change to a
        BOOST_CHECK_EQUAL(b, -9);
        BOOST_CHECK_EQUAL(glob, -9);

        list(10); // no enumeration provided means calling only globally connected callbacks
        BOOST_CHECK_EQUAL(a, 2);
        BOOST_CHECK_EQUAL(b, -9);
        BOOST_CHECK_EQUAL(glob, 10);

        list['c'](100); // 'c' has nothing connected to it
        BOOST_CHECK_EQUAL(a, 2);
        BOOST_CHECK_EQUAL(b, -9);
        BOOST_CHECK_EQUAL(glob, 100);

        holder.clear();
        BOOST_CHECK(list.Empty());
    }

BOOST_AUTO_TEST_SUITE_END()