#include "myfs.h"
#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

int main()
{
	create_myfs(20);
	mkdir_myfs("myroot");
	chdir_myfs("myroot");
	mkdir_myfs("mydocs");
	mkdir_myfs("mycode");
	chdir_myfs("mydocs");
	mkdir_myfs("mytext");
	mkdir_myfs("mypapers");
	chdir_myfs("..");
	chdir_myfs("..");

	pid_t p;
	p = fork();

	if(p==0)
	{
		//P1
		chdir_myfs("myroot");
		chdir_myfs("mydocs");
		chdir_myfs("mytext");

		int fd = open_myfs("alphabet.txt",'w');
		for(char i = 'A'; i <= 'Z';i++)
		{
			write_myfs(fd,1,&i);
		}
		close_myfs(fd);
		showfile_myfs("alphabet.txt");
		cout << "\n";
		ls_myfs();
	}
	else
	{
		//P2
		chdir_myfs("myroot");
		chdir_myfs("mycode");

		copy_pc2myfs("myfs.cpp","myfs.cpp");
		ls_myfs();

	}
}