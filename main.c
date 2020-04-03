#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char **alloc_2d_char(int words_num, int word_size) {
    int i,j;
    char *data = (char *)malloc(words_num*word_size*sizeof(char));
    char **mat= (char **)malloc(words_num*sizeof(char*));
    for (i=0; i<words_num; i++)
        mat[i] = &(data[word_size*i]);
    return mat;
}

int count=0;
char **ReadFromFile(int start , int end,char **result)
{
    int i,j;
    char fileName[48];
	char delim []= " ";
    // printf("Enter the Query : ");
    char q[] = "photosynthesis";
    for(i=start; i<=end; ++i)
    {
        sprintf(fileName, "CS471-Parallel-A2-2020//Aristo-Mini-Corpus//Aristo-Mini-Corpus P-%d.txt", i);
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
               // printf("%s\n",line);
               for(j=0;j<strlen(line);++j){
                   result[count][j] = line[j];
               }
               count++;
            }
			strcpy(q,"photosynthesis");
            c++;
        }
        fclose(f);
   }
    return result;
}
void WriteOnFile(char **result){
    FILE *f = fopen("output.txt", "a");
    char searchResult[30];
    sprintf(searchResult,"Search Results Found : %d\n\n",count);
    fprintf(f,"%s",searchResult);
    int i;
    for(i=0;i<count;++i){
        fprintf(f,"%s",result[i]);
    }
    fclose(f);
}
int main()
{

    int i;
    char **result;
    result = alloc_2d_char(1500,256);
    result = ReadFromFile(1,50,result);
    WriteOnFile(result);
    //for(i=0;i<count;++i){
       // printf("%s\n",result[i]);
    //}
    return 0;
}
