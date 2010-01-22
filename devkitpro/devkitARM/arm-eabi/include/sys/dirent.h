/* <dirent.h> includes <sys/dirent.h>, which is this file.  On a
   system which supports <dirent.h>, this file is overridden by
   dirent.h in the libc/sys/.../sys directory.  On a system which does
   not support <dirent.h>, we will get this file which uses #error to force
   an error.  */

#ifndef _dirent_h_
#define _dirent_h_

#include <sys/dir.h>
#include <sys/types.h>
#include <sys/syslimits.h>


#ifdef __cplusplus
extern "C" {
#endif
	
	struct dirent {
		ino_t	d_ino;
		char	d_name[NAME_MAX+1];
	};
	
	typedef struct {
		long int        position;
		DIR_ITER*       dirData;
		struct dirent   fileData;
	} DIR;
	
	int closedir(DIR *dirp);
	DIR *opendir(const char *dirname);
	struct dirent *readdir(DIR *dirp);
	int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
	void rewinddir(DIR *dirp);
	void seekdir(DIR *dirp, long int loc);
	long int telldir(DIR *dirp);
	
#ifdef __cplusplus
}
#endif

#endif // _dirent_h_