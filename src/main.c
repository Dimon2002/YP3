#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define BUF_SIZE 256

pid_t pid1, pid2;

int pipe1[2], pipe2[2];

void sig_handler1(int sig) 
{
    printf("P2: received signal from P1\n");
}

void sig_handler2(int sig) 
{
    printf("P1: received signal from P2\n");
}

int main()
{


    if (pipe(pipe1) < 0 || pipe(pipe2) < 0)
    {
        perror("pipe error");
        exit(1);
    }

    printf("Parent process %d created pipe\n", getpid());

    pid1 = fork();
    if (pid1 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    } 
    else if (pid1 == 0)
    {
        printf("P1: was created with id - %d\n", getpid());
        // Child process P1   
        pid2 = fork();
        if (pid2 == -1) 
        {
            perror("fork");
            exit(EXIT_FAILURE);
        } 

        if (pid2 != 0)
        {   
            close(pipe1[0]); // Close read end of the pipe
            char buf[BUF_SIZE];
            sprintf(buf, "Data from P1");
            printf("P1: preparing data\n");
            write(pipe1[1], buf, sizeof(buf)); // Write data to the pipe
            printf("P1: sending data to Pipe1\n");
            //close(pipe1[1]); // Close write end of the pipe
            sleep(1);
            printf("Status sending 1: %d\n", kill(pid2, SIGUSR1));
            printf("P1: sending signal to P2\n");
            printf("P1: waiting for signal from P2\n");
            signal(SIGCONT, sig_handler2);
            pause();
            //sleep(2);
            read(pipe2[0], buf, sizeof(buf));
            printf("P1: reading data from Pipe2\n");
            printf("P1: data from pipe2 - %s\n", buf);
            write(pipe1[1], buf, sizeof(buf));
            printf("P1: sending data to Pipe1\n");
            waitpid(pid2, NULL, 0);
            exit(EXIT_FAILURE);
        }
        else if (pid2 == 0) 
        {
            // Child process P2
            printf("P2: was created with id - %d\n", getpid());
            printf("P2: waiting for signal from P1\n");
            signal(SIGUSR1, sig_handler1);
            pause();
            close(pipe2[0]); // Close read end of the pipe
            char buf[BUF_SIZE];
            sprintf(buf, "Data from P2");
            printf("P2: preparing data\n");
            write(pipe2[1], buf, sizeof(buf)); // Write data to the pipe
            printf("P2: sending data to Pipe2\n");
            close(pipe2[1]); // Close write end of the pipe

            printf("Status sending 2: %d\n", kill(pid2, SIGCONT));
            
            printf("P2: sending signal to P1\n");
            exit(EXIT_SUCCESS);
        }
    } 
    else
    {
        // Parent process
        close(pipe1[1]); // Close write end of the pipe
        char buf[BUF_SIZE];
        int bytes_read;
        waitpid(pid1, NULL, 0); // Wait for P1 to finish
        
        while ((bytes_read = read(pipe1[0], buf, sizeof(buf))) > 0) {
            printf("Parent: received data from pipe: %s\n", buf);
        }

        close(pipe1[0]); // Close read end of the pipe

        printf("Parent: all child processes finished\n");
        exit(EXIT_SUCCESS);
    }
    
}