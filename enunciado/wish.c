#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*Variables globales*/
#define MAX_SIZE 100 //Consideracion de buffer para la linea de entrada de la consola
#define TOK_DELIM " \t\r\n\a" //Delimitadores para STRTOK
#define ERROR_MSG "An error has occurred\n" //Mensaje de error global

char *paths[5] = {
    "/bin/",
    "/usr/bin/",
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

void exec_builtin_cd(char **line){
    /*Valimos que esta no tenga un argumento de comando vacio*/
    if(line[1] == NULL){
            printf(ERROR_MSG);
    /*Ejecutamos la llamada al sistema para el comando CD*/
    } else {
        if(chdir(line[1]) != 0){
                perror(ERROR_MSG);
        }
    }
}

void exec_builtin_path(char *paths[], char **line){
    /*En caso de no recibir argumentos en el comando, imprimimos los Paths*/
    if(line[1] == NULL){
        int directions = 0;
        printf("Paths:\n");
        while(paths[directions]){
            printf("- %s \n",paths[directions]);
                directions++;
        }
    /*De lo contrario, añadiremos los paths especificados*/
    } else {
        int x = 0;
        while(line[x+1] != NULL && paths[x] != NULL){
            int compare = 0;
            compare = strcmp(line[x+1], paths[x]);
            /*Si el path que queremos ingresar ya se encuentra, continuamos al siguiente*/
            if(compare == 0){
                x++;
            } 
            /*De lo contrario lo añadimos*/
            if(compare != 0 && paths[x+1]){
                paths[x+1] = line[x+1];
                paths[x+2] = NULL;
                compare = 0;
            }
            x++;
        }
    }
}

void exec_builtin_exit(char **line){
    //Se valida que el comando no reciba argumentos
    if (line[1] != NULL){
        printf(ERROR_MSG);
    } else {
        exit(0);
    }
}

void exec_remote_cmd(char *cmd[]){
    char *aux_array[30];
    int index = 0;
    int aux_index = 0;

    while(cmd[index] != NULL){
        if(!strcmp(cmd[index], "&") || cmd[index+1] == NULL){
            int pid = fork();
            //Validacion de la creacion del proceso hijo
            if(pid < 0){
                printf(ERROR_MSG);
                exit(1);
            } else if (pid == 0){
                //Ejecucion del proceso hijo
                if(cmd[index+1] == NULL){
                    aux_array[aux_index] = cmd[index];
                    aux_index++;
                }
                aux_array[aux_index] = NULL;
                execvp(aux_array[0], aux_array);
            } else {
                //Ejecucion que se da en el proceso padre cuando termina el hijo
                wait(NULL);
                aux_index = 0;
            }
        } else {
            aux_array[aux_index] = cmd[index];
            aux_index++;
        }
        index++;
    }
}

void interactive(){
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
                //Comandos built-in
                switch(command){
                    case cd:  
                        exec_builtin_cd(command_array);
                        break;
                    case path: 
                        exec_builtin_path(paths, command_array);
                        break;
                    case endup:
                        exec_builtin_exit(command_array);
                        break;
                    default:
                        printf("command not found\n");
                }
            } else {
            /*Comando externo*/
                int y=0;
                char cmd_path[MAX_SIZE];
                int return_value = -1;

                //Validacion del encuentro del ejecutable del comando a ejecutar, segun un path
                while(paths[y] != NULL && return_value == -1){
                    strcpy(cmd_path, paths[y]);
                    if(command_array[0] != NULL){
                        strcat(cmd_path, command_array[0]);
                    }
                    return_value = access(cmd_path, X_OK);
                    y++;
                }
                //Si se encuentra el archivo ejecutable del comando en algun path, se procede a ejecutar el mismo
                if(return_value != -1){
                    exec_remote_cmd(command_array);
                } else {
                    printf(ERROR_MSG);
                }
            }
    }while(1);
}

void exec_batch_mode(char *argv[]){
    FILE *file;
    char *line;
	size_t len = 0;
	ssize_t lineSize = 0;
	file = fopen(argv[1], "r");
	if (file == NULL){
		write(STDERR_FILENO, ERROR_MSG, strlen(ERROR_MSG) * sizeof(char));
		exit(1);
	}

	lineSize = getline(&line, &len, file);

    //Recorrido del archivo
	while (lineSize >= 0){
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
        if(command != not_command){
        //Comandos built-in
            switch(command){
                case cd:  
                    exec_builtin_cd(command_array);
                    break;
                case path: 
                    exec_builtin_path(paths, command_array);
                    break;
                case endup:
                    exec_builtin_exit(command_array);
                    break;
                default:
                    printf("command not found\n");
            }
        } else {
        /*Comando externo*/
            int y=0;
            char cmd_path[MAX_SIZE];
            int return_value = -1;

            while(paths[y] != NULL && return_value == -1){
                strcpy(cmd_path, paths[y]);
                if(command_array[0] != NULL){
                    strcat(cmd_path, command_array[0]);
                }
                return_value = access(cmd_path, X_OK);
                y++;
            }

            if(return_value != -1){
                exec_remote_cmd(command_array);
            } else {
                printf(ERROR_MSG);
            }
        }
		lineSize = getline(&line, &len, file);
	}
	exit(0);
}

int main(int argc, char*argv[]){
    
    //- argc = 1 -> Modo interactivo: Comando externo o built in
    if(argc == 1){
        interactive();
    //- argc = 2 -> Modo batch: Se recibe un archivo como argumento
    } else if(argc == 2){
        exec_batch_mode(argv);
    //- argc > 2 -> Error  
    } else {
        printf(ERROR_MSG);
    }

}