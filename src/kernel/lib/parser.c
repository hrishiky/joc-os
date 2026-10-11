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


int parser_peek(parser_t* parser) {
    if (parser->index >= parser->size) {
        return PARSER_END;
    }

    return parser->buffer[parser->index];
}

int parser_peek_n(parser_t* parser, int n) {
    if (parser->index + n >= parser->size) {
        return PARSER_END;
    }

    return parser->buffer[parser->index + n];
}

int parser_seek(parser_t* parser, ssize_t index, ssize_t offset) {
    ssize_t new_index;

    switch (index) {
        case PARSER_SEEK_START:
            new_index = offset;
            break;
        
        case PARSER_SEEK_CURRENT:
            new_index = parser->index + offset;
            break;
        
        case PARSER_SEEK_END:
            new_index = parser->size + offset;
            break;
        
        default:
            new_index = index + offset;
            break;
    }

    if (new_index < 0) {
        return PARSER_FAIL;
    }

    parser->index = new_index;

    if (parser->index >= parser->size) {
        parser->end = true;
        return PARSER_END;
    }

    return parser_peek(parser);
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

int parser_put(parser_t* parser, uint8_t value) {
    if (parser->end) {
        return PARSER_END;
    }

    parser->buffer[parser->index] = value;

    return value;
}