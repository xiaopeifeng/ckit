#ifndef CKIT_SYSINFO_H
#define CKIT_SYSINFO_H

/*KB*/
unsigned long get_partition_avail_size(const char *path);
unsigned long get_mem_avail_size();
unsigned long get_mem_total_size();

#endif
