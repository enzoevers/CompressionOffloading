#pragma once

#include <stdio.h>

struct IDeflate {
    const int (*Defalte)(FILE *input, FILE *output, void *options);
};