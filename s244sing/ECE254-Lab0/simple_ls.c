#include <stdio.h>
#include <sys/types.h>	// For opendir()
#include <dirent.h>	// For opendir()
#include <stdlib.h> 	// For exit()
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>	// For ctime()
#include <grp.h>	// For getgrgid()

#define STRLEN1 128
#define STRLEN2 64

int main( int argc, char* argv[] )
{
	DIR *p_dir;
	struct dirent *p_dirent;

	if( argc < 2 )
	{
		printf("Usage: %s <dir name>\n", argv[0]);
		exit(1);
	}

	if( (p_dir = opendir(argv[1])) == NULL )
	{
		printf("opendir(%s) failed\n", argv[1]);
		exit(1);
	}

	printf("Files currently in this directory:\n");
	printf("----------------------------------\n");

	while( (p_dirent = readdir(p_dir)) != NULL )
	{
		char *str_path = p_dirent->d_name;

		if ( str_path == NULL )
		{
			printf("Null pointer found!\n");
			exit(2);
		}
		else
		{
			printf("%s\n", str_path);
		}
	}

	printf("File permissions:\n");
	printf("----------------\n"); 
	int file;
	char str[] = "---";
	struct stat buf;

	for( file = 1; file < argc; file++ )
	{
		printf("Owner permission of %s: ", argv[file]);
		if( lstat(argv[file], &buf) < 0)
		{
			perror("lstat failed\n");
			continue;
		}
		
		mode_t mode = buf.st_mode;
		
		str[0] = (mode & S_IRUSR) ? 'r' : '-';
		str[1] = (mode & S_IWUSR) ? 'w' : '-';
		str[2] = (mode & S_IWUSR) ? 'x' : '-';

		printf("%s\n", str);
	}	

	printf("File type:\n");
	printf("---------\n");

	char *ptr;

	for (file = 1; file < argc; file++ )
	{
		printf("%s: ", argv[file]);
		if (lstat(argv[file], &buf) < 0)
		{
			perror("lstat failed");
			continue;
		}

		if( S_ISREG(buf.st_mode)) 	ptr = "regular file";
		else if (S_ISDIR(buf.st_mode))  ptr = "directory";
		else if (S_ISCHR(buf.st_mode))  ptr = "character special";
		else if (S_ISBLK(buf.st_mode))  ptr = "block special";
		else if (S_ISFIFO(buf.st_mode)) ptr = "fifo";
#ifdef S_ISLNK
		else if (S_ISLNK(buf.st_mode))  ptr = "symbolic link";
#endif
#ifdef S_ISSOCK
		else if (S_ISSOCK(buf.st_mode)) ptr = "socket";
#endif
		else				ptr = "unknown file bro :(";
		
		printf("%s\n", ptr);
	}


	printf("Various Times:\n");
	printf("-------------\n");

	for( file = 1; file < argc; file++ )
	{
		if (lstat(argv[file], &buf) < 0)
		{
			perror("lstat failed");
			continue;
		}
		
		time_t accesstime = buf.st_atime;
		char* timeptr = ctime(&accesstime);
		printf("Last Access time: %s\n", timeptr);

		time_t modifytime = buf.st_mtime;
		timeptr = ctime(&modifytime);
		printf("Last Modify time: %s\n", timeptr);
		
		time_t statuschangetime = buf.st_ctime;
		timeptr = ctime(&statuschangetime);
		printf("Last Status Change time: %s\n", timeptr);
	}
	
	printf("Ownerships:\n");
	printf("-----------\n");

	for( file = 1; file < argc; file++ )
	{
		if (stat(argv[file], &buf ) < 0 )
		{ 	
			perror("stat() failed");
			continue;
		}

		struct group* groupname_t;
		struct passwd* username_t;

		uid_t grpid = buf.st_uid;
		groupname_t = getgrgid(grpid);
		printf("group: %s\n", groupname_t->gr_name);
				
	}

	return 0;
}