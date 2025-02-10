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
    if (scanf("%s", filename) != 1) {
        fprintf(stderr, "Ошибка ввода имени файла\n");
        exit(EXIT_FAILURE);
    }

    // Создаем дочерний процесс
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {    /* Дочерний процесс */
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
    } else {    /* Родительский процесс */
        close(pipefd[1]); 

        char buffer[1024];
        ssize_t nbytes;
        while ((nbytes = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            if (write(STDOUT_FILENO, buffer, nbytes) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }

        close(pipefd[0]);
        wait(NULL); 
        exit(EXIT_SUCCESS);
    }
}
