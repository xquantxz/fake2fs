#define _XOPEN_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#include "superblock.h"
#include "fake.h"

void show_help(char *progname);

static char *short_options = "hl";
static struct option long_options[] = {
	{"help", 0, 0, 'h'},
	{"mtime", 1, 0, 'm'},
	{"wtime", 1, 0, 'w'},
	{"creator-os", 1, 0, 'o'},
	{"list", 0, 0, 'l'},
	{NULL, 0, 0, 0},
};

int main(int argc, char **argv)
{
	int option;
	char *fspath = NULL;
	char *mtime = NULL;
	char *wtime = NULL;
	void *fsmap = NULL;


	int list_only = 0;
	int creator_os = -1;

	char *progname = "fake2fs";
	if (argc != 0) progname = argv[0];

	while ((option = getopt_long(argc, argv,
		short_options, long_options, NULL)) != -1) {

		switch (option) {
			case 'p':
				puts(optarg);
				fspath = optarg;
				break;
			case 'm':
				mtime = optarg;
				break;
			case 'w':
				wtime = optarg;
				break;
			case 'o':
				if (strcmp("linux", optarg) == 0) {
					creator_os = OS_LINUX;
				} else if (strcmp("hurd", optarg) == 0) {
					creator_os = OS_HURD;
				} else if (strcmp("masix", optarg) == 0) {
					creator_os = OS_MASIX;
				} else if (strcmp("other", optarg) == 0) {
					creator_os = OS_OTHER;
				} else {
					fprintf(stderr, "Invalid OS name\n");
					fprintf(stderr, "Valid options: linux, hurd, masix, other\n");
					return EXIT_FAILURE;
				}

				break;
			case 'l':
				list_only = 1;
				break;
			case '?':
				show_help(progname);
				return EXIT_FAILURE;
			case 'h':
			default:
				show_help(progname);
				return EXIT_SUCCESS;
		}

	}

	if (optind < argc) {
		fspath = argv[optind];
	} else {
		fprintf(stderr, "No path provided\n");
		return EXIT_FAILURE;
	}

	int fd = open(fspath, O_RDWR);

	if (fd < 0) {
		perror("open()");
		return EXIT_FAILURE;
	}

	fsmap = mmap_ext2_file(fd);
	close(fd);

	if (list_only) {
		read_ext2_file(fsmap);
		goto out;
	}

	if (creator_os != -1) {
		set_ext2_creator_os(fsmap, creator_os);
		puts("Creator OS changed.");
	}

	if (mtime) {
		struct tm time = {};
		strptime(mtime, "%d/%m/%Y", &time);
		set_ext2_mtime(fsmap, mktime(&time));
		puts("Last mount time changed.");
	}

	if (wtime) {
		struct tm time = {};
		strptime(wtime, "%d/%m/%Y", &time);
		set_ext2_wtime(fsmap, mktime(&time));
		puts("Last write time changed.");
	}

out:
	munmap_ext2_file(fsmap);

	return EXIT_SUCCESS;
}

void show_help(char *progname)
{
	printf("Usage: %s [OPTIONS] [PATH]\n", progname);
}

