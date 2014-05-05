/*
 * NAME:
 * fmfilesystem
 *
 * PURPOSE:
 * To access filesystem functions.
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * NA
 *
 * OUTPUT:
 * NA
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, METNO/FOU, 16.10.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 20.10.2009: Added fmreaddir, fmfilelist_sort.
 * Øystein Godøy, METNO/FOU, 21.10.2009: Added fmstarcdirs
 *
 * ID:
 * $Id$
 */

#include <fmutil.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

/*
 * Allocate memory to hold filelist.
 */
int fmfilelist_alloc(fmfilelist *mylist,int nfiles) {

    char *where = "fmfilelist_alloc";
    char what[FMSTRING256];

    if (fmalloc_char_vector(&(mylist->path),FMSTRING1024)) {
	sprintf(what,"Could not allocate vector for path");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }

    if (fmalloc_byte_2d(&(mylist->filename),nfiles,FMSTRING128)) {
	sprintf(what,"Could not allocate array for filenames");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }

    mylist->nfiles = nfiles;

    return(FM_OK);
}

/*
 * Free memory allocated for filelist.
 */
int fmfilelist_free(fmfilelist *mylist) {

    fmfree_byte_2d(mylist->filename,mylist->nfiles);
    fmfree_char_vector(mylist->path);

    mylist->path = NULL;
    mylist->filename = NULL;
    mylist->nfiles = 0;

    return(FM_OK);
}

/*
 * Sort the list with filenames in alphabetical order.
 */
int fmfilelist_sort(fmfilelist *flist) {

    const char *where="fmfilelist_sort";
    char mystring[FMSTRING512];
    int i, j;

    for (i=0; i<flist->nfiles; i++) {
	for (j=0;j<(flist->nfiles)-1;j++) {
	    if (strcmp((flist->filename)[j],(flist->filename)[j+1]) > 0) {
		strcpy(mystring, (flist->filename)[j+1]);
		strcpy((flist->filename)[j+1],(flist->filename)[j]);
		strcpy((flist->filename)[j],mystring);
	    }
	}
    } 

    return(FM_OK);
}

/*
 * Create symbolic links, it should test on return values to see if the error
 * could be bypassed...
 */
int fmlinkfiles(fmfilelist src, fmfilelist dst) {

    char *where = "fmlinkfiles";
    char what[FMSTRING256];
    char file1[FMSTRING1024],file2[FMSTRING1024];
    int i, len1, len2;

    if (src.nfiles != dst.nfiles) {
	sprintf(what,"Inconsistent number of files, src: %d, dst: %d\n",
	src.nfiles,dst.nfiles);
	fmerrmsg(where,what);
	return(FM_SYNTAX_ERR);
    }

    for (i=0;i<dst.nfiles;i++) {
	len1 = strlen(src.path)+strlen(src.filename[i])+2;
	len2 = strlen(dst.path)+strlen(dst.filename[i])+2;
	if (len1 > FMSTRING1024 || len2 > FMSTRING1024) {
	    sprintf(what,"Filelists differ in length");
	    fmerrmsg(where,what);
	    return(FM_SYNTAX_ERR);
	}
	sprintf(file1,"%s/%s",src.path,src.filename[i]);
	sprintf(file2,"%s/%s",dst.path,dst.filename[i]);
	if (fmfileexist(file2) == FM_OK) continue;
	if (symlink(file1,file2) < 0) {
	    sprintf(what,
		    "Could not link files %s and %s",
		   file1, file2);
	    fmerrmsg(where,what);
	    return(FM_IO_ERR);
	}
    }

    return(FM_OK);
}

/*
 * Remove the selected files from the filesystem.
 */
int fmremovefiles(fmfilelist mylist) {

    char *where = "fmremovefiles";
    char what[FMSTRING256];
    char *filename;
    int i;

    if (fmalloc_char_vector(&(filename),FMSTRING1024)) {
	sprintf(what,"Could not allocate filename string");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }

    for (i=0;i<mylist.nfiles;i++) {
	if ((strlen(mylist.path)+strlen(mylist.filename[i])+2) > FMSTRING1024) {
	    sprintf(what,
	    "Filename (%s/%s) is too long",
	    mylist.path,mylist.filename[i]);
	    fmerrmsg(where,what);
	    return(FM_SYNTAX_ERR);
	}
	sprintf(filename,"%s/%s",mylist.path,mylist.filename[i]);
	printf("Removing: %s\n",filename);
	if (unlink(filename)) {
	    sprintf(what,"Could not remove %s", filename);
	    fmerrmsg(where,what);
	}
    }

    fmfree_char_vector(filename);

    return(FM_OK);
}

/*
 * Read the contents of a directory. Hidden files are discarded.
 */
int fmreaddir(char *idir, fmfilelist *flist) {

    const char *where="fmreaddir";
    DIR *dirp;
    struct dirent *direntp;
    int nf, i;

    dirp = opendir(idir);
    if (!dirp) {
	fmerrmsg(where,"Opendir did not work properly");
	return(FM_IO_ERR);
    } 

    nf = 0;
    while ((direntp = readdir(dirp)) != NULL) {
	if (direntp->d_name[0] == '.') {
	    continue;
	}
	nf++;
    }
    if (nf == 0) {
	fmerrmsg(where,"Directory contains no files.");
	return(FM_IO_ERR);
    }

    if (fmfilelist_alloc(flist, nf)) {
	fmerrmsg(where,"Could not allocate flist");
	return(FM_MEMALL_ERR);
    }
    sprintf(flist->path,"%s",idir);

    rewinddir(dirp);
    i = 0;
    while ((direntp = readdir(dirp)) != NULL) {
	if (direntp->d_name[0] == '.') {
	    continue;
	}
	strcpy((flist->filename)[i],direntp->d_name);
	i++;
    }
    if (closedir(dirp) != 0) {
	fmerrmsg(where,"check_avhrr","Could not close directory");
	return(FM_IO_ERR);
    }
    if (i != nf) {
	fmerrmsg(where," First time loop: %d, second time loop: %d files\n", 
		nf, i);
	return(FM_OTHER_ERR);
    }

    return(FM_OK);
}

/*
 * Given a start and end time, generate the directory structure used at
 * /starc. Only the datebased structure is generated, the base directory,
 * e.g. /starc/DNMI_OBS, is added when used to read contents.
 */
int fmstarcdirs(fmtime start, fmtime end, fmstarclist *dirlist) {

    const char *where="fmstarcdirs";
    fmsec1970 epstart, epend, ep;
    fmtime dumpdate;
    const int daysec=24*3600;
    int days, i;

    epstart = tofmsec1970(start);
    epend = tofmsec1970(end);

    days = (int) ceil((double) epend- (double) epstart)/(24.*3600.);

    if (fmalloc_byte_2d(&(dirlist->dirname), days, FMSTRING16)) {
	fmerrmsg(where,"Could not allocate dirlist");
	return(FM_MEMALL_ERR);
    }
    dirlist->nfiles = days;

    ep = epstart;
    for (i=0;i<days;i++) {
	if (tofmtime((fmsec1970) ep, &dumpdate)) {
	    fmerrmsg(where,"Could not convert to fmtime object");
	    return(FM_OTHER_ERR);
	}
	sprintf((dirlist->dirname)[i],"%4d/%02d/%02d",
		dumpdate.fm_year,
		dumpdate.fm_mon,
		dumpdate.fm_mday
		);
	ep += daysec;
    }

    return(FM_OK);
}
