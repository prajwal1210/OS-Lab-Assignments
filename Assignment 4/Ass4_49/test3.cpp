#include "myfs.h"
#include <bits/stdc++.h>
using namespace std;

int main()
{
	create_myfs(2);
	restore_myfs("mydump_49.backup");
	ls_myfs();
	int fd = open_myfs("mytest.txt",'r');
	int *n = new int[100];
	int i = 0;
	while(!eof_myfs(fd))
	{
		int l = read_myfs(fd,sizeof(int),(char*)(n+i));
		i++;
		
	}
	cout << "Numbers read :" << endl;
	for(int i = 0; i < 100; i++)
	{
		cout << n[i] << endl; 
	}
	close_myfs(fd);
	sort(n, n + 100);
	fd = open_myfs("sorted.txt",'w');
	int k = write_myfs(fd,100*sizeof(int),(char*)n);
	cout << "Bytes Written to sorted.txt - " << k << endl;
	close_myfs(fd);

	fd = open_myfs("sorted.txt",'r');
	int read;
	cout << "Numbers read from sorted.txt:" << endl;
	while(!eof_myfs(fd))
	{	
		int l = read_myfs(fd,sizeof(int),(char*)(&read));
		cout << read << endl;
		
	}
	close_myfs(fd);

}