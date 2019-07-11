#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid1, pid2, pid3;
   

    pid1 = fork();
    if(pid1 < 0)
    {
          perror("Erreur de création du processus\n");
          exit(EXIT_FAILURE);
    }

    if(pid1>0)

    {
      pid2 = fork();
      if(pid2 < 0)
      {
      perror("Erreur de création du second processus\n");
      exit(EXIT_FAILURE);
      } 

      if(pid2>0)
      {
          pid3 = fork();
            if(pid3 < 0)
            {
              perror("Erreur de création du troisieme processus\n");
              exit(EXIT_FAILURE);
            }

           if(pid3>0)
            {
                printf("pid du premier processus  %d\n", pid1);
                printf("pid du second processus %d\n", pid2);
                printf("pid su troisieme processus  %d\n",pid3);
            }
        }


    }
}
