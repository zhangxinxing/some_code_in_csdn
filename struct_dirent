/*
* linux struct dirent,DIR,struct stat
* date: 2015-7-16
* author: zhang
* compiled by gcc
*/

#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

int main(int argc,char **argv)
{
	struct dirent *dirp; 
	struct stat buffer; 
	DIR *dp; //结构体，具体参阅http://www.liweifan.com/2012/05/13/linux-system-function-files-operation/
	
	if (argc != 2)
	{
		printf("argument is error!\n");
		return 0;
	}
	
	if ((dp = opendir(argv[1])) == NULL)
	{
		printf("file path is error!\n");
		return 0;
	}
	

	while(dirp = readdir(dp))
	{
		printf("filename: %s\n",dirp->d_name);//文件名
		stat(dirp->d_name,&buffer);
	//	printf("%d\n",stat(dirp->d_name,&buffer));
		printf("st_mode: %d\n",buffer.st_mode); //文件访问权限
		printf("st_uid: %d\n",buffer.st_uid);   //所有者用户识别号
		printf("st_mtime: %d\n",buffer.st_mtime);//最后一次修改该文件的时间
		printf("\n");
	
		
	}
	
	closedir(dp);
		
	return 0;
}
