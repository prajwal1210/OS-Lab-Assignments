#ifndef MYS_H
#define MYS_H

#include <ctime>
#include <bitset>
#include <sys/stat.h>
#include <vector>
#include <semaphore.h>



/*Constants*/

#define BLOCK_SIZE (1<<8) 	//Block size
#define MAX_INS (1<<10)		//Max no of inodes

#define MAX_MEM (1<<21)		//Max no of blocks assuming 512MB as max 

#define MAX_PTR 10			//Maximum pounters in an inode
#define MAX_FILENAME 30		//Maximum size of filename



/*Super Block*/
typedef struct sup{
	int totalsize;
	int max_inodes;
	int act_inodes;
	int max_blocks;
	int act_blocks;
	std::bitset<MAX_INS> inode_map;
	std::bitset<MAX_MEM> block_map;
}super_t;

const int SUP_SIZE = sizeof(super_t);

const int SUP_BLOCK_SIZE = SUP_SIZE/BLOCK_SIZE + 1;



/*Enum for filetype*/
enum filetype{
	regular = 0,
	directory = 1
};


/*Inode structure*/
typedef struct i{
	char filename[MAX_FILENAME];
	filetype file_t;
	int filesize;
	time_t last_modified;
	time_t last_read;
	mode_t mode;
	int db_pointer[MAX_PTR];
}inode_t;


const int INODE_SIZE = sizeof(inode_t);

typedef struct list{
	inode_t list[MAX_INS];
}inode_list_t;

const int INODE_LIST_SIZE = sizeof(inode_list_t);

const int INODE_BLOCK_SIZE = INODE_LIST_SIZE/BLOCK_SIZE + 1;


/*Data Blocks*/
typedef struct data{
	char data[BLOCK_SIZE];
	
}data_block_t;


const int MAX_DATA_BLOCKS = MAX_MEM - INODE_LIST_SIZE - SUP_BLOCK_SIZE;

typedef struct tab
{
	int inode_no;
	int byteoffset;
	int rw;

}entry_t;

typedef struct fs
{
	char* mem;
	super_t* super;
	inode_list_t* inode;
	data_block_t* db_blocks;
	std::vector<entry_t*> table;
}mrfs;


typedef struct ind{
	int indirect[BLOCK_SIZE/4];
}ind_t;

typedef struct dirfile{
	char filename[30];
	short int inode;
}dir_entry_t;

typedef struct ls_list
{
	dir_entry_t di;
	int* ptr;
}ls_list_t;

int create_myfs(int size);

int copy_pc2myfs(char* source, char *dest);

int copy_myfs2pc(char* source, char *dest);

int rm_myfs(char* filename);

int showfile_myfs(char* filename);

int ls_myfs();

int mkdir_myfs(char* dirname);

int chdir_myfs(char *dirname);

int rmdir_myfs(char* dirname);

int open_myfs(char* filename, char mode);

int close_myfs(int fd);

int read_myfs(int fd, int nbytes, char* buff);

int write_myfs(int fd, int n_bytes, char* buff);

int eof_myfs(int fd);

int dump_myfs(char* dumpfile);

int restore_myfs(char* dumpfile);

int status_myfs();

int chmod_myfs(char* name, int mode);

int next_inode();

int next_free_block();
#endif