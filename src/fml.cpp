#include <algorithm>
#include <boost/regex.hpp>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "fml.hpp"

static const boost::regex ignored_line(" *(?:#.*)?");
static const boost::regex start_indentation("( *).*");

static const boost::regex valid_line(
    " *"              // indentation,
    "(("              //
        "[^\\s/:]+/"  // a directory
    "|"               // or
        "[^\\s/:]+:"  // a file...
        "\\s*[^\\s]+" // ...and its contents,
    ")\\s*)*"         // optionally space-delimited tokens
    "(?:#.*)?"        // and a comment if you want
);

static const boost::regex tokens(
        "([^\\s/:]+/)"     // a directory
    "|"                    // or
        "([^\\s/:]+):\\s*" // a file...
        "([^\\s]+)"        // ...and its contents
);


FML::FML(std::istream &input):
    values(std::make_shared<std::map<std::string, std::string>>()),
    error(false) {

        // For keeping track of the current "path"
        std::list<std::pair<ssize_t, std::vector<std::string>>> stack;
        stack.push_back(std::make_pair(-1, std::vector<std::string>({""})));

        // For each line
        std::string line;
        while (std::getline(input, line)) {
            // Ignore blank, indentation-only and comment-only lines
            if (boost::regex_match(line, ignored_line)) { continue; }

            // And check if the line is valid
            error |= !boost::regex_match(line, valid_line);

            // If not, still continue to see what can be understood from the file,
            // as the tokens are still valid; it's just some text is ignored

            // Get the indentation
            boost::smatch indents;
            boost::regex_match(line, indents, start_indentation);
            ssize_t indentation(ssize_t(indents[1].str().size()));

            // Reset the stack and add a fresh level
            while (indentation <= stack.back().first) { stack.pop_back(); }
            stack.push_back(std::make_pair(indentation, stack.back().second));

            // Get the valid tokens
            boost::sregex_iterator begin(std::begin(line), std::end(line), tokens), end;
            for (auto match=begin; match != end; ++match) {
                // If it's a directory, add it to this stack
                if ((*match)[1].matched) {
                    stack.back().second.push_back((*match)[1].str());
                }
                // If it's a file,
                else {
                    // Generate the path we're adding the contents to...
                    std::stringstream path;
                    copy(std::begin(stack.back().second),
                         std::end(  stack.back().second),
                         std::ostream_iterator<std::string>(path));

                    // ...including the filename
                    path << (*match)[2].str();

                    // And push the file
                    (*values)[path.str()] = (*match)[3].str();
                }
            }
        }
}

std::map<std::string, std::string> FML::as_map() {
    return *values;
}

bool FML::valid() {
    return !error;
}


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

    std::map<std::string, std::string> map_equivalent({
        { "path/1/x", "0" },
        { "path/1/y", "0" },
        { "path/1/z", "0" },
        { "path/2/y", "0" },
        { "path/2/z", "0" }
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

                for (const auto pair : my_data) {
                    REQUIRE(pair.second == "0");
                }

                std::equal(my_data.cbegin(), my_data.cend(), map_equivalent.cbegin());

            }

            THEN("the output is exportable") {

                REQUIRE(my_data.as_map() == map_equivalent);

            }
        }
    }
}
