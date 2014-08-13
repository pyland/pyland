#include <istream>
#include <map>
#include <memory>
#include <string>

class FML {
    public:
        using const_iterator = std::map<std::string, std::string>::const_iterator;

        explicit FML(std::istream &input);

        template <typename T=std::string>
        T get(std::string where);

        std::map<std::string, std::string> as_map();

        const_iterator begin()  const;
        const_iterator cbegin() const;

        const_iterator end()  const;
        const_iterator cend() const;

        bool valid();

    private:
        std::shared_ptr<std::map<std::string, std::string>> values;
        bool error;
};
