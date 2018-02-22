#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

char **words;
int *countingAmount;
int sizeofArray;
int total;
int amountOfWords;
int lengthOfWord( char*finding){
    int i =0;
    while (finding[i] != '\0'){
        i++;
    }
    return i+1;
}
int sameWord( char* first, char* second){
    int i = 0;
    while (first[i] != '\0' || second[i] != '\0') {
        if(first[i] != second[i]){
            return 1;
        }
        i++;
    }
    return 0;
}

int findLocation(char * finding){
    for (int i =0; i<sizeofArray; i++){
        if (sameWord(words[i],finding) == 0){
            return i;
        }
    }
    return -1;
}
void reallocating(){
    if( sizeofArray %32 == 0){
        printf("Re-allocated parallel arrays to be size %d.\n", sizeofArray+32 );
        fflush( stdout );
        countingAmount = realloc(countingAmount, (sizeofArray + 32)*sizeof(int));
        words = realloc(words, (sizeofArray + 32)*sizeof(char*));
        total+=32;
        
    }
}
void addWord(char * adding){
    int loc = findLocation(adding);
   
    if (loc == -1){
         words[sizeofArray] = malloc(lengthOfWord(adding));
        int i = 0;
        for (; adding[i] != '\0';i++){
            words[sizeofArray][i] = adding[i];
        }
        words[sizeofArray][i] = '\0';
        countingAmount[sizeofArray] = 1;
        sizeofArray++;
        reallocating();
    }
    else{
        countingAmount[loc]++;
    }
    
}
int main(int argc, char *argv[]){
    words = calloc( 32, sizeof(char*));
    countingAmount = calloc (32, sizeof(int));
    DIR *dir;
    struct stat file_info;
    int c;
    total =32;
    int amountOfWords = 0;
    FILE *entry_file;
    char pathname[1024];
    sizeofArray =0;
    char * name = malloc(80);
    int location = 0;
    struct dirent *dp;
    if(argc == 3){
        int amount = atoi(argv[2]);
        if (amount <= 0){
            perror("ERROR");
            fflush( stdout );
            return 0;
        }
    }
    if(argc == 1){
        perror("ERROR");
        fflush( stdout );
        return 0;
    }
    printf("Allocated initial parallel arrays of size 32.\n");
    fflush( stdout );
    if ((dir = opendir(argv[1])) == NULL) {
        perror ("ERROR: Cannot open directory\n");
        return 0;
    }
    while ((dp = readdir(dir))!= NULL){
        if (strcmp(dp->d_name,".DS_Store")!=0){
            //name =  malloc(80);
            sprintf( pathname, "%s/%s", argv[1], dp->d_name );
            entry_file = fopen(pathname, "r");
            lstat(pathname, &file_info);
            if(S_ISREG(file_info.st_mode)) {
                if (entry_file) {
                    while ((c =fgetc(entry_file)) != EOF )
                    {
                        if (ispunct(c)){
                            name[location] = '\0';
                            if (location >=2){
                                amountOfWords++;
                                addWord(name);
                            }
                            location = 0;
                            name[0] = '\0';
                        }
                        else if (isalpha(c) || isdigit(c))
                        {
                            name[location] = c;
                            location++;
                        }
                        else
                        {
                            name[location] = '\0';
                            if (location >=2){
                                amountOfWords++;
                                addWord(name);
                            }
                            location = 0;
                            name[0] = '\0';
                        }
                    }
                }
                else {
                    fprintf(stderr, "Error : Failed to open entry file\n");
                    fflush( stdout );
                    return 1;
                }
               
                //free(name);
            }
             fclose(entry_file);
        }
        
    }
    closedir(dir);
    printf("All done (successfully read %d words; %d unique words).\n",amountOfWords,sizeofArray);
    
    if (argc == 2){
        printf("All words (and corresponding counts) are:\n");
        for (int i = 0; i<sizeofArray; i++){
            printf( "%s -- %d\n",words[i],countingAmount[i]);
        }
        fflush( stdout );
    }
    else if(argc == 3){
        int amount = atoi(argv[2]);
        if (amount > 0){
            printf("First %d words (and corresponding counts) are:\n",amount);
            for (int i = 0; i<amount; i++){
                printf( "%s -- %d\n",words[i],countingAmount[i]);
            }
            printf("Last %d words (and corresponding counts) are:\n",amount);
            for (int i = sizeofArray-amount; i<sizeofArray; i++){
                printf( "%s -- %d\n",words[i],countingAmount[i]);
            }
            fflush( stdout );
        }
        else {
            perror("ERROR");
            fflush( stdout );
        }
    }
    else {
        perror("ERROR: enter valid amont of inputs\n");
        fflush( stdout );
    }
    for (int i = 0; i< total; i++){
        free(words[i]);
    }
    free(name);
    free(words);
    free(countingAmount);
    return 0;
}
