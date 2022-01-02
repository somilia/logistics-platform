#include "common.h"

//Permet de connaitre lequel du transport A/B/C/ou D est à un portique donné
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
/* 
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

    //pthread_mutex_lock(&mutex_container[typeT][lettre]); //on lit les containers de la péniche
    switch (typeT){
        case PENICHE:
            if (peniche[lettre].nb_container>0){
                for (int i = 0; i < peniche[lettre].nb_container; i++){
                    if (container_peniche[lettre][i].destination != peniche[lettre].destination && container_peniche[lettre][i].id != 0) { 
                        
                        pthread_mutex_lock(&mutex_nb_transport);
                            
                            if (container_peniche[lettre][i].destination == camion[container_peniche[lettre][i].destination].destination 
                                && camion[container_peniche[lettre][i].destination].nb_container < Capacite[CAMION]
                                && nb_transport_portique[portique][CAMION + container_peniche[lettre][i].destination%2]!=0
                            ) {
                                //Si le container possède une destination égale à celle d'un camion du portique 1
                                transfert_container(container_peniche[PositionToTransport(portique, PENICHE)][i], peniche[PositionToTransport(portique, PENICHE)], camion[container_peniche[lettre][i].destination]);
                                printf("*C*");
                            }

                        if (container_peniche[lettre][i].destination == train[PositionToTransport(portique, TRAIN)].destination
                            && train[PositionToTransport(portique, TRAIN)].nb_container < Capacite[TRAIN]
                            && nb_transport_portique[portique][TRAIN]!=0
                        ) {
                            transfert_container(container_peniche[lettre][i], peniche[lettre], train[PositionToTransport(portique, TRAIN)]);
                            printf("*T*");
                        }

                        pthread_mutex_unlock(&mutex_nb_transport);
                    }
                }
            } 
            break;  
        case TRAIN:
            for(int i=0; i<CAPACITE_TRAIN; i++){
                if (container_train[lettre][i].destination != train[lettre].destination && container_train[lettre][i].id != 0) { 

                    pthread_mutex_lock(&mutex_nb_transport);

                    if (container_train[lettre][i].destination == camion[container_train[lettre][i].destination].destination 
                        && camion[container_train[lettre][i].destination].nb_container < Capacite[CAMION]
                        && nb_transport_portique[portique][CAMION + container_train[lettre][i].destination%2]!=0
                    ) {
                        //Si le container possède une destination égale à celle d'un camion du portique 1
                        transfert_container(container_train[PositionToTransport(portique, TRAIN)][i], train[PositionToTransport(portique, TRAIN)], camion[container_train[lettre][i].destination]);
                    }

                    if (container_train[lettre][i].destination == peniche[PositionToTransport(portique, PENICHE)].destination 
                        && peniche[PositionToTransport(portique, PENICHE)].nb_container < Capacite[PENICHE]
                        && nb_transport_portique[portique][PENICHE]!=0
                    ) {
                        transfert_container(container_train[lettre][i], train[lettre], peniche[PositionToTransport(portique, PENICHE)]);
                    }
                    
                    pthread_mutex_unlock(&mutex_nb_transport);
                }
            }
            break;
        default:
            for(int i=0; i<CAPACITE_CAMION; i++){
                pthread_mutex_lock(&mutex_nb_transport);
                if (container_camion[lettre]->destination != camion[lettre].destination && container_camion[lettre][i].id != 0){
                    if (container_camion[lettre]->destination == peniche[PositionToTransport(portique, PENICHE)].destination 
                        && peniche[PositionToTransport(portique, PENICHE)].nb_container < Capacite[PENICHE]
                        && nb_transport_portique[portique][PositionToTransport(portique, PENICHE)]!=0
                        ) {
                        //Si le container possède une destination égale à celle d'une péniche
                        transfert_container(container_camion[lettre][i], camion[lettre], peniche[PositionToTransport(portique, PENICHE)]);
                    }
                    if (container_camion[lettre]->destination == train[PositionToTransport(portique, PENICHE)].destination 
                        && train[PositionToTransport(portique, TRAIN)].nb_container < Capacite[TRAIN]
                        && nb_transport_portique[portique][PositionToTransport(portique, TRAIN)]!=0
                        ) {
                        //Si le container possède une destination égale à celle d'un train
                        transfert_container(container_camion[lettre][i], camion[lettre], train[PositionToTransport(portique, PENICHE)]);
                    }
                    int camion_oppose = lettre+1; // A ou C on veut B ou D
                    if(lettre==B || lettre == D) { camion_oppose = lettre-1;} // B ou D on veut A ou C
                    if (container_camion[lettre]->destination == camion[camion_oppose].destination 
                        && camion[camion_oppose].nb_container < Capacite[CAMION]
                        && nb_transport_portique[portique][CAMION + camion_oppose%2]!=0
                        ) {
                        //Si le container possède une destination égale à celle d'un camion
                        transfert_container(container_camion[lettre][i], camion[lettre], camion[camion_oppose]);
                    }
                }  
                pthread_mutex_unlock(&mutex_nb_transport);
             }    
            break;   
        
    }

    pthread_mutex_unlock(&mutex_container[typeT][lettre]);
    
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
                    break;
                }
            }
            break;
        case TRAIN:
            for (int i=0; i<CAPACITE_TRAIN; i++) {
                if(container_train[origine.lettreABCD][i].id == container.id){
                    tmp = container_train[origine.lettreABCD][i];
                    container_train[origine.lettreABCD][i] = contNULL;
                    train[origine.lettreABCD].nb_container--;
                    break;
                }
            }
            break;       
        case CAMION:
            for (int i=0; i<CAPACITE_CAMION; i++) {
                if(container_camion[origine.lettreABCD][i].id == container.id){
                    tmp = container_camion[origine.lettreABCD][i];
                    container_camion[origine.lettreABCD][i] = contNULL;
                    camion[origine.lettreABCD].nb_container--;
                    break;
                }
            }
            break;
    }
    int i = 0;
    switch(destination.typeTransport){
        case PENICHE:
            /*while (container_peniche[destination.lettreABCD][i].id != contNULL.id && i<CAPACITE_PENICHE)
            {
                i++;
            } 
            container_peniche[destination.lettreABCD][i]=tmp;
            peniche[destination.lettreABCD].nb_container++;*/

            for (int i=0; i<CAPACITE_TRAIN; i++) {
                if(container_peniche[destination.lettreABCD][i].id == contNULL.id){
                    container_peniche[destination.lettreABCD][i]=tmp;
                    peniche[destination.lettreABCD].nb_container++;
                    break;
                }
            }
            break;
            
        case TRAIN:
            for (int i=0; i<CAPACITE_TRAIN; i++) {
                if(container_train[destination.lettreABCD][i].id == contNULL.id){
                    container_train[destination.lettreABCD][i]=tmp;
                    train[destination.lettreABCD].nb_container++;
                    break;
                }
            }
            break;
            
        case CAMION:
            for (int i=0; i<CAPACITE_CAMION; i++) {
                if(container_camion[destination.lettreABCD][i].id == contNULL.id){
                    container_camion[destination.lettreABCD][i]=tmp;
                    camion[destination.lettreABCD].nb_container++;
                    break;
                }
            }
            break;
    }
    pthread_mutex_unlock(&mutex_portique[origine.position]);

    int p = origine.position+1;
    printf("\n**\nLe portique %d transporte le container %ld ",p,container.id);
    printf("de %s (%ld) ",transportString[origine.typeTransport],origine.id);
    printf("vers %s (%ld)\n**\n",transportString[destination.typeTransport],destination.id);
}