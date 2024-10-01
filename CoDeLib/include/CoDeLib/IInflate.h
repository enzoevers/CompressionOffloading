#pragma once

#include <stdio.h>

typedef enum { INFLATE_SUCCESS, INFLATE_ERROR } INFLATE_RETURN_CODES;

struct IInflate {
    /*!
     * @brief Inflate the input file and write the output to the output file.
     *
     * @param input The input file.
     * @param output The output file.
     * @param options The options for the inflation.
     *
     * @return DINLATE_SUCCESS if the inflation was successful, INFLATE_ERROR
     * otherwise.
     */
    INFLATE_RETURN_CODES (*Inflate)(FILE *input, FILE *output, void *options);
};