
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
#define C 2
#define D 3

#define P1 0
#define P2 1

#define NB_PENICHE 3
#define NB_TRAIN 3
#define NB_CAMION 3

#define CAPACITE_PENICHE 4
#define CAPACITE_TRAIN 3
#define CAPACITE_CAMION 1

#define DELAI_ATTENTE 3 //Temps en secondes qu'attend un transport au port avant de repartir

//--------------pthread -------------------------------
pthread_t tid_portique[2];

pthread_t tid_peniche[NB_PENICHE+1];
pthread_t tid_train[NB_TRAIN+1];
pthread_t tid_camion[NB_CAMION+1];

//pthread_t tid_containerPeniche[2][CAPACITE_PENICHE];

//-------------- Mutex-------------------------------
pthread_mutex_t printf_mutex;
pthread_mutex_t mutex_creation_transport;
pthread_mutex_t mutex_nb_transport;

pthread_cond_t cond_nb_transport;

//-------------- Enum -------------------------------
typedef enum {NORD, SUD, OUEST, EST} Destination;
typedef enum {WPENICHE, wTRAIN, wCAMION} typeTransport;

//-------------- ???? --------------------------------------
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
    int position; //P1 ou P2
    int compteurGlobal;
    //Container *container;
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
int typeToCapacite(int typeTransport);   
int superviseur(int portique);    
void afficherTransport(int portique, int typeT);
void transfer_vers_P2(Transport transport); 


//-------------- Variables globales -----------------------
//constantes
const int Capacite[3] = {CAPACITE_PENICHE,CAPACITE_TRAIN,CAPACITE_CAMION};
const char * transportString[3] = {"PENICHE","TRAIN","CAMION"};
const int portiquePlace[3] = {1,1,4};
//variables

/*Transport penicheA;
Transport penicheB;
Transport trainA;
Transport trainB;*/
int nb_transport_portique[2][3];//portique[P1 pu P2][PENICHE TRAIN OU CAMION]

Transport peniche[2];       //peniche[A ou B]
Transport train[2];         //train[A ou B]
Transport camion[2][4];     //camion[portique][A B C ou D] 

Container container_peniche[2][CAPACITE_PENICHE]; 
Container container_train[2][CAPACITE_TRAIN];
Container container_camionP1[4][CAPACITE_CAMION];
Container container_camionP2[4][CAPACITE_CAMION];

// TEST TABLEAU D'ADRESSE DE TABLEAU 2D
//Container * container_address[4] = {&container_peniche[2][CAPACITE_PENICHE], &container_train[2][CAPACITE_TRAIN],&container_camionP1[4][CAPACITE_CAMION], &container_camionP2[4][CAPACITE_CAMION]}; //tableau de pointeur de tableau contenant les containers ci-dessus
//Container * container_address[];
//{container_peniche, container_train,container_camionP1, container_camionP2}; //tableau de pointeur de tableau contenant les containers ci-dessus

int nombre_aleatoire(int min, int max) {
  return(min + (rand() % (max - min)));
}

//Fonction qui remplit le tableau d'un transport avec des éléments de types structure Container

void remplir_transport(Transport transport) {
    int a_ou_b = transport.compteurGlobal%2;

    for (int i=0; i < transport.nb_container; i++) {
        switch(transport.typeTransport){
            case PENICHE:
                container_peniche[a_ou_b][i]= create_container(transport.destination);;
                break;
            case TRAIN:
                container_train[a_ou_b][i]= create_container(transport.destination);;
                break;
            case CAMION:
                if (transport.destination < 2 ) //Si camion P1 donc destination < 2 
                {
                    container_camionP1[a_ou_b][i]= create_container(transport.destination);;
                } else {
                    container_camionP2[a_ou_b][i]= create_container(transport.destination);;
                }
                break;
        }
    }

    //Ne pas oublier d'écraser les anciens transports.
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
    //Creation du transport
    Transport transport;
    transport.destination = nombre_aleatoire(0, 4);
    transport.typeTransport = arg[1]; //typeTransport;
    transport.nb_container = nombre_aleatoire(0, Capacite[transport.typeTransport]);
    transport.position = P1;
    transport.compteurGlobal = arg[0];

    //On attend s'il ya de la place au portique 1
    
    /*MUTEX COND*/
    pthread_mutex_lock(&mutex_nb_transport);
    
    if(transport.typeTransport == CAMION){  //Peniche ou Train
        while (nb_transport_portique[P1][transport.typeTransport] >=4 ){ 
            //Pas de place au portique 1
            pthread_cond_wait(&cond_nb_transport, &mutex_nb_transport);
        }
    } else { //Camion
        while (nb_transport_portique[P1][transport.typeTransport] > 0 ){ 
            //Pas de place au portique 1
            pthread_cond_wait(&cond_nb_transport, &mutex_nb_transport);
        }

    }
    nb_transport_portique[P1][transport.typeTransport]++;
    pthread_mutex_unlock(&mutex_nb_transport);
    //--------------Remplissage du transport------------
    switch(transport.typeTransport){
        case PENICHE:
            transport.id = tid_peniche[arg[0]];
            peniche[transport.compteurGlobal%2] = transport;
            break;
        case TRAIN:
            transport.id = tid_train[arg[0]];
            train[transport.compteurGlobal%2] = transport;
            break;
        case CAMION:
            transport.id = tid_camion[arg[0]];
            camion[P1][transport.compteurGlobal%2] = transport;
            break;
    }
    printf("\t(%c-%c-%d) ", transportString[transport.typeTransport][0],'A'+transport.compteurGlobal%2, transport.id);
    remplir_transport(transport);
    //--------------Fin du remplissage du transport------------
    
    pthread_mutex_unlock(&mutex_creation_transport);
    
    //--- Traitement du transport -----------
    for(int temps=0; temps<DELAI_ATTENTE; temps++)
    {
        sleep(1);
        temps++;
    }

    //--- Fin avec P1 donc transfert vers P2 --- 
    pthread_mutex_lock(&mutex_nb_transport);
    
    if(transport.typeTransport != CAMION){  //Peniche ou Train
        while (nb_transport_portique[P2][transport.typeTransport] > 0){ 
            //Pas de place au portique 2
            pthread_cond_wait(&cond_nb_transport, &mutex_nb_transport);
        }
    } else { //Camion
        while (nb_transport_portique[P2][transport.typeTransport] >=4 ){ //Tester avec les quatres places de camions
            //Pas de place au portique 2
            pthread_cond_wait(&cond_nb_transport, &mutex_nb_transport);
        }

    }
    transfer_vers_P2(transport);
    nb_transport_portique[P1][transport.typeTransport]--;
    pthread_mutex_unlock(&mutex_nb_transport);

    //--- Traitement du transport -----------
    for(int temps=0; temps<DELAI_ATTENTE; temps++)
    {
        sleep(1);
        temps++;
    }


    //--- Affichage du contenu du transport ------------------------
    pthread_mutex_lock(&printf_mutex);
    sleep(1);

    //--- Fin d'affichage du contenu du transport ------------------------
    pthread_mutex_unlock(&printf_mutex);

   

    pthread_mutex_lock(&mutex_nb_transport);
    nb_transport_portique[P2][transport.typeTransport]--;
    pthread_mutex_unlock(&mutex_nb_transport);
    pthread_cond_signal(&cond_nb_transport);
}

void * fonc_portique(int portique){
    /*if(i==1){ //portique 1
        superviseur(i);
    }
    else { //portique 2
        
    }*/
    while (1)
    {
        if(portique == P1){
            while(1){
                pthread_mutex_lock(&mutex_nb_transport);
                if(nb_transport_portique[P1][PENICHE] !=0 && nb_transport_portique[P1][TRAIN]!=0)
                    for(int typeT = 0;typeT<3;typeT++){
                    {
                        afficherTransport(portique,typeT);
                    }                
                    sleep(1);
                }
                pthread_mutex_unlock(&mutex_nb_transport);                    
            }
        } else {
        //Portique 2
        //rien
        }
    }
}

void transfer_vers_P2(Transport transport){
    transport.position = P2;
}

void * fonc_superviseur(int i){
}

void afficherTransport(int portique, int typeT){
    if (nb_transport_portique[P1][typeT]!=0){
        int ab;
        if(peniche[A].position == P1) //=0
        {
            ab = A; //A=0
        } 
        else {
            ab = B; //B=1
        }                
        printf("\nPortique %d:",portique+1);
        printf("\n\t• Peniche %c: id:%d - destination:%d - nb contenair:%d",ab+'A', peniche[ab].id,peniche[ab].destination,peniche[ab].nb_container);
        if (peniche[ab].nb_container>0)
        {
            for (int i = 0; i < peniche[ab].nb_container; i++){
                printf("\n\t\t> Contenair: %d - destination: %d",container_peniche[ab][i].id,container_peniche[ab][i].destination);
            }
        }
        
        if(train[A].position == P1) //=0
        {
            ab = A; //A=0
        } else {
            ab = B; //B=1
        } 
        printf("\n\t• Train %c: id:%d - destination:%d - nb contenair:%d",ab+'A', train[ab].id,train[ab].destination,train[ab].nb_container);
        if (train[ab].nb_container>0)
        {
            for (int i = 0; i < train[ab].nb_container; i++){
                printf("\n\t\t> Contenair: %d - destination: %d",container_train[ab][i].id,container_train[ab][i].destination);
            }
        }
    }
}

int superviseur(int portique) {
    

    return 0;
}


int main() {
    srand(time(NULL));
	int i=1;	

    //Initialisation des mutex
    pthread_mutex_init(&printf_mutex,0);
    pthread_mutex_init(&mutex_creation_transport,0);
    pthread_mutex_init(&mutex_nb_transport,0);
    
    pthread_cond_init(&cond_nb_transport, NULL);
    //Création des threads portiques
    for(i=0;i<2;i++){
        pthread_create(tid_portique,0, (void *(*)())(fonc_portique),(void*)i);   
	}
   
   
	//Création des threads transport
	for(i=0;i<NB_PENICHE;i++){
        pthread_mutex_lock(&mutex_creation_transport);
        int arg[2]={i,PENICHE};
	    pthread_create(tid_peniche+i,0, (void *(*)())(fonc_transport),(void*)arg);
	}

    for(i=0;i<NB_TRAIN;i++){
        pthread_mutex_lock(&mutex_creation_transport);
        int arg[2]={i,TRAIN};
	    pthread_create(tid_train+i,0,(void *(*)())(fonc_transport),(void*)arg);
	}
    
    
	//On attend la fin de toutes les threads
	for(i=0;i<NB_PENICHE;i++){
		pthread_join(tid_peniche[i],NULL);
	}	
	for(i=0;i<NB_TRAIN;i++){
		pthread_join(tid_train[i],NULL);
	}

    for(i=0;i<NB_CAMION;i++){
		pthread_join(tid_camion[i],NULL);
	}

    /*for(i=0;i<2;i++){
        pthread_join(tid_portique[i],NULL); 
	}*/

	//On libère les ressources
    pthread_mutex_destroy(&printf_mutex);
    pthread_mutex_destroy(&mutex_creation_transport);
    pthread_mutex_destroy(&mutex_nb_transport);

    pthread_cond_destroy(&cond_nb_transport);
	//pthread_mutex_destroy(&mutex);
	//pthread_cond_destroy(&attendre);  
	printf("\nFin de tous les threads");
	exit(0);
}