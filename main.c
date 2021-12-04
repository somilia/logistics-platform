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
//Definition de la struture Conteneur
typedef struct Container {
    pid_t id;
    Destination destination;
} Container;

int nombre_aleatoire(int min, int max) {
  return(min + (rand() % (max - min)));
}

int creer_portique(Container container) 
{
    int pid = fork();
    if(pid != 0)
        return pid;

    printf("Portique créé \n");

    while(1) {
        
        switch (container.destination)
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

int creer_peniche(int portique)
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

Container creer_Container()
{
    pid_t pid = fork();
    if(pid != 0){
        Container container;

        container.destination = nombre_aleatoire(0, 4);
        container.id = pid;
        return container;
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <nb_ship>\n", argv[0]);
        return 1;
    }
    int nb_Container;
    int n = nb_Container = atoi(argv[1]);
    int portique, peniche, camion; //train

    srand(time(NULL));
    
    for(int i = 0; i < 2; i++) {
        sleep(nombre_aleatoire(0, 5));
        Container container = creer_Container();
        portique = creer_portique(container);
        peniche = creer_peniche(portique);
    }

    for(int i = 0; i < nbr_camion; i++) {
        sleep(nombre_aleatoire(0, 5));
        camion = creer_camion(portique);
    }

    for(int i = 0; i < n; i++) {
        sleep(nombre_aleatoire(0, 5));
        creer_Container();
    }
  

}