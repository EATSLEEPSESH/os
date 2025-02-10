#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    int pipefd[2];
    pid_t cpid;
    char *argv[] = {"./child", NULL};
    char filename[256];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    printf("Введите имя файла: ");
    scanf("%s", filename);

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {    
        close(pipefd[0]); 


        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        dup2(fd, STDIN_FILENO);
        close(fd);

        execvp("./child", argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {    
        close(pipefd[1]); 

        char buffer[1024];
        ssize_t nbytes;
        while ((nbytes = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            write(STDOUT_FILENO, buffer, nbytes);
        }

        close(pipefd[0]);
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}
