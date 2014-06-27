#include <iostream>

#ifdef DEBUG
std::ostream &print_debug = std::cout;
#else
std::ostream print_debug(NULL);
#endif