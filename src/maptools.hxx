// Fixes bug with lambdas for transform_iterator on older compilers
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/algorithm/string/predicate.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <functional>
#include <map>


namespace maptools {
    template <typename T>
    CroppedIter<T> crop_iter(typename std::map<std::string, T>::const_iterator mapiter, size_t chop) {
        return boost::make_transform_iterator(
            mapiter,
            [chop] (typename std::map<std::string, T>::value_type pair) {
                return typename std::map<std::string, T>::value_type(pair.first.substr(chop), pair.second);
            }
        );
    }

    template <typename T>
    CroppedIter<T> start_of(std::map<std::string, T> const &map, std::string directory) {
        return crop_iter<T>(
            map.lower_bound(directory + "/"),
            directory.size() + 1
        );
    }

    template <typename T>
    CroppedIter<T> end_of(std::map<std::string, T> const &map, std::string directory) {
        return crop_iter<T>(
            std::find_if(map.lower_bound(directory + "/"), std::end(map),
                [&] (typename std::map<std::string, T>::value_type pair) {
                    // The end is the first item that fails the test
                    return !boost::starts_with(pair.first, directory + "/");
                }
            ),
            directory.size() + 1
        );
    }

    template <typename T>
    CroppedIter<T> const cstart_of(std::map<std::string, T> const &map, std::string directory) {
        return start_of(map, directory);
    }

    template <typename T>
    CroppedIter<T> const cend_of(std::map<std::string, T> const &map, std::string directory) {
        return end_of(map, directory);
    }
}
