#include <algorithm>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <istream>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

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

FML FML::unsafe_from_map(std::map<std::string, std::string> &mapping) { return FML(mapping); }
FML::FML(std::map<std::string, std::string> &mapping):
    values(std::make_shared<StringMap>(mapping)),
    error(false)
    {}

bool FML::valid() {
    return !error;
}

// Iterator is always const
FML::const_iterator FML::begin () const { return make_iter(values->cbegin()); }
FML::const_iterator FML::cbegin() const { return make_iter(values->cbegin()); }
FML::const_iterator FML::end   () const { return make_iter(values->cend  ()); }
FML::const_iterator FML::cend  () const { return make_iter(values->cend  ()); }

FML::const_iterator FML::begin(std::string directory) const {
    return make_iter(
        values->lower_bound(directory + "/"),
        directory.size() + 1
    );
}

FML::const_iterator FML::end(std::string directory) const {
    return make_iter(
        std::find_if(values->lower_bound(directory + "/"), std::end(*values),
            [&] (StringMap::value_type pair) {
                // The end is the first item that fails the test
                return !boost::starts_with(pair.first, directory + "/");
            }
        ),
        directory.size() + 1
    );
}

FML::const_iterator FML::make_iter(StringMap::const_iterator mapiter, size_t chop) const {
    return boost::make_transform_iterator(
        mapiter,
        [chop] (StringMap::value_type pair) {
            return CastablePair(std::make_pair(pair.first.substr(chop), pair.second));
        }
    );
}
