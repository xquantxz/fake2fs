#ifndef FAKE_H
#define FAKE_H

#include <stdint.h>

void *mmap_ext2_file(int fd);
int munmap_ext2_file(void *fsmap);
void read_ext2_file(char *fsmap);
void set_ext2_creator_os(char *fsmap, uint32_t creator_os);
void set_ext2_mtime(char *fsmap, uint32_t mtime);
void set_ext2_wtime(char *fsmap, uint32_t wtime);

#endif

