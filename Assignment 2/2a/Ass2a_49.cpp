#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h>
#include<time.h>
using namespace std;
using namespace std;

#define FILLED -1
#define EMPTY 1
#define BUFFSIZE 5

typedef struct memory{
	int front;
	int rear; 
	int data[BUFFSIZE];

}memo;

bool isfull(memo* ptr)
{
	if((ptr->rear+1)%BUFFSIZE == ptr->front)
	{
		return true;
	}
	else
		return false;
}

bool isempty(memo* ptr)
{
	if(ptr->rear == ptr->front)
		return true;
	else 
		return false;
}

int checkPrime(int prime)
{
	int flag = 0;
	for(int k=2; k<=sqrt(prime); k++)
		{
			if((prime%k) == 0)
			{
				return 0;
			}
		}
	return 1;

}

int main()
{


		// Take as input the no. of producers and consumers 
		int NP,NC;
		cout << "Enter the number of Producers:";
		cin >> NP;
		cout << "Enter the number of Consumers:";
		cin >> NC;

		// Set key for the shared memory
		int key = 697;


		//Create a shared memory segment
		int shm_id = shmget(key, sizeof(memo), IPC_CREAT | 0666); 
		if(shm_id < 0)
		{
			cout << "shmget error" << endl;
			exit(-1);
		}

		// Attach the shared memory segment to the parent process
		memo* mpointer = (memo*)shmat(shm_id,NULL,0);
		if(mpointer == (memo*)-1)
			{
				cout << "attach error" << endl;
				exit(-1);
			}

		//Initialize the shared memory segment
		for(int i = 0; i< BUFFSIZE; i++)
			mpointer->data[i]=0;
		mpointer->front = 0;
		mpointer->rear = 0;


		//Array to store the pids to kill later 
		pid_t killarray[NP+NC];
		int k = 0;

		memo* pointer;
		for(int i = 0; i<NP; i++)
			{
				pid_t childp = fork();
				if(childp == 0)
				{
					srand(k*7+i);

					//Attach the shared memory segment to the producer
					pointer = (memo*)shmat(shm_id,NULL,0);
					if(pointer == (memo*)-1)
					{
						cout << "attach error" << endl;
						exit(-1);
					}
					
					while(1)
					{
					//Generates a random number
					int x;
					while(true)
					{
						x = rand()%(100*i) + 1;
						if(checkPrime(x)==1)
							break;
					}
									
					//Generate a random time between 0 and 5 to sleep for
					double random = (((double)rand())/(RAND_MAX))*5;
					sleep(random);
					
					//Wait if the buffer is full
					while(isfull(pointer));
					
					//Insert data into the buffer
					pointer->data[pointer->rear] = x;
					pointer->rear = (pointer->rear+1)%BUFFSIZE;
					time_t timer;
					time(&timer);
					cout << "Producer " << i << ": " << x << " Timestamp: "<< ctime(&timer);
					}
					shmdt(pointer);
					exit(0);
				}
				else{
					killarray[k]=childp;
					k++;
					continue;
				}
			}
			for(int j = 0; j<NC; j++)
			{
				pid_t childc = fork();
				if(childc == 0)
				{
					srand(k*7+j);
					//Attach the memory segment to the child 
					pointer = (memo*)shmat(shm_id,NULL,0);
					if(pointer == (memo*)-1)
					{
						cout << "attach error" << endl;
						exit(-1);
					}
					while(1)
					{
					double random = (((double)rand())/(RAND_MAX))*5;
					sleep(random);

					//Wait if the buffer is empty
					while(isempty(pointer));
					
					//Consume data at the start of the queue 
					time_t timer;
					time(&timer);
					cout << "Consumer " << j << ": " << pointer->data[pointer->front] << " Timestamp: "<< ctime(&timer);

					pointer->front = (pointer->front+1)%BUFFSIZE;
					}
					shmdt(pointer);
					exit(0);
				}
				else{
					killarray[k]=childc;
					k++;
					continue;
				}
			}


			sleep(20);
			for(int x = 0; x < NP+NC; x++)
			{
				kill(killarray[x],SIGTERM);
			}
			shmctl(shm_id, IPC_RMID, NULL);


}