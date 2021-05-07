#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*Variables globales*/
#define MAX_SIZE 100 //Consideracion de buffer para la linea de entrada de la consola
#define TOK_DELIM " \t\r\n\a" //Delimitadores para STRTOK
#define ERROR_MSG "An error has occurred\n" //Mensaje de error global

char *system_commands[] = {
    "/bin/",
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
                    printf("Recibo el comando %s y el argumento %s \n", command_array[0], command_array[1]);
                    break;
                case path:
                    printf("path\n");
                    break;
                case endup:
                    exit(0);
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