#include "parser.h"

#include "stdint.h"
#include "stdbool.h"

void parser_init(parser_t* parser, void* buffer, size_t index, size_t size) {
    parser->buffer = (uint8_t*) buffer;
    parser->index = index;
    parser->size = size;

    if (index < size) {
        parser->end = true;
    } else {
        parser->end = false;
    }
}

int parser_peak(parser_t* parser) {
    if (parser->index >= parser->size) {
        return PARSER_END;
    }

    return parser->buffer[parser->index];
}

int parser_read(parser_t* parser) {
    if (parser->end) {
        return PARSER_END;
    }

    parser->index++;

    if (parser->index >= parser->size) {
        parser->end = true;
        return PARSER_END;
    }

    return parser->buffer[parser->index];
}