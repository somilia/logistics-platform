#include "common.h"

int nombre_aleatoire(int min, int max) {
  return(min + (rand() % (max - min)));
}

int typeToCapacite(int typeTransport) { //a changer en variable global
    int Capacite[3] = {CAPACITE_PENICHE,CAPACITE_TRAIN,CAPACITE_CAMION};
    return Capacite[typeTransport];
}

//Fonction qui remplit le tableau d'un transport avec des éléments de types structure Container
void remplir_transport(Transport transport) {
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

void afficherTransport(int portique, int typeT){
    if (nb_transport_portique[portique][typeT]>0){
        int ab=A;
        //printf(" P:%d T:%d C(A):%d C(B):%d",nb_transport_portique[P1][0],nb_transport_portique[P1][1],nb_transport_portique[P1][2],nb_transport_portique[P1][3]);
        switch (typeT){
            case PENICHE:
                if(peniche[A].position != portique){
                    ab = B; //A=0
                }              
                
                printf("\n\t• Peniche %c: id:%ld - destination:%c - nb contenair:%d",ab+'A', peniche[ab].id,destinationString[peniche[ab].destination][0],peniche[ab].nb_container);
                if (peniche[ab].nb_container>0){
                    for (int i = 0; i < Capacite[typeT]; i++){
                        if(container_peniche[ab][i].id != 0 ){    
                            printf("\n\t\t> Contenair: %ld - destination: %c",container_peniche[ab][i].id,destinationString[container_peniche[ab][i].destination][0]);
                        }
                    }
                }
                break;
            
            case TRAIN:
                if(train[A].position != portique){
                    ab = B; //A=0
                } 
                printf("\n\t• Train %c: id:%ld - destination:%c - nb contenair:%d",ab+'A', train[ab].id,destinationString[train[ab].destination][0],train[ab].nb_container);
                if (train[ab].nb_container>0){
                    for (int i = 0; i < Capacite[typeT]; i++){
                        if(container_train[ab][i].id != 0 ){ 
                            printf("\n\t\t> Contenair: %ld - destination: %c",container_train[ab][i].id,destinationString[container_train[ab][i].destination][0]);
                        }
                    }
                }
                break;

            default:
                if(portique == P1){
                    typeT -= 2;
                }
                printf("\n\t• Camion %c: id:%ld - destination:%c - nb contenair:%d",typeT+'A', camion[typeT].id,destinationString[camion[typeT].destination][0],camion[typeT].nb_container);
                if(camion[typeT].nb_container>0){
                    for (int i = 0; i < Capacite[CAMION]; i++){
                        if(container_camion[typeT][i].id != 0 ){    
                            printf("\n\t\t> Contenair: %ld - destination: %c",container_camion[typeT][i].id,destinationString[container_camion[typeT][i].destination][0]);
                        }
                    }
                }
                
            
                break;   
        }
    }
}

void afficher_container(Transport transport){
    pthread_mutex_lock(&mutex_printf);
    int portique = transport.position;
    int lettre = transport.lettreABCD;
    int type = transport.typeTransport;

    printf("\n• %s %c - id:%ld",transportString[type],'A'+lettre, transport.id);
    printf("\n-> Destination:%c - Nb contenair:%d",destinationString[transport.destination][0], transport.nb_container);
    if (transport.nb_container>0){
        for (int i = 0; i < Capacite[type]; i++){
            switch (type) {
            case PENICHE:
                if(container_peniche[lettre][i].id != 0 ){    
                    printf("\n  > Contenair: %ld - destination: %c",container_peniche[lettre][i].id,destinationString[container_peniche[lettre][i].destination][0]);
                }
                break;

            case TRAIN:
                if(container_train[lettre][i].id != 0 ){    
                    printf("\n  > Contenair: %ld - destination: %c",container_train[lettre][i].id,destinationString[container_train[lettre][i].destination][0]);
                }
                break;

            case CAMION:
                if(container_camion[lettre][i].id != 0 ){    
                    printf("\n  > Contenair: %ld - destination: %c",container_camion[lettre][i].id,destinationString[container_camion[lettre][i].destination][0]);
                }
                break;    
            default:
                break;
            }
            
        }
    }
    pthread_mutex_unlock(&mutex_printf);
}