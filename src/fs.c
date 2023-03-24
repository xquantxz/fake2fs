#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "superblock.h"

#define MAP_SIZE (0x1000)
#define EXT2_SIGNATURE (0xef53)

static inline int valid_ext2_signature(unsigned short sig) {
	return (sig == EXT2_SIGNATURE);
}

static char *creator_os_string(int creator_os)
{
	switch (creator_os) {
	case OS_LINUX:
		return "linux";
	case OS_HURD:
		return "hurd";
	case OS_MASIX:
		return "masix";
	case OS_FREEBSD:
		return "freebsd";
	case OS_OTHER:
		return "other";
	default:
		return "unknown";
	}
}

static struct ext2_super_block *get_ext2_superblock(char *fsmap)
{
	struct ext2_super_block *superblock;

	/* skip boot block */
	superblock = (struct ext2_super_block *)(fsmap+0x400);

	if (!valid_ext2_signature(superblock->s_magic)) {
		puts("Invalid ext2 superblock signature");
		return NULL;
	}

	return superblock;
}

void read_ext2_file(char *fsmap)
{
	struct ext2_super_block *superblock = get_ext2_superblock(fsmap);

	if (!superblock) return;

	long mount_time = superblock->s_mtime;
	long write_time = superblock->s_wtime;

	printf("Creator OS ID: %d (%s)\n",
		superblock->s_creator_os,
		creator_os_string(superblock->s_creator_os));
	printf("Last time mounted: %s", ctime(&mount_time));
	printf("Last time written: %s", ctime(&write_time));
}

void set_ext2_creator_os(char *fsmap, uint32_t creator_os)
{
	struct ext2_super_block *superblock = get_ext2_superblock(fsmap);

	if (!superblock) return;

	superblock->s_creator_os = creator_os;
}

void set_ext2_mtime(char *fsmap, uint32_t mtime)
{
	struct ext2_super_block *superblock = get_ext2_superblock(fsmap);

	if (!superblock) return;

	superblock->s_mtime = mtime;
}

void set_ext2_wtime(char *fsmap, uint32_t wtime)
{
	struct ext2_super_block *superblock = get_ext2_superblock(fsmap);

	if (!superblock) return;

	superblock->s_wtime = wtime;
}

void *mmap_ext2_file(int fd)
{
	return mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

int munmap_ext2_file(void *fsmap)
{
	return munmap(fsmap, MAP_SIZE);
}

