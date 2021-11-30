#include <stdio.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/sem.h> 

typedef enum {NORD, SUD, OUEST, EST} Destination;

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

int creer_portique() //tube ?
{
    int pid = fork();
    if(pid != 0)
        return pid;

    printf("Portique créé \n");

    while(1) {
        
        int requete[2];
        read(tube_dechargement, &bateau, 3 * sizeof(int));
        printf("Déchargement du bateau %d en cours\n", peniche[0]);
        write(transport, marchandise, sizeof(int));
        sigsuspend(&old_mask);
        printf("Déchargement du bateau %d terminé\n", peniche[0]);
        kill(peniche[0], SIGUSR2);



        switch (destination)
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

        requete[1] = -requete[1];
        printf("Bateau %d s'amarre au quai de chargement\n", getpid());
        write(portique, requete, 3 * sizeof(int));
        sigsuspend(&old_mask);
        printf("Peniche %d s'en va après avoir chargé %d marchandises\n", getpid(), marchandise);
        }

    }
}

void creer_peniche(int portique)
{
    int pid = fork();
    if(pid != 0)
        return;

    int stock = nombre_aleatoire(1, 10);
    printf("Stock de a peniche: %d\n", stock);


    int marchandise = nombre_aleatoire(1, 5);
    Destination destination = nombre_aleatoire(0, 3);
    int requete[3] = { getpid(), marchandise, destination};

    if(destination == NORD) 
    {
        requete[1] = -requete[1];
        printf("Peniche %d s'amarre au quai de chargement\n", getpid());
        write(portique, requete, 3 * sizeof(int));
        sigsuspend(&old_mask);
        printf("Peniche %d s'en va après avoir chargé %d marchandises\n", getpid(), marchandise);
    }
    else 
    {
        printf("Peniche %d s'amarre au quai de déchargement\n", getpid());
        write(portique, requete, 3 * sizeof(int));
        sigsuspend(&old_mask);
        printf("Peniche %d s'en va après avoir déchargé %d caisses\n", getpid(), marchandise);
    }

  exit(0);
}

void creer_train(Destination destination) //2 trains unidirectionnels
{
    int pid = fork();
    if(pid != 0)
        return;

    int wagon = nombre_aleatoire(1, 10);
    printf("Nombre de wagon: %d\n", wagon);  

    if(destination == NORD) 
    {
        requete[1] = -requete[1];
        printf("Bateau %d s'amarre au quai de chargement\n", getpid());
        write(portique, requete, 3 * sizeof(int));
        sigsuspend(&old_mask);
        printf("Peniche %d s'en va après avoir chargé %d marchandises\n", getpid(), marchandise);
    }
    else 
    {
        printf("Bateau %d s'amarre au quai de déchargement\n", getpid());
        write(portique, requete, 3 * sizeof(int));
        sigsuspend(&old_mask);
        printf("Bateau %d s'en va après avoir déchargé %d caisses\n", getpid(), marchandise);
    }

    exit(0);
}