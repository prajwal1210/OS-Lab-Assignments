#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <fcntl.h>
using namespace std;

int main()
{
	cout << "Choose  appropriate options:\n";
	cout << "A. Run an internal command\n";
	cout << "B. Run an external command\n";
	cout << "C. Run an external command by redirecting standard input from a file\n";
	cout << "D. Run an external command by redirecting standard output to a file\n"; 
	cout << "E. Run an external command in the background\n";
	cout << "F. Run several external commands in the pipe mode\n";
	cout << "G. Quit the shell\n";
	while(1)
	{
		//Take input of option and Command
		cout << "Enter Option: ";
		string opt;
		getline(cin,opt);
		// If option is G then terminate
		if(opt.compare("G")== 0)
			break;
		cout << "Enter Command: ";
		string s;
		getline(cin,s);
		int status = 0;
		stringstream str(s);
		string token;
		int n=0;
		
		//Count the no. of tokens in the command
		while(str >> token)
		{
			n++;
		}
		char** arglist = new char*[n+1];
		stringstream str2(s);
		int i=0;
		
		// tokenize the command 
		while(str2 >> token)
		{
			char *arg = new char[token.size()+1];
			copy(token.begin(),token.end(),arg);
			arg[token.size()]='\0';
			arglist[i] = arg;
			i++;
		}
		arglist[i] = NULL;
		int length = i;

		//Option A
		if(opt.compare("A")==0)
		{
			if(strcmp(arglist[0],"mkdir")==0)
				mkdir(arglist[1], S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
			else if(strcmp(arglist[0],"chdir")==0)
				chdir(arglist[1]);
			else if(strcmp(arglist[0],"rmdir")==0)
				rmdir(arglist[1]);
		}

		//Option B
		else if (opt.compare("B") == 0 ){
 			pid_t newp = fork();
			if(newp == 0)
			{
				execvp(arglist[0],arglist);
				break;
			}
			else
			{
				waitpid(newp, NULL, 0);
				continue;
			}
		}
		
		//Option C
		else if(opt.compare("C") == 0)
		{
			int mark = -1;
			for(int i=0; i < length; i++ )
			{
				if(strcmp(arglist[i],"<")==0)
					mark = i;
			} 
			if(mark == -1 || mark == length -1)
				continue;
			char** newarglist = new char*[mark+1];
			for(int i=0; i < mark; i++ )
			{
				newarglist[i] = arglist[i];
			}
			int filedesc = open(arglist[mark+1], O_RDONLY);
			pid_t newp = fork();
			if(newp == 0)
			{
				close(0);
				dup(filedesc);
				execvp(newarglist[0],newarglist);
				cout << "error" << endl;
				break;
			}
			else
			{
				waitpid(newp, NULL, 0);
				close(filedesc);
				continue;
			}
		}

		//Option D
		else if(opt.compare("D") == 0)
		{
			int mark = -1;
			for(int i=0; i < length; i++ )
			{
				if(strcmp(arglist[i],">")==0)
					mark = i;
			} 
			if(mark == -1 || mark == length -1)
				continue;
			char** newarglist = new char*[mark+1];
			for(int i=0; i < mark; i++ )
			{
				newarglist[i] = arglist[i];
			}
			FILE *w = fopen(arglist[mark+1],"w");
			int filedesc = fileno(w);
			pid_t newp = fork();
			if(newp == 0)
			{
				close(1);
				dup(filedesc);
				execvp(newarglist[0],newarglist);
				cout << "error" << endl;
				break;
			}
			else
			{
				waitpid(newp, NULL, 0);
				close(filedesc);
				continue;
			}
		}

		//Option E
		else if (opt.compare("E") == 0 ){

			pid_t newp = fork();
			if(newp == 0)
			{
				execvp(arglist[0],arglist);
				break;
			}
			else
			{
				continue;
			}
		}

		//Option F
		else if (opt.compare("F") == 0)
		{
			int x = fork();
			if (x == 0) {
				int commandnum = 0;
				//Calculate the number of commands
				for (int i=0;i<length;i++)
				{
					if (strcmp(arglist[i],"|") == 0)
					{
						commandnum++;
					}
				}

				char*** newarglist = (char ***)malloc((commandnum+1) * sizeof(char **));
				for (int i = 0; i < commandnum+1; i++) {
					newarglist[i] = (char **)malloc(length * sizeof(char *));
					for (int j = 0; j < length; j++) {
						newarglist[i][j] = (char *)malloc(100 * sizeof(char));
					}
				}
				int j=0;
				int i=0;
				int k;

				//Put each command into a seperate index of another array
				while(i < length)
				{

					k = i;
					int l = 0;
					while(strcmp(arglist[k],"|")!=0 && k < length)
					{
						newarglist[j][l] = arglist[k];
						k++;
						l++;
						if(k >= length)
							break;
					}
					newarglist[j][l] = NULL;
					j++;
					k++;
					i = k;
				}

				// 1 to n-1 commands
				for(i = 0; i < j-1; i++)
				{
					int pipefd[2];
					pipe(pipefd);
					pid_t newp = fork();
					if(newp == 0)
					{
						dup2(pipefd[1],1);
						execvp(newarglist[i][0],newarglist[i]);
						cout << "error" << endl;
						exit(0);
					}
					else{
					waitpid(newp, NULL, 0);
					close(pipefd[1]);
					dup2(pipefd[0],0);
					}
				}

				//last command
				execvp(newarglist[i][0],newarglist[i]);
				cout << "error" << endl;
				exit(0);
			} 
			else 
			{
				waitpid(x, NULL, 0);
			}
		}
	}

}
