
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
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
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#define MY_ADDR "127.0.0.1"
#define MY_PORT 56789
#define LISTEN_BACKLOG 50

union semun {
   int  val;
   struct semid_ds *buf;
   unsigned short  *array;
   struct seminfo  *__buf;

};

int main(int argc, char *argv[]){
   int sfd; // socket du serveur
   int cfd; // socket du client
   struct sockaddr_in my_addr; // socket addr du serveur
   struct sockaddr_in peer_addr; // socket addr d'un client

   socklen_t peer_addr_size; // la taille du sock
   pid_t child;

   char buffer[10]; // pour recevoir la salutation du client

   sfd = socket(AF_INET, SOCK_STREAM, 0); // création de la socket
   if (sfd < 0){ // Une erreur s'est produite la socket n'a pas pu être créer
      printf("Le SE n'a pas pu créer la socket %d\n", sfd);
      exit(-1);
   }

   /* La structure est remise à 0 car elle doit servir à stocker
    * l'identité du serveur*/
   memset(&my_addr, 0, sizeof(struct sockaddr_in));

    /* donne une identite a la socket. */
   my_addr.sin_family = AF_INET;
   my_addr.sin_port = htons (MY_PORT);
   inet_aton(MY_ADDR, (struct in_addr *)&my_addr.sin_addr.s_addr);

   /* on demande au SE de confirmer l'identité de la socket
    * Cela autorise le SE d'exploitation à forwarder les requête
    * Sur le port 56789 à ce processus */
   if (bind(sfd, (struct sockaddr *) &my_addr,
                   sizeof(struct sockaddr_in)) < 0){
       printf("bind error\n"); // l'association a echouée
       exit(-1);
  }

   /* on demande au SE de définir le nombre de clients
    * que le serveur peut
    * traiter en même temps dans sa file d'attente */
   if (listen(sfd, LISTEN_BACKLOG) < -1)
               perror("listen\n");

   // on se bloque en attendant les connexion des client
   peer_addr_size = sizeof(struct sockaddr_in);
   while(1){
         cfd = accept(sfd, (struct sockaddr *) &peer_addr,
                   &peer_addr_size);
        if (cfd < 0){
            perror("accept\n");
            exit(-1); // Une erreur s'est produite
        }
        /* Nous créons un fils pour gérer ce client */
        child = fork();
        if(child < 0){ // le fils n'a pas pu être créé
           perror("errreur de création du fils\n");
        }
        if(child==0){
             /* Nous sommes dans le fils nous attendons la requête du client */
             printf("indentité du client %d\n", peer_addr.sin_port);
             /*Lecture des donnees envoyées par le client*/
             while(read(cfd, buffer, 10))
                 printf("%s", buffer);
             printf("\n");
       pid_t pid1, pid2;
    int status; // le status des fils
    union semun sem_arg; // paramètre pour configurer le semaphore
    // Argument du premier processus
    char *argp1[] = {"w", NULL, NULL};
    // clé pour le semaphore
    key_t sem_key = ftok("semfile",75);
    // on demande au system de nous créer le semaphore
    int semid = semget(sem_key, 1, 0666|IPC_CREAT);

    // la valeur du semaphore est initialisée à 1
    sem_arg.val = 1;
    if(semctl(semid, 0, SETVAL, sem_arg)==-1){
       perror("semctl");
       exit(1);
    }

    // clé pour la mémoire protégée
    key_t key = ftok("shmfile",65);

    int a = 0; // shared data (la variable partagée)

    // On demande au system de creer la memoire partagee
    int shmid = shmget(key,1024,0666|IPC_CREAT);
    // on attache la memoire partagee a str
    char *str = (char*) shmat(shmid,(void*)0,0);
    // ecriture de 0 dans la mémoire partagée
    sprintf(str, "%d", a);

    //création du premier processus (pour lancer le process w)
    pid1 = fork();
    if(pid1 < 0){
          perror("Erreur de création du processus\n");
          exit(EXIT_FAILURE);
    }

    if(pid1 == 0){
        execv("./w", argp1);
    }

    else{
        // creation du second processus pour lancer r
        char *argp2[] = {"Q1", NULL, NULL};
        pid2 = fork();
        sleep(7);
        if(pid2 < 0){
          perror("Erreur de création du second processus\n");
          pid1 = wait(&status);
          exit(EXIT_FAILURE);
        }

        if(pid2 == 0){
            execv("./w", argp2);
        }

        else{
                // On attend la fin des deux processus
                pid1 = wait(&status);
                printf("Status de l'arret du fils %d %d\n", status, pid1);
                pid2 = wait(&status);
                printf("Status de l'arret du fils %d %d\n", status, pid2);

                // on lit la dernière valeur de la variable partagée
                a = atoi(str);
                printf("Valeur Finale de a = %d\n", a);
                //le processus détache str de la mémoire partagée
                shmdt(str);
                // destruction de la mémoire
                shmctl(shmid,IPC_RMID,NULL);
                // des truction du semaphore
                if(semctl(semid, 0, IPC_RMID, NULL) == -1){
                   perror("semctl");
                   exit(1);
                }
        }


    }
             /*Fin du traitement le fils se termine*/
             close(sfd);
             break;

        }

        else{
          /*Dans le père: le père attent un autre client*/
          close(cfd);
        }
  }
}
