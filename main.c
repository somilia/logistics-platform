#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h> 

//headers
#include "headers/semaphore.h"

#define nbr_cargo 10
#define nbr_wagon 10
#define nbr_camion 10


typedef enum {NORD, SUD, OUEST, EST, ETRANGER} Destination;

typedef struct Marchandise {
    pid_t id;
    Destination destination;
} Marchandise;

int nombre_aleatoire(int min, int max) {
  return(min + (rand() % (max - min)));
}

int creer_portique(Marchandise marchandise) 
{
    int pid = fork();
    if(pid != 0)
        return pid;

    printf("Portique créé \n");

    while(1) {
        
        switch (marchandise.destination)
        {
        case 0:
            
            break;
        case 1:
            
            break;
        case 2:
            
            break;
        case 3:
            
            break;
        default:
            printf("ca cherche les problemes");
            break;

        }

    }
}

int creer_bateau(int portique)
{
    int pid = fork();
    if(pid != 0)
        return pid;

    

    exit(0);
}

int creer_camion(int portique)
{
    int pid = fork();
    if(pid != 0)
        return pid;


    exit(0);
}

int creer_train(Destination destination) //2 trains unidirectionnels
{
    int pid = fork();
    if(pid != 0)
        return pid;
 

    exit(0);
}

Marchandise creer_marchandise()
{
    pid_t pid = fork();
    if(pid != 0)
        Marchandise march;
        
        march.destination = nombre_aleatoire(0, 4);
        march.id = pid;
        return march;

}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <nb_ship>\n", argv[0]);
        return 1;
    }

    int n = nb_marchandise = atoi(argv[1]);

    srand(time(NULL));
    for(int i = 0; i < nbr_camion; i++) {
        sleep(nombre_aleatoire(0, 5));
        int camion = creer_camion(portique);
    }
    for(int i = 0; i < 2; i++) {
        sleep(nombre_aleatoire(0, 5));
        int bateau = creer_bateau(portique);
        int portique = creer_portique();
    }


    for(int i = 0; i < n; i++) {
        sleep(nombre_aleatoire(0, 5));
        creer_marchandise();
    }
  

}