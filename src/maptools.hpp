#ifndef MAPTOOLS_H
#define MAPTOOLS_H

// Fixes bug with lambdas for transform_iterator on older compilers
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/iterator/transform_iterator.hpp>
#include <string>

namespace maptools {
    template <typename T>
    using CroppedIter = boost::transform_iterator<
        std::function<typename std::map<std::string, T>::value_type (typename std::map<std::string, T>::value_type)>,
        typename std::map<std::string, T>::const_iterator
    >;

    template <typename T>
    CroppedIter<T> start_of(std::map<std::string, T> const &map, std::string directory="");
    template <typename T>
    CroppedIter<T> end_of(std::map<std::string, T> const &map, std::string directory="");

    template <typename T>
    CroppedIter<T> const cstart_of(std::map<std::string, T> const &map, std::string directory="");
    template <typename T>
    CroppedIter<T> const cend_of(std::map<std::string, T> const &map, std::string directory="");
}

#include "maptools.hxx"

#endif
