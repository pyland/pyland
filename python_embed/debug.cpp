#include <string>
#include <iostream>

std::ostream print_null(NULL);

#ifdef DEBUG
std::ostream &print_debug = std::cout;
#else
std::ostream &print_debug = print_null;
#endif