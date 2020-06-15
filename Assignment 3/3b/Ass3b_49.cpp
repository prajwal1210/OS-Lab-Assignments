#include <iostream>
#include <pthread.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <signal.h>
#include <ctime>

using namespace std;

#define MAX_THREADS 100
#define THREAD_TIME 1 
#define NUM_SIZE 100000
#define READY  0
#define RUNNING 1
#define TERM 2

typedef struct threadstat
{
	pthread_t thread;
	int status;
	int index;
}threads;

threads STATUS[MAX_THREADS];
int c_id;
int n;
//int ind[MAX_THREADS];

void to_run(int id){
	//resume();
	signal(SIGUSR2, to_run);
}

void to_pause(int id)
{
	pause();
	signal(SIGUSR1, to_pause);
}


void* worker(void* param)
{
	int id = *(int *)param;
	srand(id*7+3);
	STATUS[id].status = RUNNING;
	int num[NUM_SIZE];
	for(int i=0; i<NUM_SIZE; i++)
	{
		num[i] = rand()%10000 + 1;
	}
	sort(num, num + 1000);	
	int temp = rand()%9+1;

	while(temp)
	{
			temp = sleep(temp);
			}
	//sleep(time);
	//cout << "Ended " << id << endl;
	STATUS[id].status = TERM;
}

void* scheduler_fn(void* param){
	queue <int> ready;
	for(int i=0;i<n;i++)
	{
		ready.push(i);
	}
	c_id = ready.front();
	int p_id;
	cout << "Scheduler started" << endl;
	pthread_kill(STATUS[c_id].thread,SIGUSR2);
	sleep(THREAD_TIME);
	while(ready.size()!=1 && !ready.empty())
	{

		if(STATUS[c_id].status == TERM)
		{
			//cout << c_id << " Terminated" << endl;
		}
		else
		{	
			pthread_kill(STATUS[c_id].thread,SIGUSR1);
			//cout << "Pushing" << endl;
			ready.push(c_id);
			//c_id = ready.front();
		} 	
		//pthread_kill(STATUS[c_id].thread,SIGUSR1);
		p_id = c_id;
		ready.pop();
		//cout<<"Size of queue is:" << ready.size() <<endl; 
		if(ready.empty())
		{
			pthread_exit(0);
		}
		else
		{
			c_id = ready.front();
			//cout << p_id << "-->" <<c_id << endl;
			pthread_kill(STATUS[c_id].thread,SIGUSR2);
			sleep(THREAD_TIME);

		}
	}
	if(ready.size())
	{
		p_id = ready.front();
		c_id = p_id;
		pthread_kill(STATUS[p_id].thread,SIGUSR2);
		while(STATUS[p_id].status!=TERM);
	}
	else
	{
		pthread_exit(0);
	}
}

void *reporter_fn(void *param)
{
	int p_id = c_id, printed[n];
	for(int i=0; i<n; i++)
		printed[i] = 0;
	int count =0;
	while(1)
	{
		if(p_id != c_id)
		{
			cout<< "Worker " << p_id << " has been suspended.";
			cout<< "Worker " << c_id << " has started." << endl;
			p_id = c_id;
		}
		for(int i = 0; i < n; i++)
		{
			if(STATUS[i].status == TERM && printed[i] == 0)
			{
				cout<< "Worker " << i << " has terminated."<<endl;
				printed[i] = 1;
				count++;
			}
		}
		if (count == n)
		{
			cout << "All workers have ended" << endl;
			pthread_exit(0);
		}
		//sleep(THREAD_TIME);
	}
}


int main()
{
	cout << "Enter the number of processes: " << endl;
	cin >> n;
	
	signal(SIGUSR1, to_pause);
	signal(SIGUSR2, to_run);

	for(int i = 0; i< n;i++)
	{
		STATUS[i].index = i;
		pthread_create(&STATUS[i].thread, NULL, worker, &STATUS[i].index);
		pthread_kill(STATUS[i].thread, SIGUSR1);
	}

	pthread_t scheduler, reporter;
	pthread_create(&scheduler,NULL,scheduler_fn,NULL);
	pthread_create(&reporter,NULL,reporter_fn,NULL);
	for(int i = 0; i<n;i++)
	{
		pthread_join(STATUS[i].thread, NULL);
	}

	pthread_join(scheduler,NULL);
	pthread_join(reporter,NULL);
	return 0;
}