#include "sysinfo.h"

#include <sys/sysinfo.h>
#include <sys/statvfs.h>

unsigned long get_partition_avail_size(const char *path) {
  unsigned long ksize = 0;
  struct statvfs stat;

  if (statvfs(path, &stat) != 0) {
    return ksize;
  }

  if (stat.f_bsize >= 1024) {
    stat.f_bsize /= 1024;
    ksize = stat.f_bsize * stat.f_bavail;
  } else if (stat.f_bavail >= 1024) {
    stat.f_bavail /= 1024;
    ksize = stat.f_bsize * stat.f_bavail;
  } else {
    ksize = stat.f_bsize * stat.f_bavail / 1024;
  }

  return ksize;
}

// sysconf was not support by our libc
unsigned long get_mem_avail_size() {
  struct sysinfo info;
  sysinfo(&info);

  unsigned long freebytes = info.mem_unit * info.freeram;
  return freebytes / 1024;
}

unsigned long get_mem_total_size() {
  struct sysinfo info;
  sysinfo(&info);

  unsigned long totalbytes = info.mem_unit * info.totalram;
  return totalbytes / 1024;
}
