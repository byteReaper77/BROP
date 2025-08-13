#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h> 
#include <setjmp.h>

#define STACK_VALUE 256
#define RET_OFFSET (STACK_VALUE - 16)
sigjmp_buf jumpBuffer;
char stack[STACK_VALUE];
void s()
{
	printf("\e[0;33m[+] Stack Value : ===================================\e[0m\n");
}
void exitSycall()
{
	__asm__ volatile 
	(
    "syscall\n\t"
    :
    : "A"(0x3C), 
      "D"(0)
    : "rcx", 
      "r11", 
      "memory"
    );
    fflush(NULL);
}
void stackvalue(void (*ptr)())
{
	
	memcpy(stack, &ptr, sizeof(ptr));
	void (*rP)();
	memcpy(&rP, stack, sizeof(rP));
//stack[STACK_VALUE-1] = '\0';
	printf("%p\n", rP);

	if (memcmp(&ptr, stack, sizeof(ptr)) == 0)
	{
		 printf("\e[0;36m[+] Found address in stack (%p)\e[0m\n", (void*)ptr);
		 
	} 
	else
	{
		printf("\e[0;31m[-] Not found address in stack (%p)!\e[0m\n", (void*)ptr);
		
	}
	
	
}


void a()
{
	__asm__ volatile
	(
		"nop\n\t"
		"nop\n\t"
		:
		:
		:
	);
}

void b()
{
	__asm__ volatile
	(
		"nop\n\t"
		"nop\n\t"
		:
		:
		:
	);
}

void sigsegv_handler(int sig) 
{
    const char msg[] = "\e[0;31m[-] Segfault/Illegal instruction caught, skipping address...\e[0m\n";
    write(STDERR_FILENO, 
    	msg, 
    	sizeof(msg)-1);
    siglongjmp(jumpBuffer, 
    	1);
}

void setOffset(void *addr)
{
    if (RET_OFFSET + (int)sizeof(addr) <= STACK_VALUE) 
    {
        memcpy(&stack[RET_OFFSET], &addr, sizeof(addr));
    } 
    else 
    {
        fprintf(stderr, "\e[0;31m[-] Set Offset: offset out of range\e[0m\n");
    }
}


void brop()
{
	uintptr_t start = 0x400000;
	uintptr_t end   = 0x401000;
	for (volatile uintptr_t pointer = start; pointer < end; pointer += 8)
	{
		
		if (sigsetjmp(jumpBuffer, 1) != 0)
		{
    		continue;
		}
		usleep(18000);
		setOffset((void *)pointer);
		uint64_t raxValue ;
		raxValue = (uint64_t)pointer;
		__asm__ volatile 
		(
			"jmp *%0\n\t"
			:
			: "r"(raxValue)
			:

		);
		
		char pidstr[32];
		pid_t pid = getpid();
		if (pid == 0)
		{
			printf("[-] PID Value Error !\e[0m\n");
		}
		int len = snprintf(pidstr, 
			sizeof(pidstr), 
			"%d", 
			(int)pid);
		if (len < 0 || len >= (int)sizeof(pidstr)) 
		{
		    fprintf(stderr, "[-] PID Format Error !\e[0mn");
		    exit(1);
		}
		printf("\e[0;34m[+] Write PID value SUCCESS.\e[0m\n");

		FILE *f =popen("ps aux", "r");
		if (f == NULL)
		{
			printf("\e[0;31m[-] Error Read File !\e[0m\n");
			exitSycall();
		}
		const char *word[] = {pidstr,
			"brop", 
			"BROP",
			"exploit",
			NULL}; 
		printf("[+] PID : %s\n", pidstr);
		int found = 0;
		char linePs[1500];
		while (fgets(linePs, 
			sizeof(linePs), 
			f) != NULL)
		{
			for (int j = 0; word[j] != NULL; ++j)
			{
				if (strstr(linePs, word[j]) != NULL)
				{
					found  = 1;
					break;
				}
			}
			if (found) break;
			
		}
		if (found) 
		{
		    printf("\e[0;35m[+] ========================================================\e[0m\n");
		    printf("\e[0;34m[+] Word found in ps aux PID !\e[0m\n");
		    printf("\e[0;34m[+] Return value addrress is Correct\e[0m\n");
		    printf("\e[0;35m============================================================\e[0m\n");
		} 
		else 
		{
		    printf("\e[0;35m[-] ========================================================\e[0m\n");
		    printf("\e[0;31m[-] Not found PID value in process.\e[0m\n");
		    printf("\e[0;31m[-] Program exit or crash.\e[0m\n");
		    printf("\e[0;35m============================================================\e[0m\n");
		}

		pclose(f);
		
		
	}

}
int main()
{
	signal(SIGSEGV, sigsegv_handler);
    signal(SIGILL,  sigsegv_handler);
	void (*ptr1)() = &a;
	void (*ptr2)() = &b;
	if ((void*)ptr1 == NULL || (void*)ptr2 == NULL)
	{
		printf("\e[0;31m[-] NULL pointer \"a()\"!\e[0m\n");
		printf("\e[0;31m[-] NULL pointer \"b()\"!\e[0m\n");
	}
	printf("\e[0;36m[+] Pointer Not NULL \"a()\".\e[0m\n");
	printf("\e[0;36m[+] Pointer Not NULL \"b()\".\e[0m\n");
	printf("\e[0;34m[+] Pointer a() : %p\e[0m\n", (void*)ptr1);
	printf("\e[0;34m[+] Pointer b() : %p\e[0m\n", (void*)ptr2) ;
	s();
	stackvalue((void*)ptr1);
	stackvalue((void*)ptr2);
	printf("\e[0;32m[+] I started trying to find the address using BROP...\e[0m\n");
	
	if (setjmp(jumpBuffer) == 0)
	{
		brop();
	}
	else 
	{
		printf("[-] Faild jump !\e[0m\n");
	}	
	return 0;
}



