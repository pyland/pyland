#include <iostream>

#ifdef DEBUG
extern std::ostream &print_debug;
#else
extern std::ostream print_debug;
#endif