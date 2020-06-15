#include <unistd.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
using namespace std;

int main()
{

	while(1)
	{
		cout << ">";
		string s;
		getline(cin,s);
		if(s.compare("quit")==0)
			break;
		else{

			//convert the input string to a array of char* tokens
			stringstream str(s);
			string token;
			int n=0;
			while(str >> token)
			{
					n++;
			}
			char** arglist = new char*[n];
			stringstream str2(s);
			int i=0;
			while(str2 >> token)
			{
				char *arg = new char[token.size()+1];
				copy(token.begin(),token.end(),arg);
				arg[token.size()]='\0';
				arglist[i] = arg;
				i++;
			}

			int p[2];
			pipe(p);

			pid_t newp = fork();
			if(newp == 0)
			{
				execvp(arglist[0],arglist);
			}
			else
			{
				waitpid(newp, NULL, 0);
				continue;
			}
		}
	}

}