#include "catch.hpp"

#include "fml.hpp"

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
            FML my_data(fake_file);

            THEN("the output is valid") {
                REQUIRE(my_data.valid());
            }

            THEN("strings can be accessed") {
                REQUIRE(my_data.get<std::string>("this/m/g/kz") == "lol");
                REQUIRE(my_data.get<std::string>("this/is/quite/simple/for/most") == "true");
            }

            THEN("integers can be accessed") {
                REQUIRE(my_data.get<int>("this/is/quite/simple/for/but/not/all") == 12);
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
            "    1/y:0\n"
            "        z:0\n"
        );
        std::istringstream fake_file(fake_file_contents);

        WHEN("parsed") {
            FML my_data(fake_file);

            THEN("the output is valid") {
                REQUIRE(my_data.valid());
            }

            THEN("everything exists") {
                REQUIRE(my_data.get<int>("path/1/x") == 0);
                REQUIRE(my_data.get<int>("path/1/y") == 0);
                REQUIRE(my_data.get<int>("path/1/z") == 0);
                REQUIRE(my_data.get<int>("path/2/y") == 0);
                REQUIRE(my_data.get<int>("path/2/z") == 0);
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
            FML my_data(fake_file);

            THEN("the output is not valid") {
                REQUIRE(!my_data.valid());
            }

            THEN("some things exists") {
                REQUIRE(my_data.get<int>("x") == 0);
                REQUIRE(my_data.get<int>("m") == 0);
            }
        }
    }
}

SCENARIO("FML can be iterated", "[fml][iterate]" ) {

    std::map<std::string, int> map_equivalent({
        { "path/1/x", 0 },
        { "path/1/y", 0 },
        { "path/1/z", 0 },
        { "path/2/y", 0 },
        { "path/2/z", 0 }
    });

    GIVEN("a basic configuration") {

        std::string fake_file_contents(
            "path/1/x:0\n"
            "path/2/y:0\n"
            "    z:0\n"
            "path/\n"
            "    1/y:0\n"
            "        z:0\n"
        );
        std::istringstream fake_file(fake_file_contents);

        WHEN("parsed") {
            FML my_data(fake_file);

            THEN("the output is iterable") {
                for (auto pair : my_data) {
                    REQUIRE(pair.second == "0");
                }

                for (std::pair<std::string, int> pair : my_data) {
                    REQUIRE(map_equivalent.at(pair.first) == pair.second);
                }
            }

            THEN("the output is can be accessed") {
                for (auto pair : map_equivalent) {
                    REQUIRE(my_data.get<int>(pair.first) == pair.second);
                }
            }

            THEN("the output is convertable to a map") {
                std::map<std::string, int> new_map;
                for (auto pair : my_data) { new_map.insert(pair); }
                REQUIRE(map_equivalent == new_map);
            }
        }
    }
}
