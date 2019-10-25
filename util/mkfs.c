#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "vsfs.h"

/////////////////////////////////////////////////////////////////////////////// 
/// usage : mkfs <device-name> 



int main(int argc, char *argv[])
{
    char *device_name = NULL;       		      
	int fd = 0;
	struct vsfs_disk_super_block sb = {0};

    if (argc != 2)
	{
	fprintf(stderr, "mkfs <device-name>");
	return 0;
    }
    device_name = argv[1];

	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		fprintf(stderr, "open device %s failed", argv[1]);
	}
	/* step1: write superblock*/
	sb.dsb_magic =  VSFS_MAGIC;
	sb.dsb_version = VSFS_VERSION;
	lseek(fd, 0, SEEK_SET);
	write(fd, &sb, sizeof(sb));

    /* step2: write inode bitmap*/
    /* step3: write data bitmap*/
    /* step4: initialize inode region*/
    /* step5: initialize data region*/

}
