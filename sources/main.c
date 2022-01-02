#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "transport.c"
#include "dechargement.c"

void * fonc_transport(int arg[]){
    //-- Création du transport --
    Transport transport;
    transport.typeTransport = arg[1]; //typeTransport;

    transport.destination = nombre_aleatoire(0, 4);

    if(transport.typeTransport == PENICHE){
        int dest[2] = {NORD, EST};
        int index = nombre_aleatoire(0, 2);
        transport.destination = dest[index];
    }
    else if(transport.typeTransport == TRAIN){
        int dest[2] = {SUD, OUEST};
        int index = nombre_aleatoire(0, 2);
        transport.destination = dest[index];
    }


    transport.nb_container = nombre_aleatoire(0, Capacite[transport.typeTransport]+1);
    transport.position = P1;
    transport.compteurGlobal = arg[0];
    transport.lettreABCD = transport.compteurGlobal%2;
    pthread_mutex_unlock(&mutex_arg);

    if(transport.typeTransport == CAMION){
        transport.lettreABCD = transport.destination;
    }

    int portique_camion = P1;
    if(transport.typeTransport == CAMION && transport.destination >=2){ //camion destination = EST ou OUEST Donc Camion C ou D
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
            // On fait rien si le camion est un camion-portique-2 (celui-ci va aller au portique 2 directement)
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
    
    remplir_transport(transport); //On rempli les transports de contenairs
    afficher_container(transport); //On affiche les containers

    pthread_mutex_unlock(&mutex_transport[transport.typeTransport][transport.lettreABCD]);  
    pthread_mutex_unlock(&mutex_nb_transport);
    pthread_mutex_unlock(&mutex_creation_transport[transport.typeTransport]);
    //--------------Fin de création et remplissage du transport------------
    
    if(portique_camion == P1){
        pthread_mutex_lock(&mutex_printf);
        printf("\n\t\t\t\t\t\t\t -> %c (%c)(%c) %ld arrive au P1", transportString[transport.typeTransport][0],'A'+transport.lettreABCD,destinationString[transport.destination][0],transport.id);
        pthread_mutex_unlock(&mutex_printf);
        usleep(100);
 
        //--- Effectuer les transferts de container --
        dechargement(transport);
        
        
        pthread_mutex_lock(&mutex_printf);
        printf("\n\t\t\t\t\t\t\t    %c (%c)(%c) %ld attend au P1", transportString[transport.typeTransport][0],'A'+transport.lettreABCD,destinationString[transport.destination][0] ,transport.id);
        pthread_mutex_unlock(&mutex_printf);

        usleep(DELAI_ATTENTE);
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
            afficher_container(transport);
        }   
    } else { //Cas pour Peniche ou Train 
        while (nb_transport_portique[P2][transport.typeTransport] >= 1 ){ 
            //Pas de place au portique 2 pour ce transport

            pthread_cond_wait(&cond_nb_transport[transport.typeTransport], &mutex_nb_transport); 
        }
        nb_transport_portique[P2][transport.typeTransport] = 1;
        nb_transport_portique[P1][transport.typeTransport] = 0;
    }
    
    transfert_vers_P2(&transport); //On transfère pas de camion, le camion va directement au portique lié à sa destination
    
    usleep(100);
    pthread_mutex_unlock(&mutex_nb_transport);
    pthread_cond_signal(&cond_nb_transport[transport.typeTransport]); //Le transport de type X vient de libérer une place, il réveille les transports X en attente...
    usleep(100);

    //--- Traitement du transport -----------
    if(!(transport.typeTransport == CAMION && portique_camion == P1)){ //Si ce n'est pas un camion-portique-1
        dechargement(transport);

        pthread_mutex_lock(&mutex_printf);
        printf("\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t   %c (%c)(%c) %ld attend au P2", transportString[transport.typeTransport][0],'A'+transport.lettreABCD,destinationString[transport.destination][0] ,transport.id);
        pthread_mutex_unlock(&mutex_printf);

        usleep(DELAI_ATTENTE);

        pthread_mutex_lock(&mutex_nb_transport);
        if(transport.typeTransport != CAMION){
            nb_transport_portique[P2][transport.typeTransport] = 0;
        } else {
            nb_transport_portique[P2][transport.destination] = 0;
        }
        afficher_container(transport);
        pthread_mutex_unlock(&mutex_nb_transport);
    }

    //Le transport a fini, il quitte le port
    pthread_mutex_lock(&mutex_printf);
    printf("\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t * %c (%c)(%c) %ld quitte le port *", transportString[transport.typeTransport][0],'A'+transport.lettreABCD,destinationString[transport.destination][0] ,transport.id);
    
    pthread_mutex_lock(&mutex_nb_transport_termine);
    nb_transport_termine++;
    printf(" (%d/%d)",nb_transport_termine,nb_transport_total);
    pthread_mutex_unlock(&mutex_nb_transport_termine);
    
    pthread_mutex_unlock(&mutex_printf);

    pthread_cond_signal(&cond_nb_transport[transport.typeTransport]); //Le transport de type X vient de libérer une place, il réveille les transports X en attente...
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

    pthread_mutex_lock(&mutex_printf);
    if(transport->typeTransport == CAMION && transport->lettreABCD > 1) {
        printf("\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t ->%c (%c)(%c) %ld arrive au P2", transportString[transport->typeTransport][0],'A'+transport->lettreABCD,destinationString[transport->destination][0] ,transport->id);
    }
    else if(transport->typeTransport != CAMION) {
        printf("\n\t\t\t\t\t\t\t\t\t\t   >> %c (%c)(%c) %ld va au P2 >>", transportString[transport->typeTransport][0],'A'+transport->lettreABCD,destinationString[transport->destination][0] ,transport->id);
    }
    pthread_mutex_unlock(&mutex_printf);
}

int main() {

    srand(time(NULL));

    printf("\nBonjour, bienvenue au port !\n Aujourd'hui, il y aura \n");
    printf("\t• %d peniches", NB_PENICHE);
    printf("\t• %d trains", NB_TRAIN);
    printf("\t• %d camions \n\n", NB_CAMION);


    //-- Initialisation des mutex --

    for (int i = 0; i < 3; i++) {
        pthread_mutex_init(&mutex_creation_transport[i],0);
        for(int j=0;j<4;j++){
            pthread_mutex_init(&mutex_dechargement[i][j],0);
            pthread_mutex_init(&mutex_transport[i][j],0);
            pthread_mutex_init(&mutex_container[i][j],0);
        }    
    }

    for(int i = 0; i<2; i++){
        pthread_mutex_init(&mutex_portique[i],0); 
        pthread_mutex_init(&mutex_aff_portique[i],0);
    }
    
    pthread_mutex_init(&mutex_printf,0);
    pthread_mutex_init(&mutex_nb_transport,0);
    pthread_mutex_init(&mutex_nb_transport_termine,0);
    
    pthread_mutex_init(&mutex_arg,0);

    for (int i = 0; i < 3; i++) {
        pthread_cond_init(&cond_nb_transport[i], NULL);
    }

	//-- Création des threads transports --
	for(int i=0;i<NB_PENICHE;i++){
        pthread_mutex_lock(&mutex_arg);
        int arg[2]={i,PENICHE};
	    pthread_create(tid_peniche+i,0, (void *(*)())(fonc_transport),(void*)arg);
	}

    for(int i=0;i<NB_TRAIN;i++){
        pthread_mutex_lock(&mutex_arg);
        int arg[2]={i,TRAIN};
	    pthread_create(tid_train+i,0,(void *(*)())(fonc_transport),(void*)arg);
	}

    for(int i=0;i<NB_CAMION;i++){
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

	//-- On libère les ressources -- 
    for (int i = 0; i < 3; i++) {
        pthread_mutex_destroy(&mutex_creation_transport[i]);
    }
    pthread_mutex_destroy(&mutex_printf);
    pthread_mutex_destroy(&mutex_nb_transport_termine);
 
    for(int i = 0; i<2; i++){
        pthread_mutex_destroy(&mutex_portique[i]); 
        pthread_mutex_destroy(&mutex_aff_portique[i]); 
    }

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

	printf("\n\n\n* Fin de tous les threads, tous les transports sont partis. *\n\n");
	exit(0);
}
