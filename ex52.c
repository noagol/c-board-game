
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>


#define BOARD_GAME_SIZE 20
#define STAR '*'
#define SPACE ' '
#define NEW_LINE "\n"
#define SHAPE_CHAR '-'
#define QUIT_GAME 'q'
#define MOVE_LEFT 'a'
#define MOVE_RIGHT 'd'
#define MOVE_DOWN 's'
#define TRANSPOSE 'w'

/**
 * Structure for the game board.
 * Save board values.
 */
typedef struct Board {
    char frame[BOARD_GAME_SIZE][BOARD_GAME_SIZE];
} Board;

/**
 * Structure for the shape ---.
 */
typedef struct Shape {
    // shape position
    int xPos;
    int yPos;
    // vertical / horizontal
    int isVertical;

    Board *board;

    // add shape to the game
    void (*addShapeToGame)(struct Shape *);

    // movement
    void (*moveRight)(struct Shape *);

    void (*moveLeft)(struct Shape *);

    void (*moveDown)(struct Shape *);

    // change position
    void (*changeOrientation)(struct Shape *);
} Shape;

// init the board and shape 
Board board;
Shape shape;
int flag = 0;

// deceleration
void run();

void createBord(Board *board);

void showBoard(Board *board);

void initializeSpace(Board *board);

void moveRight(Shape *object);

void moveLeft(Shape *object);

void moveDown(Shape *object);

void signalHandle(int signo);

void changeOrientation(Shape *object);

void alarmHandle(int signo);

void clearVertical(Shape *object);

void clearHorizontal(Shape *object);

/**
 * Main function
 * @return
 */
int main() {
    run();
    return 0;
}

/**
 * Remove vertical shape from board
 * @param object the shape object
 */
void clearVertical(Shape *object) {
    // clear the shape - put spaces
    object->board->frame[object->yPos - 1][object->xPos] = SPACE;
    object->board->frame[object->yPos][object->xPos] = SPACE;
    object->board->frame[object->yPos + 1][object->xPos] = SPACE;
}

/**
 * Remove horizontal shape from board
 * @param object the shape object
 */
void clearHorizontal(Shape *object) {
    // clear the shape - put spaces
    object->board->frame[object->yPos][object->xPos] = SPACE;
    object->board->frame[object->yPos][object->xPos + 1] = SPACE;
    object->board->frame[object->yPos][object->xPos - 1] = SPACE;
}


/**
 * Add new shape to the board game ---
 * @param object the shape object
 */
void addShapeToGame(Shape *object) {
    // find middle point
    object->xPos = ((BOARD_GAME_SIZE - 2) / 2);
    // set y to zero and horizontal shape.
    object->yPos = 0;
    object->isVertical = 0;

    // draw the shape
    object->board->frame[object->yPos][object->xPos - 1] = SHAPE_CHAR;
    object->board->frame[object->yPos][object->xPos] = SHAPE_CHAR;
    object->board->frame[object->yPos][object->xPos + 1] = SHAPE_CHAR;

}

/**
 * Move the shape right
 * @param object the game object
 */
void moveRight(Shape *object) {
    // check if get to the end right of the board
    if ((!object->isVertical && object->xPos == BOARD_GAME_SIZE - 3) ||
        (object->isVertical && object->xPos == BOARD_GAME_SIZE - 2)) {
        return;
    }

    // check if the shape is horizontal
    if (!object->isVertical) {
        // move right
        object->board->frame[object->yPos][object->xPos + 2] = SHAPE_CHAR;
        // change x position
        object->xPos = object->xPos + 1;
        object->board->frame[object->yPos][object->xPos - 2] = SPACE;

        // shape is vertical
    } else if (object->isVertical) {
        // delete the shape
        clearVertical(object);
        // change position to next position
        object->xPos = object->xPos + 1;

        // draw the shape after movement right
        object->board->frame[object->yPos - 1][object->xPos] = SHAPE_CHAR;
        object->board->frame[object->yPos][object->xPos] = SHAPE_CHAR;
        object->board->frame[object->yPos + 1][object->xPos] = SHAPE_CHAR;
    }
}

/**
 * Move the shape lest
 * @param object the game object
 */
void moveLeft(Shape *object) {
    // check if get to the end left of the board
    if ((!object->isVertical && object->xPos == 2) || (object->isVertical && object->xPos == 1)) {
        return;
    }
    // shape is horizontal
    if (!object->isVertical) {
        object->board->frame[object->yPos][object->xPos - 2] = SHAPE_CHAR;
        object->xPos = object->xPos - 1;
        object->board->frame[object->yPos][object->xPos + 2] = SPACE;
        // shape is vertical
    } else if (object->isVertical) {

        // delete the shape
        clearVertical(object);
        // change position to next position
        object->xPos = object->xPos - 1;
        // draw the shape after movement left
        object->board->frame[object->yPos - 1][object->xPos] = SHAPE_CHAR;
        object->board->frame[object->yPos][object->xPos] = SHAPE_CHAR;
        object->board->frame[object->yPos + 1][object->xPos] = SHAPE_CHAR;
    }
}

/**
 * Move the shape down
 * @param object the game object
 */
void moveDown(Shape *object) {

    // check if we get to the end of the board
    if (!object->isVertical && object->yPos + 2 >= BOARD_GAME_SIZE) {
        // delete the shape
        clearHorizontal(object);
        // add new shape to game
        object->addShapeToGame(object);
        return;

    } else if (object->isVertical && object->yPos + 3 >= BOARD_GAME_SIZE) {
        // delete the shape
        clearVertical(object);
        // add new shape to game
        object->addShapeToGame(object);
        return;
    }

    if (!object->isVertical) {
        // delete the shape
        clearHorizontal(object);

        // change y position to next position
        object->yPos = object->yPos + 1;
        // draw after move down
        object->board->frame[object->yPos][object->xPos] = SHAPE_CHAR;
        object->board->frame[object->yPos][object->xPos + 1] = SHAPE_CHAR;
        object->board->frame[object->yPos][object->xPos - 1] = SHAPE_CHAR;
    } else if (object->isVertical) {
        // delete the shape
        clearVertical(object);

        // change y position to next position
        object->yPos = object->yPos + 1;
        // draw after move down
        object->board->frame[object->yPos - 1][object->xPos] = SHAPE_CHAR;
        object->board->frame[object->yPos][object->xPos] = SHAPE_CHAR;
        object->board->frame[object->yPos + 1][object->xPos] = SHAPE_CHAR;
    }

}

/**
 * The function change the shape position.
 * if the shape is vertical change to horizontal and the opposite.
 * @param object
 */
void changeOrientation(Shape *object) {
    // Unable to change position
    if (object->isVertical &&
        (object->xPos == BOARD_GAME_SIZE - 2 || object->xPos == 1)) {
        return;
        // Unable to change position
    } else if (!object->isVertical &&
               (object->yPos == BOARD_GAME_SIZE - 2 || object->yPos == 0)) {
        return;
        // if shape is vertical
    } else if (object->isVertical) {
        // change boolean
        object->isVertical = 0;
        // clear shape from board
        clearVertical(object);
        // draw horizontal shape
        object->board->frame[object->yPos][object->xPos] = SHAPE_CHAR;
        object->board->frame[object->yPos][object->xPos + 1] = SHAPE_CHAR;
        object->board->frame[object->yPos][object->xPos - 1] = SHAPE_CHAR;
        // if the shape is horizontal
    } else if (!object->isVertical) {
        // change boolean
        object->isVertical = 1;
        // clear shape from board
        clearHorizontal(object);
        // draw vertical shape
        object->board->frame[object->yPos - 1][object->xPos] = SHAPE_CHAR;
        object->board->frame[object->yPos][object->xPos] = SHAPE_CHAR;
        object->board->frame[object->yPos + 1][object->xPos] = SHAPE_CHAR;
    }
}

/**
 * Initialize new shape (---)
 * @param shape the shape
 * @param board the board game
 */
void initializeShape(Shape *shape, Board *board) {
    shape->board = board;
    shape->addShapeToGame = addShapeToGame;
    shape->moveDown = moveDown;
    shape->moveLeft = moveLeft;
    shape->moveRight = moveRight;
    shape->changeOrientation = changeOrientation;

}

/**
 * Run the tetris game
 */
void run() {
    // create new board
    createBord(&board);
    initializeShape(&shape, &board);
    // add the shape (game object) to board
    shape.addShapeToGame(&shape);
    showBoard(&board);
    // handle alarm
    signal(SIGALRM, alarmHandle);
    // wait 1 sec
    alarm(1);
    // handle signal
    signal(SIGUSR2, signalHandle);
    // wait until flag is true
    while (!flag) {
        pause();
    }
}

/**
 * Create new board for the tetris game
 * @param board the board game
 */
void createBord(Board *board) {

    // draw spaces in the array
    initializeSpace(board);

    int i;

    // draw bottom border
    for (i = 0; i < BOARD_GAME_SIZE; i++) {
        board->frame[BOARD_GAME_SIZE - 1][i] = STAR;
    }

    // draw left and right border
    for (i = 0; i < BOARD_GAME_SIZE - 1; i++) {
        board->frame[i][0] = STAR;
        board->frame[i][BOARD_GAME_SIZE - 1] = STAR;
    }
}

/**
 * Draw spaces in all the array (board)
 * @param board the board game
 */
void initializeSpace(Board *board) {
    int i, j;

    // draw spaces
    for (i = 0; i < BOARD_GAME_SIZE; i++) {
        for (j = 0; j < BOARD_GAME_SIZE; j++) {
            board->frame[i][j] = SPACE;
        }
    }
}

/**
 * Draw the board game
 * @param board the board game
 */
void showBoard(Board *board) {
    system("clear");
    int i;
    // draw each line in the array
    for (i = 0; i < BOARD_GAME_SIZE; i++) {
        write(STDOUT_FILENO, board->frame[i], BOARD_GAME_SIZE);
        write(STDOUT_FILENO, NEW_LINE, strlen(NEW_LINE));

    }
}

/**
 * Handel when signal is happen,
 * when the user pressed a key.
 * @param signo the num
 */
void signalHandle(int signo) {
    // Set signal handler for next signals
    signal(SIGUSR2, signalHandle);
    char userInput;
    // get char from user
    userInput = (char) getchar();
    // input is q
    if (userInput == QUIT_GAME) {
        close(STDIN_FILENO);
        flag = 1;
        return;
        // input is a
    } else if (userInput == MOVE_LEFT)
        shape.moveLeft(&shape);
        // input is d
    else if (userInput == MOVE_RIGHT)
        shape.moveRight(&shape);
        // input is s
    else if (userInput == MOVE_DOWN)
        shape.moveDown(&shape);
        // input is w
    else if (userInput == TRANSPOSE) {
        shape.changeOrientation(&shape);
    }
    // show the shape after movement
    showBoard(&board);
}

/**
 * Handel when alarm is happen.
 * Move the shape down.
 * @param signo the num
 */
void alarmHandle(int signo) {
    // Set signal alarm for next alarm
    signal(SIGALRM, alarmHandle);
    alarm(1);
    // move shape down and print to screen.
    shape.moveDown(&shape);
    showBoard(&board);
}