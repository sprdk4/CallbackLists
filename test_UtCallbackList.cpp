#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_CallbackList

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <cmath>
#include "UtCallbackList.hpp"

void asFunctionPtr(char a, char* b) { *b = a; }

BOOST_AUTO_TEST_SUITE(CallbackListTests)

    BOOST_AUTO_TEST_CASE(test_member_func_connect) {
        struct meow {
            int calledNum = 0;

            bool memFunc(int i) {
                calledNum = i;
                return false;
            }
        };

        meow a;
        CallbackList<bool(int dtype)> list;
        CallbackHolder holder;
        holder.emplace(list.connect(&a, &meow::memFunc));
        BOOST_REQUIRE(!list.Empty());
        BOOST_REQUIRE(!holder.empty());

        //test calling member functors
        for (int numCalled:{1, 6, -60, 10063, -79824, 0}) {
            list(numCalled);
            BOOST_CHECK_EQUAL(a.calledNum, numCalled);
        }
    }

    BOOST_AUTO_TEST_CASE(test_free_function_connect) {
        CallbackHolder holder;
        CallbackList<void(char, char*)> list{};
        holder.emplace(list.connect(asFunctionPtr));
        char changedChar;
        for (char charToChange:{'a', 'd', '0', 'Z', '?'}) {
            list(charToChange, &changedChar);
            BOOST_CHECK_EQUAL(changedChar, charToChange);
        }

    }

    BOOST_AUTO_TEST_CASE(test_lambda_connect) {
        CallbackHolder holder;
        CallbackList<void(std::string)> list{};
        std::string stringToChange;

        holder.emplace(list.connect([&](const std::string& a) { stringToChange = a; }));

        for (const std::string& str:{"qwerty", "easter", "", "kitty cat", "purple"}) {
            list(str); //called the callback list with the str.
            BOOST_CHECK_EQUAL(str, stringToChange);
        }

    }

    BOOST_AUTO_TEST_CASE(test_stdFunction_connect) {
        CallbackHolder holder;
        CallbackList<void(std::string)> list{};
        std::string stringToChange;

        std::function<void(const std::string&)> func = [&](const std::string& a) { stringToChange = a; };
        holder.emplace(list.connect(func));

        for (const std::string& str:{"qwerty", "easter", "", "kitty cat", "purple"}) {
            list(str); //called the callback list with the str.
            BOOST_CHECK_EQUAL(str, stringToChange);
        }

    }
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(CallbackHolderTests)

    BOOST_AUTO_TEST_CASE(test_callback_lifetime /* ,*boost::unit_test::depends_on("CallbackListTests/test_free_function_connect")*/) {
        CallbackList<void(char, char*)> list{};
        char changedChar;
        {
            CallbackHolder holder;
            holder.emplace(list.connect(asFunctionPtr));
            BOOST_REQUIRE(!list.Empty());
            BOOST_REQUIRE(!holder.empty());

            list('_', &changedChar);
            BOOST_CHECK_EQUAL('_', changedChar);

            list('Q', &changedChar);
            BOOST_CHECK_EQUAL('Q', changedChar);
        }

        list('D', &changedChar);
        BOOST_CHECK_NE('D', changedChar);
        BOOST_CHECK_EQUAL('Q', changedChar);
    }

    BOOST_AUTO_TEST_CASE(test_multiple_callbacks_lifetime) {
        int topInt = 0;
        int midInt = 0;
        int bottomInt = 0;

        auto checkTopMidBottom = [&](int top, int mid, int bottom) {
            BOOST_CHECK_EQUAL(topInt, top);
            BOOST_CHECK_EQUAL(midInt, mid);
            BOOST_CHECK_EQUAL(bottomInt, bottom);
        };
        CallbackList<void(int)> list{};
        {
            CallbackHolder topHolder;
            topHolder.emplace(list.connect([&](int a) { topInt = a; }));
            list(1);
            checkTopMidBottom(1, 0, 0);
            {
                CallbackHolder midHolder;
                midHolder.emplace(list.connect([&](int a) { midInt = a; }));
                list(4);
                checkTopMidBottom(4, 4, 0);
                {
                    CallbackHolder bottomHolder;
                    bottomHolder.emplace(list.connect([&](int a) { bottomInt = a; }));
                    list(-5);
                    checkTopMidBottom(-5, -5, -5);
                }
                list(100);
                checkTopMidBottom(100, 100, -5);
            }
            list(9);
            checkTopMidBottom(9, 100, -5);

            {
                CallbackHolder midHolder;
                midHolder.emplace(list.connect([&](int a) { midInt = a; }));
                list(5);
                checkTopMidBottom(5, 5, -5);
                {
                    CallbackHolder bottomHolder;
                    bottomHolder.emplace(list.connect([&](int a) { bottomInt = a; }));
                    list(16);
                    checkTopMidBottom(16, 16, 16);
                }
                list(12);
                checkTopMidBottom(12, 12, 16);
            }
            list(1);
            checkTopMidBottom(1, 12, 16);
        }
        BOOST_CHECK(list.Empty());
        list(2);
        checkTopMidBottom(1, 12, 16);
    }

BOOST_AUTO_TEST_SUITE_END()


