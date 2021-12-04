#include "headers/semaphore.h"

#define CAPACITE_PENICHE 10
#define CAPACITE_TRAIN 10
#define NB_CAMION 10


typedef enum {NORD, SUD, OUEST, EST} Destination;
//Definition de la struture Conteneur
typedef struct Container {
    pid_t id;
    Destination destination;
} Container; 

int nombre_aleatoire(int min, int max) {
  return(min + (rand() % (max - min)));
}
/*//ton code Samy
Container * remplir_transport(Container *transport, Destination destination, int nb_container) {

    Container container_peniche[CAPACITE_PENICHE];

    for(int i=0; i < nb_container; i++) {
        Container cont;
        cont = creer_container(destination);
        container_peniche[i].destination = cont.destination;
        container_peniche[i].id = cont.id;
    }

    return container_peniche;
}*/
void remplir_transport(Container transport[], Destination dest, int nb_container) {
    
    for (int i=0; i < nb_container; i++) {
        transport[i] = creer_container(dest);
    }


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

int creer_peniche()
{
    int pid = fork();
    if(pid != 0)
        return pid;

    Destination destination = nombre_aleatoire(0, 3);
    int nb_container = nombre_aleatoire(0, CAPACITE_PENICHE);

    Container container_peniche[CAPACITE_PENICHE];
    remplir_transport(container_peniche, destination, nb_container);

    //ton code Samy
   /* Destination destination = nombre_aleatoire(0, 3);
    int nb_container = nombre_aleatoire(0, CAPACITE_PENICHE);

    Container container_peniche[CAPACITE_PENICHE];
    remplir_transport(container_peniche, destination, nb_container);
    */


    exit(0);
}

int creer_camion(int portique)
{
    int pid = fork();
    if(pid != 0)
        return pid;

    /*Destination destination = nombre_aleatoire(0, 3);
    int nb_container = nombre_aleatoire(0, 1);*/

    exit(0);
}

int creer_train(Destination destination) //2 trains unidirectionnels
{
    int pid = fork();
    if(pid != 0)
        return pid;
 

    exit(0);
}

Container creer_container(Destination destination)
{
    pid_t pid = fork();
    if(pid != 0){
        Container cont;
        do{
            cont.destination = nombre_aleatoire(0, 3);
        } while(cont.destination != destination);        
        cont.id = pid;
        return cont;
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <nb_ship>\n", argv[0]);
        return 1;
    }
    int nb_Container;
    int n = nb_Container = atoi(argv[1]);
    int portique, peniche, camion; //train

    srand(time(NULL));

    creer_peniche();

    for(int i = 0; i < 2; i++) {
        sleep(nombre_aleatoire(0, 5));
        /*Container container = creer_container();
        portique = creer_portique(container);
        peniche = creer_peniche(portique);*/
    }

    for(int i = 0; i < NB_CAMION; i++) {
        sleep(nombre_aleatoire(0, 5));
        //camion = creer_camion(portique);
    }

    for(int i = 0; i < n; i++) {
        sleep(nombre_aleatoire(0, 5));
        //creer_container();
    }
  

}
