#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

int main(int argc,char * argv[])
{
	int pid = fork();
	if(pid==0)
	{
		sleep(5);
	}
	else
	{
		int t = getpid();
		wait();
		invoked_syscalls(t);
		//invoked_syscalls(pid);
	}
	exit();


}