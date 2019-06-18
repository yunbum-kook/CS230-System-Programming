#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>

/*
    Print all files and folders in the current working directory,
*/
void ls(){
    DIR *curr_dir;
    curr_dir = opendir(".");
    int count = 0;
    
    struct dirent *entry;
    if (curr_dir != NULL){
        while((entry = readdir(curr_dir))){
            if (!strcmp(entry->d_name, ".")){
                continue;
            }else if (!strcmp(entry->d_name, "..")){
                continue;
            }else{
                if (count == 0){
                    printf("%s", entry->d_name);
                    count += 1;
                }else{
                    printf(" %s", entry->d_name);
                }
            }
        }
        printf("\n");
    }   
}


/*
    Make a new directory called 'name'.
*/
void makedir(char* name){
	mkdir(name, 0777);
}


/*
    Remove a directory called 'name'.
*/
void removedir(char* name){
	rmdir(name);
}


/*
    Change current working directory to directory 'name'.
*/
void changedir(char* name){ 
	chdir(name);
}


/*
    Get 'command' as an input,
    and execute proper functionalities according to command line.
*/
void shell (char* command){
    char* cp = (char *) malloc(sizeof(char) * strlen(command));
    strcpy(cp, command);
    cp[strlen(cp)-1] = '\0';
    
    char* split;
    split = strtok(cp, " ");
    if (!strcmp(split, "ls")){
        ls();
    }else if (!strcmp(split, "mkdir")){
        split = strtok(NULL, " ");
        makedir(split);
    }else if (!strcmp(split, "rmdir")){
        split = strtok(NULL, " ");
        removedir(split);
    }else if (!strcmp(split, "cd")){
        split = strtok(NULL, " ");
        changedir(split);
    }else{
        printf("Invalid Command\n");
    }
    free(cp);
}