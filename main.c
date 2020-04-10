#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi.h"
// allocate 2d_char array contagious 
char **alloc_2d_char(int lines, int line_size) {
    int i,j;
    char *data = (char *)malloc(lines*line_size*sizeof(char));
    char **mat= (char **)malloc(lines*sizeof(char*));
    for (i=0; i<lines; i++)
        mat[i] = &(data[line_size*i]);
    return mat;
}


int count;
char **ReadFromFile(int start , int end,char **result)
{
	count=0;
	//printf("Count = %d\n",count);
    int i,j;
    char fileName[48];
	char delim []= " ";
    // printf("Enter the Query : ");
    char q[] = "photosynthesis";
    // loop for files
    for(i=start; i<=end; ++i)
    {
        sprintf(fileName, "CS471-Parallel-A2-2020//Aristo-Mini-Corpus//Aristo-Mini-Corpus P-%d.txt", i);
        //printf("%s\n",fileName);

        FILE *f = fopen(fileName, "r");

        char line[256];
        int c=1;
	// read lines from file 
        while (fgets(line, sizeof(line), f))
        {
           // printf("%s\n",line);
            bool is_found = false;
	    // get query token with space delimiter 
            char *qstr = strtok(q, delim);
	    // loop for each token 
            while(qstr != NULL){
		// check if token in thise line or not 
                char *pch = strstr(line,qstr);
                if(!pch){
                    is_found = false;
                    break;
                }
                is_found = true;
	        // get next token or next word 
                qstr = strtok(NULL, delim);
            }
            if(is_found){
               //printf("file number %d line number %d \n",i,c);
               //printf("%s\n",line);
	       // if query in line put line in result array 
               for(j=0;j<strlen(line);++j){
                   result[count][j] = line[j];
               }
               count++;
            }
		// update the strtok with same string 
			strcpy(q,"photosynthesis");
            c++;
        }
        fclose(f);
   }
    return result;
}
void WriteOnFile(char **result ,int count){
	//printf("the count = %d",count);
    FILE *f = fopen("output.txt","w");
    char searchResult[30];
    sprintf(searchResult,"Search Results Found : %d\n\n",count);
    fprintf(f,"%s",searchResult);
    int i;
    for(i=0;i<count;++i){
        fprintf(f,"%s",result[i]);
    }
    fclose(f);
}

int main(int argc , char * argv[])
{
	double performance  =0, end,start; 
	char **selive_result,**final_result;
	int searchResult=0;
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
	
	start = MPI_Wtime();

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);


	int file_nums = 50, file_lines = 30 , line_size = 256;
	
    int num_of_files_sent = file_nums/(p-1);  
	int reminder = file_nums % (p-1);
	int start_remider_file =  file_nums - reminder + 1;
	//printf("start_remider_file : %d" ,start_remider_file);
	int start_file;
	
	selive_result = alloc_2d_char(num_of_files_sent*file_lines,line_size);
	final_result = alloc_2d_char(file_nums*file_lines,line_size);
	
	if( my_rank == 0){
        int i,j;
		
		int index = 1;
		// loop for sending the start file index to each slaive
		for( dest = 1; dest < p ; dest++){ 
		    MPI_Send(&index,1, MPI_INT, dest, tag, MPI_COMM_WORLD);
			
			index += num_of_files_sent;
		}
		// recive result from each slaive
		for(source = 1; source < p ; source++){ 
			MPI_Recv(&(selive_result[0][0]),num_of_files_sent*file_lines*line_size, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
		    MPI_Recv(&count,1, MPI_INT,source,tag, MPI_COMM_WORLD,&status);
			
			for(i=0;i<count;++i){
				for(j=0;j<line_size;++j){
					final_result[searchResult+i][j]  = selive_result[i][j];
				}
			}
			searchResult += count;
		}
		//printf("The final count : %d\n\n",count);
		//printf("The Search Result : %d\n\n",searchResult);
		// handel the reminder file 
		if(reminder > 0){
		   char **reminder_result;
		   reminder_result = alloc_2d_char(reminder*file_lines,line_size);
           reminder_result = ReadFromFile(start_remider_file,file_nums,reminder_result);
		  // printf("The riminder count : %d\n\n",count);
		   //searchResult += count;
		   //printf("The Search Result after reminder : %d\n\n",searchResult);
		   	for(i=0;i<count;++i){
				for(j=0;j<line_size;++j){
					final_result[searchResult+i][j]  = reminder_result[i][j];
				}
			}
		   searchResult += count;
		}
		WriteOnFile(final_result,searchResult);
		
	}else{
		MPI_Recv(&start_file,1, MPI_INT,0,tag, MPI_COMM_WORLD,&status);
		
		//printf("start file index : %d\n\n",start_file);
		
		selive_result = ReadFromFile(start_file,start_file+num_of_files_sent-1,selive_result);
		
		//printf("Count = %d\n",count);
		
		MPI_Send(&(selive_result[0][0]),num_of_files_sent*file_lines*line_size, MPI_CHAR,0, tag, MPI_COMM_WORLD);
		MPI_Send(&count,1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		
	}

	/* shutdown MPI */
    end = MPI_Wtime();
	MPI_Finalize();
	//performance  += end - start; 
	//printf("Start = %f\n\n end = %f\n\n",start,end);
	printf("The process %d took in s : %f\n\n",my_rank,end - start);
	//printf("The Performaance : %f\n\n",performance);
	
	return 0;
}
