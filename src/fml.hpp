#include <istream>
#include <map>
#include <memory>
#include <string>

class FML {
    public:
        explicit FML(std::istream &input);

        template <typename T=std::string>
        T get(std::string where);

        std::shared_ptr<std::map<std::string, std::string>> values;
    private:
        bool error;
};
