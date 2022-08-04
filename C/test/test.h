#ifndef LOGGER_TEST_H
#define LOGGER_TEST_H

#include <stdbool.h>
#include "utils.h"

typedef struct {
    char *name;

    void (*before)();

    bool (*run)();

    void (*after)();
} Test;

static void nothing() {
    // Do nothing
}

static void clearDir() {
    rmDir("logs");
}

#endif //LOGGER_TEST_H
