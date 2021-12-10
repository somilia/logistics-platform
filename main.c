
//#include "headers/semaphore.h" //warning dedans
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NB_PENICHE 2
#define NB_TRAIN 10
#define NB_CAMION 10

#define CAPACITE_PENICHE 4
#define CAPACITE_TRAIN 10


pthread_t tid_peniche[NB_PENICHE+1];

typedef enum {NORD, SUD, OUEST, EST} Destination;
//Definition de la struture Conteneur
typedef struct Container {
    pid_t id;
    Destination destination;
} Container;

//Prototypes
Container create_container(Destination dest);
void * fonc_peniche(int i);
int nombre_aleatoire(int min, int max);
//

void remplir_transport(Container transport[], Destination dest, int nb_container) {
    for (int i=0; i < nb_container; i++) {
        transport[i] = create_container(dest);
    }
}

Container create_container(Destination dest){
    pid_t pid = fork();
    if (pid != 0){
        Container cont;
        do{
            cont.destination = nombre_aleatoire(0, 4);
        } while(cont.destination != dest);
        cont.id = pid;
        return cont;
    }
}

int nombre_aleatoire(int min, int max) {
  return(min + (rand() % (max - min)));
}

void creer_peniche()
{

}

void * fonc_peniche(int i){
    
    //Remplissage de la péniche
    Destination destination = nombre_aleatoire(0, 3);
    int nb_container = nombre_aleatoire(0, CAPACITE_PENICHE);

    Container container_peniche[CAPACITE_PENICHE];
    remplir_transport(container_peniche, destination, nb_container);
    
    printf("\nPeniche id %d",tid_peniche[i]);

    for(int i=0;i<CAPACITE_PENICHE;i++){
        printf("\nid:%d\t destination:%d",container_peniche[i].id,container_peniche[i].destination);
    }

    //While
}
/*
int creer_camion(int portique)
{
    int pid = fork();
    if(pid != 0)
        return pid;

    Destination destination = nombre_aleatoire(0, 3);
    int nb_container = nombre_aleatoire(0, 1);

    Container container_peniche[CAPACITE_PENICHE];
    remplir_transport(container_peniche, destination, nb_container);


    exit(0);
}

int creer_train(Destination destination) //2 trains unidirectionnels
{
    int pid = fork();
    if(pid != 0)
        return pid;

    Destination destination = nombre_aleatoire(0, 3);
    int nb_container = nombre_aleatoire(0, CAPACITE_TRAIN);

    Container container_peniche[CAPACITE_PENICHE];
    remplir_transport(container_peniche, destination, nb_container);

    exit(0);
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
*/

int main() {
	int i;	

	//initialise le mutex pointé par mutex, 0=paramètre par défault
	//pthread_mutex_init(&mutex,0);
	
	//initialise la variable condition cond
	//pthread_cond_init(&attendre,0);
	//pthread_cond_init(&dormir,0);

	//creation des threads clients

	for(i=0;i<NB_PENICHE;i++){
	    pthread_create(tid_peniche+i,0,(void *(*)())fonc_peniche,(void*)i);
	}

	//attend la fin de toutes les threads clients
	for(i=0;i<NB_PENICHE;i++){
		pthread_join(tid_peniche[i],NULL);
	}	
	
	//On libère les ressources
	//pthread_mutex_destroy(&mutex);
	//pthread_cond_destroy(&attendre);  
	
	exit(0);
}