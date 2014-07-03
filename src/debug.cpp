#include <string>
#include <iostream>

static std::ostream print_null(nullptr);

#ifdef DEBUG
extern std::ostream &print_debug = std::cout;
#else
extern std::ostream &print_debug = print_null;
#endif
