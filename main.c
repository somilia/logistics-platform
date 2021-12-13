
//#include "headers/semaphore.h" //warning dedans
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define TRAIN 1
#define PENICHE 0
#define CAMION 2

#define A 0
#define B 1

#define NB_PENICHE 2
#define NB_TRAIN 2
#define NB_CAMION 3

#define CAPACITE_PENICHE 4
#define CAPACITE_TRAIN 3
#define CAPACITE_CAMION 1

#define DELAI_ATTENTE 3 //Temps en secondes qu'attend un transport au port avant de repartir

//--------------pthread -------------------------------
pthread_t tid_portique1;
pthread_t tid_portique2;

pthread_t tid_peniche[NB_PENICHE+1];
pthread_t tid_train[NB_TRAIN+1];
pthread_t tid_camion[NB_CAMION+1];

//pthread_t tid_containerPeniche[2][CAPACITE_PENICHE];

//-------------- Mutex-------------------------------
static pthread_mutex_t printf_mutex;
pthread_mutex_t mutex_creation_container;

//-------------- Enum -------------------------------
typedef enum {NORD, SUD, OUEST, EST} Destination;
typedef enum {WPENICHE, wTRAIN, wCAMION} typeTransport;

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
    Container *container;
   // Container container[typeToCapacite(typeTransport)];
} Transport;

//-------------- Prototypes -------------------------------
Container create_container(Destination dest);       
void * fonc_transport(int arg[]);
void * fonc_container(int i);                       

void * fonc_peniche(int i);                         
void * fonc_train(int i);    
void * fonc_camion(int i);    

int nombre_aleatoire(int min, int max);    
int portique(); 
int typeToCapacite(int typeTransport);        


//-------------- Variables globales -----------------------
int Capacite[3] = {CAPACITE_PENICHE,CAPACITE_TRAIN,CAPACITE_CAMION};
char * transportString[3] = {"PENICHE","TRAIN","CAMION"};

Transport penicheA;
Transport penicheB;
Transport trainA;
Transport trainB;

Container container_peniche[2][CAPACITE_PENICHE]; 

Container container_train[2][CAPACITE_TRAIN];

Container container_camionP1[4][CAPACITE_CAMION];
Container container_camionP2[4][CAPACITE_CAMION];

Container * container_address[] = {container_peniche, container_train, container_camionP1, container_camionP2}; //tableau de pointeur de tableau contenant les containers ci-dessus

int peniche_P1=0;
int peniche_P2=0;
//----------------------------------------------------------

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

    int camionP2 = 0; //
    if (transport.typeTransport == CAMION && transport.destination > 1) //Si camion P2 donc destination>1 
    {
        camionP2=1; //on récupère le tab container_camionP2
    }
    
    for (int i=0; i < transport.nb_container; i++) {
        //containerTab[i] = create_container(transport.destination);
        container_address[transport.typeTransport+camionP2][i] = create_container(transport.destination);
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
    transport.typeTransport = arg[1]; //typeTransport;

    transport.nb_container = (0, Capacite[transport.typeTransport]);
    switch(transport.typeTransport){
        case PENICHE:
            transport.id = tid_peniche[arg[0]];
            break;
        case TRAIN:
            transport.id = tid_train[arg[0]];
            break;
        case CAMION:
            transport.id = tid_camion[arg[0]];
            break;
    }
    
    
    remplir_transport(transport);
    
    pthread_mutex_lock(&printf_mutex);

    sleep(2);
    //printf("\n\n* Peniche id %lu \t Destination: %d \t Nb de contenair %d", transport.id, transport.destination, transport.nb_container);
    printf("\n\n* %s id %lu \t Destination: %d \t Nb de contenair %d",transportString[transport.typeTransport], transport.id, transport.destination, transport.nb_container);
    
    for(int j=0;j<transport.nb_container;j++){
        printf("\n\t=>%c (%u) Container id:%u\t destination:%u\n",transportString[transport.typeTransport][0] , transport.id, container_address[transport.typeTransport][j].id, container_address[transport.typeTransport][j].destination);
    }
/*
    for(int n=0;n<transport.nb_container;n++){
        printf("\n\t=>P (%lu) Container id:%lu\t destination:%d\n", transport.id, transport.container[n].id, transport.container[n].destination);
    }*/
    pthread_mutex_unlock(&printf_mutex);

    for(int temps=0; temps<DELAI_ATTENTE; temps++)
    {
        sleep(1);
        temps++;
    }
}

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
    pthread_create(&tid_portique1,0, (void *(*)())(fonc_portique),NULL);
    pthread_create(&tid_portique2,0, (void *(*)())(fonc_portique),NULL);
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