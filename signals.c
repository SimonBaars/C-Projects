#include<stdio.h>
#include<signal.h>
#include<unistd.h>

int breaking = 0;

void handler(int signo)
{
   switch (breaking) 
   {
       case 0:  printf("Don't kill me ...\n");
                break;
       case 1:  printf("You don't have to do that!\n");
                break;
       case 2:  printf("Stop it!\n");
                break;
       case 3:  printf("I'm out.\n");
                _Exit(0);
                break;
                
       default:  printf("Something went wrong.\n");
                _Exit(0);
                break;
  }

  breaking++;

}

int main(void)
{

  if (signal(SIGINT, handler) == SIG_ERR) 
          printf("SIGINT not catachable.\n");

  if (signal(SIGKILL, handler) == SIG_ERR) 
          printf("SIGKILL not catachable.\n");
  
  while(1) 
    sleep(1);
  
  return 0;
}