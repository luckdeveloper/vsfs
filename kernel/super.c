#include <linux/module.h>
#include <linux/init.h>
#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include "vsfs.h"

//forward declarition
//===========================================================================//
struct dentry* vsfs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data);
int vsfs_fill_super(struct super_block *sb, void *data, int slient);
static int vsfs_parse_options(struct super_block *sb, char *options);
static struct vsfs_super_block* vsfs_read_super_block(struct super_block *sb);

//===========================================================================//
struct file_system_type vsfs_fs_type =
{
	.owener = THIS_MODULE,
	.name = "vsfs",
	.mount = vsfs_mount,
	.kill_sb = kill_block_super,
	.fs_flags = FS_REQUIERS_DEV,
};

static int __init vsfs_init(void)
{
	int ret = 0;

	ret = register_filesystem(&vsfs_fs_type);
	if (ret != 0)
	{
		printk(KERN_ERR "VSFS: register_filesystem failed, ret=%d\n", ret);
		return ret;
	}
	return 0;
}

static void __exit vsfs_exi(void)
{
	int ret = 0;
	printk(KERN_INFO"Unloading my module.\n");

	ret = unregister_filesystem(&vsfs_fs_type);
	if (ret != 0)
	{
		printk(KERN_ERR "VSFS: failed to unregister_filesystem, ret=%d\n", ret);
		return ret;
	}
	return 0;
}


struct dentry* vsfs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data)
{
	struct dentry *dentry;
	dentry = mount_bdev(fs_type, flags, dev_name, data, vsfs_fill_super);
	if (IS_ERR(dentry))
	{
		printk(KERN_ERR "VSFS: Error in mounting");
	}
	return dentry;
}



static const struct super_operations vsfs_sops = {
	.alloc_inode = vsfs_alloc_inode,
	.destroy_inode = vsfs_destory_inode,
	.put_super = vsfs_put_super,
};

static inline void vsfs_super_block_fill(struct vsfs_super_block *sb,
			struct vsfs_disk_super_block const *dsb)
{
	sb->sb_magic 		= be32_to_cpu(dsb->dsb_magic);
	sb->sb_inode_blocks = be32_to_cpu(dsb->dsb_inode_blocks);
	sb->sb_block_size 	= be32_to_cpu(dsb->dsb_block_size);
	sb->sb_root_inode 	= be32_to_cpu(dsb->dsb_root_inode);
	sb->sb_inodes_in_block =
		sb->sb_block_size / sizeof(struct vsfs_disk_inode);
}

int vsfs_fill_super(struct super_block *sb, void *data, int slient)
{
	struct inode *root_inode;
	struct vsfs_super_block *vs_sb;

	vs_sb = vsfs_read_super_block(sb);
	if (unlikely(vs_sb == NULL))
	{
		printk(KERN_ERR "VSFS: read superblock failed\n");
		return -EIO;
	}


	sb->s_magic 	= VSFS_MAGIC;
	sb->s_fs_info 	= vs_sb;
	sb->s_maxbytes 	= VSFS_DEFAULT_BLOCK_SIZE;
	sb->s_op 		= &vsfs_sops;

	root_inode = new_inode(sb);
	root_inode->i_ino = VSFS_ROOTDIR_INODE_NO;
	inode_init_owner(root_inode, NULL, S_IFDIR);
	root_inode->i_sb = sb;
	root_inode->i_op = &vsfs_inode_ops;
	root_inode->i_fop = &vsfs_dir_operations;
	root_inode->i_atime = root_inode->i_mtime = root_inode->i_ctime =
		current_time(root_inode);

	root_inode->i_private =
		vsfs_get_inode(sb, VSFS_ROOTDIR_INODE_NO);

	/* TODO: move such stuff into separate header. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)
	sb->s_root = d_make_root(root_inode);
#else
	sb->s_root = d_alloc_root(root_inode);
	if (!sb->s_root) iput(root_inode);
#endif

	if (!sb->s_root)
	{
		return -ENOMEM;
	}

	ret = vsfs_parse_options(sb, data);
	return ret;
}

static struct vsfs_super_block* vsfs_read_super_block(struct super_block *sb)
{
	struct vsfs_super_block *vsb = (struct vsfs_super_block *)
		kzalloc(sizeof(struct aufs_super_block), GFP_NOFS);
	struct vsfs_disk_super_block *vs_dsb;
	struct buffer_head *bh;

	if (!vsb)
	{
		printk(KERN_ERR"aufs cannot allocate super block\n");
		return NULL;
	}

	//read first block, whick block no is 0.
	// block size is XXX ? sb->blocksize
	bh = sb_bread(sb, 0);
	if (!bh)
	{
		printk(KERN_ERR"cannot read 0 block\n");
		goto free_memory;
	}

	vs_dsb = (struct vsfs_disk_super_block *)bh->b_data;
	vsfs_super_block_fill(vs_sb, vs_dsb);
	brelse(bh);

	if (vs_dsb->asb_magic != VSFS_MAGIC)
	{
		printk(KERN_ERR"wrong magic number %lu\n",
			   (unsigned long)vs_dsb->dsb_magic);
		goto free_memory;
	}

	printk(KERN_ERR"aufs super block info:\n"
		   "\tmagic           = %lu\n"
		   "\tinode blocks    = %lu\n"
		   "\tblock size      = %lu\n"
		   "\troot inode      = %lu\n"
		   "\tinodes in block = %lu\n",
		   (unsigned long)vsb->sb_magic,
		   (unsigned long)vsb->sb_inode_blocks,
		   (unsigned long)vsb->sb_block_size,
		   (unsigned long)vsb->sb_root_inode,
		   (unsigned long)vsb->sb_inodes_in_block);

	return vsb;

free_memory:
	kfree(vsb);
	return NULL;
}

static int vsfs_parse_options(struct super_block *sb, char *options)
{
	return 0;
}

module_init(vsfs_init);
module_exit(vsfs_exit);
MODULE_LICENSE("GPL");
