
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
static pthread_mutex_t printf_mutex;

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

void remplir_transport(Container transport[], Destination destinationTransport, int nb_container) {
    for (int i=0; i < nb_container; i++) {
        transport[i] = create_container(destinationTransport);
    }
}

Container create_container(Destination destinationTransport){
    pid_t pid = fork();
    if (pid == 0){
        Container cont;
        do{
            cont.destination = nombre_aleatoire(0, 4);
        } while(cont.destination == destinationTransport);
        cont.id = getpid();
        return cont;
    }
    exit(0);
}

int nombre_aleatoire(int min, int max) {
  return(min + (rand() % (max - min)));
}

void creer_peniche()
{

}

void * fonc_peniche(int i){
    
    //Remplissage de la péniche
    Destination destinationPeniche = nombre_aleatoire(0, 4);
    int nb_container = nombre_aleatoire(0, CAPACITE_PENICHE);
    Container container_peniche[CAPACITE_PENICHE];

    
    remplir_transport(container_peniche, destinationPeniche, nb_container);
    pthread_mutex_lock(&printf_mutex);

    printf("\n\n* Peniche id %u \t Destination: %u \t Nb de contenair %u", tid_peniche[i], destinationPeniche, nb_container);

    for(int n=0;n<nb_container;n++){
        printf("\n\t=> (%u) Container id:%u\t destination:%u\n", tid_peniche[i], container_peniche[n].id, container_peniche[n].destination);
    }
    pthread_mutex_unlock(&printf_mutex);

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
    srand(time(NULL));
	int i;	

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
	printf("\n");
	exit(0);
}