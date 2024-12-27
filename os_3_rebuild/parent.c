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

    // Создаем pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Читаем имя файла от пользователя
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
        close(pipefd[0]); // Закрываем чтение из pipe

        // Перенаправляем стандартный вывод в pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Открываем файл для чтения
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // Перенаправляем стандартный ввод на файл
        dup2(fd, STDIN_FILENO);
        close(fd);

        // Запускаем дочерний процесс
        execvp("./child", argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {    /* Родительский процесс */
        close(pipefd[1]); // Закрываем запись в pipe

        // Читаем вывод дочернего процесса из pipe
        char buffer[1024];
        ssize_t nbytes;
        while ((nbytes = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            if (write(STDOUT_FILENO, buffer, nbytes) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }

        close(pipefd[0]);
        wait(NULL); // Ждем завершения дочернего процесса
        exit(EXIT_SUCCESS);
    }
}
