#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


uint8_t* read_whole_file(const char* path, uint32_t* len) {
  FILE* fp = fopen(path, "rb");
  if (!fp) {
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  uint8_t* cont = malloc(fsize + 1);
  cont[fsize] = '\0';
  size_t rlen = fread(cont, 1, fsize, fp);
  if (rlen != fsize) {
    printf("fread not whole cont len: %zu, file size: %ld", rlen, fsize);
    fclose(fp);
    free(cont);
    *len = 0;
    return NULL;
  }

  if (len) {
    *len = fsize;
  }

  fclose(fp);

  return cont;
}

bool write_file(const char* path, uint8_t* cont, uint32_t len) {
  FILE* fp = fopen(path, "w");
  if (!fp) {
    return false;
  }

  if (fwrite(cont, len, 1, fp) != 1) {
    fclose(fp);
    return false;
  }

  fclose(fp);
  return true;
}

bool write_filefd(int fd, uint8_t* cont, uint32_t len) {
  uint32_t written = 0;

  while (written < len) {
    ssize_t n = write(fd, cont + written, len - written);

    if (n > 0) {
      written += n;
    } else {
      if (errno == EINTR) {
        continue;
      } else {
        return false;
      }
    }
  }

  return true;
}

bool recurive_mkdir(const char* dir) {
  char tmp[256];
  char* p = NULL;
  size_t len;

  snprintf(tmp, sizeof(tmp), "%s", dir);
  len = strlen(tmp);

  if (tmp[len - 1] == '/') {
    tmp[len - 1] = 0;
  }

  for (p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = 0;

      mkdir(tmp, S_IRWXU);
      *p = '/';
    }
  }

  if (mkdir(tmp, S_IRWXU) == 0) {
    return true;
  } else {
    return false;
  }
}

