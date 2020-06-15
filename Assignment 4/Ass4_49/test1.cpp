#include "myfs.h"
#include <bits/stdc++.h>
using namespace std;

int main()
{
	create_myfs(20);
	copy_pc2myfs("./Input/1.png","1.png");
	copy_pc2myfs("./Input/2.png","2.png");
	copy_pc2myfs("./Input/3.pdf","3.pdf");
	copy_pc2myfs("./Input/4.jpg","4.jpg");
	copy_pc2myfs("./Input/5.txt","5.txt");
	copy_pc2myfs("./Input/6.txt","6.txt");
	copy_pc2myfs("./Input/7.txt","7.txt");
	copy_pc2myfs("./Input/8.txt","8.txt");
	copy_pc2myfs("./Input/9.txt","9.txt");
	copy_pc2myfs("./Input/10.txt","10.txt");
	copy_pc2myfs("./Input/11.txt","11.txt");
	copy_pc2myfs("./Input/12.txt","12.txt");
	//copy_myfs2pc("12.txt","12.txt");
	//copy_myfs2pc("2.png","2.png");
	//copy_myfs2pc("3.pdf","3.pdf");
	// mkdir_myfs("new");
	// ls_myfs();
	// chdir_myfs("new");
	// copy_pc2myfs("./Input/10.txt","10.txt");
	// copy_pc2myfs("./Input/11.txt","11.txt");
	// copy_pc2myfs("./Input/12.txt","12.txt");
	// mkdir_myfs("new");
	// chdir_myfs("new");
	// copy_pc2myfs("./Input/10.txt","10.txt");
	// copy_pc2myfs("./Input/11.txt","11.txt");
	// copy_pc2myfs("./Input/12.txt","12.txt");
	// ls_myfs();
	// chdir_myfs("..");
	// chdir_myfs("..");
	// rmdir_myfs("new");
	// ls_myfs();
	cout << "Files in the current directory:\n";
	ls_myfs();
	char in[30];
	while(1)
	{
		cout << "Enter the file name to be deleted: ";
		scanf("%s",in);
		if(strcmp(in,"q") == 0)
			exit(0);
		int x = rm_myfs(in);
		if(x != -1)
			cout << "File deleted Successfuly. After deletion:\n";
		else 
			cout << "File not present. No deletion. Current directory contents:\n";
		ls_myfs();
	}

}
