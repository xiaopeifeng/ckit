#include <string.h>
#include <stdio.h>

#include "strings.h"

#define ERROR_BUFSIZE 64
__thread char tls_error_buf[ERROR_BUFSIZE];

const char* ckit_error(int err_code) {
  int n = strerror_r(err_code, tls_error_buf, ERROR_BUFSIZE);
  if (n == 0) {
    return tls_error_buf;
  } else {
    snprintf(tls_error_buf, ERROR_BUFSIZE, "Unknown error %d", err_code);
    return tls_error_buf;
  }
}

void strip_newline(char* src, int len) {
  while (len > 0) {
    if (src[len - 1] == '\n') src[len - 1] = '\0';
    len--;
  }
}

