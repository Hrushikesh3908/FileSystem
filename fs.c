#include "fs.h"
#include <stdlib.h>
#include<stdio.h>
#include<string.h>


struct superblock sb;
struct inode *inodes;
struct disk_block *dbs;

//initialize new filesystem
void create_fs() {
		
	sb.num_inodes = 10;
	sb.num_blocks = 100;
	sb.size_blocks = sizeof(struct disk_block);

	int i;
	inodes = malloc(sizeof(struct inode) * sb.num_inodes);
	
	//inodes
	for(i=0; i< sb.num_inodes;i++) {

		inodes[i].size = -1;
		inodes[i].first_block = -1;
		strcpy(inodes[i].name, "emptyfi");
	}

	//data blocks
	dbs = malloc(sizeof(struct disk_block) * sb.num_blocks);
	for(i=0 ; i <sb.num_blocks ;i++) {
		dbs[i].next_block_num = -1;
	}

}	//create_fs

void mount_fs() {


  FILE *file ;
        file = fopen("fs_data","r");

        //superblock 
        fread(&sb,sizeof(struct superblock) ,1 ,file);

        //inodes
	inodes = malloc(sizeof(struct inode)  * sb.num_inodes);
	dbs = malloc(sizeof(struct disk_block)  * sb.num_blocks);
       
	 fread(inodes,sizeof(struct inode),sb.num_inodes,file);
        fread(dbs,sizeof(struct inode),sb.num_blocks,file);
        fclose(file);

}//mount-fs

void sync_fs() {
	FILE *file ;
	file = fopen("fs_data","w+");
	
	//superblock 
	fwrite(&sb,sizeof(struct superblock), 1, file);
	
	//inodes

	fwrite(inodes,sizeof(struct inode),sb.num_inodes,file);
	fwrite(dbs,sizeof(struct disk_block) , sb.num_blocks, file);
	fclose(file);
}//sync-fs()



void print_fs() {

	printf("Superblock Infor\n");
	printf("\tnum inodes %d \n ",sb.num_inodes);
	printf("\tnum blocks %d \n ",sb.num_blocks);
	printf("\tsize block %d \n ",sb.size_blocks);
	printf("indoes\n");

	int i;
	for(i=0; i<sb.num_inodes;i++)	{
	
	printf("\t size :%d block %d name %s \n",inodes[i].size, inodes[i].first_block, inodes[i].name);
	inodes[i].size = -1;
	strcpy(inodes[i].name , "emptyfi");
	}
	for(i =0 ;i <sb.num_blocks;i++) {

	printf("\tblock num :%d next block %d\n",i, dbs[i].next_block_num);

	}
		
}//print_fs


int find_empty_inode() {

	int i ;
	for( i=0 ; i<sb.num_inodes; i++) {
		if(inodes[i].first_block == -1 ) {
			return i ;
		}
	}

	return -1;	
}//find_empty_inode 

int find_empty_block() {

        int i ;
        for( i=0 ; i<sb.num_blocks; i++) {
                if(dbs[i].next_block_num == -1 ) {
                        return i ;
                }
        }

        return -1;
}//find_empty_block

int allocate_file(char name[8])  {

        //find an empty inode 
        int in = find_empty_inode();


        // find / clain a disk block 
        int block = find_empty_block();

	
        //claim it 
        inodes[in].first_block = block ;
        dbs[block].next_block_num =  -2;
        
	strcpy(inodes[in].name , name );
	//return the fd 
        return in;

}//allocate_file

void shorten_file(int bn) {
	int nn = dbs[bn].next_block_num;
	if(nn > 0) {
		shorten_file(nn);
	}
	dbs[bn].next_block_num = -1;
	
}//shorten_file 

void set_filesize(int filenum, int size) {
	//how may blocks  should we have
	int tem = size + BLOCKSIZE -1 ;

	int num = tem /BLOCKSIZE;
	
	int bn = inodes[filenum].first_block;
	num--;
	//grow the file if nexessary
	while(num > 0 ) {

		//check next block number
		 int next_num = dbs[bn].next_block_num;
		if(next_num == -2) {

			int empty = find_empty_block() ;
			dbs[bn].next_block_num = empty ;
			dbs[empty].next_block_num = -2;
		}
		bn = dbs[bn].next_block_num;
		num--;

	}	

	//shorten if necessary
	shorten_file(bn);
	dbs[bn].next_block_num = -2;
}//set_filesize
int get_block_num(int file, int offset) {

	int togo = offset;
	int bn = inodes[file].first_block;

	while(togo > 0 ) {

		bn = dbs[bn].next_block_num;
		togo--;
	}
	return bn;

}


void write_byte(int filenum,int pos , char *data) {
	//calculate which block 
	int relative_block = pos / BLOCKSIZE ;
	//find the block number
	int bn = get_block_num(filenum, relative_block);

	//calculate the offset in the block 
	int offset = pos % BLOCKSIZE; 

	//write the data
	dbs[bn].data[offset] = (*data) ;

}//write byte
