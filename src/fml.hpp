#ifndef FML_H
#define FML_H

#include <boost/lexical_cast.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

class FML {
    private:
        using StringMap = std::map<std::string, std::string>;

        class CastablePair: public std::remove_const<StringMap::value_type>::type {
            public:
                CastablePair(std::remove_const<StringMap::value_type>::type other):
                    std::remove_const<StringMap::value_type>::type(other) {}

                template <typename S, typename T>
                operator const std::pair<S, T>() {
                    return std::make_pair(first, boost::lexical_cast<T>(second));
                }
        };

    public:
        using const_iterator = boost::transform_iterator<
            std::function<CastablePair (StringMap::value_type)>,
            StringMap::const_iterator
        >;

        explicit FML(std::istream &input);

        bool valid();

        template <typename T=std::string>
        T get(std::string where) { return boost::lexical_cast<T>(values->at(where)); }

        // Iterator is always const
        const_iterator begin () const { return make_iter(values->cbegin()); }
        const_iterator cbegin() const { return make_iter(values->cbegin()); }
        const_iterator end   () const { return make_iter(values->cend  ()); }
        const_iterator cend  () const { return make_iter(values->cend  ()); }

    private:
        const_iterator make_iter(StringMap::const_iterator mapiter) const {
            return boost::make_transform_iterator(
                mapiter,
                [] (StringMap::value_type pair) { return CastablePair(pair); }
            );
        }

        std::shared_ptr<StringMap> values;
        bool error;

};

#endif
