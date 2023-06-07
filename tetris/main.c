#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

#define LEFT_KEY 0x25     // The key to move left, default = 0x25 (left arrow)
#define RIGHT_KEY 0x27    // The key to move right, default = 0x27 (right arrow)
#define ROTATE_KEY 0x26   // The key to rotate, default = 0x26 (up arrow)
#define DOWN_KEY 0x28     // The key to move down, default = 0x28 (down arrow)
#define FALL_KEY 0x20     // The key to fall, default = 0x20 (spacebar)

#define FALL_DELAY 500    // The delay between each fall, default = 500
#define RENDER_DELAY 100  // The delay between each frame, default = 100

#define LEFT_FUNC() GetAsyncKeyState(LEFT_KEY) & 0x8000
#define RIGHT_FUNC() GetAsyncKeyState(RIGHT_KEY) & 0x8000
#define ROTATE_FUNC() GetAsyncKeyState(ROTATE_KEY) & 0x8000
#define DOWN_FUNC() GetAsyncKeyState(DOWN_KEY) & 0x8000
#define FALL_FUNC() GetAsyncKeyState(FALL_KEY) & 0x8000

#define CANVAS_WIDTH 10
#define CANVAS_HEIGHT 20

typedef enum {
    RED = 41,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE,
    CYAN,
    WHITE,
    BLACK = 0,
}Color;

typedef enum {
    EMPTY = -1,
    I,
    J,
    L,
    O,
    S,
    T,
    Z
}ShapeId;

typedef struct {
    ShapeId shape;
    Color color;
    int size;
    char rotates[4][4][4];
}Shape;

typedef struct
{
    int x;
    int y;
    int score;
    int rotate;
    int fallTime;
    ShapeId queue[4];
}State;

typedef struct {
    Color color;
    ShapeId shape;
    bool current;
}Block;

Shape shapes[7] = {
    {
        .shape = I,
        .color = CYAN,
        .size = 4,
        .rotates =
        {
            {
                {0, 0, 0, 0},
                {1, 1, 1, 1},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 1, 0}
            },
            {
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {1, 1, 1, 1},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0}
            }
        }
    },
    {
        .shape = J,
        .color = BLUE,
        .size = 3,
        .rotates =
        {
            {
                {1, 0, 0},
                {1, 1, 1},
                {0, 0, 0}
            },
            {
                {0, 1, 1},
                {0, 1, 0},
                {0, 1, 0}
            },
            {
                {0, 0, 0},
                {1, 1, 1},
                {0, 0, 1}
            },
            {
                {0, 1, 0},
                {0, 1, 0},
                {1, 1, 0}
            }
        }
    },
    {
        .shape = L,
        .color = YELLOW,
        .size = 3,
        .rotates =
        {
            {
                {0, 0, 1},
                {1, 1, 1},
                {0, 0, 0}
            },
            {
                {0, 1, 0},
                {0, 1, 0},
                {0, 1, 1}
            },
            {
                {0, 0, 0},
                {1, 1, 1},
                {1, 0, 0}
            },
            {
                {1, 1, 0},
                {0, 1, 0},
                {0, 1, 0}
            }
        }
    },
    {
        .shape = O,
        .color = WHITE,
        .size = 2,
        .rotates =
        {
            {
                {1, 1},
                {1, 1}
            },
            {
                {1, 1},
                {1, 1}
            },
            {
                {1, 1},
                {1, 1}
            },
            {
                {1, 1},
                {1, 1}
            }
        }
    },
    {
        .shape = S,
        .color = GREEN,
        .size = 3,
        .rotates =
        {
            {
                {0, 1, 1},
                {1, 1, 0},
                {0, 0, 0}
            },
            {
                {0, 1, 0},
                {0, 1, 1},
                {0, 0, 1}
            },
            {
                {0, 0, 0},
                {0, 1, 1},
                {1, 1, 0}
            },
            {
                {1, 0, 0},
                {1, 1, 0},
                {0, 1, 0}
            }
        }
    },
    {
        .shape = T,
        .color = PURPLE,
        .size = 3,
        .rotates =
        {
            {
                {0, 1, 0},
                {1, 1, 1},
                {0, 0, 0}
            },

                {{0, 1, 0},
                {0, 1, 1},
                {0, 1, 0}
            },
            {
                {0, 0, 0},
                {1, 1, 1},
                {0, 1, 0}
            },
            {
                {0, 1, 0},
                {1, 1, 0},
                {0, 1, 0}
            }
        }
    },
    {
        .shape = Z,
        .color = RED,
        .size = 3,
        .rotates =
        {
            {
                {1, 1, 0},
                {0, 1, 1},
                {0, 0, 0}
            },
            {
                {0, 0, 1},
                {0, 1, 1},
                {0, 1, 0}
            },
            {
                {0, 0, 0},
                {1, 1, 0},
                {0, 1, 1}
            },
            {
                {0, 1, 0},
                {1, 1, 0},
                {1, 0, 0}
            }
        }
    },
};

void setBlock(Block* block, Color color, ShapeId shape, bool current)
{
    block->color = color;
    block->shape = shape;
    block->current = current;
}

void resetBlock(Block* block)
{
    block->color = BLACK;
    block->shape = EMPTY;
    block->current = false;
}

bool move(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH], int originalX, int originalY, int originalRotate, int newX, int newY, int newRotate, ShapeId shapeId) {
    Shape shapeData = shapes[shapeId];
    int size = shapeData.size;

    // check if the new position is valid to place the block
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (shapeData.rotates[newRotate][i][j]) {
                if (newX + j < 0 || newX + j >= CANVAS_WIDTH || newY + i < 0 || newY + i >= CANVAS_HEIGHT) {
                    return false;
                }
                if (!canvas[newY + i][newX + j].current && canvas[newY + i][newX + j].shape != EMPTY) {
                    return false;
                }
            }
        }
    }

    // remove the old position
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (shapeData.rotates[originalRotate][i][j]) {
                resetBlock(&canvas[originalY + i][originalX + j]);
            }
        }
    }

    // move the block
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (shapeData.rotates[newRotate][i][j]) {
                setBlock(&canvas[newY + i][newX + j], shapeData.color, shapeId, true);
            }
        }
    }

    return true;
}

void printCanvas(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH], State* state)
{
    printf("\033[0;0H\n");
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        printf("|");
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            printf("\033[%dm\u3000", canvas[i][j].color);
        }
        printf("\033[0m|\n");
    }

    Shape shapeData = shapes[state->queue[1]];
    printf("\033[%d;%dHNext:", 3, CANVAS_WIDTH * 2 + 5);
    for (int i = 1; i <= 3; i++)
    {
        shapeData = shapes[state->queue[i]];
        for (int j = 0; j < 4; j++) {
            printf("\033[%d;%dH", i * 4 + j, CANVAS_WIDTH * 2 + 15);
            for (int k = 0; k < 4; k++) {
                if (j < shapeData.size && k < shapeData.size && shapeData.rotates[0][j][k]) {
                    printf("\x1b[%dm  ", shapeData.color);
                }
                else {
                    printf("\x1b[0m  ");
                }
            }
        }
    }
    return;
}

int clearLine(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH]) {
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            if (canvas[i][j].current) {
                canvas[i][j].current = false;
            }
        }
    }

    int linesCleared = 0;

    for (int i = CANVAS_HEIGHT - 1; i >= 0; i--)
    {
        bool isFull = true;
        for (int j = 0; j < CANVAS_WIDTH; j++)
        {
            if (canvas[i][j].shape == EMPTY) {
                isFull = false;
                break;
            }
        }

        if (isFull) {
            linesCleared += 1;

            for (int j = i; j > 0; j--)
            {
                for (int k = 0; k < CANVAS_WIDTH; k++)
                {
                    setBlock(&canvas[j][k], canvas[j - 1][k].color, canvas[j - 1][k].shape, false);
                    resetBlock(&canvas[j - 1][k]);
                }
            }
            i++;
        }
    }


    return linesCleared;
}

void logic(Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH], State* state)
{
    if (ROTATE_FUNC()) {
        int newRotate = (state->rotate + 1) % 4;
        if (move(canvas, state->x, state->y, state->rotate, state->x, state->y, newRotate, state->queue[0]))
        {
            state->rotate = newRotate;
        }
    }
    else if (LEFT_FUNC()) {
        if (move(canvas, state->x, state->y, state->rotate, state->x - 1, state->y, state->rotate, state->queue[0]))
        {
            state->x -= 1;
        }
    }
    else if (RIGHT_FUNC()) {
        if (move(canvas, state->x, state->y, state->rotate, state->x + 1, state->y, state->rotate, state->queue[0]))
        {
            state->x += 1;
        }
    }
    else if (DOWN_FUNC()) {
        state->fallTime = FALL_DELAY;
    }
    else if (FALL_FUNC()) {
        state->fallTime += FALL_DELAY * CANVAS_HEIGHT;
    }

    state->fallTime += RENDER_DELAY;

    while (state->fallTime >= FALL_DELAY) {
        state->fallTime -= FALL_DELAY;

        if (move(canvas, state->x, state->y, state->rotate, state->x, state->y + 1, state->rotate, state->queue[0])) {
            state->y++;
        }
        else {
            state->score += clearLine(canvas);
            //printf("\033[%d;%dHLines cleared: %d", CANVAS_HEIGHT + 1, CANVAS_WIDTH * 2 + 5, 0, state->score);

            state->x = CANVAS_WIDTH / 2;
            state->y = 0;
            state->rotate = 0;
            state->fallTime = 0;
            state->queue[0] = state->queue[1];
            state->queue[1] = state->queue[2];
            state->queue[2] = state->queue[3];
            state->queue[3] = rand() % 7;

            if (!move(canvas, state->x, state->y, state->rotate, state->x, state->y, state->rotate, state->queue[0]))
            {
                printf("\033[%d;%dH\x1b[41m GAME OVER \x1b[0m\033[%d;%dH", CANVAS_HEIGHT - 3, CANVAS_WIDTH * 2 + 5, CANVAS_HEIGHT + 5, 0);
                exit(0);
            }
        }
    }
    return;
}

void printStartScreen()
{

    int color = 1;
    int row = 3;
    int col = 0;

    while (1)
    {
        // 判斷使用哪種顏色
        if (color == 1)
        {
            printf("\033[91m"); // 紅色
        }
        else if (color == 2)
        {
            printf("\033[93m"); // 黃色
        }
        else if (color == 3)
        {
            printf("\033[92m"); // 綠色
        }
        else if (color == 4)
        {
            printf("\033[96m"); // 青色
        }
        else if (color == 5)
        {
            printf("\033[94m"); // 藍色
        }
        else if (color == 6)
        {
            printf("\033[95m"); // 紫色
        }

        // 設定印出位置
        printf("\033[%d;%dH", row, col);

        // 印出閃爍的文字
        printf("\033[5m\n");
        printf("######## ######## ######## ######    ######   #####  \n");
        printf("   ##    ##          ##    ##    ##    ##   ##     ##\n");
        printf("   ##    ##          ##    ##    ##    ##    ##      \n");
        printf("   ##    #######     ##    ######      ##      ###   \n");
        printf("   ##    ##          ##    ##   ##     ##        ##  \n");
        printf("   ##    ##          ##    ##    ##    ##   ##     ##\n");
        printf("   ##    ########    ##    ##     ## ######   #####  \n");
        printf("\033[25m\033[22m\n");
        // 切換顏色
        if (color == 6)
        {
            color = 0;
        }
        color++;
        printf("\n");
        printf("\n");
        printf("\n");
        printf("\n");
        printf("\n");
        printf("\n");
        printf("\n");
        printf("\n");
        printf("\x1b[33m*************** Press \033[4mEnter\033[24m to start ***************\x1b[0m\n");

        // 延遲一段時間，以控制閃爍速度
        // 請根據你的需求調整延遲時間
        Sleep(500); // 延遲 500 毫秒 (0.5 秒)
    }
}

int main()
{
    printStartScreen();
    getchar();
    srand(time(NULL));
    State state = {
        .x = CANVAS_WIDTH / 2,
        .y = 0,
        .score = 0,
        .rotate = 0,
        .fallTime = 0
    };

    for (int i = 0; i < 4; i++)
    {
        state.queue[i] = rand() % 7;
    }

    Block canvas[CANVAS_HEIGHT][CANVAS_WIDTH];
    for (int i = 0; i < CANVAS_HEIGHT; i++)
    {
        for (int j = 0; j < CANVAS_WIDTH; j++)
        {
            resetBlock(&canvas[i][j]);
        }
    }

    system("cls");

    move(canvas, state.x, state.y, state.rotate, state.x, state.y, state.rotate, state.queue[0]);

    while (1)
    {
        logic(canvas, &state);
        printCanvas(canvas, &state);
        Sleep(100);
    }

}