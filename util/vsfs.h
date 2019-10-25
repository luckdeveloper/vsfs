#ifndef VSFS_H__
#define VSFS_H__

// for mkfs, user mode

//===========================================================================//
struct vsfs_disk_super_block {
    uint32_t dsb_magic;
    uint32_t dsb_version;
    uint32_t dsb_block_size;
    uint32_t dsb_root_inode;
};

struct vsfs_disk_inode {
    uint32_t	di_first;
    uint32_t	di_blocks;
    uint32_t	di_size;
    uint32_t	di_gid;
    uint32_t	di_uid;
    uint32_t	di_mode;
    uint64_t 	di_ctime;
};

struct vsfs_disk_dentry {

};

#define VSFS_MAGIC 0x68696869
#define VSFS_VERSION 0x20191016

#endif
