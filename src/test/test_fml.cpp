#include <algorithm>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <utility>

#include "catch.hpp"
#include "fml.hpp"
#include <iostream>
SCENARIO("FML can parse basic linear structure", "[fml][parse]" ) {

    GIVEN("a basic configuration") {

        std::string fake_file_contents(
            "this/\n"
            "    is/quite/simple/\n"
            "    \n"
            "        for/most: true but/not/all: 12\n"
            "    m/g/\n"
            "# Hyar we go...\n"
            "        kz:lol mbar: 123ttx___\n"
            "and/some/more: 7\n"
        );
        std::istringstream fake_file(fake_file_contents);

        WHEN("parsed") {
            std::map<std::string, std::string> my_data;
            bool error(fml::from_stream(fake_file, my_data));

            THEN("the output is error") {
                REQUIRE(!error);
            }

            THEN("strings can be accessed") {
                REQUIRE(my_data.at("this/m/g/kz") == "lol");
                REQUIRE(my_data.at("this/is/quite/simple/for/most") == "true");
            }
        }
    }
}

SCENARIO("FML can parse duplicated structure", "[fml][parse]" ) {

    GIVEN("configuration with duplicated paths") {

        std::string fake_file_contents(
            "path/1/x:0\n"
            "path/2/y:0\n"
            "    z:0\n"
            "path/\n"
            "    1/y:1\n"
            "        z:0\n"
        );
        std::istringstream fake_file(fake_file_contents);

        WHEN("parsed") {
            std::map<std::string, int> my_data;
            bool error(fml::from_stream(fake_file, my_data));

            THEN("the output is error") {
                REQUIRE(!error);
            }

            THEN("everything exists") {
                REQUIRE(my_data.at("path/1/x") == 0);
                REQUIRE(my_data.at("path/1/y") == 1);
                REQUIRE(my_data.at("path/1/z") == 0);
                REQUIRE(my_data.at("path/2/y") == 0);
                REQUIRE(my_data.at("path/2/z") == 0);
            }
        }
    }
}


SCENARIO("FML can handle somewhat broken configuration", "[fml][error]" ) {

    GIVEN("configuration with broken syntax") {

        std::string fake_file_contents(
            "x:0\n"
            "sdaf adsf m:0\n"
        );
        std::istringstream fake_file(fake_file_contents);

        WHEN("parsed") {
            std::map<std::string, int> my_data;
            bool error(fml::from_stream(fake_file, my_data));

            THEN("the output is not error") {
                REQUIRE(error);
            }

            THEN("some things exists") {
                REQUIRE(my_data.at("x") == 0);
                REQUIRE(my_data.at("m") == 0);
            }
        }
    }
}
