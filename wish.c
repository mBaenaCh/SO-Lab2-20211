#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SIZE 100


char *system_commands[] = {
    "/bin/"
};

typedef enum{endup,cd, ls, wc, not_command} builtin_command;

const static struct{
    builtin_command command;
    char* string_command;
} commands[] = {
    {cd, "cd"},
    {ls, "ls"},
    {wc, "wc"},
    {endup, "exit"}
};

builtin_command str_to_command(char * str_command){
    for(int i=0; i<4; i++){
        if(!strcmp(str_command,commands[i].string_command)){
            return commands[i].command;
        }
    }
    return not_command;
}

int main(int argc, char*argv[]){
    char str[MAX_SIZE];

    do{
        printf("wish> ");
        fgets(str, MAX_SIZE, stdin);
        char * p = str;
        while(*p != '\n'){
            p++;
        }
        *p = '\0';
        builtin_command command = str_to_command(str);
        if(command != not_command){
            switch(command){
                case cd:   
                    printf("cd\n");
                    break;
                case ls:
                    printf("ls\n");
                    break;
                case wc:
                    printf("wc\n");
                    break;
                case endup:
                    exit(0);
                    break;
                default:
                    printf("command not found\n");
            }
        } else {
            /*Ejecucion de comandos que no son built in*/
        }
        
    }while(1);
}