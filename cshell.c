
#include<stdio.h>
#include<assert.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<time.h>

typedef struct{
    char* name;
    char* value;
}EnvVar;

typedef struct{
    EnvVar** envls;
    int size;
    int position;
}EnvLs;

typedef struct {
    char *name;
    struct tm time;
    int return_value;
} Command;

typedef struct {
    /* data */
    Command** comm;
    int position;
    int size;
}Command_ls ;

//define global variables:
char* built_in_funcs[] = {"exit","log","print","theme"};
EnvLs env;
Command_ls command;
//-----------------------------------------------------------------------------
//PRE: this function is to read line of cmd from user input
char *read_cmd(){
    int size = 1024;
    char *cmd;
    //first give allocation to char* cmd
    cmd = (char*)malloc(sizeof(char) * size);//need free
    //now get input from user
    int c;
    printf("cshell> ");
    c = getchar();
    int i;
    for(i=0; (c != EOF && c != '\n'); i++){
        //check if user input has exceeded string length
        if(i >= size){
            size *= 2;
            cmd = realloc(cmd, sizeof(char) * size);
        }
        cmd[i] = c;
        c = getchar();
    }
    //reading complete
    //now add '\0' to the end of cmd
    //check if i reaches size
    if(i >= size){
        size += 1;
        cmd = realloc(cmd,size);
    }
    cmd[i] = '\0';
    //debug message:
    //printf("%s\n", cmd);
    //return cmd
    return cmd;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------
//PRE: this fuction is to split cmd in to tokens
char** parse_cmd(char* cmd){
    const char* delim = " \t";
    int size = 64;
    char ** tokens = (char**)malloc(sizeof(char*) * size);//need free
    //debug message:
    char* token = NULL;
    token = strtok(cmd, delim);
    int i;
    for(i=0; token != NULL; i++){
        if(i >= size){
            size *= 2;
            tokens = realloc(tokens, sizeof(char*) * size);
        }
        tokens[i] = token;
        token = strtok(NULL, delim);
    }
    if(i>=size){
        size += 1;
        tokens = realloc(tokens, sizeof(char*) * size);
    }
    tokens[i] = NULL;
    return tokens;

}
//--------------------------------------------------------------------------------------
//PRE: this func is to search from envvar with given name 
EnvVar* search_env(char* name){
    //check if name is null
    if(name == NULL){
        //print error message of no input
        printf("null input for search_env\n");
        return NULL;
    }
    //check if there is element in env
    if(env.position == 0){
        return NULL;
    }
    for(int i=0; i<env.position; i++){
        if(strcmp(env.envls[i]->name,name) == 0){
            return env.envls[i];
        }
    }
    return NULL;
}
//POST: this func return pointer to ENVVAR that has given name, not exist if return null
//---------------------------------------------------------------------------------------
//PRE: this func is to create a new ENVVAR and assign name & value with input 
EnvVar* exec_ENVVAR(char** arg){
    char* argv = arg[0];
    //printf("%s\n", arg[0]);
    int length = strlen(argv);
    //printf("debug message: length is: %d\n", length);
    EnvVar* env_var = (EnvVar*)malloc(sizeof(EnvVar));//need double free
    
    if(!env_var){
        printf("creation error\n");
        return NULL;
    }
    env_var->name = (char*)malloc(sizeof(char) * (length));
    
    env_var->value = (char*)malloc(sizeof(char) * (length));
    
    int i=0;
    while(argv[i] != '='){
        //printf("debug message: %s", argv);
        if(i == length){
            //no equal sign in the string, syntax error:
            free(env_var->name);
            free(env_var->value);
            free(env_var);
            return NULL;
        }
        env_var->name[i] = argv[i];
        //printf("debug message: in loop");
        i++;
    }
    env_var->name[i] = '\0';
    i++;
    int j=0; // j refers to position of env.value[]
    while(i < length){
        env_var->value[j] = argv[i];
        i++;
        j++;
    }
    //j=0 refers to no increment on env_var->value
    if(j == 0){
        //env->value has no value:
        free(env_var->value);
        env_var->value = NULL;
    }
    else{
        env_var->value[j] = '\0';
    }
    return env_var;
}//POST: if it returns NULL, then no equal sign is found=>syntax error
//if it returns a pointer with its value being NULL, then no value is found
//---------------------------------------------------------------------------------------------------
//PRE: this func is to exit 
int exec_exit(){
    //print exit message:
    printf("Bye!\n"); 
    return 0;
}
//----------------------------------------------------------------------------------------------
//PRE: this func is to display history cmds, print each command in command->comm
int exec_log(){
    //check if command is empty
    if(command.position == 0){
        //print message of empty command
        printf("no command was executed\n");
        return 1;
    }
    for(int i=0; i<command.position; i++){
        //print time first:
        printf("%s\n",asctime(&(command.comm[i]->time)));
        //then print command
        printf("%s ", command.comm[i]->name); 
        //then print return value
        printf("%d\n", command.comm[i]->return_value);
    }
        
    return 1;
}
//--------------------------------------------------------------------------------------------------
//PRE: this func is to print content or envvar
int exec_print(char** argv){
    //check if it's NUll
    if(!argv){
        //print error message and return
        printf("no printing value");
        return 1;
    }
    //check is argv is a envar
    if(argv[0][0] == '$'){
        //search for name in envvar
        //need to implement a envVar_search func
        EnvVar* search_res;
        search_res = search_env(argv[0]);
            if(search_res == NULL){
                //this means there is no envvar matches, print error message
                printf("no such variable exist\n");
            }
            else{
                //search_res is not null, print its value
                printf("%s\n", search_res->value);
                
            }
    }
    else{
        for(int i=0; argv[i] != NULL; i++){
            printf("%s ", argv[i]);
        }
        printf("\n");
        return 1;
    }
}
//---------------------------------------------------------------------------------------------------
//PRE: this func is to change the theme
int exec_theme(char* color){
    if(strcmp(color,"red") == 0){
        printf("\033[0;31m");
    }
    else if(strcmp(color,"green")==0){
        printf("\033[0;32m");
    }
    else if(strcmp(color,"blue")==0){
        printf("\033[0;34m");
    }
    else if(strcmp(color,"reset")==0){
        printf("\033[0m");
    }
    else{
        printf("invalid color\n");
    }
    return 1;
}
//--------------------------------------------------------------------------------------------------
int exe_builtin(char** argv){
    //printf("in builtin part\n");
    if(strcmp(argv[0], built_in_funcs[0]) == 0){
        //go to exit
        //printf("in exit part\n");
        int res =  exec_exit(); 
        //printf("%d\n",res);
        return res;
    }
    else if(strcmp(argv[0], built_in_funcs[1]) == 0){
        //go to log
        int res =  exec_log();
        return res;
    }
    else if(strcmp(argv[0], built_in_funcs[2]) == 0){
        //go to print
        int res = exec_print(&argv[1]);
        return res;
    }
    else if(strcmp(argv[0], built_in_funcs[3]) == 0){
        //go to theme
        int res = exec_theme(argv[1]);
        return res;
    }
}
//------------------------------------------------------------------------------------------------------
int exec_nonbuiltin(char** argv){
    pid_t pid;
    pid = fork();
    if(pid == 0){
        //going to child process
        if(execvp(argv[0], argv) == -1){
            //print error message for executing error
            printf("invalid command\n");
            exit(1);
        }
    }
    else if(pid < 0){
        //print error message of creating fork error
        printf("error creating fork\n");
        return 1;
    }
    else{
        //we are in parent process right now
        wait(NULL);
        return 1;
    }
}
//--------------------------------------------------------------------------------------------
//PRE: this function is the main execution function.
int exe_cmd( char** arg){
    //printf("%s\n", arg[0]);
    //variable decalartion:
    EnvVar* temp, *search_res;
    //check if arg is null
    if(arg[0] == NULL){
        return 1;
    }
    //--------------form of $VAR=VAL:--------------
    if(arg[0][0] == '$'){
        //printf("in envvar part\n");
        //check if env is full
        //printf("%s", arg[0]);
        if(env.position >= env.size){
            env.size *= 2;
            env.envls = realloc(env.envls, sizeof(EnvVar*) * env.size);
        }
        //printf("reaching here\n");
        temp = exec_ENVVAR(arg);
        //check env:
        if(!temp){
            //print error message for no equal sign
            //free memory:
            free(temp);
        }
        //check if env->value is null
        else if(!temp->value){
            //print error message for no values.
            //free space and not increase position
            free(temp->name);
            free(temp);
        }
        else{
            //check if temp->name exist
            search_res = search_env(temp->name);
            if(search_res == NULL){
                //this means there is no envvar matches, increment occur
                env.envls[env.position] = temp;
                env.position++;
            }
            else{
                //search_res is not null, modify its value by new value
                char* switch_sup = search_res->value;
                search_res->value = temp->value;
                temp->value = switch_sup;
                //free temp
                free(temp->name);
                free(temp->value);
                free(temp);
            }
        }
        printf("Environment Variable Adding Complete\n");
        return 1;
    }
    else{
    //-----------------------------------------
    
    //-----------end reading env var----------------
    //-----------executing executable program----------
    //first part of executing built in program
        for(int i=0; i<4; i++){
           
            if(strcmp(arg[0], built_in_funcs[i]) == 0){
                int res = exe_builtin(arg);
                return res;
            }
        }
    //then go to part of executing non-built-in funcs
        return exec_nonbuiltin(arg);
    }
}
//-----------------------------------------------------------------------------------------------------------
Command* generate_cmd(char* arg, int ret){
    //initialize variables
    int len = strlen(arg);
    Command* comd = (Command*)malloc(sizeof(Command));
    comd->name = (char*)malloc(sizeof(char) * (len+1));
    
    //set time element
    time_t ti = time(NULL);
    comd->time = *(localtime(&ti));
    //set name element
    strcpy(comd->name, arg);
    //set return element
    comd->return_value = ret;

    return comd;
}
//PRE: this func is to implement cmd with argv[0], time & return value
void implement_cmd(char** argv, int ret_value){
    //check argv[0] is null
    int posit = command.position;
    if(argv[0] == NULL){
        return;
    }
    
    command.comm[posit] = generate_cmd(argv[0],ret_value);
    
    command.position++;
    if(command.position >= command.size){
        command.size *= 2;
        command.comm = realloc(command.comm, sizeof(Command*) * command.size);
    }
}
//-----------------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    //variable declaration:
    char* cmd;
    char** tokens;
    char* file_name;
    int status = 1;
    env.envls = (EnvVar**)malloc(sizeof(EnvVar*));//need free
    env.envls[0] = NULL;
    env.size = 1;
    env.position = 0;  
    command.size = 1;
    command.position = 0;
    command.comm = (Command**)malloc(sizeof(Command*));
    int interac_mode = 1;
    FILE *f;
    //printf("reaching here\n");
    //printf("reach here\n");
    //main loop of cshell------------------
    while (status)
    {
        
        if(interac_mode){
            cmd = read_cmd();
            //debug part for read_cmd()
            //printf("%s\n", cmd);
            //free(cmd);
        }
        else{
            // script mode
            int size = 1024;
            //first give allocation to char* cmd
            cmd = (char*)malloc(sizeof(char) * size);//need free
            //now get input from user
            int c;
            c = fgetc(f);
            int i;
            for(i=0; (c != EOF && c != '\n'); i++){
                //check if user input has exceeded string length
                if(i >= size){
                size *= 2;
                cmd = realloc(cmd, sizeof(char) * size);
                }
                cmd[i] = c;
                c = fgetc(f);
            }
                //reading complete
                //now add '\0' to the end of cmd
                //check if i reaches size
            if(i >= size){
                size += 1;
                cmd = realloc(cmd,sizeof(char)*size);
            }
            cmd[i] = '\0';
            if(c == EOF){
                //We have reached the end of file
                interac_mode = 1;
                fclose(f);
                free(file_name);
            }
        }
//--------------------------------------      
        tokens = parse_cmd(cmd);
        
        char compstr[] = "./cshell";
        if(interac_mode == 1 && strcmp(tokens[0], compstr) == 0){
            interac_mode = 0;
            //free(cmd);
            printf("%s\n", tokens[0]);
            printf("%s\n", tokens[1]);
            int len = strlen(tokens[1]);
            file_name = (char*)malloc(sizeof(char) * (len+1));
            strcpy(file_name, tokens[1]);
            
            free(tokens);
            free(cmd);
            f = fopen(file_name,"r");
            continue;
        }
       //debug part for parse_cmd
        // for(int i=0; tokens[i] != NULL; i++){
        //    printf("in index %d: %s\n" ,i,tokens[0]);
        // }
    //     free(tokens);
    //     free(cmd);
//-------------------------------------------      
        //printf("%s\n" ,tokens[0]);
        status = exe_cmd(tokens);
        //printf("end execution\n");
       //increment of command
        implement_cmd(tokens, status);
    //free cmd & tokens
        free(tokens);
        free(cmd);
    }
   //dynamic varable free
   //free ENVVAR:
    for(int i=0; i<env.position; i++){
       free(env.envls[i]->name);
       free(env.envls[i]->value);
       free(env.envls[i]);
    }
    free(env.envls);
   //free CMD
   for(int i=0; i<command.position; i++){
       free(command.comm[i]->name);
       free(command.comm[i]);
   }
   free(command.comm);
}
