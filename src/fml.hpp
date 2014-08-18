#ifndef FML_H
#define FML_H

// Fixes bug with lambdas for transform_iterator on older compilers
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/lexical_cast.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <functional>
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

        FML() = default;
        explicit FML(std::istream &input);
        static FML unsafe_from_map(std::shared_ptr<std::map<std::string, std::string>> input);

        bool valid();

        template <typename T=std::string>
        T get(std::string where) { return boost::lexical_cast<T>(values->at(where)); }

        // Iterator is always const
        const_iterator begin () const;
        const_iterator cbegin() const;
        const_iterator end   () const;
        const_iterator cend  () const;

        const_iterator begin(std::string directory) const;
        const_iterator end  (std::string directory) const;

    private:
        explicit FML(std::shared_ptr<std::map<std::string, std::string>> input);

        const_iterator make_iter(StringMap::const_iterator mapiter, size_t chop=0) const;

        std::shared_ptr<StringMap> values;
        bool error;
};

#endif
