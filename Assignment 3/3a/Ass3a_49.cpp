#include <bits/stdc++.h>

using namespace std;

void sort(int *ar,int *cbt,int n)
{
	int min=11,temp,i=0,index;
	while(i<n)
	{
		min = 11;
		for (int j=i;j<n;j++)
		{
			if(ar[j] < min)
			{
				min = ar[j];
				index = j;
			}
		}
		temp = ar[i];
		ar[i] = ar[index];
		ar[index] = temp;
		temp = cbt[i];
		cbt[i] = cbt[index];
		cbt[index] = temp;
		i++;
	}
}

void FCFS(int *ar, int *cbt,int n)
{
	double avg = 0;
	int total_time = 0;
	int i =0;
	for(i=0;i<n;i++)
	{
		if(total_time < ar[i])
			total_time = ar[i];
		total_time += cbt[i];
		avg += total_time - ar[i];
	}
	avg = avg/n;
	cout << "Average Turnaround time using FCFS scheduling algorithm is: "<< avg <<endl;
}

void PSJF(int *ar,int *cbt,int n)
{
	// for(int i=0;i<n;i++)
	// 	cout << ar[i] << " ";
	// cout << endl;
	int total_time=0,job=0,flag[n],fin[n],i,temp[n],count=0,min=21,temp1;
	double avg;
	for(i=0;i<n;i++)
	{
		flag[i] = 0;
		fin[i] = 0;
		temp[i] = 0;
	}
	while(job < n)
	{

		min=21;
		for(i=0;i<n;i++)
		{
			if(ar[i] <= total_time && flag[i] == 0)
			{
				temp[count] = i;
				count++;
				flag[i] = 1;
			}
		}
		for(i=0;i<count;i++)
		{
			if(cbt[temp[i]] < min && cbt[temp[i]] > 0)
			{
				min = cbt[temp[i]];
				temp1 = temp[i];
			}
		}
		if(min > 20)
			{
				if(count < n)
				{
					total_time = ar[count];
					continue;
				}
				break;
			}
		else
		{
			//cout << temp1 << endl;
			if(count < n){
				if(total_time+cbt[temp1] <= ar[count])
				{
					job++;
					fin[temp1] = total_time+cbt[temp1];
					total_time = total_time+cbt[temp1];
					cbt[temp1] = 0;
				}
				else
				{

					cbt[temp1] -= (ar[count]-total_time);
					total_time = ar[count];
				}
			}
			else
			{
					job++;
					fin[temp1] = total_time+cbt[temp1];
					total_time = total_time+cbt[temp1];
					cbt[temp1] = 0;

			}
		}
		//cout << total_time << endl;
	}
	for(i=0;i<n;i++)
	{
		//cout << fin[i] << endl;
		avg += fin[i] - ar[i];
	}
	avg = avg/n;
	cout << "Average Turnaround time using PSJF (t = 1s) scheduling algorithm is: "<< avg <<endl;
}

void RoundRobin(int *ar,int *cbtor,int n, int delta)
{
	int job = 0,total_time=0,i,temp,flag[n],fin[n],cond,count=0;
	int cbt[n];
	for(int i=0; i< n ;i++)
		cbt[i] = cbtor[i];
	double avg = 0;
	for(i=0;i<n;i++)
	{
		flag[i] = 0;
		fin[i] = 0;
	}
	queue <int> que;
	que.push(0);
	count++;
	while(job<n)
	{
		for(i=count;i<n;i++)
			{
				if(ar[i] <= total_time)
				{
					que.push(i);
					count++;
				}
			}
		if(que.empty())
		{
			total_time += 1;
			continue;
		}
		temp = que.front();
		que.pop();
		if(cbt[temp] > delta)
		{
			cbt[temp] = cbt[temp] - delta;
			total_time += delta;
			cond=1;
			//cout<<"1 "<<total_time<<" "<<temp+1<<endl;
		}
		else if(cbt[temp] == delta)
		{
			job++;
			cbt[temp] = 0;
			fin[temp] = total_time+delta;
			total_time += delta;
			cond=2;
			//cout<<"2 "<<total_time<<" "<<temp+1<<endl;
		}
		else
		{
			job++;
			fin[temp] = total_time+cbt[temp];
			total_time += cbt[temp];
			cbt[temp] = 0;
			cond=3;
			//cout<<"3 "<<total_time<<" "<<temp+1<<endl;
		}
		if(cond == 1)
		{
			for(i=count;i<n;i++)
			{
				if(ar[i] <= total_time)
				{
					que.push(i);
					count++;
				}
			}
			que.push(temp);			
		}
		else if(cond == 2)
		{
			for(i=count;i<n;i++)
			{
				if(ar[i] <= total_time)
				{
					que.push(i);
					count++;
				}
			}
		}
		else if(cond ==3)
		{
			for(i=count;i<n;i++)
			{
				if(ar[i] <= total_time)
				{
					que.push(i);
					count++;
				}
			}
		}
	}
	for(i=0;i<n;i++)
	{
		//cout<<"F"<<fin[i]<<endl;
		avg += fin[i] - ar[i];
	}
	avg = avg/n;
	cout << "Average Turnaround time using RoundRobin (t = "  << delta << "s) scheduling algorithm is: "<< avg <<endl;
}

int main()
{
	ofstream ofs;
	ofs.open("output.txt", std::ofstream::out | std::ofstream::app);
	int n,temp;
	double l;
	double r;
	cout<<"Enter the number of processes"<<endl;
	cin>>n;
	cout<<"Enter a mean"<<endl;
	cin>>l;
	int ar[n],cbt[n];
	ar[0] = 0;
	cout<< "Arrival times: "<< endl << ar[0] << endl;
	for(int i=1; i<n; i++)
	{
		r = ((double) rand() / (RAND_MAX));
		temp = (int)fmod(((-1/l)*log(r)),10.0);
		ar[i] = temp+ar[i-1];
		cout<<ar[i]<<endl;	
	}
	cout<< "Burst times: "<<endl;
	for(int i=0; i<n; i++)
	{
		r = ((double) rand() / (RAND_MAX));
		cbt[i] = (int)(r*19.0) + 1;
		cout<<cbt[i]<<endl;	
	}
	//sort(ar,cbt,n);

	int temp1[n];
	int temp2[n];
	for(int i = 0; i< n ;i++)
	{
		temp1[i] = cbt[i];
		temp2[i] = cbt[i];
	}
	FCFS(ar,cbt,n);
	PSJF(ar,temp1,n);
	RoundRobin(ar,temp2,n,1);
	RoundRobin(ar,temp2,n,2);
	RoundRobin(ar,temp2,n,5);
	ofs << "N- " << n << "  Mean- " << l << endl;
	ofs << left << setw(10) << "Process";
	ofs << left << setw(10) << "Arr Time";
	ofs << left << setw(10) << "Burst Time";
	ofs << endl;
	for(int i = 0; i<n;i++)
	{
		ofs << left << setw(10) << i  << left << setw(10) << ar[i]  << left << setw(10) << cbt[i] << endl;
	}
	ofs << endl;
	ofs.close();
}