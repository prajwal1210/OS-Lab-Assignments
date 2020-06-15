#include "myfs.h"
#include <bits/stdc++.h>
using namespace std;

int main()
{
	create_myfs(2);
	int fd = open_myfs("mytest.txt",'w');
	int *n = new int;
	char* data;
	for(int i = 0; i < 100; i++)
	{
		*n = rand()%1000;
		data = (char*)n;
		int k = write_myfs(fd,sizeof(int),data);
		cout << "Number generated - "<< *n << endl;
	}
	close_myfs(fd);
	fd = open_myfs("mytest.txt",'r');
	int N;
	cout << "Enter N : ";
	cin >> N;
	int size = sizeof(int)*100;
	char copydata[size];
	int l = read_myfs(fd,size,copydata);
	cout << "Bytes Read - " << l << endl;
	for(int i = 0; i < size; i+=4)
	{
		int* t = (int *)(copydata+i);
		cout << "Number read - " << *t << endl; 
	}
	for(int i=1;i<=N;i++)
	{
		stringstream ss;
		ss << "mytest-";
		ss << i;
		ss << ".txt";
		string str = ss.str();
		int fd_temp = open_myfs((char*)str.c_str(),'w');
		int k = write_myfs(fd_temp,size,copydata);
		cout << "Written " << k << " bytes to " << str << endl;
		close_myfs(fd_temp);
	}
	close_myfs(fd);
	//ls_myfs();
	dump_myfs("mydump_49.backup");

}