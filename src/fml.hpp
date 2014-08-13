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
    using str = std::string;

    public:
        template <typename T=str>
        using const_iterator = boost::transform_iterator<
            std::function<std::pair<str const, T> (std::pair<str const, str>)>,
            std::map<str, str>::const_iterator
        >;

        explicit FML(std::istream &input);

        template <typename T=str>
        T get(str where) { return boost::lexical_cast<T>(values->at(where)); }

        std::map<str, str> as_map();

        template <typename T=str>
        const_iterator<T> begin() const { return this->cbegin<T>(); }

        template <typename T=str>
        const_iterator<T> end() const { return this->cend<T>(); }

        template <typename T=str>
        const_iterator<T> cbegin() const {
            return boost::make_transform_iterator(values->cbegin(), &FML::lexical_pair_cast<T>);
        }

        template <typename T=str>
        const_iterator<T> cend() const {
            return boost::make_transform_iterator(values->cend(), &FML::lexical_pair_cast<T>);
        }

        bool valid();

    private:
        std::shared_ptr<std::map<str, str>> values;
        bool error;

        template <typename T>
        static std::pair<str, T> lexical_pair_cast(std::pair<str, str> other) {
            return std::make_pair(other.first, boost::lexical_cast<T>(other.second));
        }
};

#endif
