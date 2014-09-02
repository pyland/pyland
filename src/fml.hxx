#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <istream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>

static const boost::regex ignored_line(" *(?:#.*)?");
static const boost::regex start_indentation("( *).*");

static const boost::regex valid_line(
    " *"              // indentation,
    "(("              //
        "[^\\s/:]+/"  // a directory
    "|"               // or
        "[^\\s:]+:"  // a file...
        "\\s*[^\\s]+" // ...and its contents,
    ")\\s*)*"         // optionally space-delimited tokens
    "(?:#.*)?"        // and a comment if you want
);

static const boost::regex tokens(
        "([^\\s/:]+/)"     // a directory
    "|"                    // or
        "([^\\s:]+):\\s*" // a file...
        "([^\\s]+)"        // ...and its contents
);

namespace fml {
    template <typename T>
    bool from_stream(std::istream &input, std::map<std::string, T> &output) {
        // For keeping track of the current "path"
        std::list<std::pair<ssize_t, std::vector<std::string>>> stack;
        stack.push_back(std::make_pair(-1, std::vector<std::string>({""})));

        bool error(false);

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
                    output[path.str()] = boost::lexical_cast<T>((*match)[3].str());
                }
            }
        }

        return error;
    }
}
