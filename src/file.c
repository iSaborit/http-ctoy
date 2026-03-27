#include "file.h"
#include <stdio.h>
#include <stdlib.h>

file_content file_read(const char *path) {
    file_content file = {0};

    FILE *f = fopen(path, "rb");
    if (!f)
        return file;

    if (fseek(f, 0, SEEK_END) != 0) {
        perror("fseek");
        goto fail;
    }

    long length = ftell(f);
    if (length < 0) {
        perror("ftell");
        goto fail;
    }

    if (fseek(f, 0, SEEK_SET) != 0) {
        perror("fseek");
        goto fail;
    }

    if (length == 0) {
        fclose(f);
        return file; // archivo vacío válido
    }

    file.size = (size_t)length;
    file.data = malloc(file.size);
    if (!file.data) {
        perror("malloc");
        goto fail;
    }

    size_t read_bytes = fread(file.data, 1, file.size, f);
    if (read_bytes != file.size) {
        perror("fread");
        free(file.data);
        file.data = NULL;
        file.size = 0;
        goto fail;
    }

    fclose(f);
    return file;

fail:
    fclose(f);
    return (file_content){0};
}

void file_delete(file_content *file) {
    free(file->data);
    file->size = 0;
    file->data = NULL;
}
