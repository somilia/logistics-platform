#include <stdio.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/sem.h> 

#define nbr_cargo 10
#define nbr_wagon 10
#define nbr_camion 10


#define IFLAGS (SEMPERM | IPC_CREAT)
#define SKEY   (key_t) IPC_PRIVATE
#define SEMPERM 0600                  /* Permission */
#define MUTEX 0

struct sembuf sem_oper_P;  /* Operation P */
struct sembuf sem_oper_V;  /* Operation V */

typedef enum {NORD, SUD, OUEST, EST} Destination;

typedef struct Marchandise {
    pid_t id;
    Destination destination;
} Marchandise;

int nombre_aleatoire(int min, int max) {
  return(min + (rand() % (max - min)));
}

int initsem(key_t semkey) 
{
    
	int status = 0;		
	int semid_init;
   	union semun {
		int val; 		//valeur d'initialisation SETVAL
		struct semid_ds *buffer;
		ushort * array;	//tableau pour GETALL, SETALL
	} ctl_arg;
    if ((semid_init = semget(semkey, 2, IFLAGS)) > 0) {
		
	    	ushort array[2] = {, };
	    	ctl_arg.array = array;
	    	status = semctl(semid_init, 0, SETALL, ctl_arg);
    }
   if (semid_init == -1 || status == -1) { 
	perror("Erreur initsem");
	return (-1);
    } else return (semid_init);
}
void P(int semnum)
{
    sem_oper_P.sem_num = semnum;
    sem_oper_P.sem_op  = -1 ;
    sem_oper_P.sem_flg = 0 ;
    semop(semid,&sem_oper_P,1);
}
void V(int semnum)
{
    sem_oper_V.sem_num = semnum;
    sem_oper_V.sem_op  = 1 ;
    sem_oper_V.sem_flg  = 0 ;
    semop(semid,&sem_oper_V,1);
}

int creer_portique(Marchandise marchandise) 
{
    int pid = fork();
    if(pid != 0)
        return pid;

    printf("Portique créé \n");

    while(1) {
        
        switch (marchandise.destination)
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
            print("ca cherche les problemes");
            break;

        }

    }
}

int creer_peniche(int portique)
{
    int pid = fork();
    if(pid != 0)
        return pid;


    exit(0);
}

int creer_camion(int portique)
{
    int pid = fork();
    if(pid != 0)
        return pid;


    exit(0);
}



int creer_train(Destination destination) //2 trains unidirectionnels
{
    int pid = fork();
    if(pid != 0)
        return pid;
 

    exit(0);
}

Marchandise creer_marchandise()
{
    pid_t pid = fork();
    if(pid != 0)
        Marchandise march;

        march.destination = nombre_aleatoire(0, 3);
        march.id = pid;
        return march;

}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <nb_ship>\n", argv[0]);
        return 1;
    }

    int n = nb_marchandise = atoi(argv[1]);

    srand(time(NULL));
    for(int i = 0; i < nbr_camion; i++) {
        sleep(nombre_aleatoire(0, 5));
        int camion = creer_camion(portique);
    }
    for(int i = 0; i < 2; i++) {
        sleep(nombre_aleatoire(0, 5));
        int peniche = creer_peniche(portique);
        int portique = creer_portique();
    }


    for(int i = 0; i < n; i++) {
        sleep(nombre_aleatoire(0, 5));
        creer_marchandise();
    }
  

}