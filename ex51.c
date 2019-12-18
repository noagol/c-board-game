#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <string.h>

#define ERROR_SYSTEM_CALL "Error in system call"
#define EXECUTABLE_NAME "./draw.out"

#define QUIT_GAME 'q'
#define MOVE_LEFT 'a'
#define MOVE_RIGHT 'd'
#define MOVE_DOWN 's'
#define TRANSPOSE 'w'

// deceleration
char getch();

void errorSysCall();

int isChar(char input);

int execute();

/**
 * Main function
 * @return
 */
int main() {
    return execute();
}

/**
 * Run program
 * @return status to main
 */
int execute() {

    pid_t pid;
    // fd[0] - read from pipe fd[1] - write to pipe
    int fd[2];
    pipe(fd);
    // Fork a child process
    pid = fork();

    if (pid < 0) {
        // Error
        errorSysCall();
        return -1;
    } else if (pid == 0) {
        // Child process

        // close stream to write
        close(fd[1]);

        // Set stdin to fd
        dup2(fd[0], STDIN_FILENO);

        // Run executable
        char *run[sizeof(EXECUTABLE_NAME)] = {EXECUTABLE_NAME};

        if (execv(run[0], run) < 0) {
            // close stream to read
            close(fd[0]);
            errorSysCall();
            return -1;
        }

        return -1;

    } else {
        // Father process

        // close stream to read
        close(fd[0]);
        char input;

        do {
            // read user input char
            input = getch();
            // check user input
            if (isChar(input)) {
                // try to write the char to pipe
                if (write(fd[1], &input, sizeof(input)) < 0) {
                    errorSysCall();
                }
                // send signal to child process
                kill(pid, SIGUSR2);
            }
        } while (input != 'q');

        // close stream to write
        close(fd[1]);
        return 0;
    }
}

/**
 * Prints error when system call happen.
 */
void errorSysCall() {
    //system call - error
    write(2, ERROR_SYSTEM_CALL, 21);
}

/**
 * Check if the user input is valid
 * @param input the user input
 * @return true if the char is valid
 */
int isChar(char input) {
    return (input == MOVE_LEFT || input == MOVE_DOWN || input == MOVE_RIGHT
            || input == TRANSPOSE || input == QUIT_GAME);
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}