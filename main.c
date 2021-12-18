
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//-- Nb de transports créés --
#define NB_PENICHE 3
#define NB_TRAIN 3
#define NB_CAMION 3

//-- Numéros de référence du type de transport --
#define TRAIN 1
#define PENICHE 0
#define CAMION 2

//-- Numéros de référence du transport --
#define A 0
#define B 1
#define C 2
#define D 3

//-- Numéros de référence du portique --
#define P1 0
#define P2 1

//-- Paramètre des transport --
#define CAPACITE_PENICHE 4
#define CAPACITE_TRAIN 3
#define CAPACITE_CAMION 1

#define DELAI_ATTENTE 4 //Temps en secondes qu'attend un transport au port avant de repartir

//--------------pthread -------------------------------
pthread_t tid_portique[2];

pthread_t tid_peniche[NB_PENICHE+1];
pthread_t tid_train[NB_TRAIN+1];
pthread_t tid_camion[NB_CAMION+1];

//pthread_t tid_containerPeniche[2][CAPACITE_PENICHE];

//-------------- Mutex-------------------------------
pthread_mutex_t printf_mutex;
pthread_mutex_t mutex_creation_transport[3];
pthread_mutex_t mutex_nb_transport;
pthread_mutex_t mutex_portique1;

pthread_cond_t cond_nb_transport;
pthread_cond_t cond_portique1;

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
void transfer_vers_P2(Transport * transport); 


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
int nb_transport_portique[2][3] = {0};//portique[P1 pu P2][PENICHE TRAIN OU CAMION]

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
    //-- Création du transport --
    Transport transport;
    transport.destination = nombre_aleatoire(0, 4);
    transport.typeTransport = arg[1]; //typeTransport;
    transport.nb_container = nombre_aleatoire(0, Capacite[transport.typeTransport]);
    transport.position = P1;
    transport.compteurGlobal = arg[0];

    
    //-- On attend qu'il ya de la place au portique 1 --
    
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

    remplir_transport(transport);
    pthread_mutex_unlock(&mutex_nb_transport);
    //--------------Fin de création et remplissage du transport------------

    pthread_mutex_unlock(&mutex_creation_transport[transport.typeTransport]);

    pthread_mutex_lock(&printf_mutex);
    printf("\n ->%c (%c) %d arrive au P1", transportString[transport.typeTransport][0],'A'+transport.compteurGlobal%2 ,transport.id);
    pthread_mutex_unlock(&printf_mutex);
    usleep(100);

    //--- Traitement du transport -----------
    pthread_cond_signal(&cond_portique1);

    pthread_mutex_lock(&printf_mutex);
    printf("\n   %c (%c) %d attend au P1", transportString[transport.typeTransport][0],'A'+transport.compteurGlobal%2 ,transport.id);
    pthread_mutex_unlock(&printf_mutex);

    for(int temps=0; temps<DELAI_ATTENTE; temps++)
    {
        //printf(".");
        sleep(1);
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
    //printf("\n ----%c (%c) %d position: %d", transportString[transport.typeTransport][0],'A'+transport.compteurGlobal%2 ,transport.id,transport.position);
    transfer_vers_P2(&transport);
    //printf("\n ----%c (%c) %d position: %d", transportString[transport.typeTransport][0],'A'+transport.compteurGlobal%2 ,transport.id,transport.position);

    nb_transport_portique[P1][transport.typeTransport]--;
    pthread_mutex_lock(&printf_mutex);
    printf("\n >>%c (%c) %d va au P2>>", transportString[transport.typeTransport][0],'A'+transport.compteurGlobal%2 ,transport.id);
    pthread_mutex_unlock(&printf_mutex);

    usleep(100);
    pthread_mutex_unlock(&mutex_nb_transport);
    usleep(100);

    //--- Traitement du transport -----------
    pthread_mutex_lock(&printf_mutex);
    printf("\n   %c (%c) %d attend au P2", transportString[transport.typeTransport][0],'A'+transport.compteurGlobal%2 ,transport.id);
    pthread_mutex_unlock(&printf_mutex);

    for(int temps=0; temps<DELAI_ATTENTE; temps++)
    {
        //printf(".");
        sleep(1);
    }


    //--- Affichage du contenu du transport ------------------------
    /*pthread_mutex_lock(&printf_mutex);
    sleep(1);

    //--- Fin d'affichage du contenu du transport ------------------------
    pthread_mutex_unlock(&printf_mutex);*/

    pthread_mutex_lock(&mutex_nb_transport);
    nb_transport_portique[P2][transport.typeTransport]--;
    pthread_mutex_unlock(&mutex_nb_transport);

    pthread_mutex_lock(&printf_mutex);
    printf("\n * %c (%c) %d quitte le port *", transportString[transport.typeTransport][0],'A'+transport.compteurGlobal%2 ,transport.id);
    pthread_mutex_unlock(&printf_mutex);

    pthread_cond_signal(&cond_nb_transport);
    usleep(200);
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
                pthread_mutex_lock(&mutex_portique1);
                pthread_cond_wait(&cond_portique1,&mutex_portique1);
                pthread_mutex_lock(&mutex_nb_transport);

                if(nb_transport_portique[P1][PENICHE] !=0 || nb_transport_portique[P1][TRAIN]!=0){
                    
                    pthread_mutex_lock(&printf_mutex);
                    printf("\nPortique %d:",portique+1);
                    for(int typeT = 0;typeT<3;typeT++){
                        afficherTransport(portique,typeT);
                    }  
                    pthread_mutex_unlock(&printf_mutex);              
                    sleep(1);
                } 
                pthread_mutex_unlock(&mutex_nb_transport); 
                pthread_mutex_unlock(&mutex_portique1);
                usleep(200);                
            }
        } else {
        //Portique 2
        //rien
        }
    }
}

void transfer_vers_P2(Transport * transport){
    transport->position = P2;
    
    switch(transport->typeTransport){
        case PENICHE:
            peniche[transport->compteurGlobal%2].position = P2;
            break;
        
        case TRAIN:
            train[transport->compteurGlobal%2].position = P2;
            break;

        case CAMION:
            //camion[transport->compteurGlobal%2].position = P2;
            break;
    }
}

void * fonc_superviseur(int i){
}

void afficherTransport(int portique, int typeT){
    if (nb_transport_portique[P1][typeT]>0){
        int ab=A;
        switch (typeT){
            case PENICHE:
                if(peniche[A].position == P2){
                    ab = B; //A=0
                }              
                
                printf("\n\t• Peniche %c: id:%d - destination:%d - nb contenair:%d",ab+'A', peniche[ab].id,peniche[ab].destination,peniche[ab].nb_container);
                if (peniche[ab].nb_container>0){
                    for (int i = 0; i < peniche[ab].nb_container; i++){
                        printf("\n\t\t> Contenair: %d - destination: %d",container_peniche[ab][i].id,container_peniche[ab][i].destination);
                    }
                }
                break;
            
            case TRAIN:
                if(train[A].position == P2){
                    ab = B; //A=0
                } 
                printf("\n\t• Train %c: id:%d - destination:%d - nb contenair:%d",ab+'A', train[ab].id,train[ab].destination,train[ab].nb_container);
                if (train[ab].nb_container>0){
                    for (int i = 0; i < train[ab].nb_container; i++){
                        printf("\n\t\t> Contenair: %d - destination: %d",container_train[ab][i].id,container_train[ab][i].destination);
                    }
                }
                break;

            case CAMION:
                break;   
        }
    }
}

int superviseur(int portique) {
    

    return 0;
}


int main() {
    srand(time(NULL));
	int i=1;	

    //-- Initialisation des mutex --
    for (i = 0; i < 3; i++) {
        pthread_mutex_init(&mutex_creation_transport[i],0);
    }
    
    pthread_mutex_init(&printf_mutex,0);
    pthread_mutex_init(&mutex_nb_transport,0);
    pthread_mutex_init(&mutex_portique1,0); //Possibilite de creer un tableau avec les deux portiques

    pthread_cond_init(&cond_nb_transport, NULL);
    pthread_cond_init(&cond_portique1, NULL);

    //-- Création des threads portiques --
    for(i=0;i<2;i++){
        pthread_create(tid_portique,0, (void *(*)())(fonc_portique),(void*)i);   
	}
   
	//-- Création des threads transport --
	for(i=0;i<NB_PENICHE;i++){
        pthread_mutex_lock(&mutex_creation_transport[PENICHE]);
        int arg[2]={i,PENICHE};
	    pthread_create(tid_peniche+i,0, (void *(*)())(fonc_transport),(void*)arg);
	}

    for(i=0;i<NB_TRAIN;i++){
        pthread_mutex_lock(&mutex_creation_transport[TRAIN]);
        int arg[2]={i,TRAIN};
	    pthread_create(tid_train+i,0,(void *(*)())(fonc_transport),(void*)arg);
	}
    
	//-- On attend la fin de toutes les threads -- 
	for(i=0;i<NB_PENICHE;i++){
		pthread_join(tid_peniche[i],NULL);
	}	
	for(i=0;i<NB_TRAIN;i++){
		pthread_join(tid_train[i],NULL);
	}

    for(i=0;i<NB_CAMION;i++){
		pthread_join(tid_camion[i],NULL);
	}

    for(i=0;i<2;i++){
        pthread_join(tid_portique[i],NULL); 
	}

	//-- On libère les ressources -- 
    for (i = 0; i < 3; i++) {
        pthread_mutex_destroy(&mutex_creation_transport[i]);
    }
    pthread_mutex_destroy(&printf_mutex);
    pthread_mutex_destroy(&mutex_nb_transport);
    pthread_mutex_destroy(&mutex_portique1);

    pthread_cond_destroy(&cond_nb_transport);
    pthread_cond_destroy(&cond_portique1);
     
	printf("\nFin de tous les threads, tous les transports sont partis");
	exit(0);
}