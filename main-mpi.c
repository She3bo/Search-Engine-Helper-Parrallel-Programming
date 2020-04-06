#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi.h"

char **alloc_2d_char(int lines, int line_size) {
    int i,j;
    char *data = (char *)malloc(lines*line_size*sizeof(char));
    char **mat= (char **)malloc(lines*sizeof(char*));
    for (i=0; i<lines; i++)
        mat[i] = &(data[line_size*i]);
    return mat;
}

int count=0;

char **ReadFromFile(int start , int end,char **result)
{
	//printf("Count = %d\n",count);
    int i,j;
    char fileName[48];
	char delim []= " ";
    // printf("Enter the Query : ");
    char q[] = "Energy";
    for(i=start; i<=end; ++i)
    {
        sprintf(fileName, "Aristo-Mini-Corpus//Aristo-Mini-Corpus P-%d.txt", i);
        //printf("%s\n",fileName);

        FILE *f = fopen(fileName, "r");

        char line[256];
        int c=1;
        while (fgets(line, sizeof(line), f))
        {
           // printf("%s\n",line);
            bool is_found = false;
            char *qstr = strtok(q, delim);
            while(qstr != NULL){
                char *pch = strstr(line,qstr);
                if(!pch){
                    is_found = false;
                    break;
                }
                is_found = true;
                qstr = strtok(NULL, delim);
            }
            if(is_found){
               //printf("file number %d line number %d \n",i,c);
               //printf("%s\n",line);
               for(j=0;j<strlen(line);++j){
                   result[count][j] = line[j];
               }
               count++;
            }
			strcpy(q,"Energy");
            c++;
        }
        fclose(f);
   }
    return result;
}
void WriteOnFile(char **result ,int count){
	//printf("the count = %d",c);
    FILE *f = fopen("output.txt","a");
    char searchResult[30];
    //sprintf(searchResult,"Search Results Found : %d\n\n",c);
    //fprintf(f,"%s",searchResult);
    int i;
    for(i=0;i<count;++i){
        fprintf(f,"%s",result[i]);
    }
    fclose(f);
}

int main(int argc , char * argv[])
{
	char **result;int c=0;
	int my_rank;		/* rank of process	*/
	int p;			/* number of process	*/
	int source;		/* rank of sender	*/
	int dest;		/* rank of reciever	*/
	int tag = 0;		/* tag for messages	*/
	char message[100];	/* storage for message	*/
	MPI_Status status;	/* return status for 	*/
				/* recieve		*/


	/* Start up MPI */
	MPI_Init( &argc , &argv );

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);


	int file_nums = 50, file_lines = 30 , line_size = 256;
	
    int num_of_files_sent =file_nums/(p-1);  
	int reminder = file_nums % (p-1);
	int start_remider_file =  file_nums - reminder + 1;
	int start_file_index=0;
	
	result = alloc_2d_char(num_of_files_sent*file_lines,line_size);
	
	if( my_rank == 0){
       // int i,j,k;
		
		int index = 1;
		for( dest = 1; dest < p ; dest++){ 
		    MPI_Send(&index,1, MPI_INT, dest, tag, MPI_COMM_WORLD);
			
			index += num_of_files_sent;
		}
		int x=0,y=0,ind=0;
		for(source = 1; source < p ; source++){ 
			MPI_Recv(&(result[0][0]),num_of_files_sent*file_lines*line_size, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
		    MPI_Recv(&count,1, MPI_INT,source,tag, MPI_COMM_WORLD,&status);
			
			WriteOnFile(result,count);
		}
		/*
		if(reminder > 0){

		}
		*/
	}else{
		MPI_Recv(&start_file_index,1, MPI_INT,0,tag, MPI_COMM_WORLD,&status);
		
		//printf("start file index : %d\n\n",start_file_index);
		
		result = ReadFromFile(start_file_index,start_file_index+num_of_files_sent-1,result);
		
		//printf("Count = %d\n",count);
		
		MPI_Send(&(result[0][0]),num_of_files_sent*file_lines*line_size, MPI_CHAR,0, tag, MPI_COMM_WORLD);
		MPI_Send(&count,1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		
	}

	/* shutdown MPI */
	MPI_Finalize();
	
	return 0;
}
