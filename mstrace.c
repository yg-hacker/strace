#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/reg.h>
#include "macroc.h"

int main (void)
{
	pid_t pid;
	pid = fork();
	long int syscallID;
	int flag =0, newline = 0;
	unsigned int value;
	Macroc();
	if (pid < 0)
	{
		printf ("fork fail!\n");
		return -1;
	}
	else if (pid == 0)
	{
		ptrace(PTRACE_TRACEME, 0,NULL , NULL);
		execl("./hello","hello", NULL);
	}
	else
	{
		int val;
		wait (&val);
		if (WIFEXITED(val))
			return 0;
		syscallID = ptrace (PTRACE_PEEKUSER, pid, ORIG_RAX*8, NULL);
		printf ("Process executed system call ID:%ld\n", syscallID);
		printf("\n函数名为 %s\n", name[syscallID]);
		ptrace (PTRACE_SYSCALL, pid, NULL, NULL);
		while(1)
		{
			wait(&val);
			if (WIFEXITED (val))
				return 0;
			if (flag == 0)
			{
				if(newline) newline = 2;
				syscallID = ptrace (PTRACE_PEEKUSER, pid, ORIG_RAX*8, NULL);
//				printf("\nthe name is %s\n", name[syscallID]);	
//				int syscallNAME;
//				ptrace(PTRACE_GETREGS, pid, 0x1C*4, syscallNAME);
//				printf("%d\n", syscallNAME);
				
				unsigned int syscallspara[7];
				syscallspara[0] = ptrace (PTRACE_PEEKUSER, pid, 0x00*4, NULL);
				syscallspara[1] = ptrace (PTRACE_PEEKUSER, pid, 0x04*4, NULL);
				syscallspara[2] = ptrace (PTRACE_PEEKUSER, pid, 0x08*4, NULL);
				syscallspara[3] = ptrace (PTRACE_PEEKUSER, pid, 0x0C*4, NULL);
				syscallspara[4] = ptrace (PTRACE_PEEKUSER, pid, 0x10*4, NULL);
				syscallspara[5] = ptrace (PTRACE_PEEKUSER, pid, 0x14*4, NULL);
				printf("参数为%#x %#x %#x %#x %#x %#x\n", syscallspara[0],syscallspara[1],syscallspara[2],syscallspara[3],syscallspara[4],syscallspara[5]);


				if(syscallID == 1) newline = 1;
				printf ("Process executed system call ID:%ld",syscallID);
				printf("\n函数名为%s\n", name[syscallID]);

				if(newline == 2) printf("\n");
				flag = 1;
			}
			else
			{
				value = ptrace (PTRACE_PEEKUSER, pid, RAX*4,NULL);
				printf ("返回值=\t%#x\n", value);
				flag = 0;
			}
			ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		}
	}
}
