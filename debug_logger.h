#ifndef DEBUG_LOGGER_H
#define DEBUG_LOGGER_H

#include <iostream>
#include "build_config.h"

#ifdef DEBUG
#define log(message) std::cout << "[DEBUG] " << __FILE__ << ' ' << __FUNCTION__ << ':' << __LINE__ << ": " << message << '\n';
#else
#define log(message) std::cout << message << '\n';
#endif

#endif // DEBUG_LOGGER_H
