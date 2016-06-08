#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>

#ifdef PATH_MAX
static int pathmax = PATH_MAX;   //假如limits.h中定义了PATH_MAX，就直接取limits.h定义的值
#else
static int pathmax = 0;  //如果limits.h并未定义PATH_MAX,暂且给定0，在后面会计算出这个变量的值
#endif

#define SUSV3  200112L

static long posix_version = 0;

/* If PATH_MAX is indeterminate, no guarantee this is adequate */
#define PATH_MAX_GUESS  1024

char *
path_alloc(int *sizep) /* also return allocated size, if nonnull */
{
 char *ptr;
 int size;
 int errno;

 if (posix_version == 0)
  posix_version = sysconf(_SC_VERSION); //获取posix标准版本号

 if (pathmax == 0) {  /* first time through */  //如果在limits.h中未定义PATH_MAX
  errno = 0;      //将错误号置0
  if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0) {           //假定当前工作目录是 / ，获取相对于 / 的相对地址长度，也就相当是绝对地址的长度，当返回值小于0时，可能有下面两种情况
     if (errno == 0)
    pathmax = PATH_MAX_GUESS; /* it's indeterminate */   //当错误号依旧为0，也就是pathconf()并为发生错误，这种状态下，我们认定PATH_MAX为不确定的，我们只能猜测某个值，这里猜测为1024
   else
    printf("pathconf error for _PC_PATH_MAX"); //错误号不为0时，pathconf()执行出错
  } else {
   pathmax++;  /* add one since it's relative to root */   //当pathconf()返回>0的值时，认为PATH_MAX为运行时的限制值，故绝对地址是相对地址加上/的长度，也就是+1
  }
 }
 if (posix_version < SUSV3)
  size = pathmax + 1;  //当posix版本小于上述值时，绝对路径末尾要加上null
 else
  size = pathmax;  //以后的版本不需要在路径末尾加上null

 if ((ptr = malloc(size)) == NULL)       
  printf("malloc error for pathname");

 if (sizep != NULL)
  *sizep = size;
 return(ptr);
}




typedef int Myfunc(const char *, const struct stat *, int);
static Myfunc myfunc;
static int myftw(char *, Myfunc *);
static int dopath(Myfunc *);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char *argv[])
{
	int ret;
	if(argc != 2)
	{
		printf("usage:ftw <starting_pathname>");
		exit(0);
	}

	ret = myftw(argv[1], myfunc);
	ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
	
	if(ntot == 0)
		ntot = 1;

	printf("regular files = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);
	
	printf("directories = %7ld, %5.2f %%\n", ndir, ndir*100.0/ntot);

	printf("block special = %7ld, %5.2f %%\n", nblk, nblk*100.0/ntot);

	printf("char special = %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);

	printf("FIFOs = %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot);

	printf("symbolic link = %7ld, %5.2f %%\n", nslink, nslink*100.0/ntot);

	printf("sockets = %7ld, %5.2f %%\n", nsock, nsock*100.0/ntot);

	exit(ret);
}


#define FTW_F 1
#define FTW_D 2
#define FTW_DNR 3
#define FTW_NS 4

static char *fullpath;
static int myftw(char *pathname, Myfunc *func)
{
	int len;
	fullpath = path_alloc(&len);
	
	strncpy(fullpath, pathname, len);
	fullpath[len-1] = 0;
	
	return (dopath(func));
}

static int dopath(Myfunc* func)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	int ret;
	char *ptr;
	
	if(lstat(fullpath, &statbuf) < 0)
		return (func(fullpath, &statbuf, FTW_NS));

	if(S_ISDIR(statbuf.st_mode) == 0)
		return (func(fullpath, &statbuf, FTW_F));
	
	if((ret = func(fullpath, &statbuf, FTW_D)) != 0)
		return (ret);
	
	ptr = fullpath + strlen(fullpath);
	
	*ptr++ = '/';
	*ptr = 0;
	
	if((dp = opendir(fullpath)) == NULL)
		return (func(fullpath, &statbuf, FTW_DNR));

	while((dirp = readdir(dp)) != NULL)
	{
		if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
			continue;
	
		strcpy(ptr, dirp->d_name);
		
		if((ret = dopath(func)) != 0)
			break;
	}

	ptr[-1] = 0;
	
	if(closedir(dp) < 0)
		printf("can't close directory %s", fullpath);
	
	return (ret);
}


static int myfunc(const char *pathname, const struct stat *statptr, int type)
{
	switch(type)
	{
		case FTW_F:
			switch(statptr->st_mode & S_IFMT)
			{
				case S_IFREG: nreg++; break;
				case S_IFBLK: nblk++; break;
				case S_IFCHR: nchr++; break;
				case S_IFIFO: nfifo++; break;
				case S_IFLNK: nslink++; break;
				case S_IFSOCK: nsock++; break;
				case S_IFDIR: 
						printf("for S_IFDIR for %s", pathname);
			}
			break;

		case FTW_D:
			ndir++;
			break;
		
		case FTW_DNR:
			printf("can't read directory %s", pathname);
			break;
	
		case FTW_NS:
			printf("stat error for %s", pathname);
			break;
		
		default:
			printf("unknown type %d for pathname %s", type, pathname);		
	}
	return 0;
}
