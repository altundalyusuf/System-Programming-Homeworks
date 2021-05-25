/*  Bu program system call kullanılarak dosya işlemleri, string, array, pointer ve proccess kullanılmasının anlaşılması üzerine bir çalışmadır.*/


#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int wordNumber()
{
	int fd,t,i=0;
	int nwords = 0;
	char buf[254];

	fd = open("programs.txt", O_RDONLY);
	if(fd == -1)
	{
		perror("cannont open file");	return -1;
	}
	

	while((t = read(fd,buf,254)) > 0)
	{
		buf[t]='\0';
		while(buf[i]!='\0')
		{
			if(buf[i] == ' ' && buf[i+1]!=' ')
				nwords++;
			++i;
		}
	
		close(fd);
	}	
	return nwords+1;
}

int childProcess(char prog[],char **args)
{
	int pid;
	pid = fork();
	if(pid>0)
	{
		printf("\nParent ID: %d, pid: %d\n",getpid(),pid);
		return 0;
	}
			
	else if(pid==0)
	{
		printf("\nChild ID: %d, pid: %d",getpid(),pid);
		execv(prog, args);
		return 0;
	}
		
	else
	{
		perror("ERROR!");
		return -1;
			
	}
} 

int main(int argc, char **argv)
{
	int wn= wordNumber();
	int fd,t,len,i=0;
	char buf[254];
	
	
	char* split;
	
	fd = open("programs.txt", O_RDONLY);
	if(fd == -1)
	{
		perror("cannont open file");	return -1;
	}
	

	while((t = read(fd,buf,254)) > 0)
	{	
		buf[t]='\0';		
	}
	
	char **args = malloc(wn * sizeof(char *));		
	split= strtok(buf," ");
	while(split != NULL)
	{
		len=strlen(split);
		args[i] = malloc((len + 1) * sizeof(char));	
		strncpy(args[i], split ,len);			
		split=strtok(NULL," ");
		
		++i;	
	
	} 
	
	char prog[50] ="/usr/bin/";
	strcat(prog, args[0]);
	childProcess(prog,args);
	close(fd);
	
	return 0;
}

