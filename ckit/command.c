#include "command.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int ik_execute_cmd(const char *cmd, char **results) {
  FILE *fp = NULL;

  if (cmd == NULL) {
    return -1;
  }

  /* type 're' should be in order, or will report error  */
  fp = popen(cmd, "re");
  if (fp == NULL) {
    printf("popen failed, %s", strerror(errno));
    return -1;
  }

  char buf[1024];
  size_t len = 0;
  size_t result_len = 0;

  if (results) {
    *results = NULL;
  }

  do {
    len = fread(buf, sizeof(char), 1024, fp);
    if (len > 0) {
      if (!results) {
        // drain the pipe out, or subprocess will generate echo pipe
        // write error
        continue;
      }

      *results = (char *)realloc(*results, result_len + len + 1);
      memcpy(*results + result_len, buf, len);
      result_len = result_len + len;
    }

    if (len == 0) {
      if (errno == EINTR) {
        /* interrupt, try again */
        continue;
      }
      break;
    }

  } while (!feof(fp));

  if (result_len > 0) {
    (*results)[result_len] = '\0';
  }

  return WEXITSTATUS(pclose(fp));
}
