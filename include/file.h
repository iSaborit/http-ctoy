#ifndef HTTPSV_FILE_H
#define HTTPSV_FILE_H

#include <stddef.h>

typedef struct {
    char *data;
    size_t size;
} file_content;

file_content file_read(const char *path);
void file_delete(file_content *file);

#endif 
