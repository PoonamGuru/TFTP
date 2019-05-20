#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<stdio.h>
#include <unistd.h>
#include<string.h>
int main()
{
    int fd,ret;
    char buff[500];
    fd = open("f4.txt" , O_RDONLY);
    if(fd != -1)
    {
	do
	{
	    ret = read(fd, buff, 5);
	    buff[ret] = '\0';

	    if(ret != -1)
	    {
		printf("ret %d len %ld\n", ret , strlen(buff));
	//	puts(buff);
	    }
	}while(ret == 5);
    }
    return 0;
}
