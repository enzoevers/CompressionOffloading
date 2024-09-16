#pragma once

#include <stdio.h>

typedef enum { DEFLATE_SUCCESS, DEFLATE_ERROR } DEFLATE_RETURN_CODES;

struct IDeflate {
    /*!
     * @brief Deflate the input file and write the output to the output file.
     *
     * @param input The input file.
     * @param output The output file.
     * @param options The options for the deflation.
     *
     * @return DEFLATE_SUCCESS if the deflation was successful, DEFLATE_ERROR
     * otherwise.
     */
    DEFLATE_RETURN_CODES (*Deflate)(FILE *input, FILE *output, void *options);
};