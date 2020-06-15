#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;


int main()
{
	int pipead[2];
	int pipebd[2];
	int pipecd[2];
	pipe(pipead);
	pipe(pipebd);
	pipe(pipecd);
	//fork for process b
	pid_t b;
	b = fork();

	if(b == 0)
	{
		//fork for process c
		pid_t c;
		c = fork();

		if(c==0)
		{
			//fork for process d
			pid_t d;
			d = fork();

			if(d == 0)
			{
				//process d
				int a,b,c;
				read(pipead[0],&a,sizeof(int));
				read(pipebd[0],&b,sizeof(int));
				read(pipecd[0],&c,sizeof(int));
				int na=1, nb=1, nc = 1;
				int t = 0;
				while(na <= 100 || nb <= 100 || nc <= 100)
				{
					int m = min(min(a,b),c);
					if(m == a)
					{
						cout << a << endl;
						t++;
						na++;
						if(na > 100)
							a = 2000;
						else
							read(pipead[0],&a,sizeof(int));
						
					}
					else if(m == b)
					{
						cout << b << endl;
						t++;
						nb++;
						if(nb > 100)
							b = 2000;
						else
							read(pipebd[0],&b,sizeof(int));
						
					}
					else
					{
						cout << c << endl;
						t++;
						nc++;
						if(nc > 100)
							c = 2000;
						else
							read(pipecd[0],&c,sizeof(int));

					}

				}
				cout << "Count of no.s printed - " << t << endl;
			}
			else 
			{
				//process c
				srand(d);
				int arrC[100];
				for(int i = 0;i < 100;i++)
				{
					arrC[i] = rand()%1000 + 1;
				}	
				sort(arrC, arrC + 100);
				for(int i = 0;i < 100;i++)
				{
					write(pipecd[1], &arrC[i], sizeof(int));
				}
			}
		}
		else
		{
			//process b
			srand(c);
			int arrB[100];
			for(int i = 0;i < 100;i++)
			{
				arrB[i] = rand()%1000 + 1;
			}	
			sort(arrB, arrB + 100);
			for(int i = 0;i < 100;i++)
			{
				write(pipebd[1], &arrB[i], sizeof(int));
			}

		}

		
	}
	else
	{
		//process a
		srand(b);
		int arrA[100];
		for(int i = 0;i < 100;i++)
		{
			arrA[i] = rand()%1000 + 1;
		}	
		sort(arrA, arrA + 100);
		for(int i = 0;i < 100;i++)
		{
			write(pipead[1], &arrA[i], sizeof(int));
		}
	}

}