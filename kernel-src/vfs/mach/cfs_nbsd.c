#ifndef _BLURB_
#define _BLURB_
/*

            Coda: an Experimental Distributed File System
                             Release 3.1

          Copyright (c) 1987-1996 Carnegie Mellon University
                         All Rights Reserved

Permission  to  use, copy, modify and distribute this software and its
documentation is hereby granted,  provided  that  both  the  copyright
notice  and  this  permission  notice  appear  in  all  copies  of the
software, derivative works or  modified  versions,  and  any  portions
thereof, and that both notices appear in supporting documentation, and
that credit is given to Carnegie Mellon University  in  all  documents
and publicity pertaining to direct or indirect use of this code or its
derivatives.

CODA IS AN EXPERIMENTAL SOFTWARE SYSTEM AND IS  KNOWN  TO  HAVE  BUGS,
SOME  OF  WHICH MAY HAVE SERIOUS CONSEQUENCES.  CARNEGIE MELLON ALLOWS
FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION.   CARNEGIE  MELLON
DISCLAIMS  ANY  LIABILITY  OF  ANY  KIND  FOR  ANY  DAMAGES WHATSOEVER
RESULTING DIRECTLY OR INDIRECTLY FROM THE USE OF THIS SOFTWARE  OR  OF
ANY DERIVATIVE WORK.

Carnegie  Mellon  encourages  users  of  this  software  to return any
improvements or extensions that  they  make,  and  to  grant  Carnegie
Mellon the rights to redistribute these changes without encumbrance.
*/

static char *rcsid = "/afs/cs/project/coda-rvb/cvs/src/coda-4.0.1/kernel-src/vfs/mach/cfs_nbsd.c,v 1.1.1.1 1996/11/22 19:16:03 rvb Exp";
#endif /*_BLURB_*/


/* 
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 * This code was written for the Coda file system at Carnegie Mellon
 * University.  Contributers include David Steere, James Kistler,
 * M. Satyanarayanan, and Brian Noble.  
 */

/* 
 * HISTORY
 * cfs_nbsd.c,v
 * Revision 1.2  1996/01/02 16:56:52  bnoble
 * Added support for Coda MiniCache and raw inode calls (final commit)
 *
 * Revision 1.1.2.1  1995/12/20 01:57:17  bnoble
 * Added CFS-specific files
 *
 */

/* NetBSD-specific routines for the cfs code */
#ifdef __NetBSD__

#include <cfs/cfs.h>
#include <cfs/cfs_vnodeops.h>
#include <cfs/cnode.h>

/* What we are delaying for in printf */
int cfs_printf_delay = 1000000;  /* in microseconds */
static int lockdebug = 0;

/* Definition of the vfs operation vector */

/*
 * Some NetBSD details:
 * 
 *   cfs_start is called at the end of the mount syscall.
 *
 *   cfs_init is called at boot time.
 */

extern int cfsdebug;

int cfs_vnop_print_entry = 0;

#ifdef __GNUC__
#define ENTRY    \
   if(cfs_vnop_print_entry) myprintf(("Entered %s\n",__FUNCTION__))
#else
#define ENTRY
#endif 

 
struct vfsops cfs_vfsops = {
    MOUNT_CFS,
    cfs_mount,
    cfs_start,
    cfs_unmount,
    cfs_root,
    cfs_quotactl,
    cfs_statfs,
    cfs_sync,
    cfs_vget,
    (int (*) (struct mount *, struct fid *, struct mbuf *, struct vnode **,
	      int *, struct ucred **))
	eopnotsupp,
    (int (*) (struct vnode *, struct fid *)) eopnotsupp,
    cfs_init,
    0
};

/* NetBSD interfaces to the vnodeops */
int cfs_nb_open      __P((struct vop_open_args *));
int cfs_nb_close     __P((struct vop_close_args *));
int cfs_nb_read      __P((struct vop_read_args *));
int cfs_nb_write     __P((struct vop_write_args *));
int cfs_nb_ioctl     __P((struct vop_ioctl_args *));
int cfs_nb_select    __P((struct vop_select_args *));
int cfs_nb_getattr   __P((struct vop_getattr_args *));
int cfs_nb_setattr   __P((struct vop_setattr_args *));
int cfs_nb_access    __P((struct vop_access_args *));
int cfs_nb_readlink  __P((struct vop_readlink_args *));
int cfs_nb_abortop   __P((struct vop_abortop_args *));
int cfs_nb_fsync     __P((struct vop_fsync_args *));
int cfs_nb_inactive  __P((struct vop_inactive_args *));
int cfs_nb_lookup    __P((struct vop_lookup_args *));
int cfs_nb_create    __P((struct vop_create_args *));
int cfs_nb_remove    __P((struct vop_remove_args *));
int cfs_nb_link      __P((struct vop_link_args *));
int cfs_nb_rename    __P((struct vop_rename_args *));
int cfs_nb_mkdir     __P((struct vop_mkdir_args *));
int cfs_nb_rmdir     __P((struct vop_rmdir_args *));
int cfs_nb_symlink   __P((struct vop_symlink_args *));
int cfs_nb_readdir   __P((struct vop_readdir_args *));
int cfs_nb_bmap      __P((struct vop_bmap_args *));
int cfs_nb_strategy  __P((struct vop_strategy_args *));
int cfs_nb_lock      __P((struct vop_lock_args *));
int cfs_nb_unlock    __P((struct vop_unlock_args *));
int cfs_nb_islocked  __P((struct vop_islocked_args *));
int nbsd_vop_error   __P((void *));
int nbsd_vop_nop     __P((void *));

int cfs_nb_reclaim   __P((struct vop_reclaim_args *));

/* Definition of the vnode operation vector */

int (**cfs_vnodeop_p)();
struct vnodeopv_entry_desc cfs_vnodeop_entries[] = {
    { &vop_default_desc, nbsd_vop_error },
    { &vop_lookup_desc, cfs_nb_lookup },           /* lookup */
    { &vop_create_desc, cfs_nb_create },		/* create */
    { &vop_mknod_desc, nbsd_vop_error },	/* mknod */
    { &vop_open_desc, cfs_nb_open },		/* open */
    { &vop_close_desc, cfs_nb_close },		/* close */
    { &vop_access_desc, cfs_nb_access },		/* access */
    { &vop_getattr_desc, cfs_nb_getattr },		/* getattr */
    { &vop_setattr_desc, cfs_nb_setattr },		/* setattr */
    { &vop_read_desc, cfs_nb_read },		/* read */
    { &vop_write_desc, cfs_nb_write },		/* write */
    { &vop_lease_desc, nbsd_vop_nop },          /* lease */
    { &vop_ioctl_desc, cfs_nb_ioctl },		/* ioctl */
    { &vop_select_desc, cfs_nb_select },		/* select */
    { &vop_mmap_desc, nbsd_vop_error },	/* mmap */
    { &vop_fsync_desc, cfs_nb_fsync },		/* fsync */
    { &vop_seek_desc, nbsd_vop_error },	/* seek */
    { &vop_remove_desc, cfs_nb_remove },		/* remove */
    { &vop_link_desc, cfs_nb_link },		/* link */
    { &vop_rename_desc, cfs_nb_rename },		/* rename */
    { &vop_mkdir_desc, cfs_nb_mkdir },		/* mkdir */
    { &vop_rmdir_desc, cfs_nb_rmdir },		/* rmdir */
    { &vop_symlink_desc, cfs_nb_symlink },		/* symlink */
    { &vop_readdir_desc, cfs_nb_readdir },		/* readdir */
    { &vop_readlink_desc, cfs_nb_readlink },	/* readlink */
    { &vop_abortop_desc, cfs_nb_abortop },	/* abortop */
    { &vop_inactive_desc, cfs_nb_inactive },	/* inactive */
    { &vop_reclaim_desc, cfs_nb_reclaim },	/* reclaim */
    { &vop_lock_desc, cfs_nb_lock },	/* lock */
    { &vop_unlock_desc, cfs_nb_unlock },	/* unlock */
    { &vop_bmap_desc, cfs_nb_bmap },		/* bmap */
    { &vop_strategy_desc, cfs_nb_strategy },	/* strategy */
    { &vop_print_desc, nbsd_vop_error },	/* print */
    { &vop_islocked_desc, cfs_nb_islocked },	/* islocked */
    { &vop_pathconf_desc, nbsd_vop_error },	/* pathconf */
    { &vop_advlock_desc, nbsd_vop_nop },	/* advlock */
    { &vop_blkatoff_desc, nbsd_vop_error },	/* blkatoff */
    { &vop_valloc_desc, nbsd_vop_error },	/* valloc */
    { &vop_vfree_desc, nbsd_vop_error },	/* vfree */
    { &vop_truncate_desc, nbsd_vop_error },	/* truncate */
    { &vop_update_desc, nbsd_vop_error },	/* update */
    { &vop_bwrite_desc, nbsd_vop_error },	/* bwrite */
    { (struct vnodeop_desc*)NULL, (int(*)())NULL }
};

/* Definitions of NetBSD vnodeop interfaces */

/* A generic panic: we were called with something we didn't define yet */
int
nbsd_vop_error(void *anon) {
    struct vnodeop_desc **desc = (struct vnodeop_desc **)anon;

    myprintf(("Vnode operation %s called, but not defined\n",
	      (*desc)->vdesc_name));
    panic("nbsd_vop_error");
    return 0;
}

/* A generic do-nothing.  For lease_check, advlock */
int
nbsd_vop_nop(void *anon) {
    struct vnodeop_desc **desc = (struct vnodeop_desc **)anon;

    if (cfsdebug) {
	myprintf(("Vnode operation %s called, but unsupported\n",
		  (*desc)->vdesc_name));
    }
    return (0);
}

int
cfs_nb_open(ap)
    struct vop_open_args *ap;
{
    ENTRY;
    return (cfs_open(&(ap->a_vp), ap->a_mode, ap->a_cred, ap->a_p));
}

int
cfs_nb_close(ap)
    struct vop_close_args *ap;
{
    ENTRY;
    return (cfs_close(ap->a_vp, ap->a_fflag, ap->a_cred, ap->a_p));
}

int
cfs_nb_read(ap)
    struct vop_read_args *ap;
{
    ENTRY;
    return(cfs_rdwr(ap->a_vp, ap->a_uio, UIO_READ,
		    ap->a_ioflag, ap->a_cred, ap->a_uio->uio_procp));
}

int
cfs_nb_write(ap)
    struct vop_write_args *ap;
{
    ENTRY;
    return(cfs_rdwr(ap->a_vp, ap->a_uio, UIO_WRITE,
		    ap->a_ioflag, ap->a_cred, ap->a_uio->uio_procp));
}

int
cfs_nb_ioctl(ap)
    struct vop_ioctl_args *ap;
{
    ENTRY;
    return (cfs_ioctl(ap->a_vp, ap->a_command, ap->a_data, ap->a_fflag,
		      ap->a_cred, ap->a_p));
}

int
cfs_nb_select(ap)
    struct vop_select_args *ap;
{
    ENTRY;
    return (cfs_select(ap->a_vp, ap->a_which, ap->a_cred, ap->a_p));
}

int
cfs_nb_getattr(ap)
    struct vop_getattr_args *ap;
{
    ENTRY;
    return (cfs_getattr(ap->a_vp, ap->a_vap, ap->a_cred, ap->a_p));
}

int
cfs_nb_setattr(ap)
    struct vop_setattr_args *ap;
{
    ENTRY;
    return (cfs_setattr(ap->a_vp, ap->a_vap, ap->a_cred, ap->a_p));
}

int
cfs_nb_access(ap)
    struct vop_access_args *ap;
{
    ENTRY;
    return (cfs_access(ap->a_vp, ap->a_mode, ap->a_cred, ap->a_p));
}

int
cfs_nb_readlink(ap)
    struct vop_readlink_args *ap;
{
    ENTRY;
    return (cfs_readlink(ap->a_vp, ap->a_uio, ap->a_cred, 
			 ap->a_uio->uio_procp));
}


/*
 * CFS abort op, called after namei() when a CREATE/DELETE isn't actually
 * done. If a buffer has been saved in anticipation of a cfs_create or
 * a cfs_remove, delete it.
 */
/* ARGSUSED */
int
cfs_nb_abortop(ap)
	struct vop_abortop_args /* {
		struct vnode *a_dvp;
		struct componentname *a_cnp;
	} */ *ap;
{
	if ((ap->a_cnp->cn_flags & (HASBUF | SAVESTART)) == HASBUF)
		free(ap->a_cnp->cn_pnbuf, M_NAMEI);
	return (0);
}

int
cfs_nb_fsync(ap)
    struct vop_fsync_args *ap;
{
    ENTRY;
    return (cfs_fsync(ap->a_vp, ap->a_cred, ap->a_p));
}

int
cfs_nb_inactive(ap)
    struct vop_inactive_args *ap;
{
    ENTRY;
    /* XXX - at the moment, inactive doesn't look at cred, and doesn't
       have a proc pointer.  Oops. */
    return (cfs_inactive(ap->a_vp, NULL, GLOBAL_PROC));
}

int
cfs_nb_lookup(ap)
    struct vop_lookup_args *ap;
{
    /* 
     * It looks as though ap->a_cnp->ni_cnd->cn_nameptr holds the rest
     * of the string to xlate, and that we must try to get at least
     * ap->a_cnp->ni_cnd->cn_namelen of those characters to macth.  I
     * could be wrong. 
     */
    char                   tname[MAXPATHLEN];   /* overkill, but... */
    struct componentname  *cnp = ap->a_cnp;
    int                    result;
    
    ENTRY;
    bcopy(cnp->cn_nameptr, tname,  cnp->cn_namelen);
    tname[cnp->cn_namelen] = '\0';

    result = cfs_lookup(ap->a_dvp, tname, ap->a_vpp, cnp->cn_cred, 
			cnp->cn_proc);

    /* 
     * If we are creating, and this was the last name to be looked up,
     * and the error was ENOENT, then there really shouldn't be an
     * error and we can make the leaf NULL and return success.  Since
     * this is supposed to work under Mach as well as NetBSD, we're
     * leaving this fn wrapped.  We also must tell lookup/namei that
     * we need to save the last component of the name.  (Create will
     * have to free the name buffer later...lucky us...)
     */
    if (((cnp->cn_nameiop == CREATE) || (cnp->cn_nameiop == RENAME))
	&& (cnp->cn_flags & ISLASTCN)
	&& (result == ENOENT))
    {
	result = EJUSTRETURN;
	cnp->cn_flags |= SAVENAME;
	*ap->a_vpp = NULL;
    }

    /* 
     * If we are removing, and we are at the last element, and we
     * found it, then we need to keep the name around so that the
     * removal will go ahead as planned.  Unfortunately, this will
     * probably also lock the to-be-removed vnode, which may or may
     * not be a good idea.  I'll have to look at the bits of
     * cfs_remove to make sure.  We'll only save the name if we did in
     * fact find the name, otherwise cfs_nb_remove won't have a chance
     * to free the pathname.  
     */
    if ((cnp->cn_nameiop == DELETE)
	&& (cnp->cn_flags & ISLASTCN)
	&& !result)
    {
	cnp->cn_flags |= SAVENAME;
    }

    /* 
     * If the lookup went well, we need to (potentially?) unlock the
     * parent, and lock the child.  We are only responsible for
     * checking to see if the parent is supposed to be unlocked before
     * we return.  We must always lock the child (provided there is
     * one, and (the parent isn't locked or it isn't the same as the
     * parent.)  Simple, huh?  We can never leave the parent locked unless
     * we are ISLASTCN
     */
    if (!result || (result == EJUSTRETURN)) {
	if (!(cnp->cn_flags & LOCKPARENT) || !(cnp->cn_flags & ISLASTCN)) {
	    if ((result = VOP_UNLOCK(ap->a_dvp))) {
		return result; 
	    }	    
	    /* 
	     * The parent is unlocked.  As long as there is a child,
	     * lock it without bothering to check anything else. 
	     */
	    if (*ap->a_vpp) {
		if ((result = VOP_LOCK(*ap->a_vpp))) {
		    printf("cfs_nb_lookup: ");
		    panic("unlocked parent but couldn't lock child");
		}
	    }
	} else {
	    /* The parent is locked, and may be the same as the child */
	    if (*ap->a_vpp && (*ap->a_vpp != ap->a_dvp)) {
		/* Different, go ahead and lock it. */
		if ((result = VOP_LOCK(*ap->a_vpp))) {
		    printf("cfs_nb_lookup: ");
		    panic("unlocked parent but couldn't lock child");
		}
	    }
	}
    } else {
	/* If the lookup failed, we need to ensure that the leaf is NULL */
	/* Don't change any locking? */
	*ap->a_vpp = NULL;
    }
    return result;
}

int
cfs_nb_create(ap)
    struct vop_create_args *ap;
{
    char                   tname[MAXPATHLEN];   /* overkill, but... */
    struct componentname  *cnp = ap->a_cnp;
    int                    result;

    ENTRY;
    /* All creates are exclusive XXX */
    /* I'm assuming the 'mode' argument is the file mode bits XXX */
    bcopy(cnp->cn_nameptr, tname,  cnp->cn_namelen);
    tname[cnp->cn_namelen] = '\0';

    result = cfs_create(ap->a_dvp, tname, ap->a_vap, EXCL, 
			ap->a_vap->va_mode, ap->a_vpp, cnp->cn_cred,
			cnp->cn_proc);

    /* Locking strategy. */
    /*
     * In NetBSD, all creates must explicitly vput their dvp's.  We'll
     * go ahead and use the LOCKLEAF flag of the cnp argument.
     * However, I'm pretty sure that create must return the leaf
     * locked; so there is a DIAGNOSTIC check to ensure that this is
     * true.
     */
    vput(ap->a_dvp);
    if (!result) {
	if (cnp->cn_flags & LOCKLEAF) {
	    if ((result = VOP_LOCK(*ap->a_vpp))) {
		printf("cfs_nb_create: ");
		panic("unlocked parent but couldn't lock child");
	    }
	}
#ifdef DIAGNOSTIC
	else {
	    printf("cfs_nb_create: LOCKLEAF not set!\n");
	}
#endif /* DIAGNOSTIC */
    }
    /* Have to free the previously saved name */
    /* 
     * This condition is stolen from ufs_makeinode.  I have no idea
     * why it's here, but what the hey...
     */
    if ((cnp->cn_flags & SAVESTART) == 0) {
	FREE(cnp->cn_pnbuf, M_NAMEI);
    }
    return result;
}

int
cfs_nb_remove(ap)
    struct vop_remove_args *ap;
{
    char                   tname[MAXPATHLEN];   /* overkill, but... */
    struct componentname  *cnp = ap->a_cnp;
    int                    result;

    ENTRY;
    bcopy(cnp->cn_nameptr, tname,  cnp->cn_namelen);
    tname[cnp->cn_namelen] = '\0';

    result = cfs_remove(ap->a_dvp, tname, cnp->cn_cred, cnp->cn_proc);

    /* 
     * Regardless of what happens, we have to unconditionally drop
     * locks/refs on parent and child.  (I hope).  This is based on
     * what ufs_remove seems to be doing.
     */
    if (ap->a_dvp == ap->a_vp) {
	vrele(ap->a_vp);
    } else {
	vput(ap->a_vp);
    }
    vput(ap->a_dvp);

    if ((cnp->cn_flags & SAVESTART) == 0) {
	FREE(cnp->cn_pnbuf, M_NAMEI);
    }
    return (result);
}

int
cfs_nb_link(ap)
    struct vop_link_args *ap;
{
    char                   tname[MAXPATHLEN];   /* overkill, but... */
    struct componentname  *cnp = ap->a_cnp;
    int                    result;

    ENTRY;

    if (cfsdebug & CFSDBGMSK(CFS_LINK)) {
	struct cnode *cp;
	struct cnode *tdcp;

	cp = VTOC(ap->a_vp);
	tdcp = VTOC(ap->a_tdvp);
	myprintf(("nb_link:   vp fid: (%x.%x.%x)\n",
		  cp->c_fid.Volume, cp->c_fid.Vnode, cp->c_fid.Unique));
	myprintf(("nb_link: tdvp fid: (%x.%x.%x)\n",
		  tdcp->c_fid.Volume, tdcp->c_fid.Vnode, tdcp->c_fid.Unique));
	
    }

    bcopy(cnp->cn_nameptr, tname,  cnp->cn_namelen);
    tname[cnp->cn_namelen] = '\0';

    /* 
     * WARNING! XXX 
     * The link system call, in it's infinite wisdom, packs it's
     * arguments as (directory-in-which-link-goes, vnode-to-link-to,
     * name) but VOP_LINK expects (vnode, directory, name).  As a
     * result, tdvp is *really* what vp should be, and vice versa.
     * So, DON'T PANIC.  I *know* they're backwards here.  Sigh.
     */

    /*
     * According to the ufs_link operation here's the locking situation:
     *     We enter with the thing called "vp" (the directory) locked.
     *     We must unconditionally drop locks on "vp"
     *
     *     We enter with the thing called "tdvp" (the linked-to) unlocked,
     *       but ref'd (?)
     *     We seem to need to lock it before calling cfs_link, and
     *       unconditionally unlock it after.
     */
    
    if ((ap->a_vp != ap->a_tdvp) && (result = VOP_LOCK(ap->a_tdvp))) {
	goto exit;
    }
	
    result = cfs_link(ap->a_tdvp, ap->a_vp, tname, cnp->cn_cred, 
		      cnp->cn_proc);

 exit:

    if (ap->a_vp != ap->a_tdvp) {
	VOP_UNLOCK(ap->a_tdvp);
    }
    vput(ap->a_vp);

    /* Drop the name buffer if we don't need to SAVESTART */
    if ((cnp->cn_flags & SAVESTART) == 0) {
	FREE(cnp->cn_pnbuf, M_NAMEI);
    }

    
    
    return result;
}

int
cfs_nb_rename(ap)
    struct vop_rename_args *ap;
{
    char                   fname[MAXPATHLEN];   /* overkill, but... */
    char                   tname[MAXPATHLEN];   /* overkill, but... */
    struct componentname  *fcnp = ap->a_fcnp;
    struct componentname  *tcnp = ap->a_tcnp;
    int                    result;

    ENTRY;
    bcopy(fcnp->cn_nameptr, fname,  fcnp->cn_namelen);
    bcopy(tcnp->cn_nameptr, tname,  tcnp->cn_namelen);
    fname[fcnp->cn_namelen] = '\0';
    tname[tcnp->cn_namelen] = '\0';

    /* Hmmm.  The vnodes are already looked up.  Perhaps they are locked?
       This could be Bad. XXX */
#ifdef DIAGNOSTIC
    if ((fcnp->cn_cred != tcnp->cn_cred)
	|| (fcnp->cn_proc != tcnp->cn_proc))
    {
	panic("cfs_nb_rename: component names don't agree");
    }
#endif DIAGNOSTIC
    result = cfs_rename(ap->a_fdvp, fname, ap->a_tdvp, tname, fcnp->cn_cred,
			fcnp->cn_proc);
    
    /* It seems to be incumbent on us to drop locks on all four vnodes */
    /* From-vnodes are not locked, only ref'd.  To-vnodes are locked. */
    
    vrele(ap->a_fvp);
    vrele(ap->a_fdvp);

    if (ap->a_tvp) {
	if (ap->a_tvp == ap->a_tdvp) {
	    vrele(ap->a_tvp);
	} else {
	    vput(ap->a_tvp);
	}
    }

    vput(ap->a_tdvp);
    
    return result;
}

int
cfs_nb_mkdir(ap)
    struct vop_mkdir_args *ap;
{
    char                   tname[MAXPATHLEN];   /* overkill, but... */
    struct componentname  *cnp = ap->a_cnp;
    int                    result;

    ENTRY;
    bcopy(cnp->cn_nameptr, tname,  cnp->cn_namelen);
    tname[cnp->cn_namelen] = '\0';

    result = cfs_mkdir(ap->a_dvp, tname, ap->a_vap, ap->a_vpp, cnp->cn_cred,
		       cnp->cn_proc);
    /*
     * Currently, all mkdirs explicitly vput their dvp's.
     * It also appears that we *must* lock the vpp, since
     * lockleaf isn't set, but someone down the road is going
     * to try to unlock the new directory.
     */
    vput(ap->a_dvp);
    if (!result) {
	if ((result = VOP_LOCK(*ap->a_vpp))) {
	    panic("cfs_nb_mkdir: couldn't lock child");
	}
    }
    /* Have to free the previously saved name */
    /* 
     * ufs_mkdir doesn't check for SAVESTART before freeing the
     * pathname buffer, but ufs_create does.  For the moment, I'll
     * follow their lead, but this seems like it is probably
     * incorrect.  
     */
    FREE(cnp->cn_pnbuf, M_NAMEI);
    return (result);
}

int
cfs_nb_rmdir(ap)
    struct vop_rmdir_args *ap;
{
    char                   tname[MAXPATHLEN];   /* overkill, but... */
    struct componentname  *cnp = ap->a_cnp;
    int                    result;

    ENTRY;
    bcopy(cnp->cn_nameptr, tname,  cnp->cn_namelen);
    tname[cnp->cn_namelen] = '\0';

    result = cfs_rmdir(ap->a_dvp, tname, cnp->cn_cred, cnp->cn_proc);
    
    /*
     * regardless of what happens, we need to drop locks/refs on the 
     * parent and child.  I think. 
     */
    if (ap->a_dvp == ap->a_vp) {
	vrele(ap->a_vp);
    } else {
	vput(ap->a_vp);
    }
    vput(ap->a_dvp);

    if ((cnp->cn_flags & SAVESTART) == 0) {
	FREE(cnp->cn_pnbuf, M_NAMEI);
    }
    return (result);
}

int
cfs_nb_symlink(ap)
    struct vop_symlink_args *ap;
{
    /* 
     * XXX I'm assuming the following things about cfs_symlink's
     * arguments: 
     *       t(foo) is the new name/parent/etc being created.
     *       lname is the contents of the new symlink. 
     */
    char                   tname[MAXPATHLEN];   /* overkill, but... */
    struct componentname  *cnp = ap->a_cnp;
    int                    result;

    ENTRY;
    bcopy(cnp->cn_nameptr, tname,  cnp->cn_namelen);
    tname[cnp->cn_namelen] = '\0';

    /* XXX What about the vpp argument?  Do we need it? */
    /* 
     * Here's the strategy for the moment: perform the symlink, then
     * do a lookup to grab the resulting vnode.  I know this requires
     * two communications with Venus for a new sybolic link, but
     * that's the way the ball bounces.  I don't yet want to change
     * the way the Mach symlink works.  When Mach support is
     * deprecated, we should change symlink so that the common case
     * returns the resultant vnode in a vpp argument.
     */

    result = cfs_symlink(ap->a_dvp, tname, ap->a_vap, ap->a_target,
			cnp->cn_cred, cnp->cn_proc);

    if (!result) {
	result = cfs_lookup(ap->a_dvp, tname, ap->a_vpp, cnp->cn_cred,
			    cnp->cn_proc);
    }
    
    /* 
     * Okay, now we have to drop locks on dvp.  vpp is unlocked, but
     * ref'd.  It doesn't matter what happens in either symlink or
     * lookup.  Furthermore, there isn't any way for (dvp == *vpp), so
     * we don't bother checking.  
     */
    
    vput(ap->a_dvp);
    if (*ap->a_vpp) VN_RELE(*ap->a_vpp);

    /* 
     * Free the name buffer 
     */
    if ((cnp->cn_flags & SAVESTART) == 0) {
	FREE(cnp->cn_pnbuf, M_NAMEI);
    }
    return result;
}

int
cfs_nb_readdir(ap)
    struct vop_readdir_args *ap;
{
    ENTRY;
    return (cfs_readdir(ap->a_vp, ap->a_uio, ap->a_cred, ap->a_eofflag,
			ap->a_cookies, ap->a_ncookies, ap->a_uio->uio_procp));
}

int
cfs_nb_bmap(ap)
    struct vop_bmap_args *ap;
{
    /* XXX on the global proc */
    ENTRY;

    return (cfs_bmap(ap->a_vp, ap->a_bn, ap->a_vpp, ap->a_bnp, GLOBAL_PROC));
}

int
cfs_nb_strategy(ap)
    struct vop_strategy_args *ap;
{
    ENTRY;
    /* XXX  for the GLOBAL_PROC */
    return (cfs_strategy(ap->a_bp, GLOBAL_PROC));
}

/***************************** NetBSD-only vnode operations */
int
cfs_nb_reclaim(ap) 
    struct vop_reclaim_args *ap;
{
    struct vnode *vp = ap->a_vp;

    ENTRY;
#ifdef DIAGNOSTIC
    if (vp->v_usecount != 0) 
	vprint("cfs_nb_reclaim: pushing active", vp);
    if (VTOC(vp)->c_ovp)
	panic("cfs_nb_reclaim: c_ovp not void");
#endif DIAGNOSTIC
    cache_purge(vp);
    cfs_free(VTOC(vp));
    VTOC(vp) = NULL;
    return (0);
}

int
cfs_nb_lock(ap)
    struct vop_lock_args *ap;
{
    struct vnode *vp = ap->a_vp;
    struct cnode *cp;
    struct proc  *p = curproc; /* XXX */
    
    ENTRY;
    cp = VTOC(vp);

    if (lockdebug) {
	myprintf(("Attempting lock on %d.%d.%d\n",
		  cp->c_fid.Volume, cp->c_fid.Vnode, cp->c_fid.Unique));
    }
 start:
    while (vp->v_flag & VXLOCK) {
	vp->v_flag |= VXWANT;
	sleep((caddr_t)vp, PINOD);
    }
    if (vp->v_tag == VT_NON)
	return (ENOENT);

    if (cp->c_flags & CN_LOCKED) {
	cp->c_flags |= CN_WANTED;
#ifdef DIAGNOSTIC
	myprintf(("cfs_nb_lock: lock contention\n"));
#endif
	(void) sleep((caddr_t)cp, PINOD);
	goto start;
    }
    cp->c_flags |= CN_LOCKED;
    return (0);
}

int
cfs_nb_unlock(ap)
    struct vop_unlock_args *ap;
{
    struct cnode *cp = VTOC(ap->a_vp);

    ENTRY;
    if (lockdebug) {
	myprintf(("Attempting unlock on %d.%d.%d\n",
		  cp->c_fid.Volume, cp->c_fid.Vnode, cp->c_fid.Unique));
    }
#ifdef DIAGNOSTIC
    if ((cp->c_flags & CN_LOCKED) == 0) 
	panic("cfs_unlock: not locked");
#endif
    cp->c_flags &= ~CN_LOCKED;
    if (cp->c_flags & CN_WANTED) {
	cp->c_flags &= ~CN_WANTED;
	wakeup((caddr_t)cp);
    }
    return (0);
}

int
cfs_nb_islocked(ap)
    struct vop_islocked_args *ap;
{
    ENTRY;
    if (VTOC(ap->a_vp)->c_flags & CN_LOCKED)
	return (1);
    return (0);
}

struct vnodeopv_desc cfs_vnodeop_opv_desc = 
        { &cfs_vnodeop_p, cfs_vnodeop_entries };

/* How one looks up a vnode given a device/inode pair: */

int
cfs_grab_vnode(dev_t dev, ino_t ino, struct vnode **vpp)
{
    /* This is like VFS_VGET() or igetinode()! */
    int           error;
    struct mount *mp;

    if (!(mp = devtomp(dev))) {
	myprintf(("cfs_grab_vnode: devtomp(%d) returns NULL\n", dev));
	return(ENXIO);
    }

    /* XXX - ensure that nonzero-return means failure */
    error = VFS_VGET(mp,ino,vpp);
    if (error) {
	myprintf(("cfs_grab_vnode: iget/vget(%d, %d) returns %x, err %d\n", 
		  dev, ino, *vpp, error));
	return(ENOENT);
    }
    return(0);
}

void
print_vattr( attr )
	struct vattr *attr;
{
    char *typestr;

    switch (attr->va_type) {
    case VNON:
	typestr = "VNON";
	break;
    case VREG:
	typestr = "VREG";
	break;
    case VDIR:
	typestr = "VDIR";
	break;
    case VBLK:
	typestr = "VBLK";
	break;
    case VCHR:
	typestr = "VCHR";
	break;
    case VLNK:
	typestr = "VLNK";
	break;
    case VSOCK:
	typestr = "VSCK";
	break;
    case VFIFO:
	typestr = "VFFO";
	break;
    case VBAD:
	typestr = "VBAD";
	break;
    default:
	typestr = "????";
	break;
    }


    myprintf(("attr: type %s mode %d uid %d gid %d fsid %d rdev %d\n",
	      typestr, (int)attr->va_mode, (int)attr->va_uid,
	      (int)attr->va_gid, (int)attr->va_fsid, (int)attr->va_rdev));
    
    myprintf(("      fileid %d nlink %d size %d blocksize %d bytes %d\n",
	      (int)attr->va_fileid, (int)attr->va_nlink, 
	      (int)attr->va_size,
	      (int)attr->va_blocksize,(int)attr->va_bytes));
    myprintf(("      gen %ld flags %ld vaflags %d\n",
	      attr->va_gen, attr->va_flags, attr->va_vaflags));
    myprintf(("      atime sec %d nsec %d\n",
	      (int)attr->va_atime.ts_sec, (int)attr->va_atime.ts_nsec));
    myprintf(("      mtime sec %d nsec %d\n",
	      (int)attr->va_mtime.ts_sec, (int)attr->va_mtime.ts_nsec));
    myprintf(("      ctime sec %d nsec %d\n",
	      (int)attr->va_ctime.ts_sec, (int)attr->va_ctime.ts_nsec));
}

/* How to print a ucred */
print_cred(cred)
	struct ucred *cred;
{

	int i;

	myprintf(("ref %d\tuid %d\n",cred->cr_ref,cred->cr_uid));

	for (i=0; i < cred->cr_ngroups; i++)
		myprintf(("\tgroup %d: (%d)\n",i,cred->cr_groups[i]));
	myprintf(("\n"));

}


/* vcfsattach: do nothing */
void
vcfsattach(n)
    int n;
{
}

#endif __NetBSD__
