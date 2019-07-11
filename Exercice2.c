/* 1-) La  valeur finale dans la m�moire partag�e est 300 */
/* 2-) Pour resoudre le probl�me il faut cr�er une m�moire partag�e , puis ecrire une boucle pour chaque processus
*      afin qu'il puisse modifier la valeur 100 fois */
/* 3-) Impl�mentation */ 


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h> 
#include <sys/shm.h>  
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>


#define ITER 100



int main(int argc, char *argv[])
{
    pid_t pid1, pid2, pid3; // Cr�ation des trois processus
    int i,a=0 ;
    key_t key = ftok("shmfile",65);               //ftok genere une unique cl�
    int shmid = shmget(key,1024,0666|IPC_CREAT);   
    
     char *str = (char*) shmat(shmid,(void*)0,0);
     sprintf(str, "%d", a);                           //initialise la m�moire partag�e a 0 
   
     pid1 = fork();
    if(pid1 < 0)
    {
          perror("Erreur de cr�ation du processus\n");
          exit(EXIT_FAILURE);
    }

    if(pid1>0)

    { printf("Creation du processus 1\n");
 
      for (i=0 ; i < ITER; i++ )
      { 
         a++ ;
      }
       sprintf(str, "%d", a);                                        // modification de la valeur apres incr�mentation de a 
     printf("Valeur apres incrementation par processus 1:%s\n" ,str); 
      pid2 = fork();
      if(pid2 < 0)
      {
      perror("Erreur de cr�ation du second processus\n");
      exit(EXIT_FAILURE);
      } 

      if(pid2>0)
      {    printf("Creation du processus 2\n");
 
      for (i=0 ; i < ITER; i++ )
      { 
         a++ ;
      }
       sprintf(str, "%d", a);                                        // modification de la valeur apres incr�mentation de a 
     printf("Valeur apres incrementation par processus 2:%s\n" ,str); 
          pid3 = fork();
            if(pid3 < 0)
            {
              perror("Erreur de cr�ation du troisieme processus\n");
              exit(EXIT_FAILURE);
            }

           if(pid3>0)
            {    printf("Creation du processus 3\n");
 
      for (i=0 ; i < ITER; i++ )
      { 
         a++ ;
      }
       sprintf(str, "%d", a);                                         // modification de la valeur apres incr�mentation de a 
     printf("Valeur apres incrementation par processus 3:%s\n" ,str); 
                printf("pid du premier processus  %d\n", pid1);
                printf("pid du second processus %d\n", pid2);
                printf("pid su troisieme processus  %d\n",pid3);
            }
        }


    }
 shmdt(str); 
shmctl(shmid,IPC_RMID,NULL);
}
