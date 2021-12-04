#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <sys/sem.h> 

#define IFLAGS (SEMPERM | IPC_CREAT)
#define SKEY   (key_t) IPC_PRIVATE
#define SEMPERM 0600                  /* Permission */
#define MUTEX 0

struct sembuf sem_oper_P;  /* Operation P */
struct sembuf sem_oper_V;  /* Operation V */

int semid;

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
		
	    	ushort array[2] = {0,0 };
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


#endif