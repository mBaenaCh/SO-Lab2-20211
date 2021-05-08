#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*Variables globales*/
#define MAX_SIZE 100 //Consideracion de buffer para la linea de entrada de la consola
#define TOK_DELIM " \t\r\n\a" //Delimitadores para STRTOK
#define ERROR_MSG "An error has occurred\n" //Mensaje de error global

char *system_commands[5] = {
    "/bin/",
    "/usr/bin/",
    NULL,
    NULL,
    NULL
};

typedef enum{cd, path, endup, not_command} builtin_command;

const static struct{
    builtin_command command;
    char* string_command;
} commands[] = {
    {cd, "cd"},
    {path, "path"},
    {endup, "exit"}
};

builtin_command str_to_command(char * str_command){
    for(int i=0; i<3; i++){
        int comp = strcmp(str_command,commands[i].string_command);
        if(!strcmp(str_command,commands[i].string_command)){
            return commands[i].command;
        }
    }
    return not_command;
}

int main(int argc, char*argv[]){
    
    /*Validar en funcion de argumentos ingresados
    - argc = 0 -> Modo interactivo: Comando externo o built in
    - argc = 1 -> Modo batch: Se recibe un archivo como argumento
    - argc > 1 -> Error
    */    
    
    /*Modo interactivo*/
    char line[MAX_SIZE];
    
    do{
        printf("wish> ");

        /*Obtencion de la linea de texto recibida en consola*/
        fgets(line, MAX_SIZE, stdin);
        char * p = line;
        while(*p != '\n'){
            p++;
        }
        *p = '\0';

        /*Separacion de la linea entre comandos y argumentos*/
        char * command_array[sizeof(line)];
        char * token;
        int position = 0;
        token = strtok(line, TOK_DELIM);
        while (token != NULL){
            command_array[position] = token;
            position++;
            token = strtok(NULL, TOK_DELIM);
        }
        command_array[position] = NULL;
        
        /*Comparacion linea de comando con comandos built-in*/
        builtin_command command = str_to_command(command_array[0]);
        /*Filtrado segun comando ingresado*/
        if(command != not_command){
            //Comando built-in
            switch(command){
                case cd:  
                    if(command_array[1] == NULL){
                        printf(ERROR_MSG);
                    } else {
                        if(chdir(command_array[1]) != 0){
                            perror(ERROR_MSG);
                        }
                    }
                    break;
                case path: 
                    if(command_array[1] == NULL){
                        int directions = 0;
                        printf("Paths:\n");
                        while(system_commands[directions]){
                            printf("- %s \n",system_commands[directions]);
                            directions++;
                        }
                    } else {
                        int x = 0;
                        while(command_array[x+1] != NULL && system_commands[x] != NULL){
                            int compare = 0;
                            compare = strcmp(command_array[x+1], system_commands[x]);

                            if(compare == 0){
                                x++;
                            } 

                            if(compare != 0 && system_commands[x+1]){
                                system_commands[x+1] = command_array[x+1];
                                system_commands[x+2] = NULL;
                                x++;
                                compare = 0;
                            }
                        }
                    }
                    break;
                case endup:
                    if (command_array[1] != NULL){
                        printf(ERROR_MSG);
                    } else {
                        exit(0);
                    }
                    break;
                default:
                    printf("command not found\n");
            }
        } else {
            /*Comando externo*/
            printf("El comando ingresado no es built-in\n");

        }
    }while(1);
}