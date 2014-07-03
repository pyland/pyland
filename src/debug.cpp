#include <string>
#include <iostream>

static std::ostream print_null(nullptr);

#ifdef DEBUG
std::ostream &print_debug = std::cout;
#else
std::ostream &print_debug = print_null;
#endif
