
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//-- Nb de transports créés --
#define NB_PENICHE 3
#define NB_TRAIN 3
#define NB_CAMION 3

//-- Numéros de référence du type de transport --
#define TRAIN   1
#define PENICHE 0
#define CAMION  2

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
pthread_mutex_t mutex_portique[2];
pthread_mutex_t mutex_nb_transport_termine;

pthread_mutex_t mutex_transport[3][4];
pthread_mutex_t mutex_container[3][4];

pthread_mutex_t mutex_arg;

pthread_cond_t cond_nb_transport[3];
pthread_cond_t cond_portique1;

pthread_mutex_t mutex_dechargement[3][4];
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
    int lettreABCD;
    //Container *container;
   // Container container[typeToCapacite(typeTransport)];
} Transport;

//-------------- Prototypes -------------------------------
Container create_container(Transport transport);       
void * fonc_transport(int arg[]);
void * fonc_container(int i);                       

void * fonc_peniche(int i);                         
void * fonc_train(int i);    
void * fonc_camion(int i);    

int nombre_aleatoire(int min, int max);    
int typeToCapacite(int typeTransport);   
int superviseur(int portique);    
void afficherTransport(int portique, int typeT);
void transfert_vers_P2(Transport * transport); 
void transfert_container(Container container, Transport origine, Transport destination);
void dechargement(Transport transport);

//-------------- Variables globales -----------------------
//constantes
int nb_transport_termine = 0;
int nb_transport_total = NB_PENICHE + NB_TRAIN + NB_CAMION;

const int Capacite[3] = {CAPACITE_PENICHE,CAPACITE_TRAIN,CAPACITE_CAMION};
const char * transportString[3] = {"PENICHE","TRAIN","CAMION"};
const char * destinationString[4] = {"NORD","SUD","EST", "OUEST"};

const int portiquePlace[3] = {1,1,2}; 
//variables

/*Transport penicheA;
Transport penicheB;
Transport trainA;
Transport trainB;*/
int nb_transport_portique[2][4] = {0};
//portique[P1][PENICHE OU TRAIN OU CAMION A OU CAMION B]
//portique[P2][PENICHE OU TRAIN OU CAMION C OU CAMION D]

Transport peniche[2];       //peniche[A ou B]
Transport train[2];         //train[A ou B]
Transport camion[4];     //camion[portique][A B C ou D] 

Container container_peniche[2][CAPACITE_PENICHE]; 
Container container_train[2][CAPACITE_TRAIN];
Container container_camion[4][CAPACITE_CAMION];

/*
Container container_camionP1[2][CAPACITE_CAMION];
Container container_camionP2[2][CAPACITE_CAMION];
*/
//Container container_[4][2][CAPACITE_CAMION];

// TEST TABLEAU D'ADRESSE DE TABLEAU 2D
//Container * container_address[4] = {&container_peniche[2][CAPACITE_PENICHE], &container_train[2][CAPACITE_TRAIN],&container_camionP1[4][CAPACITE_CAMION], &container_camionP2[4][CAPACITE_CAMION]}; //tableau de pointeur de tableau contenant les containers ci-dessus
//Container * container_address[];
//{container_peniche, container_train,container_camionP1, container_camionP2}; //tableau de pointeur de tableau contenant les containers ci-dessus

int nombre_aleatoire(int min, int max) {
  return(min + (rand() % (max - min)));
}

//Fonction qui remplit le tableau d'un transport avec des éléments de types structure Container

void remplir_transport(Transport transport) {
    //int a_ou_b = transport.compteurGlobal%2;
    

    for (int i=0; i < transport.nb_container; i++) {
        switch(transport.typeTransport){
            case PENICHE:
                container_peniche[transport.lettreABCD][i]= create_container(transport);;
                break;
            case TRAIN:
                container_train[transport.lettreABCD][i]= create_container(transport);;
                break;
            case CAMION:
                container_camion[transport.lettreABCD][i]= create_container(transport);
                break;
        }
    }

    for (int i=transport.nb_container; i < Capacite[transport.typeTransport]; i++) {
        Container contNULL;
        contNULL.id = 0;
        contNULL.destination = 0;
        
        switch(transport.typeTransport){
            case PENICHE:
                container_peniche[transport.lettreABCD][i]= contNULL;
                break;
            case TRAIN:
                container_train[transport.lettreABCD][i]= contNULL;
                break;
            case CAMION:
                container_camion[transport.lettreABCD][i]= contNULL;
                break;
        }
    }

    //Ne pas oublier d'écraser les anciens container des anciens transports
}

//Fonction qui renvoie une structure Container avec une destination différentes de celle d'entrée
Container create_container(Transport transport){
    pthread_t tid_container;
    pthread_create(&tid_container,0, (void *(*)())(fonc_container),(void*)tid_container);
    
    Container nv_container;
    do{
            nv_container.destination = nombre_aleatoire(0, 4);
        } while(nv_container.destination == transport.destination);

    if(transport.typeTransport == CAMION){
        if(transport.destination <= 1){ //Camion qui va au P1/NORD/SUD
            do{
                nv_container.destination = nombre_aleatoire(0, 1);
            } while(nv_container.destination == transport.destination);  
        } 
        else { //Camion qui va au P2/EST/OUEST
            do{
                nv_container.destination = nombre_aleatoire(2, 4);
            } while(nv_container.destination == transport.destination);  
        }
    }
    nv_container.id = tid_container;
    return nv_container;
}


void * fonc_container(int i){
    //Rien
}

void * fonc_transport(int arg[]){
    //-- Création du transport --
    Transport transport;
    transport.typeTransport = arg[1]; //typeTransport;
    transport.destination = nombre_aleatoire(0, 4);
    transport.nb_container = nombre_aleatoire(0, Capacite[transport.typeTransport]+1);
    transport.position = P1;
    transport.compteurGlobal = arg[0];
    transport.lettreABCD = transport.compteurGlobal%2;
    pthread_mutex_unlock(&mutex_arg);

    if(transport.typeTransport == CAMION){
        transport.lettreABCD = transport.destination;
        //printf("\ncamion cree\n");
    }

    int portique_camion = P1;
    if(transport.typeTransport == CAMION && transport.destination >=2){ //camion destination = EST ou OUEST
        portique_camion = P2;
        transport.position = P2;
    }

    //-- On attend qu'il ya de la place au portique 1 --
    pthread_mutex_lock(&mutex_creation_transport[transport.typeTransport]); 
    pthread_mutex_lock(&mutex_nb_transport);  
    pthread_mutex_lock(&mutex_transport[transport.typeTransport][transport.lettreABCD]);  

    if(transport.typeTransport == CAMION){  //Camion
        if(transport.destination == NORD || transport.destination == SUD){ //On vérifie si c'est un camion A ou B
            while (nb_transport_portique[P1][CAMION+transport.lettreABCD] >= 1){ //vérifier la place en fonction de la zone au portique
                //Pas de place au portique 1 pour ce camion
                pthread_cond_wait(&cond_nb_transport[transport.typeTransport], &mutex_nb_transport);
            }
                nb_transport_portique[P1][CAMION+transport.lettreABCD] = 1;
        } else {
            // On fait rien si le camion est un camion-portique-2
        }   
    } 
    else { //Peniche ou Train 
        while (nb_transport_portique[P1][transport.typeTransport] >= 1 ){ 
            //Pas de place au portique 1 pour ce transport
            pthread_cond_wait(&cond_nb_transport[transport.typeTransport], &mutex_nb_transport);
        }
        nb_transport_portique[P1][transport.typeTransport] = 1;
    }
    //--------------Remplissage du transport------------
    switch(transport.typeTransport){ //rempli les tableaux des variables globaux correspondant au places des portiques occupés
        case PENICHE:
            transport.id = tid_peniche[transport.compteurGlobal];
            peniche[transport.lettreABCD] = transport;
            break;
        case TRAIN:
            transport.id = tid_train[transport.compteurGlobal];
            train[transport.lettreABCD] = transport;
            break;
        case CAMION:
            transport.id = tid_camion[transport.compteurGlobal];
            camion[transport.lettreABCD] = transport;
            break;
    }
    
    remplir_transport(transport); //rempli les contenairs du transport

    pthread_mutex_unlock(&mutex_transport[transport.typeTransport][transport.lettreABCD]);  
    pthread_mutex_unlock(&mutex_nb_transport);
    pthread_mutex_unlock(&mutex_creation_transport[transport.typeTransport]);
    //--------------Fin de création et remplissage du transport------------
    
    if(portique_camion == P1){
        pthread_mutex_lock(&printf_mutex);
        printf("\n ->%c (%c)(%c) %ld arrive au P1", transportString[transport.typeTransport][0],'A'+transport.lettreABCD,destinationString[transport.destination][0],transport.id);
        pthread_mutex_unlock(&printf_mutex);
        usleep(100);

        //--- Traitement du transport -----------
        pthread_cond_signal(&cond_portique1);
        dechargement(transport);
        //--- Effectuer les transferts de container --
        
        pthread_mutex_lock(&printf_mutex);
        printf("\n   %c (%c)(%c) %ld attend au P1", transportString[transport.typeTransport][0],'A'+transport.lettreABCD,destinationString[transport.destination][0] ,transport.id);
        pthread_mutex_unlock(&printf_mutex);

        for(int temps=0; temps<DELAI_ATTENTE; temps++)
        {
            //printf(".");
            sleep(1);
        }
    }

    //--- Fin avec P1 donc transfert vers P2 --- 
    pthread_mutex_lock(&mutex_nb_transport);
    
    if(transport.typeTransport == CAMION){  //Camion
        if(transport.destination == EST || transport.destination==OUEST){
            while (nb_transport_portique[P2][transport.lettreABCD] >= 1){ //vérifier la place en fonction de la zone au portique
                //Pas de place au portique 2 pour ce camion
                pthread_cond_wait(&cond_nb_transport[transport.typeTransport], &mutex_nb_transport);
            }
                nb_transport_portique[P2][transport.lettreABCD] = 1;
        } else {
            // On fait rien si le camion est un camion-portique-1
            nb_transport_portique[P1][CAMION+transport.lettreABCD] = 0;
        }   
    } else { //Cas pour Peniche ou Train 
        while (nb_transport_portique[P2][transport.typeTransport] >= 1 ){ 
            //Pas de place au portique 2 pour ce transport
        //printf(" P:%d T:%d C(A):%d C(B):%d",nb_transport_portique[P1][0],nb_transport_portique[P1][1],nb_transport_portique[P1][2],nb_transport_portique[P1][3]);

            pthread_cond_wait(&cond_nb_transport[transport.typeTransport], &mutex_nb_transport);
        }
        nb_transport_portique[P2][transport.typeTransport] = 1;
        nb_transport_portique[P1][transport.typeTransport] = 0;
    }
    
    transfert_vers_P2(&transport); //On transfère pas de camion, le camion va directement au portique lié à sa destination
  
        //printf(" P:%d T:%d C(A):%d C(B):%d",nb_transport_portique[P1][0],nb_transport_portique[P1][1],nb_transport_portique[P1][2],nb_transport_portique[P1][3]);
    
    usleep(100);
    pthread_mutex_unlock(&mutex_nb_transport);
    pthread_cond_signal(&cond_nb_transport[transport.typeTransport]);
    usleep(100);

    //--- Traitement du transport -----------
    if(!(transport.typeTransport == CAMION && portique_camion == P1)){
        pthread_mutex_lock(&printf_mutex);
        printf("\n   %c (%c)(%c) %ld attend au P2", transportString[transport.typeTransport][0],'A'+transport.lettreABCD,destinationString[transport.destination][0] ,transport.id);
        pthread_mutex_unlock(&printf_mutex);

        for(int temps=0; temps<DELAI_ATTENTE; temps++)
        {
            sleep(1);
        }

        pthread_mutex_lock(&mutex_nb_transport);
        if(transport.typeTransport != CAMION){
            nb_transport_portique[P2][transport.typeTransport] = 0;
        } else {
            nb_transport_portique[P2][transport.destination] = 0;
        }
        //printf("\n\n\t\tP:%d T:%d C(A):%d C(B):%d\n\n",nb_transport_portique[P1][0],nb_transport_portique[P1][1],nb_transport_portique[P1][2],nb_transport_portique[P1][3]);

        pthread_mutex_unlock(&mutex_nb_transport);
    }

    //--- Affichage du contenu du transport ------------------------
    /*pthread_mutex_lock(&printf_mutex);
    sleep(1);
    //--- Fin d'affichage du contenu du transport ------------------------
    pthread_mutex_unlock(&printf_mutex);*/

    //pthread_cond_signal(&cond_nb_transport);


    pthread_cond_signal(&cond_nb_transport[transport.typeTransport]);

    pthread_mutex_lock(&printf_mutex);
    printf("\n * %c (%c)(%c) %ld quitte le port *", transportString[transport.typeTransport][0],'A'+transport.lettreABCD,destinationString[transport.destination][0] ,transport.id);
    pthread_mutex_unlock(&printf_mutex);

    pthread_cond_signal(&cond_nb_transport[transport.typeTransport]);
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
                pthread_mutex_lock(&mutex_portique[portique]);
                pthread_cond_wait(&cond_portique1,&mutex_portique[portique]);
                pthread_mutex_lock(&mutex_nb_transport);

                if(nb_transport_portique[portique][PENICHE] !=0 
                    || nb_transport_portique[portique][TRAIN]!=0 
                    || nb_transport_portique[portique][CAMION]!=0
                    || nb_transport_portique[portique][CAMION+1]!=0
                    ){
                    
                    pthread_mutex_lock(&printf_mutex);
                    printf("\n=>Portique %d:",portique+1);
                    for(int typeT = 0;typeT<4;typeT++){
                        if(nb_transport_portique[portique][typeT] !=0 ){
                            afficherTransport(portique,typeT);
                        }        

                    }  
                    pthread_mutex_unlock(&printf_mutex);              
                    sleep(1);
                } 
                pthread_mutex_unlock(&mutex_nb_transport); 
                pthread_mutex_unlock(&mutex_portique[portique]);
                usleep(200);                
            }
        } else {
        //Portique 2
        //rien
        }
    }
}

void transfert_vers_P2(Transport * transport){
   
    
    switch(transport->typeTransport){
        case PENICHE: 
            transport->position = P2;
            peniche[transport->lettreABCD].position = P2;
            
            break;
        
        case TRAIN:
            transport->position = P2;
            train[transport->lettreABCD].position = P2;
            
            break;

        case CAMION:
            //RIEN
            break;
    }

    pthread_mutex_lock(&printf_mutex);
    if(transport->typeTransport == CAMION && transport->lettreABCD > 1) {
        printf("\n ->%c (%c)(%c) %ld arrive au P2", transportString[transport->typeTransport][0],'A'+transport->lettreABCD,destinationString[transport->destination][0] ,transport->id);
    }
    else if(transport->typeTransport != CAMION) {
        printf("\n >>%c (%c)(%c) %ld va au P2>>", transportString[transport->typeTransport][0],'A'+transport->lettreABCD,destinationString[transport->destination][0] ,transport->id);
    }
    pthread_mutex_unlock(&printf_mutex);
}



void transfert_container(Container container, Transport origine, Transport destination){

    Container contNULL;
    contNULL.id = 0;
    contNULL.destination = 0;
    Container tmp; //container temporaire
    
    pthread_mutex_lock(&mutex_portique[origine.position]);
    switch(origine.typeTransport){
        case PENICHE:
            for (int i=0; i<CAPACITE_PENICHE; i++) {
                if(container_peniche[origine.lettreABCD][i].id == container.id){
                    tmp = container_peniche[origine.lettreABCD][i];
                    container_peniche[origine.lettreABCD][i] = contNULL; //on supprime l'ancien container du tableau
                    peniche[origine.lettreABCD].nb_container--;
                }
            }
            break;
        case TRAIN:
            
            for (int i=0; i<CAPACITE_TRAIN; i++) {
                if(container_train[origine.lettreABCD][i].id == container.id){
                    tmp = container_train[origine.lettreABCD][i];
                    container_train[origine.lettreABCD][i] = contNULL;
                    train[origine.lettreABCD].nb_container--;
                }
            }
            break;       
        case CAMION:
            for (int i=0; i<CAPACITE_CAMION; i++) {
                if(container_camion[origine.lettreABCD][i].id == container.id){
                    tmp = container_camion[origine.lettreABCD][i];
                    container_camion[origine.lettreABCD][i] = contNULL;
                    camion[origine.lettreABCD].nb_container--;
                }
            }
            
            break;
    }
    int i = 0;
    switch(destination.typeTransport){
        case PENICHE:
            while (container_peniche[destination.lettreABCD][i].id != contNULL.id && i<CAPACITE_PENICHE)
            {
                i++;
            } 
            container_peniche[destination.lettreABCD][i]=tmp;
            peniche[destination.lettreABCD].nb_container++;
            
            break;
            
        case TRAIN:
            for (int i=0; i<CAPACITE_TRAIN; i++) {
                if(container_train[destination.lettreABCD][i].id == contNULL.id){
                    container_train[destination.lettreABCD][i]=tmp;
                    train[destination.lettreABCD].nb_container++;
                }
            }
            break;
            
        case CAMION:
            for (int i=0; i<CAPACITE_CAMION; i++) {
                if(container_camion[destination.lettreABCD][i].id == contNULL.id){
                    container_camion[destination.lettreABCD][i]=tmp;
                    camion[destination.lettreABCD].nb_container++;
                }
            }
            break;
    }
    pthread_mutex_unlock(&mutex_portique[origine.position]);

    pthread_mutex_lock(&printf_mutex);
    int p = origine.position+1;
    printf("\n\nLe portique %d transporte le container %ld ",p,container.id);
    printf("de %s (%ld) ",transportString[origine.typeTransport],origine.id);
    printf("vers %s (%ld)\n",transportString[destination.typeTransport],destination.id);
    pthread_mutex_unlock(&printf_mutex);
}

//Permet de connaitre lequel du transport A ou B est à un portique donné
int PositionToTransport(int portique, typeTransport type){ 

    switch(type){
        case PENICHE:
            if (peniche[A].position == portique){
                return A;
            }else if (peniche[B].position == portique){
                return B;
            }
            break;
        
        case TRAIN:
            if (train[A].position == portique){
                return A;
            }else if (train[B].position == portique){
                return B;
            }
            break;

        case CAMION:
            if(portique == P1) {
                return A;
            } else {
                return C;
            }
            break;
    }
    return 2;
}
/* //A metre ds le compte rendu

Chaque transport arrivant au portique lance une fonction déchargement 
Fonction déchargement:
    -   vérifie pour chaque contenair du transport dont la destination n'est pas la même que celle du transport, 
    -   s'il est possible d'effectuer un transfert de ce contenair avec un autre transport du portique
    -   avant chaque vérification entre la destination du contenair et la destination d'un transport on bloque ce transport, 
        on effectue un eventuel transfert et on le débloque après.

 */
void dechargement(Transport transport){
    int typeT = transport.typeTransport;
    int lettre = transport.lettreABCD; 
    int portique = transport.position;

    pthread_mutex_lock(&mutex_container[typeT][lettre]); //on lit les containers de la péniche
    switch (typeT){
        case PENICHE:
            if (peniche[lettre].nb_container>0){
                for (int i = 0; i < peniche[lettre].nb_container; i++){
                    if (container_peniche[lettre][i].destination != peniche[lettre].destination && container_peniche[lettre][i].id != 0) { 
                        //Si le container possède une destination différente de la péniche
                        //bloquage du camion

                        pthread_mutex_lock(&mutex_nb_transport);
                        if(nb_transport_portique[portique][container_peniche[lettre][i].destination] == 1){
                            pthread_mutex_lock(&mutex_transport[CAMION][container_peniche[lettre][i].destination]); //on bloque le camion au portique
                            pthread_mutex_lock(&mutex_container[CAMION][container_peniche[lettre][i].destination]);

                            if (container_peniche[lettre][i].destination == camion[container_peniche[lettre][i].destination].destination 
                                && camion[container_peniche[lettre][i].destination].nb_container < Capacite[CAMION]
                                && nb_transport_portique[portique][CAMION + container_peniche[lettre][i].destination%2]!=0
                            ) {
                                //Si le container possède une destination égale à celle d'un camion du portique 1
                                transfert_container(container_peniche[PositionToTransport(portique, PENICHE)][i], peniche[PositionToTransport(portique, PENICHE)], camion[container_peniche[lettre][i].destination]);
                            }
                            pthread_mutex_unlock(&mutex_transport[CAMION][container_peniche[lettre][i].destination]); //on debloque le camion au portique
                            pthread_mutex_unlock(&mutex_container[CAMION][container_peniche[lettre][i].destination]);
                        }
                        pthread_mutex_unlock(&mutex_nb_transport);

                        /*pthread_mutex_lock(&mutex_transport[CAMION][PositionToTransport(portique, CAMION)]); //on bloque le camion au portique
                        pthread_mutex_lock(&mutex_container[CAMION][PositionToTransport(portique, CAMION)]); //on lit les containers
                        if (container_peniche[lettre][i].destination == camion[PositionToTransport(portique, CAMION)].destination
                            && camion[PositionToTransport(portique, CAMION)].nb_container < Capacite[CAMION]
                            && nb_transport_portique[portique][PositionToTransport(portique, CAMION)]!=0
                        ) {
                            transfert_container(container_peniche[PositionToTransport(portique, PENICHE)][i], peniche[lettre], camion[PositionToTransport(portique, CAMION)]);
                            printf("*C*");
                        }
                        pthread_mutex_unlock(&mutex_transport[CAMION][PositionToTransport(portique, CAMION)]);
                        pthread_mutex_unlock(&mutex_container[CAMION][PositionToTransport(portique, CAMION)]);


                        pthread_mutex_lock(&mutex_container[CAMION][PositionToTransport(portique, CAMION)+1]);
                        pthread_mutex_lock(&mutex_transport[CAMION][PositionToTransport(portique, CAMION)+1]);
                        if (container_peniche[lettre][i].destination == camion[PositionToTransport(portique, CAMION)+1].destination
                            && camion[PositionToTransport(portique, CAMION)+1].nb_container < Capacite[CAMION]
                            && nb_transport_portique[portique][PositionToTransport(portique, CAMION)+1]!=0
                        ) {
                            transfert_container(container_peniche[PositionToTransport(portique, PENICHE)][i], peniche[lettre], camion[PositionToTransport(portique, CAMION)+1]);
                            printf("*C+1*");
                        }
                        pthread_mutex_unlock(&mutex_transport[CAMION][PositionToTransport(portique, CAMION)]+1);
                        pthread_mutex_unlock(&mutex_container[CAMION][PositionToTransport(portique, CAMION)+1]);
                        
                        */
                        
                        pthread_mutex_lock(&mutex_transport[TRAIN][PositionToTransport(portique, TRAIN)]);
                        pthread_mutex_lock(&mutex_container[TRAIN][PositionToTransport(portique, TRAIN)]);
                        if (container_peniche[lettre][i].destination == train[PositionToTransport(portique, TRAIN)].destination
                            && train[PositionToTransport(portique, TRAIN)].nb_container < Capacite[TRAIN]
                            && nb_transport_portique[portique][TRAIN]!=0
                        ) {
                            transfert_container(container_peniche[lettre][i], peniche[lettre], train[PositionToTransport(portique, TRAIN)]);
                            printf("*T*");
                        }
                        pthread_mutex_unlock(&mutex_transport[TRAIN][PositionToTransport(portique, TRAIN)]);
                        pthread_mutex_unlock(&mutex_container[TRAIN][PositionToTransport(portique, TRAIN)]);
                        
                    }
                }
            }
            
            break;  
        case TRAIN:
            for(int i=0; i<CAPACITE_TRAIN; i++){
                if (container_train[lettre][i].destination != train[lettre].destination && container_train[lettre][i].id != 0) { 
                    //Si le container possède une destination différente du train
                    
                    /*pthread_mutex_lock(&mutex_transport[CAMION][PositionToTransport(portique, CAMION)]); //on bloque le camion au portique
                    pthread_mutex_lock(&mutex_container[CAMION][PositionToTransport(portique, CAMION)]);
                    if (container_train[lettre][i].destination == camion[PositionToTransport(portique, CAMION)].destination 
                        && camion[PositionToTransport(portique, CAMION)].nb_container < Capacite[CAMION]
                    ) {
                        //Si le container possède une destination égale à celle d'un camion du portique 1
                        transfert_container(container_train[PositionToTransport(portique, TRAIN)][i], train[PositionToTransport(portique, TRAIN)], camion[PositionToTransport(portique, CAMION)]);
                    }
                    pthread_mutex_unlock(&mutex_transport[CAMION][PositionToTransport(portique, CAMION)]);
                    pthread_mutex_unlock(&mutex_container[CAMION][PositionToTransport(portique, CAMION)]);

                    pthread_mutex_lock(&mutex_container[CAMION][PositionToTransport(portique, CAMION)+1]);
                    pthread_mutex_lock(&mutex_transport[CAMION][PositionToTransport(portique, CAMION)+1]);
                    if (container_train[lettre][i].destination == camion[PositionToTransport(portique, CAMION)].destination 
                        && camion[PositionToTransport(portique, CAMION)+1].nb_container < Capacite[CAMION]
                    ) {
                        //Si le container possède une destination égale à celle d'un camion du portique 2
                        transfert_container(container_train[PositionToTransport(portique, TRAIN)][i], train[PositionToTransport(portique, TRAIN)], camion[PositionToTransport(portique, CAMION)]);
                    }
                    pthread_mutex_unlock(&mutex_transport[CAMION][PositionToTransport(portique, CAMION)]+1);
                    pthread_mutex_unlock(&mutex_container[CAMION][PositionToTransport(portique, CAMION)+1]);
                    */
                    
                    //if(nb_transport_portique[portique][camion[container_train[lettre][i].destination]] == 1){
                    pthread_mutex_lock(&mutex_nb_transport);
                    if(nb_transport_portique[portique][container_train[lettre][i].destination] == 1){
                        pthread_mutex_lock(&mutex_transport[CAMION][container_train[lettre][i].destination]); //on bloque le camion au portique
                        pthread_mutex_lock(&mutex_container[CAMION][container_train[lettre][i].destination]);

                        if (container_train[lettre][i].destination == camion[container_train[lettre][i].destination].destination 
                            && camion[container_train[lettre][i].destination].nb_container < Capacite[CAMION]
                            && nb_transport_portique[portique][CAMION + container_train[lettre][i].destination%2]!=0
                        ) {
                            //Si le container possède une destination égale à celle d'un camion du portique 1
                            transfert_container(container_train[PositionToTransport(portique, TRAIN)][i], train[PositionToTransport(portique, TRAIN)], camion[container_train[lettre][i].destination]);
                        }
                        pthread_mutex_unlock(&mutex_transport[CAMION][container_train[lettre][i].destination]); //on debloque le camion au portique
                        pthread_mutex_unlock(&mutex_container[CAMION][container_train[lettre][i].destination]);
                    }
                    pthread_mutex_unlock(&mutex_nb_transport);

                    pthread_mutex_lock(&mutex_transport[PENICHE][PositionToTransport(portique, PENICHE)]); 
                    pthread_mutex_lock(&mutex_container[PENICHE][PositionToTransport(portique, PENICHE)]);
                    if (container_train[lettre][i].destination == peniche[PositionToTransport(portique, PENICHE)].destination 
                        && peniche[PositionToTransport(portique, PENICHE)].nb_container < Capacite[PENICHE]
                        && nb_transport_portique[portique][TRAIN]!=0
                    ) {
                        transfert_container(container_train[lettre][i], train[lettre], peniche[PositionToTransport(portique, PENICHE)]);
                    }
                    pthread_mutex_unlock(&mutex_transport[PENICHE][PositionToTransport(portique, PENICHE)]); 
                    pthread_mutex_unlock(&mutex_container[PENICHE][PositionToTransport(portique, PENICHE)]);
                    
                    
                }
            }
            break;
        default:
            for(int i=0; i<CAPACITE_CAMION; i++){
                if (container_camion[lettre]->destination != camion[lettre].destination && container_camion[lettre][i].id != 0){
                    if (container_camion[lettre]->destination == peniche[PositionToTransport(portique, PENICHE)].destination 
                        && peniche[PositionToTransport(portique, PENICHE)].nb_container < Capacite[PENICHE]
                        && nb_transport_portique[portique][PositionToTransport(portique, PENICHE)]!=0
                        ) {
                        //Si le container possède une destination égale à celle d'une péniche
                        transfert_container(container_camion[lettre][i], camion[lettre], peniche[PositionToTransport(portique, PENICHE)]);
                    }
                    if (container_camion[lettre]->destination == train[lettre].destination 
                        && train[PositionToTransport(portique, TRAIN)].nb_container < Capacite[TRAIN]
                        && nb_transport_portique[portique][PositionToTransport(portique, TRAIN)]!=0
                        ) {
                        //Si le container possède une destination égale à celle d'
                        transfert_container(container_camion[lettre][i], camion[lettre], train[PositionToTransport(portique, PENICHE)]);
                    }
                    int camion_oppose = lettre+1; // A ou C on veut B ou D
                    if(lettre==B || lettre == D) { camion_oppose = lettre-1;} // B ou D on veut A ou C
                    if (container_camion[lettre]->destination == camion[camion_oppose].destination 
                        && camion[camion_oppose].nb_container < Capacite[CAMION]
                        && nb_transport_portique[portique][CAMION + camion_oppose%2]!=0
                        ) {
                        //Si le container possède une destination égale à celle d'
                        transfert_container(container_camion[lettre][i], camion[lettre], camion[camion_oppose]);
                    }
                }  
             }    
            break;   
        
    }

    pthread_mutex_unlock(&mutex_container[typeT][lettre]);
    
}  

void afficherTransport(int portique, int typeT){
    if (nb_transport_portique[P1][typeT]>0){
        int ab=A;
        //printf(" P:%d T:%d C(A):%d C(B):%d",nb_transport_portique[P1][0],nb_transport_portique[P1][1],nb_transport_portique[P1][2],nb_transport_portique[P1][3]);
        switch (typeT){
            case PENICHE:
                if(peniche[A].position == P2){
                    ab = B; //A=0
                }              
                
                printf("\n\t• Peniche %c: id:%ld - destination:%d - nb contenair:%d",ab+'A', peniche[ab].id,peniche[ab].destination,peniche[ab].nb_container);
                if (peniche[ab].nb_container>0){
                    for (int i = 0; i < Capacite[typeT]; i++){
                        if(container_peniche[ab][i].id != 0 ){    
                            printf("\n\t\t> Contenair: %ld - destination: %d",container_peniche[ab][i].id,container_peniche[ab][i].destination);
                        }
                    }
                }
                break;
            
            case TRAIN:
                if(train[A].position == P2){
                    ab = B; //A=0
                } 
                printf("\n\t• Train %c: id:%ld - destination:%d - nb contenair:%d",ab+'A', train[ab].id,train[ab].destination,train[ab].nb_container);
                if (train[ab].nb_container>0){
                    for (int i = 0; i < Capacite[typeT]; i++){
                        if(container_train[ab][i].id != 0 ){ 
                            printf("\n\t\t> Contenair: %ld - destination: %d",container_train[ab][i].id,container_train[ab][i].destination);
                        }
                    }
                }
                break;

            default:
                
                printf("\n\t• Camion %c: id:%ld - destination:%d - nb contenair:%d",typeT%2+'A', camion[typeT%2].id,camion[typeT%2].destination,camion[typeT%2].nb_container);
                if(camion[typeT%2].nb_container>0){
                    for (int i = 0; i < Capacite[typeT]; i++){
                        if(container_camion[typeT%2][i].id != 0 ){    
                            printf("\n\t\t> Contenair: %ld - destination: %d",container_camion[typeT%2][i].id,container_camion[typeT%2][i].destination);
                        }
                    }
                }
                
            
                break;   
        }
    }
}

int superviseur(int portique) {
    

    return 0;
}


int main() {
    srand(time(NULL));

    //-- Initialisation des mutex --
    for (int i = 0; i < 3; i++) {
        pthread_mutex_init(&mutex_creation_transport[i],0);
        for(int j=0;j<4;j++){
            pthread_mutex_init(&mutex_dechargement[i][j],0);
            pthread_mutex_init(&mutex_transport[i][j],0);
            pthread_mutex_init(&mutex_container[i][j],0);
        }    
    }
    
    pthread_mutex_init(&mutex_portique[P1],0); 
    pthread_mutex_init(&mutex_portique[P2],0); 
    
    pthread_mutex_init(&printf_mutex,0);
    pthread_mutex_init(&mutex_nb_transport,0);
    pthread_mutex_init(&mutex_nb_transport_termine,0);
    
    pthread_mutex_init(&mutex_arg,0);

    for (int i = 0; i < 3; i++) {
        pthread_cond_init(&cond_nb_transport[i], NULL);
    }
    pthread_cond_init(&cond_portique1, NULL);

    //-- Création des threads portiques --
    for(int i=0;i<2;i++){
        int *iptr = &i;
        pthread_create(tid_portique,0, (void *(*)())(fonc_portique),(void*)iptr);   
	}
   
	//-- Création des threads transport --
	for(int i=0;i<NB_PENICHE;i++){
        //pthread_mutex_lock(&mutex_creation_transport[PENICHE]);
        pthread_mutex_lock(&mutex_arg);
        int arg[2]={i,PENICHE};
	    pthread_create(tid_peniche+i,0, (void *(*)())(fonc_transport),(void*)arg);
	}

    for(int i=0;i<NB_TRAIN;i++){
        //pthread_mutex_lock(&mutex_creation_transport[TRAIN]);
        pthread_mutex_lock(&mutex_arg);
        int arg[2]={i,TRAIN};
	    pthread_create(tid_train+i,0,(void *(*)())(fonc_transport),(void*)arg);
	}

    for(int i=0;i<NB_CAMION;i++){
        //thread_mutex_lock(&mutex_creation_transport[CAMION]);
        pthread_mutex_lock(&mutex_arg);
        int arg[2]={i,CAMION};
	    pthread_create(tid_camion+i,0,(void *(*)())(fonc_transport),(void*)arg);
	}
    
	//-- On attend la fin de toutes les threads -- 
	for(int i=0;i<NB_PENICHE;i++){
		pthread_join(tid_peniche[i],NULL);
	}	
	for(int i=0;i<NB_TRAIN;i++){
		pthread_join(tid_train[i],NULL);
	}

    for(int i=0;i<NB_CAMION;i++){
		pthread_join(tid_camion[i],NULL);
	}

    for(int i=0;i<2;i++){
        pthread_join(tid_portique[i],NULL); 
	}

	//-- On libère les ressources -- 
    for (int i = 0; i < 3; i++) {
        pthread_mutex_destroy(&mutex_creation_transport[i]);
    }
    pthread_mutex_destroy(&printf_mutex);
    pthread_mutex_destroy(&mutex_nb_transport_termine);

    pthread_mutex_destroy(&mutex_portique[P1]); 
    pthread_mutex_destroy(&mutex_portique[P2]); 

    pthread_mutex_destroy(&mutex_arg);

    for (int i = 0; i < 3; i++) {
        pthread_cond_destroy(&cond_nb_transport[i]);
    }

    for (int i = 0; i < 3; i++) {
            for(int j=0;j<4;j++){
                pthread_mutex_destroy(&mutex_dechargement[i][j]);
                pthread_mutex_destroy(&mutex_transport[i][j]);
                pthread_mutex_destroy(&mutex_container[i][j]);
            }    
    }
    //pthread_cond_destroy(&cond_nb_transport);
    pthread_cond_destroy(&cond_portique1);
     
	printf("\nFin de tous les threads, tous les transports sont partis");
	exit(0);
}
