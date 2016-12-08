#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#define BLOCK_SIZE 8096
#define NUM_BLOCK 4226
#define NUM_FILES 128
#define NUM_INODES 128
#define MAX_BLOCKS_PER_FILE 32

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens

#define MAX_COMMAND_SIZE 255    // The maximum command-line size
#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

unsigned char data_blocks[NUM_BLOCK][BLOCK_SIZE];
int  used_blocks[NUM_BLOCK];

struct inode* inode_array [NUM_FILES];

struct directory_entry
{
    char name[32];
    int inode_index;
    int valid;
};

struct directory_entry * directory_ptr;

struct inode
{
    time_t date;
    int size;
    int block[32];
    int valid;
};
struct inode* inode_array_ptr[NUM_INODES];
int deleteIndex;

int df()  // free space on disk
{
    int count = 0;
    int i;
    for (i=130; i<NUM_BLOCK; i++)
        if(used_blocks[i]!=1)
            count++;
    return count*BLOCK_SIZE;
}
void init ()
{
    int i;
    directory_ptr = (struct directory_entry*) &data_blocks[0];
    for( i = 0; i < NUM_FILES; i++ )
    {
        directory_ptr[i].valid = 0;
    }
    int inode_idx = 0;
    for( i = 1; i < 130; i++ )
    {
        inode_array_ptr[inode_idx] = (struct inode*) &data_blocks[i];
        int j = 0;
        for( j = 0; j < 32; j++ )
        {
            inode_array_ptr[inode_idx]->block[j] = -1;
        }
        inode_idx ++;
    }
}
int findFreeDirectoryEntry()
{
    int num = -1;
    int i;
    for (i=0; i<128; i++)
        if (directory_ptr[i].valid == 0)
        {
            return i;
            break;
        }
    return num;
}
int findFreeInode()
{
    int num = -1;
    int i;
    for (i=0; i<128; i++)
        if (inode_array_ptr[i]->valid == 0)
        {
            return i;
            break;
        }
    return num;
}
int findFreeInodeBlockEntry(int inode_index)
{
    int i;
    int num = -1;
    for (i=0; i<32; i++)
    {
        if (inode_array_ptr[inode_index]->block[i]==-1)
        {
            num = i;
            break;
        }
    }
    return num;
}
int findFreeBlock()
{
    int num=-1;
    int i;
    for(i=130; i<4226; i++)
    {
        if(used_blocks[i]==0)
            return i;
    }
    return num;
}

int checkAlphanumeric(char * fileName)
{
    int i;
    for(i=0; i<strlen(fileName); i++)
    {
        if(!(fileName[i]<48 && fileName[i]<57) //number
                ||(fileName[i]>65 && fileName[i]<90) //uppercase
                || (fileName[i]>97 && fileName[i]<122) //lowercase
                || (fileName[i]=='.'))  // or "."
        {
            return 0; //none of the above is true
        }
    }
    return 1; //all good
}

int findFile(char * fileName)
{
    int fileFound = 0;
    int i;
    for(i=0; i<NUM_FILES; i++)
    {
        if((strcmp(directory_ptr[i].name, fileName)==0)
                && directory_ptr[i].valid == 1)
        {
            return i;
        }
    }
    return -1;
}

void delete(char * fileName)
{
    deleteIndex = findFile(fileName);
    if(deleteIndex ==-1)
    {
        printf("del error: File not found.\n");
        return;
    }
    directory_ptr[deleteIndex].valid= 0;
    int inode_idx = directory_ptr[deleteIndex].inode_index;
    inode_array_ptr[inode_idx]->valid = 0;
    int i;
    for (i=0; i<32; i++)
    {
        int block_index = inode_array_ptr[inode_idx]->block[i];
        used_blocks[block_index]=0;
    }
}

void undel(char * fileName)
{
    if(strcmp(&directory_ptr[deleteIndex].name, fileName){

    }
    directory_ptr[deleteIndex].valid= 1;
    int inode_idx = directory_ptr[deleteIndex].inode_index;
    inode_array_ptr[deleteIndex]->valid = 1;
    int i;
    for (i=0; i<32; i++)
    {
        int block_index = inode_array_ptr[deleteIndex]->block[i];
        used_blocks[block_index]=1;
    }
}

void list()
{
    int i;
    int isEmpty = 1;
    for (i=0; i<NUM_FILES; i++)
    {
        if(directory_ptr[i].valid==1)
        {
            int inode_idx = directory_ptr[i].inode_index;
            isEmpty = 0;
            printf("%d %s %s",inode_array_ptr[inode_idx]->size,
                   directory_ptr[i].name,ctime(&(inode_array_ptr[inode_idx]->date)));
        }
    }
    if (isEmpty)
    {
        printf("list: No files found.\n");
    }
}

void put (char * fileName)
{
    struct stat buf;
    int status =  stat(fileName, &buf );
    if(status ==-1)
    {
        printf("Error: File not found\n");
        return;
    }
    if(buf.st_size>259072)
    {
        printf("Error: File is too big\n");
        return;
    }
    if (buf.st_size>df())
    {
        printf("put error: Not enough disk space\n");
        return;
    }
    int dir_idx = findFreeDirectoryEntry();
    if(dir_idx == -1)
    {
        printf("Error: not enough room in the file system\n");
        return;
    }
    int inode_idx = findFreeInode();
    if(inode_idx == -1)
    {
        printf("Error:No free inode in the file system\n");
        return;
    }
    //check if fileExist in System
    int fileFound = findFile(fileName);
    if(fileFound !=-1)
    {
        printf("Overwriting an existing file \n");
        delete(fileName);
    }

    //All good at this point
    directory_ptr[dir_idx].valid= 1;
    strncpy(directory_ptr[dir_idx].name, fileName, strlen(fileName));
    directory_ptr[dir_idx].inode_index = inode_idx;
    inode_array_ptr[inode_idx]->size = buf.st_size;
    time(&(inode_array_ptr[inode_idx]->date));
    inode_array_ptr[inode_idx]->valid = 1;

    // Open the input file read-only
    FILE *ifp = fopen ( fileName, "r" );
    int copy_size   = buf . st_size;
    int offset      = 0;

    while( copy_size >= BLOCK_SIZE )
    {
        int block_index = findFreeBlock();
        if(block_index==-1)
        {
            printf("Error: Can't find free data blocks\n");
            return;
        }
        used_blocks[block_index]=1;
        int inode_block_entry = findFreeInodeBlockEntry(inode_idx);
        if(inode_block_entry==-1)
        {
            printf("Error: Can't find free inode blocks\n");
            return;
        }
        inode_array_ptr[inode_idx]->block[inode_block_entry] = block_index;
        fseek( ifp, offset, SEEK_SET );
        int bytes  = fread( data_blocks[block_index], BLOCK_SIZE, 1, ifp );
        if( bytes == 0 && !feof( ifp ))
        {
            printf("An error occured reading from the input file.\n");
            return;
        }
        clearerr( ifp );
        copy_size -= BLOCK_SIZE;
        offset    += BLOCK_SIZE;
        block_index ++;
    }
    if(copy_size>0)
    {
        int block_index = findFreeBlock();
        if(block_index==-1)
        {
            printf("Error: Can't find free 2 block\n");
            //Need to clearn up, already looked for free block
            return;
        }
        used_blocks[block_index]=1;
        int inode_block_entry = findFreeInodeBlockEntry(inode_idx);
        if(inode_block_entry==-1)
        {
            printf("Error: Can't find free inode blocks\n");
            //Need to clearn up, already looked for free block
            return;
        }
        inode_array_ptr[inode_idx]->block[inode_block_entry] = block_index;
        fseek( ifp, offset, SEEK_SET );
        int bytes  = fread( data_blocks[block_index], copy_size, 1, ifp );
    }
    fclose( ifp );
}
void get(char *filename1,char *filename2 )
{
    //CONFIRM file exists in data structure
    int directory_index = findFile(filename1);
    if(directory_index ==-1)
    {
        printf("File not found\n");
        return;
    }
    FILE *ofp;
    if(filename2==NULL)
        ofp= fopen(filename1,"w");
    else
        ofp= fopen(filename2,"w");
    int inode_id = directory_ptr[directory_index].inode_index;
    int filesize =inode_array_ptr[inode_id]->size;
    int copy_size = filesize;
    int offset = 0;
    int block_index=0;
    int inode_block_index =0;

    while(copy_size>0)
    {
        block_index = (inode_array_ptr[inode_id])->block[inode_block_index++];
        if(copy_size >BLOCK_SIZE)
        {
            fwrite(data_blocks[block_index], BLOCK_SIZE, 1 , ofp);
        }
        else
        {
            fwrite(data_blocks[block_index],copy_size, 1 , ofp);
        }
        offset +=BLOCK_SIZE;
        copy_size -=BLOCK_SIZE;
        fseek(ofp,offset,SEEK_SET);
    }
    fclose(ofp);
}

int main (void)
{
    char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
    init();
    int deleteFlag;
    while( 1 )
    {
        printf ("mfs> ");// Print out the msh prompt
        // Read the command from the commandline.
        // The maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there is no input
        while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];
        int   token_count = 0;
        // Pointer to point to the token
        // parsed by strsep
        char *arg_ptr;
        char *working_str  = strdup( cmd_str );
        // we are going to move the working_str pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *working_root = working_str;

        // Tokenize the input stringswith whitespace used as the delimiter
        while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL)
                && (token_count<MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
            if( strlen( token[token_count] ) == 0 )
            {
                token[token_count] = NULL;
            }
            token_count++;
        }

        //run the proper command
        if(strcmp(token[0], "put")==0)
        {
            if(strlen(token[1])>32)
            {
                printf("put error: File name too long.\n");
                continue;
            }
            else if (!checkAlphanumeric(token[1]))
            {
                printf("File name can only be alphanumeric\n");
                continue;
            }
            put(token[1]);
            deleteFlag = 0;
        }
        else if (strcmp(token[0], "get")==0)
        {
            get(token[1], token[2]);
        }
        else if (strcmp(token[0], "del")==0)
        {
            delete(token[1]);
            deleteFlag = 1;
        }
        else if (strcmp(token[0], "undel")==0)
        {
            if(deleteFlag==1)
            {
                undel(token[1]);
                deleteFlag =0;
            }
            else printf("Undelete cannot be performed at this point \n");
        }
        else if (strcmp(token[0], "list")==0)
        {
            list();
        }
        else if (strcmp(token[0], "df")==0)
            printf("%d bytes available on disk\n", df());
        else if (strcmp(token[0], "quit")==0)
            exit(0);
        else
            printf("Command not supported\n");
        free(working_root);
    }
}
