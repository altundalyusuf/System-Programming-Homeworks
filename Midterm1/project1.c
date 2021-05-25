/* Bu program parent ve child proccess'lerin anlaşılması için bir çalışmadır.*/


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int forkFunc(int pid)
{	
		if(pid>0){
			printf("a");
			return 0;
		}
			
		else if(pid==0){
			printf("B");
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
	int pid;
	pid = fork();
	
	for(int i = 0; i<10; ++i)
	{
		forkFunc(pid);
	}
	
	
	return 0;
}

