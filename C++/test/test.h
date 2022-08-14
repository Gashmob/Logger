#ifndef LOGGER_TEST_H
#define LOGGER_TEST_H

#include <string>
#include "utils.h"

typedef struct {
    std::string name;

    void (*before)();

    bool (*run)();

    void (*after)();
} Test;

#endif //LOGGER_TEST_H
