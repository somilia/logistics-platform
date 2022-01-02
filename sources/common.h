#ifndef COMMON_H
#define COMMON_H
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
#define CAPACITE_PENICHE 5
#define CAPACITE_TRAIN 4
#define CAPACITE_CAMION 1

//-- Destinations --
#define NORD 0
#define SUD 1
#define EST 2
#define OUEST 3

#define DELAI_ATTENTE 3000000 //Temps en microsecondes qu'attend un transport au port avant de repartir


//--------------pthread -------------------------------
pthread_t tid_peniche[NB_PENICHE+1];
pthread_t tid_train[NB_TRAIN+1];
pthread_t tid_camion[NB_CAMION+1];


//-------------- pthread_mutex -------------------------------
pthread_mutex_t mutex_printf;
pthread_mutex_t mutex_aff_portique[2];
pthread_mutex_t mutex_portique[2];
pthread_mutex_t mutex_creation_transport[3];
pthread_mutex_t mutex_arg;
pthread_mutex_t mutex_nb_transport;
pthread_mutex_t mutex_nb_transport_termine;
pthread_mutex_t mutex_transport[3][4];
pthread_mutex_t mutex_container[3][4];

//-------------- pthread_cond -------------------------------

pthread_cond_t cond_nb_transport[3];
pthread_mutex_t mutex_dechargement[3][4];

//-------------- Enum -------------------------------
typedef enum {_NORD, _SUD, _OUEST, _EST} Destination;
typedef enum {_PENICHE, _TRAIN, _CAMION} typeTransport;




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

void transfert_vers_P2(Transport * transport); 
void afficher_container(Transport transport);
void transfert_container(Container container, Transport origine, Transport destination);
void dechargement(Transport transport);

//-------------- Variables globales -----------------------

int nb_transport_termine = 0;
int nb_transport_total = NB_PENICHE + NB_TRAIN + NB_CAMION;

const int Capacite[3] = {CAPACITE_PENICHE,CAPACITE_TRAIN,CAPACITE_CAMION};
const char * transportString[3] = {"PENICHE","TRAIN","CAMION"};
const char * destinationString[4] = {"NORD","SUD","EST", "OUEST"};

const int portiquePlace[3] = {1,1,2}; 

int nb_transport_portique[2][4] = {0};
//portique[P1][PENICHE OU TRAIN OU CAMION A OU CAMION B]
//portique[P2][PENICHE OU TRAIN OU CAMION C OU CAMION D]

Transport peniche[2];       //peniche[A ou B]
Transport train[2];         //train[A ou B]
Transport camion[4];     //camion[portique][A B C ou D] 

Container container_peniche[2][CAPACITE_PENICHE]; 
Container container_train[2][CAPACITE_TRAIN];
Container container_camion[4][CAPACITE_CAMION];

// TEST TABLEAU D'ADRESSE DE TABLEAU 2D - Solution abandonnée
//Container * container_address[4] = {&container_peniche[2][CAPACITE_PENICHE], &container_train[2][CAPACITE_TRAIN],&container_camionP1[4][CAPACITE_CAMION], &container_camionP2[4][CAPACITE_CAMION]}; //tableau de pointeur de tableau contenant les containers ci-dessus
//Container * container_address[];
//{container_peniche, container_train,container_camionP1, container_camionP2}; //tableau de pointeur de tableau contenant les containers ci-dessus




#endif