#include <boost/regex.hpp>
#include <iostream>
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
        std::list<std::pair<int, std::vector<std::string>>> stack;
        stack.push_back(std::make_pair(-1, std::vector<std::string>({""})));

        // For each line
        std::string line;
        while (std::getline(input, line)) {
            // Ignore blank, indentation-only and comment-only lines
            if (boost::regex_match(line, ignored_line)) { continue; }

            // And check if the line is valid
            error |= boost::regex_match(line, valid_line);

            // If not, still continue to see what can be understood from the file,
            // as the tokens are still valid; it's just some text is ignored

            // Get the indentation
            boost::smatch indents;
            boost::regex_match(line, indents, start_indentation);
            int indentation(indents[1].str().size());

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

template <>
std::string FML::get<std::string>(std::string where) {
    return values->at(where);
}

template <>
int FML::get<int>(std::string where) {
    return std::stoi(this->get<std::string>(where));
}

std::map<std::string, std::string> export() {
    return *values;
}

SCENARIO("FML can parse basic linear structure", "[fml][parse]" ) {

    GIVEN( "A basic configuration" ) {

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