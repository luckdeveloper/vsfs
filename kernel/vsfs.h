#ifndef VSFS_H__
#define VSFS_H__

/*superblock at first block of disk*/
#define VSFS_SUPERBLOCK_BLOCK_NO 	0
#define VSFS_MAGIC			6869
#define VSFS_ROOTDIR_INODE_NO 	1
#define VSFS_DEFAULT_BLOCK_SIZE		4096


//===========================================================================//
struct vsfs_disk_super_block {
    __le32 dsb_magic;
    __le32 dsb_version;
    __le32 dsb_block_size;
    __le32 dsb_root_inode;
};

struct vsfs_disk_inode {
    __le32	di_first;
    __le32	di_blocks;
    __le32	di_size;
    __le32	di_gid;
    __le32	di_uid;
    __le32	di_mode;
    __le64 	di_ctime;
};

struct vsfs_disk_dentry {

};

struct vsfs_super_block
{
    unsigned long sb_magic;
    unsigned long sb_inode_blocks;
    unsigned long sb_block_size;
    unsigned long sb_root_inode;
    unsigned long sb_inodes_in_block;
};



//===========================================================================//

extern const struct address_space_operations vsfs_aops;
extern const struct inode_operations vsfs_dir_inode_ops;
extern const struct file_operations vsfs_file_ops;
extern const struct file_operations vsfs_dir_ops;
#endif
