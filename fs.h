//meta information about the filesystem
//number  of inodes
//number of disk blocks
//size of the dik blocks

#define BLOCKSIZE 512

struct superblock {
	int num_inodes;
	int num_blocks;
	int size_blocks;	

};

struct inode {
	int size;
	int first_block;
	char name[10];
	
};
struct disk_block {	
	int next_block_num;
	char data[BLOCKSIZE];
	
};

void create_fs();  //initialization new filesystem
void mount_fs();   // load a file system
void sync_fs();	   //write the file system

//return filenumber
int allocate_file(char name[8]);
void set_filesize(int filenum,int size);
void write_byte(int filenum, int pos, char *data);



void print_fs(); //print out info about the filesystem
