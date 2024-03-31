#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

int successExecution = 0;

void executeExternalCommand(char *actualCommandInput, char *fileName, int isPath, int isParallel)
{
    /* method to execute external commands */
    char *cmdArgs[1000];
    char *defaultPath1 = "/bin";
    /* tokenizing on space or new line */
    char *argToken = strtok(actualCommandInput, " \n");
    int i = 0;
    while (argToken != NULL)
    {
        cmdArgs[i] = argToken;
        i++;
        argToken = strtok(NULL, " \n");
    }
    cmdArgs[i] = NULL;
    char finalPath[2000] = {};
    if (strcmp(actualCommandInput, "myls\n") != 0 && strcmp(actualCommandInput, "myls") != 0 && strcmp(actualCommandInput, " myls") != 0)
    {
        if (strstr(finalPath, "/bin") == NULL)
        {
            strcpy(finalPath, defaultPath1);
        }
        if (cmdArgs[0][0] != '/')
        {
            strcat(finalPath, "/");
        }
    }
    strcat(finalPath, cmdArgs[0]);
    if (strcmp(cmdArgs[0], "cat") == 0)
    {
        if (strstr(cmdArgs[1], ".") == NULL)
            strcat(cmdArgs[1], ".txt");
    }
    if (finalPath[strlen(finalPath) - 1] == '\n')
    {
        finalPath[strlen(finalPath) - 1] = '\0';
    }
    if (access(finalPath, X_OK) == 0)
    {
        if (fileName == NULL)
        {
            if (strcmp(cmdArgs[i - 1], "\n") == 0)
            {
                cmdArgs[i - 1] = NULL;
            }
            if (execv(finalPath, cmdArgs) == -1)
            {
                successExecution = 0;
                exit(0);
            }
        }
        else
        {
            /* Redirecting the output to the given file */
            if (fileName[strlen(fileName) - 1] == '\n')
            {
                fileName[strlen(fileName) - 1] = '\0';
            }
            if (fileName[0] == ' ')
            {
                int x = 0;
                for (x = 0; x < strlen(fileName) - 1; x++)
                {
                    fileName[x] = fileName[x + 1];
                }
                fileName[strlen(fileName) - 1] = '\0';
            }
            /* If the user doesn't mention any file format */
            if (strstr(fileName, ".txt") == NULL)
            {
                strcat(fileName, ".txt");
            }

            int fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            /* Redirecting the output to the given file */
            dup2(fd, 1);
            /* Redirecting the error to the given file */
            dup2(fd, 2);
            close(fd);
            /* Executing the command */
            execv(finalPath, cmdArgs);
            successExecution = 0;

            perror("execv");
        }
    }
    else
    {
        successExecution = 0;
        if (isPath != 1 && isParallel == 1)
        {
            if (strcmp(finalPath, "myls") == 0 || strcmp(finalPath, "myhello") == 0)
            {
                successExecution = 1;
                size_t totalSize = strlen(actualCommandInput) + strlen("../");
                char *finalPath2 = (char *)malloc(totalSize + 1);
                finalPath2[0] = '.';
                finalPath2[1] = '.';
                finalPath2[2] = '/';
                strcat(finalPath2, finalPath);
                if (execv(finalPath2, cmdArgs) == -1)
                {
                    successExecution = 0;
                }
            }
        }
        else
        {
            printf("An error has occurred\n");
        }
        exit(0);
    }
}

void executeCommand(char *actualCommandInput, char *fileName, int isParallel)
{
    if (actualCommandInput[0] == 'c' && actualCommandInput[1] == 'd')
    {
        /* Executing built in command cd */
        char *cmdArgs[1000];
        /* Tokenizing the command by space or new line */
        char *argToken = strtok(actualCommandInput, " \n");
        int i = 0;
        while (argToken != NULL)
        {
            cmdArgs[i] = argToken;
            i++;
            argToken = strtok(NULL, " ");
        }
        cmdArgs[i] = NULL;
        int sizeOfCmdArgs = i;
        if (sizeOfCmdArgs == 1)
        {
            /* If no directory is specified, we throw an error message */
            printf("An error has occurred\n");
        }
        else if (sizeOfCmdArgs > 2)
        {
            /* cd cannot have more than 2 arguments */
            printf("An error has occurred\n");
        }
        else
        {
            /* executing cd */
            char *commandDirectory = cmdArgs[1];
            if (commandDirectory != NULL)
            {
                if (commandDirectory[strlen(commandDirectory) - 1] == '\n')
                {
                    commandDirectory[strlen(commandDirectory) - 1] = '\0';
                }
                int resultDir = chdir(commandDirectory);
                if (resultDir != 0)
                {
                    perror("chdir");
                }
            }
            else
            {
                printf("An error has occurred\n");
            }
        }
    }
    else if (strcmp(actualCommandInput, "exit") == 0 || strcmp(actualCommandInput, "exit\n") == 0)
    {
        /* exiting if input command is exit */
        exit(0);
    }
    else if (actualCommandInput[0] == 'p' && actualCommandInput[1] == 'a' && actualCommandInput[2] == 't' && actualCommandInput[3] == 'h')
    {

        char *cmdArgs[100];
        /* tokenizing the command by space or new line */
        char *argToken = strtok(actualCommandInput, " \n");
        int i = 0;
        while (argToken != NULL)
        {
            cmdArgs[i] = argToken;
            i++;
            argToken = strtok(NULL, " \n");
        }
        cmdArgs[i] = NULL;
        if (i <= 1)
        {
            /* If no argument is given, we throw an error*/
            printf("An error has occured\n");
            exit(0);
        }
        else
        {
            int j = 1;
            for (j = 1; cmdArgs[j] != NULL; j++)
            {
                /* parsing each argument to execute */
                char *commandDirectory = cmdArgs[j];
                if (commandDirectory != NULL)
                {
                    if (commandDirectory[strlen(commandDirectory) - 1] == '\n')
                    {
                        commandDirectory[strlen(commandDirectory) - 1] = '\0';
                    }
                    int resultDir = chdir(commandDirectory);
                    if (resultDir == 0)
                    break;
                }
            }
        }
    }
    else
    {
        /* Creating a child process for external command*/
        int y = fork();
        if (y == 0)
        {
            executeExternalCommand(actualCommandInput, fileName, 0, isParallel);
        }
        else
        {
            /* Parent process waits for child to complete*/
            wait(NULL);
        }
    }
}

bool checkForSymbol(char *command, char symbol)
{
    /* Common method to check for symbols like '&' or '>' */
    int siz = strlen(command);
    int i = 0;
    for (i = 0; i < siz; i++)
    {
        if (command[i] == symbol)
        {
            return true;
        }
    }
    return false;
}

void handleParalellAndRedirection(char *actualCommandInput)
{
    int isParallel = checkForSymbol(actualCommandInput, '&');
    int isRedirected = checkForSymbol(actualCommandInput, '>');
    char *parallelCommand[1000];
    /* Tokenizing the given input by '&' token */
    char *parallelToken = strtok(actualCommandInput, "&");
    int i = 0;
    while (parallelToken != NULL)
    {
        parallelCommand[i] = parallelToken;
        i++;
        parallelToken = strtok(NULL, "&");
    }
    parallelCommand[i] = NULL;
    int index = 0;
    if (!isParallel)
    {
        char *redirectionCommand[1000];
        if (!isRedirected)
        {
            /* If there are no parallel commands or redirection */
            executeCommand(actualCommandInput, NULL, 0);
        }
        else
        {
            /* Tokenizing the input by '>' character */
            char *redirectionToken = strtok(actualCommandInput, ">");
            int k = 0;
            while (redirectionToken != NULL)
            {
                redirectionCommand[k] = redirectionToken;
                k++;
                redirectionToken = strtok(NULL, ">");
            }
            redirectionCommand[k] = NULL;
            if (k != 2)
            {
                /* Number of arguments in tokenization need to be 2*/
                printf("An error has occurred\n");
            }
            else
            {
                if (k == 2 && (strcmp(redirectionCommand[1], "\n") == 0))
                {
                    /* If no filename is specified for tokenization */
                    printf("An error has occurred\n");
                }
                else if (k == 2)
                {
                    if (checkForSymbol(redirectionCommand[1], ' '))
                    {
                        /* If 2 filenames are given to redirect output*/
                        printf("An error has occurred\n");
                    }
                    else
                    {
                        /* Executing tokenization */
                        executeCommand(redirectionCommand[0], redirectionCommand[1], 0);
                    }
                }
            }
        }
    }
    else
    {
        /* If parallel */
        char *redirectionCommand[1000];
        if (i == 0 || (i == 1 && (strcmp(parallelCommand[0], "\n") == 0)))
        {
            /* Incase of no arguments*/
            printf("An error has occurred\n");
        }
        else
        {
            for (index = 0; index < i; index++)
            {
                if (isRedirected == 0)
                {
                    executeCommand(parallelCommand[index], NULL, 1);
                }
                else
                {
                    /* Tokenizing for '>' character */
                    char *redirectionToken = strtok(actualCommandInput, ">");
                    int k = 0;
                    while (redirectionToken != NULL)
                    {
                        redirectionCommand[k] = redirectionToken;
                        k++;
                        redirectionToken = strtok(NULL, ">");
                    }
                    redirectionCommand[k] = NULL;
                    if (k != 2)
                    {
                        /* exactly two arguments are required */
                        printf("An error has occurred\n");
                    }
                    else
                    {
                        if (k == 2 && (strcmp(redirectionCommand[1], "\n") == 0))
                        {
                            printf("An error has occurred\n");
                        }
                        else if (k == 2)
                        {
                            if (checkForSymbol(redirectionCommand[1], ' '))
                            {
                                printf("An error has occurred\n");
                            }
                            else
                            {
                                executeCommand(redirectionCommand[0], redirectionCommand[1], 1);
                            }
                        }
                    }
                }
            }
        }
    }
}

char *removeSpaces(char *str)
{
    /* method to remove spaces at the edges of the string */
    int length = strlen(str);
    char *temporaryString = (char *)malloc(length + 1);
    int start = 0, end = length - 1;
    while (str[start] == ' ' || str[start] == '\t')
    {
        start++;
    }
    end = length - 1;
    while (str[end] == ' ' || str[end] == '\t')
    {
        end--;
    }
    int k = 0;
    int i = 0;
    for (k = start; k <= end; k++)
    {
        temporaryString[i] = str[k];
        i++;
    }
    temporaryString[i] = '\0';
    return temporaryString;
}

void interactive_mode()
{
    while (1)
    {
        printf("dash> ");
        char *commandInput = NULL;
        size_t commandInputSize = 0;
        /*getline to take variable length input*/
        getline(&commandInput, &commandInputSize, stdin);
        /*Removing spaces from the edges of the input*/
        char *actualCommandInput = removeSpaces(commandInput);

        handleParalellAndRedirection(actualCommandInput);
    }
}

void bash_mode(const char *batch_file_name)
{
    FILE *batch_file = fopen(batch_file_name, "r");
    if (batch_file == NULL)
    {
        /* If no batch file is specified, throw an error */
        printf("An error has occurred\n");
        fflush(stdout);
        exit(0);
    }
    else
    {
        char *commandInput = NULL;
        size_t commandInputSize = 0;

        int flag = 0;
        while (getline(&commandInput, &commandInputSize, batch_file) != -1)
        {
            /* traversing each line at a time*/
            flag = 1;
            char *actualCommandInput = removeSpaces(commandInput);

            if (strcmp(actualCommandInput, "\n") != 0)
            {
                handleParalellAndRedirection(actualCommandInput);
            }
        }
        if (flag == 0)
        {
            /* When its not a readable file */
            printf("An error has occurred\n");
        }
    }
}
int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        /* interactive mode if no file is given*/
        interactive_mode();
    }
    else if (argc == 2)
    {
        /* bash mode if file is given as an argument*/
        bash_mode(argv[1]);
    }
    else
    {
        /*if multiple files are given as input*/
        printf("An error has occured\n");
    }
    return 0;
}

