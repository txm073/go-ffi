#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef long long handle_t;

typedef enum method {
    GET, POST, PUT, DELETE, PATCH
} method_t;

typedef struct resp {
    char* content;
    size_t len;
    int code;
} resp_t;

typedef enum type {
    INT8, INT16, INT32, INT64, 
    UINT8, UINT16, UINT32, UINT64,
    FLOAT32, FLOAT64, STRING, ARRAY
} type_t;

typedef struct {
    char* data;
    size_t len;
} buffer_t;

#define SUCCESS 0
#define INVALID_HANDLE -1
#define INVALID_TYPE -2
#define INVALID_KEY -3

