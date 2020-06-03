#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>

uint8_t *read_whole_file(const char *path, uint32_t *len) {
  FILE *fp = fopen(path, "rb");
  if (!fp) {
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  uint8_t *cont = malloc(fsize + 1);
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

bool write_file(const char *path, uint8_t *cont, uint32_t len) {
  FILE *fp = fopen(path, "w");
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

bool write_filefd(int fd, uint8_t *cont, uint32_t len) {
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

bool recurive_mkdir(const char *dir) {
  char tmp[256];
  char *p = NULL;
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

int create_pidfile(const char *pid_file) {
  struct flock fl;
  char pidstr[32];
  int pid_file_fd = -1;

  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;  // lock whole file
  fl.l_pid = getpid();

  pid_file_fd = open(pid_file, O_CREAT | O_WRONLY, 0666);
  if (pid_file_fd < 0) {
    printf("create pid file failed, %s", strerror(errno));
    return -1;
  }

  int flags = fcntl(pid_file_fd, F_GETFL, 0);
  fcntl(pid_file_fd, F_SETFL, flags | FD_CLOEXEC);

  if (fcntl(pid_file_fd, F_SETLK, &fl) < 0) {
    close(pid_file_fd);
    pid_file_fd = -1;
    return -1;
  }

  snprintf(pidstr, sizeof(pidstr), "%d", fl.l_pid);
  if (!write_filefd(pid_file_fd, (uint8_t *)pidstr, strlen(pidstr))) {
    close(pid_file_fd);
    pid_file_fd = -1;
    return -1;
  }

  fsync(pid_file_fd);

  return pid_file_fd;
}

bool delete_pidfile(const char *pid_file, int pid_file_fd) {
  struct flock fl;

  if (pid_file_fd < 0) {
    return false;
  }

  fl.l_type = F_UNLCK;  // unlock
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;
  fl.l_pid = getpid();

  if (fcntl(pid_file_fd, F_SETLK, &fl) < 0) {
    printf("unlock pid file failed, %s", strerror(errno));
  }

  close(pid_file_fd);
  remove(pid_file);
  pid_file_fd = -1;

  return true;
}

pid_t read_pidfile(const char *pid_file) {
  if (access(pid_file, F_OK) != 0) {
    return 0;
  }

  FILE *fp = fopen(pid_file, "rb");
  if (!fp) {
    return 0;
  }

  char pidstr[8];
  memset(pidstr, 0, sizeof(pidstr));
  fread(pidstr, 1, sizeof(pidstr), fp);
  fclose(fp);

  return strtoul(pidstr, NULL, 10);
}
