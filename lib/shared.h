#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef float float32_t;
typedef double float64_t;
typedef long long go_handle_t;

typedef enum method {
    GET, POST, PUT, DELETE, PATCH
} method_t;

typedef struct resp {
    char* content;
    size_t len;
    int code;
} resp_t;

typedef enum type {
    GO_INT8, GO_INT16, GO_INT32, GO_INT64, 
    GO_UINT8, GO_UINT16, GO_UINT32, GO_UINT64,
    GO_FLOAT32, GO_FLOAT64, GO_STRING, GO_ARRAY
} type_t;

typedef struct {
    char* data;
    size_t len;
} buffer_t;

#define SUCCESS 0
#define INVALID_HANDLE -1
#define INVALID_TYPE -2
#define INVALID_KEY -3

#define GOSTRING(__var, __str) GoString __var; __var.p = __str; __var.n = strlen(__var.p) 

#endif
