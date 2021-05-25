#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


/*number of zombies in the game: 
you should synchronize threads editing this variable*/
int zombieCounter = 0;
int killedZombies = 0;  /* dead zombie count */

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/* Keeps track of number of zombies entered.*/
void zombieEntered(){
    pthread_mutex_lock(&lock);      /* Critical section */
	zombieCounter++;
    pthread_mutex_unlock(&lock);
}
/* Keeps track of number of zombies killed.*/
void zombieKilled(){
    pthread_mutex_lock(&lock);      /* Critical section */
    zombieCounter--;
	killedZombies++;
    pthread_mutex_unlock(&lock);
}
/* Returns true if number of zombies in the room are 
greater than or equal to 100.*/
int tooManyZombiesInTheRoom(){
	if(zombieCounter >=100){
	return 1;
	}
	else{
		return 0;
	}
}

/*Returns true if more than 100 zombies have been killed.*/
int killed100Zombies(){
	if(killedZombies >= 100){
		return 1;
	}
	else{
		return 0;
	}
}
	
/* Returns true if there is at least one zombies in the room.*/
int zombiesExist(){
	if(zombieCounter > 0){
		return 1;
	}
	else{
		return 0;
	}
}
/*Returns the number of zombies killed.*/
int getKilledCount(){
	return killedZombies;
}

/* Returns the number of zombies in the room.*/
int getInTheRoomCount(){
	return zombieCounter;
}
/*doorman thread*/
void *doorMan(void *p){
    srand(time(NULL));          /* Function from time library for getting %50 possibilty */
    unsigned int usecs = 2000;    
	usleep(usecs);              /* Doorman takes zombie every 2 ms (or not )*/

	if(rand()%2 == 0){      /* %50 possiblity */
        if (tooManyZombiesInTheRoom() == 1)
        {
            printf("You all are dead.");    /* If there are 100 zombies in room then you all die.*/
            pthread_exit(NULL);             /* If you die, exit thread */
        }
         else if (killed100Zombies() == 1) {        /* If you killed 100 zombies then you survive. */
            printf("You are ALIVE !! You made it!");
            pthread_exit(NULL);     /* If game is over then exit */
        }
        else
        {
            pthread_mutex_lock(&lock);
            zombieEntered();                /* Critical section. If game is not over, then take more zombies.*/
            pthread_mutex_unlock(&lock);
        }
    }
    pthread_exit(NULL);
    
}

/*slayer thread*/
void *slayer(void *p){
    int local_kill = 0;        /* Local kills for every single one of slayers*/
    unsigned int usecs = 2000;
	usleep(usecs);

    if (tooManyZombiesInTheRoom() == 1)
    {
        printf("You all are dead.");
        pthread_exit(NULL);
    }
    else if (killed100Zombies() == 1) {
        printf("We are ALIVE !! We made it!");
        printf("I killed %d zombies.", local_kill);
        pthread_exit(NULL);
    }
    else{
        pthread_mutex_lock(&lock);
        zombieKilled();
        local_kill++;
        pthread_mutex_unlock(&lock);
    }

	
	pthread_exit(NULL);         /* Finish slayer threads */
}
/*simulator main thread*/
int main(int argc, char **argv){
	
	int n = 10;     /* Number of doormans for surviving */
	
	pthread_t ptidSlayer[3];        /* Slayer thread id */
	pthread_t ptidDoorMan[n];       /* Doorman thread id */
	
	for(int i = 0 ; i<n ; ++i){         /* Creating Doorman threads */
		pthread_create(&ptidDoorMan[i], NULL, &doorMan, NULL);
	}
	
	for(int i = 0 ; i<3; ++i){      /* Creating Slayer threads */
		pthread_create(&ptidSlayer[i], NULL, &slayer, NULL);
	}
	
	
	
	for(int k = 0; k<10; ++k ){     /* Wait for the Doorman threads before finishing main thread */
		pthread_join(ptidDoorMan[k],NULL);		
	}
	
	for(int m = 0; m<3; ++m ){      /* Wait for the Slayer threads before finishing main thread */
		pthread_join(ptidSlayer[m],NULL);		
	}
	
    if (killed100Zombies() == 1)        /* Checking the result : If you win o not */
    {
        printf("You killed all the zombies. You are safe, FOR NOW !");
    }
    else{
        printf("You and your friends died. May the God mercy on your souls.");
    }
    
	
	return 0;
}
