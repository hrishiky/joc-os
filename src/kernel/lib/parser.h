#ifndef PARSER_H
#define PARSER_H

#define PARSER_SEEK_START -1
#define PARSER_SEEK_CURRENT -2
#define PARSER_SEEK_END -3

#define PARSER_END -1
#define PARSER_FAIL -2

#include "stdint.h"
#include "stdbool.h"

typedef struct {
    uint8_t* buffer;
    size_t index;
    size_t size;
    bool end;
} parser_t;

void parser_init(parser_t* parser, void* buffer, size_t index, size_t size);

int parser_peek(parser_t* parser);
int parser_peek_n(parser_t* parser, int n);

int parser_seek(parser_t* parser, ssize_t index, ssize_t offset);

int parser_read(parser_t* parser);

int parser_put(parser_t* parser, uint8_t value);

#endif