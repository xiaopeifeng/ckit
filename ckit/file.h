#ifndef CKIT_FILE_H
#define CKIT_FILE_H

#include <stdint.h>
#include <stdbool.h>

uint8_t* read_whole_file(const char* path, uint32_t* len);

bool write_file(const char* path, uint8_t* cont, uint32_t len);
bool write_filefd(int fd, uint8_t* cont, uint32_t len);

bool recurive_mkdir(const char* dir);


#endif
