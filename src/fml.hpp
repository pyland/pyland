#ifndef FML_H
#define FML_H

#include <istream>
#include <map>
#include <string>

namespace fml {
    template <typename T>
    bool from_stream(std::istream &input, std::map<std::string, T> &output);
}

#include "fml.hxx"

#endif
