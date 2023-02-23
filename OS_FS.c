
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
//#include<iostream>
//#include<io.h>

#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFILESIZE 1024

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2

typedef struct Superblock
{
    int TotalInodes;
    int FreeInodes;
}SUPERBLOCK, *PSUPERBLOCK;

typedef struct inode
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int permission;  //1 2 3
    struct inode * next;
}INODE, *PINODE, **PPINODE;

typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode;  // 1 2 3
    PINODE ptrinode;
}FILETABLE, *PFILETABLE;

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[MAXINODE];
SUPERBLOCK SUPERBLOCKobj;
PINODE head= NULL;
 
void man(char *name)
{
    if(name == NULL)return;
    
    if(strcmp(name,"create")==0)
    {
        printf("Description : Used to create new regular file\n");
        printf("Usage : Create File_name permission\n");
    }
    else if(strcmp(name, "read")==0)
    {
        printf("Description : Used to read data from regular file\n");
        printf("Usage : read File_name No_Of_Bytes_To_Read\n");
    }
    else if(strcmp(name, "write")==0)
    {
       printf("Description : Used to write inr regular files\n ");
       printf("Usage : write File_name\n After the enter the data that we want to write\n");
    }
    else if(strcmp(name, "ls")==0)
    {
        printf("Description : Used to list all information of files\n" );
        printf("Usage : ls\n");
    }
    else if(strcmp(name, "stat")==0)
    {
        printf("Description : Used to display information of files\n ");
        printf("Usage : stat File_name\n");
    }
    else if(strcmp(name, "fstat")==0)
    {
        printf("Description : Used to display information of files\n" );
        printf("Usage : stat File_Descriptor\n");
    }
      else if(strcmp(name, "truncate")==0)
    {
        printf("Description : Used to remove data from file\n" );
        printf("Usage : truncate File_name\n");
    }
      else if(strcmp(name, "open")==0)
    {
        printf("Description : Used to open existing files\n" );
        printf("Usage : open File_name mode\n");
    }
      else if(strcmp(name, "close")==0)
    {
        printf("Description : Used to close open file\n" );
        printf("Usage : close File_name\n");
    }
      else if(strcmp(name, "closeall")==0)
    {
        printf("Description : Used to close all open files\n ");
        printf("Usage : closeall\n");
    }
      else if(strcmp(name, "lseek")==0)
    {
        printf("Description : Used to change file offset\n" );
        printf("Usage : lseek File_name ChangeInOffset StartPoint\n");
    }
      else if(strcmp(name, "rm")==0)
    {
        printf("Description : Used to delete the file\n" );
        printf("Usage : rm File_name\n");
    }
    else
    {
        printf("ERROR : No manual entry available\n");
    }
}

void DisplayHelp()
{
    printf("ls : To list out all files\n");
    printf("clear : To clear console\n");
    printf("create: To create the file\n");
    printf("open: To open the file\n");
    printf("close : To close the file\n");
    printf("closeall : To closeall open files\n");
    printf("read : To read the contents ftom the file\n");
    printf("write: To write contents into the file\n");
    printf("exit : To terminate file systen\n");
    printf("stat : To display information of file using name\n");
    printf("fstat : To display information of file using Description\n");
    printf("truncate : To remove all data from the file\n");
    printf("rm : To delete the file\n");
}

int GetFDFromName(char *name)
{
    int i=0;
    
    while(i<MAXINODE)
    {
        if(UFDTArr[i].ptrfiletable != NULL);
            if(strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name)==0)
                break;
        i++;
    }
    
    if(i== MAXINODE)       
        return -1;
    else           
        return i;
}

PINODE Get_Inode(char * name)
{
    PINODE temp= head;
    int i=0;
    
    if(name == NULL)
        return NULL;
        
    while (temp != NULL)
    {
        if(strcmp(name,temp->FileName)==0)
            break;
            temp=temp->next;
    }
    return temp;
}

void CreateDILB()
{
    int i=1;
    PINODE newn= NULL;
    PINODE temp= head;
    
    while(i<= MAXINODE)
    {
        newn= (PINODE)malloc(sizeof(INODE));
        
        newn->LinkCount=0;
        newn->ReferenceCount =0;
        newn->FileType =0;
        newn->FileSize=0;
        
        newn->Buffer= NULL;
        newn->next= NULL;
        
        newn->InodeNumber = i;
        
        if(temp == NULL)
        {
            head = newn;
            temp= head;
        }
        else
        {
            temp->next= newn;
            temp= temp->next;
        }
        i++;
    }
    printf("DILB created Successfully\n");
}

void InitialiseSuperBlock()
{
    int i=0;
    while(i < MAXINODE)
    {
        UFDTArr[i].ptrfiletable= NULL;
        i++;
    }
    
    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInodes = MAXINODE;
}

int CreateFile(char *name, int permission)
{
    int i=0;
    PINODE temp= head;
    
    if((name == NULL) || (permission == 0) || (permission > 3))
        return -1;
    
    if(SUPERBLOCKobj.FreeInodes == 0)
        return -2;
        
    (SUPERBLOCKobj.FreeInodes)--;
    
    if(Get_Inode(name) != NULL)
        return -3;
    
    while(temp != NULL)
    {
        if(temp->FileType == 0)
            break;
        temp= temp->next;
    }
    
    while(i<MAXINODE)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
            break;
        i++;
    }
    
    UFDTArr[i].ptrfiletable= (PFILETABLE)malloc(sizeof(FILETABLE));
    
    UFDTArr[i].ptrfiletable->count=1;
    UFDTArr[i].ptrfiletable->mode= permission;
    UFDTArr[i].ptrfiletable->readoffset= 0;
    UFDTArr[i].ptrfiletable->writeoffset=0;
    
    UFDTArr[i].ptrfiletable->ptrinode= temp;
    
    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
    UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
    UFDTArr[i].ptrfiletable->ptrinode->permission = permission;
    UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);
    
    return i;
}

// rm_file("Demo.txt")
int rm_file(char *name)
{
    int fd= 0;
    
    fd= GetFDFromName(name);
    if(fd == -1)
        return -1;
        
    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;
    
    if((UFDTArr[fd].ptrfiletable->ptrinode->LinkCount) == 0)
    {
        int i=0;
        UFDTArr[fd].ptrfiletable->ptrinode->FileType=0;
        while(i<(strlen(UFDTArr[fd].ptrfiletable->ptrinode->FileName)))
        {
            UFDTArr[fd].ptrfiletable->ptrinode->FileName[i]=' ';  
            i++;
        }
       
        //free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer);
        UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize=0;
        free(UFDTArr[fd].ptrfiletable);
    }
    
    UFDTArr[fd].ptrfiletable =NULL;
    (SUPERBLOCKobj.FreeInodes)++;
}

int ReadFile(int fd, char *arr, int isize)
{
    int read_size=0;
    
    if(UFDTArr[fd].ptrfiletable == NULL)      return -1;
    
    if(UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != READ+WRITE)        return -2;
    
    if(UFDTArr[fd].ptrfiletable->ptrinode->permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->permission != READ+WRITE)      return -2;
    
    if(UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)        return -3;
    
    if(UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)    return -4;

    if((UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)==0)     return -5;
    
    read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);
    if(read_size < isize)
    {
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),read_size);
        
            UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + read_size;
    }
    else
    {
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),isize);
        
            (UFDTArr[fd].ptrfiletable->readoffset) = UFDTArr[fd].ptrfiletable->readoffset + isize;
    }
    
    return isize;
}

int WriteFile(int fd, char * arr, int iSize)
{
    if(((UFDTArr[fd].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd].ptrfiletable->mode) != READ+WRITE))  return -1;
    
    if(((UFDTArr[fd].ptrfiletable->ptrinode->permission) != WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->permission) != READ+WRITE))  return -1;
    
    if((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE)     return -2;
    
    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)     return -3;

    if((UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)==0)     return -4;
    
    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset),arr,iSize);
    
    (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + iSize;
    
    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + iSize;
    
    return iSize;
}

int OpenFile(char *name, int mode)
{
    int i=0;
    PINODE temp= NULL;
    
    if(name == NULL || mode <= 0)
    return -1;
    
    temp = Get_Inode(name);
    if(temp == NULL)
    return -2;
    
    if(temp->permission < mode)
    return -3;
    
    while(i<MAXINODE)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
            break;
        i++;
    }
    
    UFDTArr[i].ptrfiletable =(PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrfiletable == NULL)     return -1;
    UFDTArr[i].ptrfiletable->count =1;
    UFDTArr[i].ptrfiletable->mode= mode;
    if(mode == READ+WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset =0;
        UFDTArr[i].ptrfiletable->writeoffset =0;
    }
    else if(mode == READ)
    {
        UFDTArr[i].ptrfiletable->readoffset =0;
    }
    else if(mode == WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset =0;
    }
    UFDTArr[i].ptrfiletable->ptrinode= temp;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;
    
    return i;
}

/*void CloseFileByName(int fd)
{
    UFDTArr[fd].ptrfiletable->readoffset =0;
    UFDTArr[fd].ptrfiletable->writeoffset =0;
    (UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount) --;
}
*/
int CloseFileByName(char *name)
{
    int i=0;
    i= GetFDFromName(name);
    if(i == -1)
        return -1;
        
    UFDTArr[i].ptrfiletable->readoffset=0;
    UFDTArr[i].ptrfiletable->writeoffset=0;

    if(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount >0)
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount) --;
    
    return 0;
}

void CloseAllFiles()
{
    int i=0;
    while(i<MAXINODE)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset =0;
            UFDTArr[i].ptrfiletable->writeoffset =0;
            if(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount >0)
            { 
                   (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount) --;
            }
            break;
        }
        i++;
    }
}

int LseekFile(int fd, int size, int from)
{
    char arr[50];
    int i=0;

    if((fd<0) || (from > 2)) return -1;
    if(UFDTArr[fd].ptrfiletable == NULL)       return -1;
    
    if((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ+WRITE))
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)    return -1;
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)     return-1;
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) +size;
        }
        else if(from == START)
        {
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))      return-1;
            if(size < 0)    return-1;
            (UFDTArr[fd].ptrfiletable->readoffset) = size;
        }
        else if(from == END)
        {
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)        return-1;
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)   return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
            while (i<size)
            { 
                arr[i] = ' ';
                i++;
            }
            WriteFile(fd,arr,size);
            
        }
    }
    else if(UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) > MAXFILESIZE)     return -1;
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)     return -1;
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))     return -1;
            (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
        }
        else if(from == START)
        {
            if(size > MAXFILESIZE)       return -1;
            if(size < 0)     return -1;
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
            (UFDTArr[fd].ptrfiletable->writeoffset)= size;
        }
        else if(from == END)
        {
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)      return -1;
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)     return -1;
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
            while (i<size)
            { 
                arr[i] = ' ';
                i++;
            }
            WriteFile(fd,arr,size);
        }
    }
}

void ls_File()
{
    int i=0;
    PINODE temp= head;
    
    if(SUPERBLOCKobj.FreeInodes == MAXINODE)
    {
        printf("Error : There are no files\n");
        return;
    }
    
    printf("\nFile name\tInode number\tFile size\tLink count\n");
    printf("_____________________________________________________________\n");
    while(temp != NULL)
    {
        if(temp->FileType != 0)
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount);
        }
        temp= temp->next;
    }
    printf("______________________________________________________________\n");
}

int fstat_File(int fd)
{
    PINODE temp= head;
    int i=0;
    
    if(fd<0)        return -1;
    
    if(UFDTArr[fd].ptrfiletable == NULL)      return -2;
    
    temp= UFDTArr[fd].ptrfiletable->ptrinode;
    
    printf("\n_____________Statistical information about file___________________\n");
    printf("File Name : %s\n", temp-> FileName);
    printf("Inode number : %d\n", temp-> InodeNumber);
    printf("File size : %d\n", temp-> FileSize);
    printf("Actual File size : %d\n", temp-> FileActualSize);
    printf("Link count : %d\n", temp-> LinkCount);
    printf("Reference Count : %d\n", temp-> ReferenceCount);
    
    if(temp->permission == 1)
        printf("File permission :Read only\n");
    else if(temp->permission == 2)
        printf("File permission: write\n");
    else if(temp->permission == 3)
        printf("File permission: Read and write\n");
    printf("_________________________________________________________\n\n");
    
    return 0;
}

int stat_File(char *name)
{
    PINODE temp= head;
    int i=0;
    
    if(name == NULL)        return -1;
    
    while(temp != NULL)
    {
        if(strcmp(name,temp->FileName) == 0)
        break;
        temp= temp->next;
    }
    
    if(temp == NULL)        return -2;
    
    printf("\n__________________________Statistical information about files__________________________\n");
    printf("File Name : %s\n", temp-> FileName);
    printf("Inode number : %d\n", temp-> InodeNumber);
    printf("File size : %d\n", temp-> FileSize);
    printf("Actual File size : %d\n", temp-> FileActualSize);
    printf("Link count : %d\n", temp-> LinkCount);
    printf("Reference Count : %d\n", temp->ReferenceCount);
    
    if(temp->permission == 1)
        printf("File permission :Read only\n");
    else if(temp->permission == 2)
        printf("File permission: write\n");
    else if(temp->permission == 3)
        printf("File permission: Read and write\n");
    printf("_________________________________________________________\n\n");
    
    return 0;
}

void Backup_file()
{
   int fd1=0,fd2=0;
   PINODE temp=head;
   char arr[1024];
  if(SUPERBLOCKobj.FreeInodes != MAXINODE)
   { 
       while(temp!=NULL)
       {
           if(temp->FileType != 0)
           {
                fd1 = GetFDFromName(temp->FileName);
                fd2 = creat((temp->FileName),0777);
                
                 if(temp->FileActualSize!=0)
                 {
                     LseekFile(fd1,0,0);
                     ReadFile(fd1,arr,(temp->FileActualSize));
                     write(fd2,arr,(temp->FileActualSize)); 
                 }
           }
          temp=temp->next;
       }            
             
   }

}

int truncate_File(char *name)
{
    int fd= GetFDFromName(name);
    if(fd == -1)
       return -1;
       
       memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
       UFDTArr[fd].ptrfiletable->readoffset =0;
       UFDTArr[fd].ptrfiletable->writeoffset =0;
       UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize= 0;
}

int main()
{
    char *ptr= NULL;
    int ret=0, fd=0, count=0;
    char command[4][80],str[80],arr[1024];
    
    InitialiseSuperBlock();
    CreateDILB();
    
    while(1)
    {
        fflush(stdin);
        strcpy(str,"");
        
        printf("\nHrushi VFS : >");
        
        fgets(str,80,stdin); //scanf("%[^"\n"]s",str);
        
        count= sscanf(str,"%s %s %s %s", command[0],command[1], command[2], command[3]);     //tockenisation
        
        if(count == 1)
        {
            if(strcmp(command[0],"ls") == 0)
            {
                ls_File();
            }
            else if(strcmp(command[0],"closeall") == 0)
            {
                CloseAllFiles();
                printf("All files closed Successfully\n");
                continue;
            }
            else if(strcmp(command[0],"clear") == 0)
            {
                system("cls");
                continue;
            }
            else if(strcmp(command[0],"help") == 0)
            {
                DisplayHelp();
                continue;
            }
            else if(strcmp(command[0],"exit") == 0)
            {
                Backup_file();
                printf("Terminating the  VFS\n\n");
                break;
            }
            else
            {
                printf("\nERROR : command not found\n");
                continue;
            }
        }
        else if(count == 2)
        {
            if(strcmp(command[0],"stat") == 0)
            {
                ret= stat_File(command[1]);
                if(ret==-1)
                    printf("Error : Incorrect parameters\n");
                if(ret== -2)
                    printf("Error : Thers is no such files\n");
                    continue;
            }
            else if(strcmp(command[0],"fstat") == 0)
            {
                ret=fstat_File(atoi(command[1]));
                if(ret == -1)
                    printf("Error : Incorrect parameters\n");
                if(ret == -2)
                    printf("Error :Thers is no such files\n");
                continue;
            }
            else if(strcmp(command[0],"close") == 0)
            {
                ret=CloseFileByName(command[1]);
                if(ret == -1)
                    printf("Error :Thers is no such files\n");
                continue;
            }
            else if(strcmp(command[0],"rm") == 0)
            {
                ret=rm_file(command[1]);
                if(ret == -1)
                    printf("Error :Thers is no such files\n");
                continue;
            }
            else if(strcmp(command[0],"man") == 0)
            {
                man(command[1]);
            }
            else if(strcmp(command[0],"write") == 0)
            {
                fd= GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR : Incorrect parameters\n");
                    continue;
                }
                printf("Enter the data: \n");
                scanf("%[^\n]",arr);
                
                ret= strlen(arr);
                if(ret==0)
                {
                    printf("ERROR : Incorrect parameters\n");
                    continue;
                }
                ret= WriteFile(fd,arr,ret);
                if(ret == -1)
                    printf("ERROR : permission denined\n");
                if(ret == -2)
                    printf("ERROR : There is no sufficient memory\n");
                if(ret == -3)
                    printf("ERROR : It is not regular file\n");
                if(ret == -4)
                    printf("ERROR : File is not open\n");    
            }
            else if(strcmp(command[0],"truncate") == 0)
            {
                ret= truncate_File(command[1]);
                if(ret == -1)
                        printf("ERROR :Incorrect parameters\n");
            }
            else
            {
                printf("\nERROR : command not found\n");
                continue;
            }
        }
        else if(count == 3)
        {
            if(strcmp(command[0],"create")==0)
            {
                ret= CreateFile(command[1],atoi(command[2]));
                if(ret >= 0)
                    printf("File is Successfully created with file descriptor :%d\n",ret);
                if(ret ==-1)
                    printf("ERROR : Incorrect parameters\n") ;
                if(ret == -2)
                    printf("ERROR :There is no inode\n") ;
                if(ret == -3)
                    printf("ERROR :File is already exist\n");
                if(ret == -4)
                    printf("ERROR :Memory allocation failure\n");
                continue;
            }
            else if(strcmp(command[0],"open")==0)
            {
                ret= OpenFile(command[1],atoi(command[2]));
                if(ret >= 0)
                    printf("File is Successfully open with file descriptor :%d\n",ret);
                if(ret ==-1)
                    printf("ERROR : Incorrect parameters\n") ;
                if(ret == -2)
                    printf("ERROR :file not present\n") ;
                if(ret == -3)
                    printf("ERROR :permission denined\n");
                continue;
            }
            else if(strcmp(command[0],"read")==0)
            {
                fd= GetFDFromName(command[1]);
                if(fd ==-1)
                {
                    printf("ERROR : Incorrect parameters\n") ;
                    continue;
                }
                ptr= (char *)malloc(sizeof(atoi(command[2]))+1);
                if(ptr == NULL)
                {
                    printf("ERROR :Memory allocation failure\n") ;
                    continue;
                }
                ret= ReadFile(fd,ptr,atoi(command[2]));
                if(ret ==-1)
                    printf("ERROR : file not existing\n") ;
                if(ret == -2)
                    printf("ERROR :permission denined\n") ;
                if(ret == -3)
                    printf("ERROR :reached at end filet\n");
                if(ret == -4)
                    printf("ERROR :It is not regular file\n");
                if(ret == -5)
                    printf("ERROR : File is not open\n");    
                if(ret== 0)
                    printf("ERROR : File is empty\n");
                if(ret > 0)
                {
                    write(2,ptr,ret);
                }
                continue;
            }
            else
            {
                printf("\nERROR : command not found\n");
                continue;
            }
        }
        else if(count == 4)
        {
            if(strcmp(command[0],"lseek") ==0)
            {
                fd= GetFDFromName(command[1]);
                if(fd== -1)
                {
                    printf("ERROR : Incorrect parameters\n");
                    continue;
                }
                ret= LseekFile(fd,atoi(command[2]),atoi(command[3]));
                if(ret == -1)
                {
                    printf("ERROR : Unable to perform lseek\n");
                }
            }
            else
            {
                printf("\nERROR : command not found\n");
                continue;
            }
        }
        else
        {
            printf("\nERROR : command not found\n");
            continue;
        }
    }
return 0;
}