
//#include "headers/semaphore.h" //warning dedans
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NB_PENICHE 2
#define NB_TRAIN 2
#define NB_CAMION 3

#define CAPACITE_PENICHE 4
#define CAPACITE_TRAIN 3
#define CAPACITE_CAMION 1

#define DELAI_ATTENTE 3 //Temps en secondes qu'attend un transport au port avant de repartir

pthread_t tid_portique1;
pthread_t tid_portique2;
pthread_t tid_peniche[NB_PENICHE+1];
pthread_t tid_train[NB_TRAIN+1];
pthread_t tid_camion[NB_CAMION+1];

//pthread_t tid_containerPeniche[2][CAPACITE_PENICHE];

static pthread_mutex_t printf_mutex;
pthread_mutex_t mutex_creation_container;

//-------------- Enum -------------------------------
typedef enum {NORD, SUD, OUEST, EST} Destination;
typedef enum {PENICHE, TRAIN, CAMION} typeTransport;

//-------------- Enum -------------------------------
int typeToCapacite(int typeTransport) { //a changer en variable global
    int Capacite[3] = {CAPACITE_PENICHE,CAPACITE_TRAIN,CAPACITE_CAMION};
    return Capacite[typeTransport];
}

//---- Définition de strutures  ---------------------------
typedef struct Container {
    pthread_t id;
    Destination destination;
} Container;

typedef struct Transport {
    pthread_t id;
    int typeTransport; 
    Destination destination;
    int nb_container;
    Container container[CAPACITE_PENICHE];
   // Container container[typeToCapacite(typeTransport)];
} Transport;

//-------------- Prototypes -------------------------------
Container create_container(Destination dest);       
void * fonc_peniche(int i);                         
void * fonc_train(int i);                           
void * fonc_container(int i);                       
int nombre_aleatoire(int min, int max);    
int portique(); 
int typeToCapacite(int typeTransport);        


//-------------- Variables globales -----------------------
Container container_peniche[NB_PENICHE][CAPACITE_PENICHE];
Container container_train[NB_TRAIN][CAPACITE_TRAIN];

Transport penicheA;
Transport penicheB;
Transport trainA;
Transport trainB;

int peniche_P1=0;
int peniche_P2=0;
//---------------------------------------------------------

int nombre_aleatoire(int min, int max) {
  return(min + (rand() % (max - min)));
}

//Fonction qui remplit le tableau d'un transport avec des éléments de types structure Container
/*void remplir_transport(Container transport[], Destination destinationTransport, int nb_container) {
    for (int i=0; i < nb_container; i++) {
        transport[i] = create_container(destinationTransport);
    }
}*/
void remplir_transport(Transport transport) {
    for (int i=0; i < transport.nb_container; i++) {
        transport.container[i] = create_container(transport.destination);
    }
}

//Fonction qui renvoie une structure Container avec une destination différentes de celle d'entrée
Container create_container(Destination destinationTransport){
    
    pthread_t tid_container;
    pthread_create(&tid_container,0, (void *(*)())(fonc_container),(void*)tid_container);

    Container cont;
    do{
            cont.destination = nombre_aleatoire(0, 4);
        } while(cont.destination == destinationTransport);

    cont.id = tid_container;

    //pthread_join(tid_container,NULL);
    return cont;
}


void * fonc_container(int i){
    //Rien
}

void * fonc_transport(int arg[]){
    //On attend s'il ya de la place au portique 1
    
    //On attend s'il ya de la place au portique 2
    
    //Remplissage du transport
   
    Transport transport;
    transport.destination = nombre_aleatoire(0, 4);
    transport.nb_container = (0, CAPACITE_PENICHE);
    transport.typeTransport = arg[1]; //typeTransport;
     switch(transport.typeTransport)
{
    case 0:
        transport.id = tid_peniche[arg[0]];;
        break;
    case 1:
        transport.id = tid_train[arg[0]];;
        break;
    case 2:
        transport.id = tid_camion[arg[0]];;
        break;
}
    
    
    remplir_transport(transport);
    

    pthread_mutex_lock(&printf_mutex);

    //printf("La peniche %d à destination de zone -%d- arrive au port",tid_peniche[i], destinationPeniche);
    sleep(2);
    printf("\n\n* Peniche id %lu \t Destination: %d \t Nb de contenair %d", transport.id, transport.destination, transport.nb_container);
    /*for(int n=0;n<nb_container;n++){
        printf("\n\t=>T (%u) Container id:%u\t destination:%u\n", tid_train[i], container_train[i][n].id, container_train[i][n].destination);
    }*/

    for(int n=0;n<transport.nb_container;n++){
        printf("\n\t=>P (%lu) Container id:%lu\t destination:%d\n", transport.id, transport.container[n].id, transport.container[n].destination);
    }
    pthread_mutex_unlock(&printf_mutex);

    for(int temps=0; temps<DELAI_ATTENTE; temps++)
    {
        sleep(1);
        temps++;
    }
}
/*
void * fonc_peniche(int i){
    //On attend s'il ya de la place au portique 1
    
    //On attend s'il ya de la place au portique 2
    
    //Remplissage de la péniche
    Destination destinationPeniche = nombre_aleatoire(0, 4);
    int nb_container = nombre_aleatoire(0, CAPACITE_PENICHE);
    
    remplir_transport(container_peniche[i], destinationPeniche, nb_container);
    

    pthread_mutex_lock(&printf_mutex);

    //printf("La peniche %d à destination de zone -%d- arrive au port",tid_peniche[i], destinationPeniche);
    sleep(2);
    printf("\n\n* Peniche id %lu \t Destination: %d \t Nb de contenair %d", tid_peniche[i], destinationPeniche, nb_container);
    /*for(int n=0;n<nb_container;n++){
        printf("\n\t=>T (%u) Container id:%u\t destination:%u\n", tid_train[i], container_train[i][n].id, container_train[i][n].destination);
    }*//*

    for(int n=0;n<nb_container;n++){
        printf("\n\t=>P (%lu) Container id:%lu\t destination:%d\n", tid_peniche[i], container_peniche[i][n].id, container_peniche[i][n].destination);
    }
    pthread_mutex_unlock(&printf_mutex);

    for(int temps=0; temps<DELAI_ATTENTE; temps++)
    {
        sleep(1);
        temps++;
    }
}

void * fonc_train(int i){
    //Remplissage du train
    Destination destinationTrain = nombre_aleatoire(0, 4);
    int nb_container = nombre_aleatoire(0, CAPACITE_TRAIN);
    
    remplir_transport(container_train[i], destinationTrain, nb_container);
    pthread_mutex_lock(&printf_mutex);

    sleep(2);
    printf("\n\n* Train id %lu \t Destination: %d \t Nb de contenair %d", tid_train[i], destinationTrain, nb_container);
    for(int n=0;n<nb_container;n++){
        printf("\n\t=>T (%lu) Container id:%lu\t destination:%d\n", tid_train[i], container_train[i][n].id, container_train[i][n].destination);
    }
    
    pthread_mutex_unlock(&printf_mutex);

    for(int temps=0; temps<DELAI_ATTENTE; temps++)
    {
        sleep(1);
        temps++;
    }
}*/

void * fonc_portique(int i){
    if(i==1){ //portique 1

    }
    else { //portique 2
        
    }
}

void * fonc_superviseur(int i){
}
int superviseur() {

    return 0;
}

int portique() {

    return 0;
}


int main() {
    srand(time(NULL));
		
    //Initialisation des mutex
    pthread_mutex_init(&printf_mutex,0);
    int i=1;
    pthread_create(tid_portique1,0, (void *(*)())(fonc_portique),(int)1);
    pthread_create(tid_portique2,0, (void *(*)())(fonc_portique),(int)2);
   // int arg[2];
	//creation des threads transport
	for(i=0;i<NB_PENICHE;i++){
        int arg[2]={i,0};
	    pthread_create(tid_peniche+i,0, (void *(*)())(fonc_transport),(void*)arg);
	}
    for(i=0;i<NB_TRAIN;i++){
        int arg[2]={i,1};
	    pthread_create(tid_train+i,0,(void *(*)())(fonc_transport),(void*)arg);
	}
    
	//attend la fin de toutes les threads
	for(i=0;i<NB_PENICHE;i++){
		pthread_join(tid_peniche[i],NULL);
	}	
	for(i=0;i<NB_TRAIN;i++){
		pthread_join(tid_train[i],NULL);
	}

    pthread_join(tid_portique1,NULL);
    pthread_join(tid_portique2,NULL);

	//On libère les ressources
    pthread_mutex_destroy(&printf_mutex);

	//pthread_mutex_destroy(&mutex);
	//pthread_cond_destroy(&attendre);  
	printf("\nFin de tous les threads");
	exit(0);
}