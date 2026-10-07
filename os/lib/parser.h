#ifndef PARSER_H
#define PARSER_H

#define PARSER_END -1

#include "stdint.h"
#include "stdbool.h"

typedef struct {
    uint8_t* buffer;
    size_t index;
    size_t size;
    bool end;
} parser_t;

void parser_init(parser_t* parser, void* buffer, size_t index, size_t size);

int parser_peak(parser_t* parser);
int parser_read(parser_t* parser);

#endif