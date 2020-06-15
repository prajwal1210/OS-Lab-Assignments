#include "myfs.h"
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>


using namespace std; 

mrfs myfs; 
int cwd = 0;
entry_t* next;
sem_t sem;

/*
 *This function returns the first free inode
 *If no free inode is available it returns -1
 */
int next_inode()
{
	int next = 0;
	for(int i = 0; i< myfs.super->max_inodes;i++)
	{
		if(myfs.super->inode_map[i] == 0)
			break;
		next++;
	}
	if(next == myfs.super->max_inodes)
		return -1;
	else
		{

			myfs.super->inode_map.set(next);
			myfs.super->act_inodes++;
			return next;
		}
}

/*
 *This function returns the first data block
 *If no free data block is available it returns -1
 */
int next_free_block()
{
	int next = 0;
	for(int i = 0; i< myfs.super->max_blocks;i++)
	{
		if(myfs.super->block_map[i] == 0)
			break;
		next++;
	}
	if(next == myfs.super->max_blocks)
		return -1;
	else
		{
			myfs.super->block_map.set(next);
			myfs.super->act_blocks++;
			//cout << next << endl;
			return next;
		}
}

/*
 *Allocates memory for the filesystem
 *Creates and initialize the super block
 *Creates and initalize the inode_list block
 *Creates the root directory
 */
int create_myfs(int size){

	sem_init(&sem,0,1);
	sem_wait(&sem);
	int size_bytes= size*(1<<20);
	int size_block = size_bytes/BLOCK_SIZE + 1;
	if( SUP_BLOCK_SIZE + INODE_BLOCK_SIZE + BLOCK_SIZE > size_block)
	{
		sem_post(&sem);
		return -1;
	}
	int size_super = SUP_BLOCK_SIZE * BLOCK_SIZE; 
	int size_inode_b = INODE_BLOCK_SIZE * BLOCK_SIZE;
	
	myfs.mem = (char*)(malloc(BLOCK_SIZE*size_block));
	if(!myfs.mem)
	{
		sem_post(&sem);
		return -1;
	}
	for(int i = 0; i < BLOCK_SIZE*size_block; i+=4)
	{
		int* t = (int*)(myfs.mem + i);
		*t = -1;
	}

	//Initialize the super block
	myfs.super = (super_t*)myfs.mem;
	myfs.super->totalsize = size_block*BLOCK_SIZE;
	myfs.super->max_inodes = MAX_INS;
	myfs.super->act_inodes = 0;
	myfs.super->max_blocks = size_block - SUP_BLOCK_SIZE - INODE_BLOCK_SIZE;
	myfs.super->act_blocks = 0;
	for(int i = 0; i < MAX_INS; i++)
		myfs.super->inode_map[i] = 0;
	for(int i = 0; i < MAX_MEM; i++)
		myfs.super->block_map[i] = 0;

	//Initialize the inode list block 
	myfs.inode = (inode_list_t*)(myfs.mem + size_super);

	//Initialize the data block segment
	myfs.db_blocks = (data_block_t*)(myfs.mem + size_super + size_inode_b);

	//Create the root directory
	cwd = 0;

	myfs.super->act_inodes = 1;
	myfs.super->inode_map[0] = 1;
	strcpy(myfs.inode->list[0].filename,"root");
	myfs.inode->list[0].file_t = directory;
	myfs.inode->list[0].filesize = 0;
	time(&(myfs.inode->list[0].last_modified));
	time(&(myfs.inode->list[0].last_read));
	myfs.inode->list[0].mode = 0777;
	// myfs.inode->list[0].permit.u = {true, true, true};
	// myfs.inode->list[0].permit.g = {true, true, true};
	// myfs.inode->list[0].permit.o = {true, true, true};
	for(int i = 0; i < MAX_PTR ; i++)
		myfs.inode->list[0].db_pointer[i] = -1;
	
	sem_post(&sem);
	return 0; 

}

/*
 *Returns a list of the files in a directory along with the corresponing pointer to the particular file entry in myfs
 */
void ls(vector<ls_list_t>* list)
{
	inode_t curr = myfs.inode->list[cwd];
	int* ptr;
	int* ptr_ind;
	int* ptr_dind;
	dir_entry_t* d; 
	ls_list_t elem;
	for(int i = 0; i<10; i++)
	{
		if(curr.db_pointer[i] == -1)
			continue;
		if(i<8)
		{
			for(int x = 0; x < BLOCK_SIZE; x+=32)
			{
				ptr = (int*)(myfs.db_blocks[curr.db_pointer[i]].data+x);
				if(*ptr!=-1)
				{
					d = (dir_entry_t*)ptr;
					elem.di = *d;
					elem.ptr = ptr;
					(*list).push_back(elem);	
				}
			}
			
		}
		else if( i == 8 )
		{
			for(int x = 0; x < BLOCK_SIZE; x+=32)
			{
				ptr = (int*)(myfs.db_blocks[curr.db_pointer[i]].data + x);
				if(*ptr!=-1)
				{	
					for(int j = 0; j < BLOCK_SIZE; j+=32)
					{

						ptr_ind = (int*)(myfs.db_blocks[*ptr].data + j);
						if(*ptr_ind!=-1)
						{
							d = (dir_entry_t*)ptr_ind;
							elem.di = *d;
							elem.ptr = ptr_ind;
							(*list).push_back(elem);	
						}
					}
				}
			}
		}
		else if(i == 9)
		{
			for(int k = 0; k< BLOCK_SIZE; k++)
			{
				ptr_dind = (int*)(myfs.db_blocks[curr.db_pointer[i]].data + k);
				if(*ptr_dind!=-1)
				{
					for(int x = 0; x < BLOCK_SIZE; x+=32)
					{
						ptr = (int*)(myfs.db_blocks[*ptr_dind].data + x);
						if(*ptr!=-1)
						{	
							for(int j = 0; j < BLOCK_SIZE; j+=32)
							{

								ptr_ind = (int*)(myfs.db_blocks[*ptr].data + j);
								if(*ptr_ind!=-1)
								{
									d = (dir_entry_t*)ptr_ind;
									elem.di = *d;
									elem.ptr = ptr_ind;
									list->push_back(elem);	
								}
							}
						}
					
					}
				}
			}
		}
	}
}

/*
 *This function searches for a free space to the add the file information in the given directory - dir
 *It returns a pointer to the start if that free location
 */
int* search_directory_space(int dir)
{
	int* ptr = NULL;
	int case_t;
	for(int i = 0; i< 10; i++)
	{
		if(myfs.inode->list[dir].db_pointer[i] == -1)
		{
			case_t = 0;
			if(i < 8)
			{
				int data_b = next_free_block();
				if(data_b == -1)
				{
					return NULL;
				}
				myfs.inode->list[dir].db_pointer[i] = data_b;
				ptr = (int*)(myfs.db_blocks[data_b].data);

			}
			else if(i == 8)
			{
				int data_b = next_free_block();
				if(data_b == -1)
				{
					return NULL;
				}
				myfs.inode->list[dir].db_pointer[i] = data_b;
				ind_t* indir = (ind_t*)myfs.db_blocks[data_b].data;

				data_b = next_free_block();
				if(data_b == -1)
				{
					return NULL;
				}
				indir->indirect[0] = data_b;
				ptr = (int*)(myfs.db_blocks[data_b].data);

			}
			else if(i == 9)
			{
				int data_b = next_free_block();
				if(data_b == -1)
				{
					return NULL;
				}
				myfs.inode->list[dir].db_pointer[i] = data_b;
				ind_t* dindir = (ind_t*)myfs.db_blocks[data_b].data;

				data_b = next_free_block();
				if(data_b == -1)
				{
					return NULL;
				}
				dindir->indirect[0] = data_b;
				ind_t* indir = (ind_t*)myfs.db_blocks[data_b].data;

				data_b = next_free_block();
				if(data_b == -1)
				{
					return NULL;
				}
				indir->indirect[0] = data_b;
				ptr = (int*)(myfs.db_blocks[data_b].data);
			}
			break;
		}
		if(i < 8)
		{
			/*First searching the first 8 direct data blocks*/
			int block = myfs.inode->list[dir].db_pointer[i];
			char* db = myfs.db_blocks[block].data;

			/*Search through the data block for a -1*/
			int flag = 0;
			for(int i = 0; i < BLOCK_SIZE; i+=32)
			{
				int* t = (int*)(db+i);
				if(*t == -1)
					{
						/*If found, then we have found and empty space to add the file to the direcrory*/
						ptr = t;
						case_t = 1;
						flag = 1;
						break;
					}
			}
			if(flag == 1)
				break;

		}
		else if(i == 8)
		{
			/*If the data blocks get over we search the indirect block*/
			int ind_ptrs = 0;
			ind_t* indi = (ind_t*)(myfs.db_blocks[myfs.inode->list[dir].db_pointer[i]].data);
			int flag = 0;
			/*Search in the indirect block for a space*/
			while(ind_ptrs < BLOCK_SIZE/4 && indi->indirect[ind_ptrs] != -1)
			{
				int block = indi->indirect[ind_ptrs];
				char* db = myfs.db_blocks[block].data;
				for(int i = 0; i < BLOCK_SIZE; i+=32)
				{	
					int* t = (int*)(db+i);
					if(*t == -1)
						{
							/*If found, then we have found an empty space*/
							ptr = t;
							case_t = 2;
							flag = 1;
							break;
						}
				}
				if(flag == 1)
					break;
				ind_ptrs++;
			}
			if(flag == 1)
				break;

			/*If a new pointer to a data block needs to be added to the indirect block then do it*/
			if(indi->indirect[ind_ptrs] == -1)
			{
				case_t = 3;
				int data_b = next_free_block();
				if(data_b == -1)
				{
					return NULL;
				}
				indi->indirect[ind_ptrs]=data_b;
				ptr = (int*)(myfs.db_blocks[data_b].data);
				break;
			}

		}
		else if(i == 9)
		{
			int dind_ptrs = 0;
			ind_t* dindirect = (ind_t*)(myfs.db_blocks[myfs.inode->list[dir].db_pointer[i]].data);
			int flag1 = 0;
			while(dind_ptrs < BLOCK_SIZE/4 && dindirect->indirect[dind_ptrs] !=-1)
			{
				/*Same as the indirect pointer find*/
				int ind_ptrs = 0;
				ind_t* indi = (ind_t*)(myfs.db_blocks[dindirect->indirect[dind_ptrs]].data);
				int flag = 0;
				/*Search in the indirect block for a space*/
				while(ind_ptrs < BLOCK_SIZE/4 && indi->indirect[ind_ptrs] != -1)
				{
					int block = indi->indirect[ind_ptrs];
					char* db = myfs.db_blocks[block].data;
					for(int i = 0; i < BLOCK_SIZE; i+=32)
					{	
						int* t = (int*)(db+i);
						if(*t == -1)
							{
								/*If found, then we have found an empty space*/
								ptr = t;
								case_t = 4;
								flag = 1;
								flag1 = 1;
								break;
							}
					}
					if(flag == 1)
							break;
					ind_ptrs++;
				}
				if(flag == 1)
				{
					flag1 = 1;
					break;
				}

				/*If a new pointer to a data block needs to be added to the indirect block then do it*/
				if(indi->indirect[ind_ptrs] == -1)
				{
					case_t = 5;
					int data_b = next_free_block();
					if(data_b == -1)
					{
						return NULL;
					}
					indi->indirect[ind_ptrs]=data_b;
					ptr = (int*)(myfs.db_blocks[data_b].data);
					flag1 =1;
					break;
				}

				dind_ptrs++;

			}
			if(flag1 == 1)
				break;
			else
			{
				if(dind_ptrs < BLOCK_SIZE/4 &&  dindirect->indirect[dind_ptrs] ==-1)
				{
					case_t = 6;
					int data_b = next_free_block();
					if(data_b == -1)
					{
						return NULL;
					}
					dindirect->indirect[dind_ptrs] = data_b;

					ind_t* indir = (ind_t*)(myfs.db_blocks[data_b].data);
					data_b = next_free_block();
					if(data_b == -1)
					{
						return NULL;
					}
					indir->indirect[0]  = data_b;
					ptr = (int*)(myfs.db_blocks[data_b].data);
					break;
				}

			}

		}
	}
	return ptr;

}

/*
 *Helper function to clean the data in case copy fails
 */
void clear_data(int inode_no, vector<int> allocated_b)
{
	for(int i = 0; i < allocated_b.size();i++)
	{
		char* temp = myfs.db_blocks[allocated_b[i]].data;
		for(int j = 0; j < BLOCK_SIZE; j+=4)
		{
			int * t = (int*)(temp+j);
			*t = -1;
		}
		myfs.super->block_map[allocated_b[i]] = 0;
		myfs.super->act_blocks--;
	}
	myfs.super->inode_map[inode_no] = 0; 
	myfs.super->act_inodes--;
}


/*
 *Create the inode for the file
 *read data from the source file block wise and store in myfs following the structure of pointers - direct, indirect and double indirect
 *Add the file details to the current working directory
 */
int copy_pc2myfs(char* source, char* destination)
{
	//
	sem_wait(&sem);
	vector<ls_list_t> v;
	ls(&v);
	int flag = 0;
	for(int i = 0; i < v.size(); i++)
	{
		if(strcmp(v[i].di.filename, destination) == 0)
		{
			cout << "Error in copy to myfs: The name already exists in the directory" << endl;
			flag = 1;
		}
	}
	if(flag == 1)
	{
		sem_post(&sem);
		return -1;
	}

	//Open the source file
	char data[BLOCK_SIZE];
	ifstream infile;
	infile.open(source, std::ifstream::binary);
	if(!infile)
	{
		sem_post(&sem);
		cout << "Unable to open" << endl;
		return -1;
	}

	infile.seekg (0, infile.end);
    int length = infile.tellg();
    infile.seekg (0, infile.beg);

	//Create the destination file
	int inode_no = next_inode();
	if(inode_no == -1)
	{
		sem_post(&sem);
		return -1;
	}
	strcpy(myfs.inode->list[inode_no].filename,destination);
	myfs.inode->list[inode_no].file_t = regular;
	myfs.inode->list[inode_no].filesize = length;
	time(&(myfs.inode->list[inode_no].last_modified));
	time(&(myfs.inode->list[inode_no].last_read));
	struct stat file;
	if((int)stat(source,&file) == -1)
	{
		sem_post(&sem);
		return -1;
	}
	myfs.inode->list[inode_no].mode = file.st_mode;
	for(int i = 0; i < MAX_PTR ; i++)
		myfs.inode->list[inode_no].db_pointer[i] = -1;
	myfs.super->inode_map[inode_no] = 1;

	vector<int> allocated_b;
	int pointer = 0;
	int data_b ;
	ind_t* indir;
	ind_t* dindirect;
	int ind_ptrs;
	int dind_ptrs;
	int cnt = 0;
	while(!infile.eof())
	{
		//infile.read(data,BLOCK_SIZE);
		if(pointer < 8)
		{
			/*For direct blocks directly add the data*/
			data_b = next_free_block();
			if(data_b == -1)
			{
				clear_data(inode_no,allocated_b);
				cout << "Error:Size limit exceeded for file - " << endl;
				sem_post(&sem);
				return -1;
			}
			//bcopy(data, myfs.db_blocks[data_b].data, BLOCK_SIZE);
			infile.read(myfs.db_blocks[data_b].data,BLOCK_SIZE);
			allocated_b.push_back(data_b);
			cnt++;
			myfs.inode->list[inode_no].db_pointer[pointer] = data_b; 
			pointer++;
			continue;
		}
		else if(pointer == 8)
		{
			/*Fpr single indirect block*/
			if(myfs.inode->list[inode_no].db_pointer[pointer] == -1)
			{
				/*Get the indirect block and store its pointer in the indirect pointer in the file inode*/
				data_b = next_free_block();
				if(data_b == -1)
				{
					clear_data(inode_no,allocated_b);
					cout << "Error:Size limit exceeded for file - " << endl;
					sem_post(&sem);
					return -1;
				}
				allocated_b.push_back(data_b);
				myfs.inode->list[inode_no].db_pointer[pointer] = data_b;
				indir = (ind_t*)myfs.db_blocks[myfs.inode->list[inode_no].db_pointer[pointer]].data;
				ind_ptrs = 0;
			}
			/*Fetch a data block*/
			data_b = next_free_block();
			if(data_b == -1)
			{
				clear_data(inode_no,allocated_b);
				cout << "Error:Size limit exceeded for file - " << endl;
				sem_post(&sem);
				return -1;
			}
			allocated_b.push_back(data_b);

			/*Copy the data into the data block*/
			//bcopy(data, myfs.db_blocks[data_b].data, BLOCK_SIZE);
			infile.read(myfs.db_blocks[data_b].data,BLOCK_SIZE);
			cnt++;
			/*Make a pointer in the indirect block*/
			indir->indirect[ind_ptrs] = data_b;
			ind_ptrs++;

			if(ind_ptrs == (BLOCK_SIZE/4))
				{
					pointer++;
					continue;
				}
		}
		else if(pointer == 9)
		{
			if(myfs.inode->list[inode_no].db_pointer[pointer] == -1)
			{
				/*Create the double indirect block*/
				data_b = next_free_block();
				if(data_b == -1)
				{
					clear_data(inode_no,allocated_b);
					cout << "Error:Size limit exceeded for file - " << endl;
					sem_post(&sem);
					return -1;
				}
				allocated_b.push_back(data_b);
				myfs.inode->list[inode_no].db_pointer[pointer] = data_b;
				dindirect = (ind_t*)(myfs.db_blocks[myfs.inode->list[inode_no].db_pointer[pointer]].data);
				dind_ptrs = 0;

				/*Create the first indirect block and store it in the double indirect block*/
				data_b = next_free_block();
				if(data_b == -1)
				{
					clear_data(inode_no,allocated_b);
					cout << "Error:Size limit exceeded for file - " << endl;
					sem_post(&sem);
					return -1;
				}
				allocated_b.push_back(data_b);
				dindirect->indirect[dind_ptrs] = data_b;
				indir = (ind_t*)(myfs.db_blocks[data_b].data);
				ind_ptrs = 0;
			}

			data_b = next_free_block();
			if(data_b == -1)
			{
				clear_data(inode_no,allocated_b);
				cout << "Error:Size limit exceeded for file - " << endl;
				sem_post(&sem);
				return -1;
			}
			allocated_b.push_back(data_b);
			/*Copy the data into the data block*/
			//bcopy(data, myfs.db_blocks[data_b].data, BLOCK_SIZE);
			infile.read(myfs.db_blocks[data_b].data,BLOCK_SIZE);
			cnt++;
			
			/*Make a pointer in the indirect block*/
			indir->indirect[ind_ptrs] = data_b;
			ind_ptrs++;

			if(ind_ptrs >= BLOCK_SIZE/4)
			{
				data_b = next_free_block();
				if(data_b == -1)
				{
					clear_data(inode_no,allocated_b);
					cout << "Error:Size limit exceeded for file - " << endl;
					sem_post(&sem);
					return -1;
				}
				allocated_b.push_back(data_b);
				dind_ptrs++;
				if(dind_ptrs == BLOCK_SIZE/4)
				{
					pointer++;
					continue;
				}
				dindirect->indirect[dind_ptrs] = data_b;
				indir = (ind_t*)(myfs.db_blocks[dindirect->indirect[dind_ptrs]].data);
				ind_ptrs = 0;
			}

		}
		else
		{
			infile.close();
			
			/*Delete data blocks that were allocated*/
			clear_data(inode_no,allocated_b);

			cout << "Error:Size limit exceeded for file - " << cnt << endl;
			sem_post(&sem);
			return -1;
		}
	}

	infile.close();
	//cout << di->filename << " - " << di->inode << endl;
	
	/*Adding the file to the current working directory*/
	int* ptr = search_directory_space(cwd);
	if(ptr == NULL)
	{
		cout << "Could not add file to directory";
		sem_post(&sem);
		return -1;	
	}
	dir_entry_t* di = (dir_entry_t*)ptr;
	strcpy(di->filename,destination);
	di->inode = (short)inode_no;
	sem_post(&sem);
	return 0;

}

/*
 *Helper function to print the permissions in ls -l format 
 */
void print_permissions(mode_t mode)
{
    printf( (S_ISDIR(mode)) ? "d" : "-");
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
}

/*
 *Prints the files in the current working directory 
 */
int ls_myfs()
{
	sem_wait(&sem);
	vector<ls_list_t> v;
	ls(&v);
	int size = v.size();
	for(int i = 0; i < size; i++)
	{
		print_permissions(myfs.inode->list[v[i].di.inode].mode);
		char* time1 = strtok(ctime(&myfs.inode->list[v[i].di.inode].last_read),(const char*)"\n");
		char* time2 = strtok(ctime(&myfs.inode->list[v[i].di.inode].last_modified),(const char*)"\n");
		cout << " " << time1 << " " << time2;
		cout << " "<< myfs.inode->list[v[i].di.inode].filesize <<" " << v[i].di.filename << " - " << v[i].di.inode << endl;
	}
	sem_post(&sem);
	return 0;
}


/*
*Helper function to locate data blocks for a file
*/
int get_pointer_by_index(inode_t node, int index)
{
	if(index < 8)
	{
		return node.db_pointer[index];
	}
	else if(8<= index && index < (8+(BLOCK_SIZE/4)))
	{
		int* ptr = (int *)(myfs.db_blocks[node.db_pointer[8]].data);
		ptr += (index - 8);
		return *ptr;
	}
	else
	{
		int* ptr = (int *)(myfs.db_blocks[node.db_pointer[9]].data);
		//cout << *ptr << " - ";
		index = index - 8 - (BLOCK_SIZE/4);
		int i1 = index/(BLOCK_SIZE/4);
		ptr+= i1;
		ptr = (int*)(myfs.db_blocks[*ptr].data);
		int i2 = index%(BLOCK_SIZE/4);
		ptr+=i2;
		//cout << *ptr << " - " << endl;
		return *ptr;
	}
}

/*
 *Removes the specified file from the current working directory if present
 */
int rm_myfs(char* filename)
{
	sem_wait(&sem);
	vector<ls_list_t> v;
	ls(&v);
	inode_t temp;
	int flag = 0;
	for(int i = 0; i < v.size(); i++)
	{
		if(strcmp(v[i].di.filename, filename) == 0)
		{
			cout << "Found " << filename <<"\nNow deleting..\n";
			temp = myfs.inode->list[v[i].di.inode];
			if(temp.file_t == regular)
			{
				int ptr_req = ceil(temp.filesize/BLOCK_SIZE);
				for(int j = 0; j<ptr_req;j++)
				{
					int db = get_pointer_by_index(temp,j);
					//cout << db << endl;
					int* t;
					for(int k =0; k < BLOCK_SIZE;k+=4)
					{
						t = (int*)(myfs.db_blocks[db].data+k);
						*t = -1;
					}
					myfs.super->block_map[db] = 0;
					myfs.super->act_blocks--;
				}
				inode_t* tmp = &myfs.inode->list[v[i].di.inode];

				/*Free all the direct, indirect and double indirect blocks*/
				for(int j = 0; j < MAX_PTR;j++)
					{
						if(j<8)
						{
							tmp->db_pointer[j] = -1;
						}
						else if(j==8)
						{
							if(tmp->db_pointer[j] != -1)
								{
									for(int l=0; l<BLOCK_SIZE;l+=4)
									{
										int* t= (int*)(myfs.db_blocks[tmp->db_pointer[j]].data+l);
										*t = -1;
									}
									myfs.super->block_map[tmp->db_pointer[j]] = 0;
									myfs.super->act_blocks--;
								}
								tmp->db_pointer[j] = -1;
						}
						else
						{
							if(tmp->db_pointer[j] != -1)
							{
								ind_t* dindirect = (ind_t*)(myfs.db_blocks[tmp->db_pointer[j]].data);
								int dind_pts = 0;
								for(int m = 0; m < BLOCK_SIZE/4; m++)
								{
									if(dindirect->indirect[m]!=-1)
									{
										for(int l=0; l<BLOCK_SIZE;l+=4)
										{
											int* t= (int*)(myfs.db_blocks[dindirect->indirect[m]].data+l);
											*t = -1;
										}
										myfs.super->block_map[dindirect->indirect[m]] = 0;
										myfs.super->act_blocks--;
									}
									dindirect->indirect[m] = -1;
								}
								myfs.super->block_map[tmp->db_pointer[j]] = 0;
								myfs.super->act_blocks--;
							}
							tmp->db_pointer[j] = -1;	
						}

					}
				strcpy(tmp->filename, "");
				tmp->filesize = 0;
				tmp->last_modified = 0;
				tmp->last_read = 0;
				for(int f = 0; f < 8; f+=1)
				{
					int* t = (v[i].ptr + f);
					*t = -1;
				}
				myfs.super->inode_map[v[i].di.inode] = 0;
				myfs.super->act_inodes--;
				flag = 1;
				break;
			}
		}
	}
	sem_post(&sem);
	if(flag == 0)
		return -1;
	else
		return 0;

}




/*
 *Finds the file in the current working directory
 *Displays the file character by character 
 *This function is only meant for text files
 */
int showfile_myfs(char* filename)
{
	sem_wait(&sem);
	vector<ls_list_t> v;
	ls(&v);
	inode_t temp;
	int flag = 0;
	char data[BLOCK_SIZE];
	for(int i = 0; i < v.size(); i++)
	{
		if(strcmp(v[i].di.filename, filename) == 0)
		{
			cout << "Found " << filename <<"\nNow showing..\n";
			//cout << MAX_MEM << endl;
			temp = myfs.inode->list[v[i].di.inode];
			if(temp.file_t == regular)
			{
				int ptr_req = ceil((float)temp.filesize/BLOCK_SIZE);
				int rem_size = temp.filesize;
				for(int j = 0; j<ptr_req;j++)
				{
					int db = get_pointer_by_index(temp,j);

					bzero(data, BLOCK_SIZE);
					bcopy(myfs.db_blocks[db].data,data,BLOCK_SIZE);

					for(int j = 0; j<min(BLOCK_SIZE,rem_size);j++)
						cout << data[j];
					rem_size -= BLOCK_SIZE;
					//cout << db << endl;
					
				}
				inode_t* tmp = &myfs.inode->list[v[i].di.inode];
				time(&(tmp->last_read));
				flag = 1;
				break;
			}
		}
	}
	sem_post(&sem);
	if(flag == 0)
		return -1;
	else
		return 0;
}


/*
 *This fucntion copies the source file in current working directory of myfs the dest in pc
 *It first list outs the file in the current working directory and finds the matching file
 *If the file is found, it is copy to the destination otherwise -1 is returned
 */
int copy_myfs2pc(char* source, char* dest)
{
	sem_wait(&sem);
	vector<ls_list_t> v;
	ls(&v);
	inode_t temp;
	int flag = 0;
	char data[BLOCK_SIZE];
	for(int i = 0; i < v.size(); i++)
	{
		if(strcmp(v[i].di.filename, source) == 0)
		{
			//cout << MAX_MEM << endl;
			temp = myfs.inode->list[v[i].di.inode];
			if(temp.file_t == regular)
			{
				cout << "Found " << source <<"\nNow Copying..\n";
				ofstream out;
				out.open(dest,std::ofstream::out | std::ofstream::binary);
				if(out.fail())
				{
					sem_post(&sem);
					return -1;
				}
				int ptr_req = ceil((float)temp.filesize/BLOCK_SIZE);
				int rem_size = temp.filesize;
				for(int j = 0; j<ptr_req;j++)
				{
					int db = get_pointer_by_index(temp,j);

					bzero(data, BLOCK_SIZE);
					bcopy(myfs.db_blocks[db].data,data,BLOCK_SIZE);

					for(int j = 0; j<min(BLOCK_SIZE,rem_size);j++)
						out << data[j];
					rem_size -= BLOCK_SIZE;
					//cout << db << endl;
					
				}
				inode_t* tmp = &myfs.inode->list[v[i].di.inode];
				time(&(tmp->last_read));
				out.close();
				flag = 1;
				break;
			}
		}
	}
	sem_post(&sem);
	if(flag == 0)
		return -1;
	else
		return 0;
}

/*
 *Creates a new directory in the current working directory
 *It first checks if the dirname already exists in the cwd and returns -1 if so
 *It then creates the new directory, adds the . and .. entries to it and add it to the current working directory
 */
int mkdir_myfs(char* dirname)
{
	sem_wait(&sem);
	vector<ls_list_t> v;
	ls(&v);
	inode_t temp;
	int flag = 0;
	for(int i = 0; i < v.size(); i++)
	{
		if(strcmp(v[i].di.filename, dirname) == 0)
		{
			cout << "Error in mkdir: The name already exists in the directory" << endl;
			flag = 1;
		}
	}
	if(flag == 1)
	{
		sem_post(&sem);
		return -1;
	}

	/*Create the new directory*/
	int inode_no = next_inode();
	if(inode_no == -1)
	{
		sem_post(&sem);
		return -1;
	}
	strcpy(myfs.inode->list[inode_no].filename,dirname);
	myfs.inode->list[inode_no].file_t = directory;
	myfs.inode->list[inode_no].filesize = 0;
	time(&(myfs.inode->list[inode_no].last_modified));
	time(&(myfs.inode->list[inode_no].last_read));
	myfs.inode->list[inode_no].mode = 0777;
	for(int i = 0; i < MAX_PTR ; i++)
		myfs.inode->list[inode_no].db_pointer[i] = -1;
	myfs.super->inode_map[inode_no] = 1;

	//Add . and .. entries
	int* ptr = search_directory_space(inode_no);
	if(ptr == NULL)
	{
		sem_post(&sem);
		cout << "Could not add file to directory";
		return -1;	
	}
	dir_entry_t* di = (dir_entry_t*)ptr;
	strcpy(di->filename,".");
	di->inode = (short)inode_no;

	ptr = search_directory_space(inode_no);
	if(ptr == NULL)
	{
		sem_post(&sem);
		cout << "Could not add file to directory";
		return -1;	
	}
	di = (dir_entry_t*)ptr;
	strcpy(di->filename,"..");
	di->inode = (short)cwd;	


	//Add the directory to the current working directory
	ptr = search_directory_space(cwd);
	if(ptr == NULL)
	{
		sem_post(&sem);
		cout << "Could not add file to directory";
		return -1;	
	}
	di = (dir_entry_t*)ptr;
	strcpy(di->filename,dirname);
	di->inode = (short)inode_no;
	sem_post(&sem);
	return 0;
}


/*
 *Finds the given directory in the current working directory and changes to it as the cwd if valid
 */
int chdir_myfs(char* dirname)
{
	sem_wait(&sem);
	vector<ls_list_t> v;
	ls(&v);
	inode_t* temp;
	int flag = 0;
	for(int i = 0; i < v.size(); i++)
	{
		if(strcmp(v[i].di.filename, dirname) == 0)
		{
			temp = &myfs.inode->list[v[i].di.inode];
			if(temp->file_t == directory)
			{
				cwd = v[i].di.inode;
				flag = 1;
			}
			time(&temp->last_read);

		}
	}
	sem_post(&sem);
	if(flag == 1)
		return 0;
	else 
	{
		cout << "No such directory present" << endl;
		return -1;
	}
}


/*
 *Recursivley removes a directory and all files and directories within it too
 */
int rmdir_myfs(char* dirname)
{
	if(chdir_myfs(dirname)==-1)
	{
		cwd = 0;
		cout << "Some error occured in deletion -" << dirname << endl;
		sem_post(&sem);
		return -1;
	}
	int inode_no = cwd;
	sem_wait(&sem);
	vector<ls_list_t> v;
	sem_post(&sem);
	ls(&v);
	inode_t* temp;
	for(int i = 0; i < v.size(); i++)
	{
		if(strcmp(v[i].di.filename, ".") == 0 || strcmp(v[i].di.filename, "..")==0)
		{
				//Do nothing 
		}
		else 
		{

			sem_wait(&sem);
			temp = &myfs.inode->list[v[i].di.inode];
			sem_post(&sem);
			cout << "Deleting " << temp->filename << endl;
			if(temp->file_t == directory)
			{
				if(rmdir_myfs(temp->filename) == -1)
				{
					return -1;
				}
			}
			else
			{
				if(rm_myfs(temp->filename) == -1)
					return -1;
			}

		}
	}
	chdir_myfs((char*)"..");
	
	//Treat the directory as a file now and delete all of its data
	sem_wait(&sem);
	myfs.inode->list[inode_no].file_t = regular;
	sem_post(&sem);
	rm_myfs(dirname);
	return 0;

}

/*
 *Saves the whole filesystem to a file in pc
 */
int dump_myfs(char* dumpfile)
{

	ofstream outfile;
	outfile.open(dumpfile,std::ofstream::out | std::ofstream::binary);
	if(outfile.fail())
		return -1;
	sem_wait(&sem);
	outfile.write(myfs.mem,myfs.super->totalsize);
	sem_post(&sem);
	if(outfile.fail())
		return -1;
	return 0; 
}

/*
 *Restores the filesystem from a file in pc
 */
int restore_myfs(char* dumpfile)
{
	ifstream infile;
	infile.open(dumpfile, std::ofstream::binary);
	if(infile.fail())
	{
		cout << "File not present" << endl;
		return -1;
	}
	sem_wait(&sem);
	infile.read(myfs.mem, myfs.super->totalsize);
	cwd = 0;
	sem_post(&sem);
	if(infile.fail())
		return -1;
	return 0; 
}


/*
 *Prints the status of the file system
 */
int staus_myfs()
{
	sem_wait(&sem);
	cout << "File System info: " << endl;
	cout << "Total File System Size - " << myfs.super->totalsize << endl;
	cout << "Maximum no of inodes - " << myfs.super->max_inodes << endl;
	cout << "Inodes currently using - " << myfs.super->act_inodes << endl;
	cout << "Maximum no of data blocks - " << myfs.super->max_blocks << endl;
	cout << "Data blocks currently using - " << myfs.super->act_blocks << endl;
	cout << "Block Size - " << BLOCK_SIZE << endl;
	sem_post(&sem);
}

/*
 * Changes the access permisisions of the file/directory in current working directory if present. Otherwise returns -1
 */
int chmod_myfs(char* name, int mode)
{
	vector<ls_list_t> v;
	sem_wait(&sem);
	ls(&v);
	sem_post(&sem);
	inode_t* temp;
	int flag = 0;
	for(int i = 0; i < v.size(); i++)
	{
		if(strcmp(v[i].di.filename, name) == 0)
		{
			sem_wait(&sem);
			temp = &myfs.inode->list[v[i].di.inode];
			if(temp->file_t == regular)
			{
				temp->mode = mode;
				flag = 1;
			}
			time(&temp->last_modified);
			sem_post(&sem);
			break;

		}
	}
	if(flag == 1)
		return 0;
	else 
	{
		cout << "No such file present" << endl;
		return -1;
	}
}


/*
 *Helper function to get the next available file descriptor
 */
int getnextfd()
{
	for(int i = 0; i < myfs.table.size(); i++)
	{
		if(myfs.table[i] == NULL)
			return i;
	}
	return myfs.table.size();
}


/*
 *Opens the file in the specified mode - read or write
 *If the mode is read then it searches for the file in the current working directory.
 *If found it assigns a file descriptor to it and then stores the file descriptor along with relevant info in a table 
 *If the mode is write then it removes the file(if present) with the corresponding name in the current working directory and creates a fresh file with the name
 *Again it assigns a file descriptor to it and then stores the file descriptor along with relevant info in a table 
 */
int open_myfs(char* filename, char mode)
{
	sem_wait(&sem);
	if(mode == 'r')
	{
		vector<ls_list_t> v;
		ls(&v);
		inode_t* temp;
		int flag = 0;
		for(int i = 0; i < v.size(); i++)
		{
			if(strcmp(v[i].di.filename, filename) == 0)
			{
				temp = &myfs.inode->list[v[i].di.inode];
				if(temp->file_t == regular)
				{
					//cout << "Changing directory..." <<endl;
					int fd = getnextfd();
					next  = new entry_t;
					next->inode_no = v[i].di.inode;
					next->byteoffset = 0;
					next->rw = 0;
					myfs.table.insert(myfs.table.begin()+fd,next);
					flag = 1;
					time(&temp->last_read);
					sem_post(&sem);
					return fd;
				}
				
			}
		}
		if(flag == 0)
		{
			sem_post(&sem);
			cout << "No such file present" << endl;
			return -1;
		}

	}
	else if(mode == 'w')
	{
		//Removing old file with the same name
		sem_post(&sem);
		rm_myfs(filename);
		sem_wait(&sem);

		//Create a new file
		int inode_no = next_inode();
		if(inode_no == -1)
		{
			sem_post(&sem);
			return -1;
		}
		strcpy(myfs.inode->list[inode_no].filename,filename);
		myfs.inode->list[inode_no].file_t = regular;
		myfs.inode->list[inode_no].filesize = 0;
		time(&(myfs.inode->list[inode_no].last_modified));
		time(&(myfs.inode->list[inode_no].last_read));
		myfs.inode->list[inode_no].mode = 0777;
		for(int i = 0; i < MAX_PTR ; i++)
			myfs.inode->list[inode_no].db_pointer[i] = -1;
		myfs.super->inode_map[inode_no] = 1;

		/*Adding the file to the current working directory*/
		int* ptr = search_directory_space(cwd);
		if(ptr == NULL)
		{
			sem_post(&sem);
			cout << "Could not add file to directory";
			return -1;	
		}
		dir_entry_t* di = (dir_entry_t*)ptr;
		strcpy(di->filename,filename);
		di->inode = (short)inode_no;

		int fd = getnextfd();
		next = new entry_t;
		next->inode_no = inode_no;
		next->byteoffset = 0;
		next->rw = 1;
		myfs.table.insert(myfs.table.begin()+fd,next);
		sem_post(&sem);
		return fd;
	}
}

/*
 *Closes the file descriptor:
 *Checks if the file descriptor is valid or not 
 *If it is valid, remove it from the table of active file descriptors
 */
int close_myfs(int fd)
{
	sem_wait(&sem);
	if(fd >= myfs.table.size())
	{
		sem_post(&sem);
		cout << "Error : File descriptor not valid" << endl;
		return -1;
	}
	else if(myfs.table[fd] == NULL)
	{
		sem_post(&sem);
		cout << "Error : File descriptor not valid" << endl;
		return -1;	
	}
	else
	{
		delete(myfs.table[fd]);
		myfs.table[fd] = NULL;
		sem_post(&sem);
		return 0;
	}
}

/*
 *Reads from the given file descriptor(if valid) nbytes number of consecutive sites starting at the byteoffset stored with the fd 
 *It stores the bytes in the buffer
 *In case the file ends before nbytes, it only reads upto the end of the file and returns
 *It returns the actual number of bytes read. In case of error, -1 is returned
 */
int read_myfs(int fd, int nbytes, char* buf)
{
	sem_wait(&sem);
	int inode;
	if(fd >= myfs.table.size())
	{
		cout << "Error : File descriptor not valid" << endl;
		sem_post(&sem);
		return -1;
	}
	else if(myfs.table[fd] == NULL)
	{
		cout << "Error : File descriptor not valid" << endl;
		sem_post(&sem);
		return -1;	
	}
	else if(myfs.table[fd]->rw != 0)
	{
		cout << "Error : File not open in read mode" << endl;
		sem_post(&sem);
		return -1;	
	}
	else
	{
		inode = myfs.table[fd]->inode_no;
		inode_t* temp = &myfs.inode->list[inode];
		int rem_bytes = nbytes;
		int bytes_read = 0;
		while(rem_bytes!=0)
		{
			int block_ptr = myfs.table[fd]->byteoffset/BLOCK_SIZE;
			block_ptr = get_pointer_by_index(*temp,block_ptr);
			
			int start_i = myfs.table[fd]->byteoffset%BLOCK_SIZE;
			int block_bytes = BLOCK_SIZE - (myfs.table[fd]->byteoffset%BLOCK_SIZE);
			
			int t = min(rem_bytes,block_bytes);
			int i = start_i;
			while(t--)
			{
				if(myfs.table[fd]->byteoffset == temp->filesize)
				{
					time(&temp->last_read);
					sem_post(&sem);
					return bytes_read;
				}
				buf[bytes_read] = myfs.db_blocks[block_ptr].data[i];
				myfs.table[fd]->byteoffset++;
				rem_bytes--;
				bytes_read++;
				i++;
			}

		}
		time(&temp->last_read);
		sem_post(&sem);
		return bytes_read;
	}	
}

/*
 *Helper function to get the data block pointer for the write function. It assigns a new datablock if necessary 
 */
int get_pointer_by_index_write(inode_t* node, int index)
{
	if(index < 8)
	{
		if(node->db_pointer[index] == -1)
		{
			int data_b = next_free_block();
			if(data_b == -1)
			{
				return -1;
			}
			node->db_pointer[index] = data_b;	
		}
		return node->db_pointer[index];
	}
	else if(8<= index && index < (8+(BLOCK_SIZE/4)))
	{
		if(node->db_pointer[8] == -1)
		{
			int data_b = next_free_block();
			if(data_b == -1)
			{
				return -1;
			}
			node->db_pointer[8] = data_b;	
		}
		int* ptr = (int *)(myfs.db_blocks[node->db_pointer[8]].data);
		ptr += (index - 8);
		if(*ptr == -1)
		{
			int data_b = next_free_block();
			if(data_b == -1)
			{
				return -1;
			}
			*ptr = data_b;	
		}
		return *ptr;
	}
	else if((8+(BLOCK_SIZE/4)) <= index && index < (8+(BLOCK_SIZE/4) + ((BLOCK_SIZE/4)*(BLOCK_SIZE/4))))
	{
		if(node->db_pointer[9] == -1)
		{
			int data_b = next_free_block();
			if(data_b == -1)
			{
				return -1;
			}
			node->db_pointer[9] = data_b;	
		}
		int* ptr = (int *)(myfs.db_blocks[node->db_pointer[9]].data);
		//cout << *ptr << " - ";
		index = index - 8 - (BLOCK_SIZE/4);
		int i1 = index/(BLOCK_SIZE/4);
		ptr+= i1;
		if(*ptr == -1)
		{
			int data_b = next_free_block();
			if(data_b == -1)
			{
				return -1;
			}
			*ptr = data_b;	
		}
		ptr = (int*)(myfs.db_blocks[*ptr].data);
		int i2 = index%(BLOCK_SIZE/4);
		ptr+=i2;
		if(*ptr == -1)
		{
			int data_b = next_free_block();
			if(data_b == -1)
			{
				return -1;
			}
			*ptr = data_b;	
		}
		//cout << *ptr << " - " << endl;
		return *ptr;
	}
	else
		return -1;
}

/*
 *Writes nbytes number of bytes present in buf to the file correspoding to the file descriptor fd(if valid) starting at the byteoffset stored with fd
 *It returns the number of bytes actually written. In case of error, -1 is returned  
 */
int write_myfs(int fd, int nbytes, char* buf)
{
	sem_wait(&sem);
	int inode;
	if(fd >= myfs.table.size())
	{
		cout << "Error : File descriptor not valid" << endl;
		sem_post(&sem);
		return -1;
	}
	else if(myfs.table[fd] == NULL)
	{
		cout << "Error : File descriptor not valid" << endl;
		sem_post(&sem);
		return -1;	
	}
	else if(myfs.table[fd]->rw != 1)
	{
		cout << "Error : File not open in write mode" << endl;
		sem_post(&sem);
		return -1;	
	}
	else
	{
		inode = myfs.table[fd]->inode_no;
		inode_t* temp = &myfs.inode->list[inode];
		int rem_bytes = nbytes;
		int bytes_written = 0;
		while(rem_bytes!=0)
		{
			int block_ptr = myfs.table[fd]->byteoffset/BLOCK_SIZE;
			block_ptr = get_pointer_by_index_write(temp,block_ptr);
			if(block_ptr == -1)
			{
				cout << "Error in write" << endl;
				time(&temp->last_modified);
				sem_post(&sem);
				return bytes_written;
			}
			int start_i = myfs.table[fd]->byteoffset%BLOCK_SIZE;
			int block_bytes = BLOCK_SIZE - (myfs.table[fd]->byteoffset%BLOCK_SIZE);
			
			int t = min(rem_bytes,block_bytes);
			int i = start_i;
			while(t--)
			{

				myfs.db_blocks[block_ptr].data[i] = buf[bytes_written];
				myfs.table[fd]->byteoffset++;
				rem_bytes--;
				bytes_written++;
				temp->filesize++;
				i++;
			}

		}
		time(&temp->last_read);
		sem_post(&sem);
		return bytes_written;
	}

}

/*
 *Checks if the file descriptor has reached the end of file or not
 *The checking is done by comparing the byteoffset with the filesize
 *In eof is reached 1 is returned, 0 if eof is not reached, and -1 for error
 */
int eof_myfs(int fd)
{
	sem_wait(&sem);
	int inode;
	if(fd >= myfs.table.size())
	{
		cout << "Error : File descriptor not valid" << endl;
		sem_post(&sem);
		return -1;
	}
	else if(myfs.table[fd] == NULL)
	{
		cout << "Error : File descriptor not valid" << endl;
		sem_post(&sem);
		return -1;	
	}
	else if(myfs.table[fd]->rw != 0)
	{
		cout << "Error : File not open in read mode" << endl;
		sem_post(&sem);
		return -1;	
	}
	else
	{
		inode = myfs.table[fd]->inode_no;
		inode_t* temp = &myfs.inode->list[inode];
		sem_post(&sem);
		if(myfs.table[fd]->byteoffset >= temp->filesize)
			return 1;
		else
			return 0;
	}

}