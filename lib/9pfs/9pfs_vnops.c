/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Authors: Cristian Banu <cristb@gmail.com>
 *
 * Copyright (c) 2019, University Politehnica of Bucharest. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * THIS HEADER MAY NOT BE EXTRACTED OR MODIFIED IN ANY WAY.
 */

#define _GNU_SOURCE

#include <fcntl.h>
#include <dirent.h>
#include <uk/config.h>
#include <uk/9p.h>
#include <uk/errptr.h>
#include <vfscore/mount.h>
#include <vfscore/dentry.h>
#include <vfscore/vnode.h>
#include <vfscore/file.h>
#include <vfscore/fs.h>

#include "9pfs.h"

int uk_9pfs_allocate_vnode_data(struct vnode *vp, struct uk_9pfid *fid)
{
	struct uk_9pfs_node_data *nd;

	nd = malloc(sizeof(*nd));
	if (nd == NULL)
		return -ENOMEM;

	nd->fid = fid;
	nd->nb_open_files = 0;
	nd->removed = false;
	vp->v_data = nd;

	return 0;
}

void uk_9pfs_free_vnode_data(struct vnode *vp)
{
	struct uk_9pdev *dev = UK_9PFS_MD(vp->v_mount)->dev;
	struct uk_9pfs_node_data *nd = UK_9PFS_ND(vp);

	if (nd->nb_open_files > 0)
		return;

	if (nd->removed)
		uk_9p_remove(dev, nd->fid);

	uk_9pfid_put(nd->fid);
	free(nd);
	vp->v_data = NULL;
}

static int uk_9pfs_readdir(struct vnode *vp, struct vfscore_file *fp,
		struct dirent *dir)
{
	return ENOENT;
}

#define uk_9pfs_seek		((vnop_seek_t)vfscore_vop_nullop)
#define uk_9pfs_ioctl		((vnop_ioctl_t)vfscore_vop_einval)
#define uk_9pfs_fsync		((vnop_fsync_t)vfscore_vop_nullop)
#define uk_9pfs_getattr		((vnop_getattr_t)vfscore_vop_nullop)
#define uk_9pfs_setattr		((vnop_setattr_t)vfscore_vop_nullop)
#define uk_9pfs_truncate	((vnop_truncate_t)vfscore_vop_nullop)
#define uk_9pfs_inactive	((vnop_inactive_t)vfscore_vop_nullop)
#define uk_9pfs_link		((vnop_link_t)vfscore_vop_eperm)
#define uk_9pfs_cache		((vnop_cache_t)NULL)
#define uk_9pfs_readlink	((vnop_readlink_t)vfscore_vop_einval)
#define uk_9pfs_symlink		((vnop_symlink_t)vfscore_vop_eperm)
#define uk_9pfs_fallocate	((vnop_fallocate_t)vfscore_vop_nullop)
#define uk_9pfs_create		((vnop_create_t)vfscore_vop_einval)
#define uk_9pfs_remove		((vnop_remove_t)vfscore_vop_einval)
#define uk_9pfs_rename		((vnop_rename_t)vfscore_vop_einval)
#define uk_9pfs_mkdir		((vnop_mkdir_t)vfscore_vop_einval)
#define uk_9pfs_rmdir		((vnop_rmdir_t)vfscore_vop_einval)
#define uk_9pfs_open		((vnop_open_t)vfscore_vop_einval)
#define uk_9pfs_close		((vnop_close_t)vfscore_vop_einval)
#define uk_9pfs_read		((vnop_read_t)vfscore_vop_einval)
#define uk_9pfs_write		((vnop_write_t)vfscore_vop_einval)
#define uk_9pfs_lookup		((vnop_lookup_t)vfscore_vop_einval)

struct vnops uk_9pfs_vnops = {
	.vop_open	= uk_9pfs_open,
	.vop_close	= uk_9pfs_close,
	.vop_read	= uk_9pfs_read,
	.vop_write	= uk_9pfs_write,
	.vop_seek	= uk_9pfs_seek,
	.vop_ioctl	= uk_9pfs_ioctl,
	.vop_fsync	= uk_9pfs_fsync,
	.vop_readdir	= uk_9pfs_readdir,
	.vop_lookup	= uk_9pfs_lookup,
	.vop_create	= uk_9pfs_create,
	.vop_remove	= uk_9pfs_remove,
	.vop_rename	= uk_9pfs_rename,
	.vop_mkdir	= uk_9pfs_mkdir,
	.vop_rmdir	= uk_9pfs_rmdir,
	.vop_getattr	= uk_9pfs_getattr,
	.vop_setattr	= uk_9pfs_setattr,
	.vop_inactive	= uk_9pfs_inactive,
	.vop_truncate	= uk_9pfs_truncate,
	.vop_link	= uk_9pfs_link,
	.vop_cache	= uk_9pfs_cache,
	.vop_fallocate	= uk_9pfs_fallocate,
	.vop_readlink	= uk_9pfs_readlink,
	.vop_symlink	= uk_9pfs_symlink
};
