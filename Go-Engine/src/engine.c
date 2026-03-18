#include <stdio.h>
#include <string.h>
#include "goteam.h"

// set globals
int board[25][25];
int board_size = 19; // default
float komi = 6.5;    // default value

static int visited[25][25];

// check if coordinates inside the board
int is_valid(int r, int c)
{
    return r >= 0 && r < board_size && c >= 0 && c < board_size;
}

int check_liberties(int r, int c, int color)
{
    if (!is_valid(r, c))
        return 0; // out of bounds
    if (board[r][c] == EMPTY)
        return 1; // empty
    if (board[r][c] != color)
        return 0; // enemy square
    if (visited[r][c])
        return 0; // already visited

    visited[r][c] = 1;

    // recursive to 4 neighbor blocks
    if (check_liberties(r + 1, c, color))
        return 1; // down
    if (check_liberties(r - 1, c, color))
        return 1; // up
    if (check_liberties(r, c + 1, color))
        return 1; // left
    if (check_liberties(r, c - 1, color))
        return 1; // right

    return 0;
}

int has_liberties(int r, int c)
{
    int color = board[r][c];
    memset(visited, 0, sizeof(visited)); // clear memory
    return check_liberties(r, c, color);
}

// capture
void remove_group(int r, int c, int color)
{
    if (!is_valid(r, c))
        return;
    if (board[r][c] != color)
        return; // if the board is the wrong color

    board[r][c] = EMPTY; // empty the block

    // remove neighbor blocks recursively
    remove_group(r + 1, c, color);
    remove_group(r - 1, c, color);
    remove_group(r, c + 1, color);
    remove_group(r, c - 1, color);
}

void clear_board()
{
    int i, j;
    for (i = 0; i < board_size; i++)
    {
        for (j = 0; j < board_size; j++)
            board[i][j] = EMPTY;
    }
}

int play_move(int color, int r, int c)
{
    // neightbor pointers
    int neighbors_r[] = {r + 1, r - 1, r, r};
    int neighbors_c[] = {c, c, c + 1, c - 1};
    int i, nr, nc;
    int opponent = color;
    // find opponent
    if (opponent == BLACK)
    {
        opponent = WHITE;
    }
    else if (opponent == WHITE)
    {
        opponent = BLACK;
    }

    int captured = 0;

    if (board[r][c] != EMPTY)
        return 0;

    board[r][c] = color;

    // check for captures
    for (i = 0; i < 4; i++)
    {
        nr = neighbors_r[i];
        nc = neighbors_c[i];
        // check neighbor blocks that are of the opponent
        if (is_valid(nr, nc) && board[nr][nc] == opponent)
        {
            // check for liberties
            if (!has_liberties(nr, nc))
            {
                remove_group(nr, nc, opponent); // remove the block
                captured = 1;
            }
        }
    }

    // check if we captured or we have any liberties to move there
    if (!captured && !has_liberties(r, c))
    {
        board[r][c] = EMPTY; // remove it from the block if it cant move there
        return 0;
    }

    return 1;
}

void show_board()
{
    int i, j;
    const char *cols = "ABCDEFGHJKLMNOPQRSTUVWXYZ"; // go doesnt use I for some reason

    printf("=\n");
    printf("   ");
    for (j = 0; j < board_size; j++)
        printf("%c ", cols[j]);
    printf("\n");

    for (i = board_size - 1; i >= 0; i--)
    {
        printf("%2d ", i + 1);
        for (j = 0; j < board_size; j++)
        {
            if (board[i][j] == EMPTY)
                printf(". ");
            else if (board[i][j] == BLACK)
                printf("X ");
            else if (board[i][j] == WHITE)
                printf("O ");
        }
        printf("%2d\n", i + 1);
    }
    printf("   ");
    for (j = 0; j < board_size; j++)
        printf("%c ", cols[j]);
    printf("\n\n");
}
int genmove(int color, int *r, int *c)
{
    int i, j;

    // scan board
    for (i = 0; i < board_size; i++)
    {
        for (j = 0; j < board_size; j++)
        {
            // check for legal move
            if (play_move(color, i, j))
            {
                *r = i;
                *c = j;
                return 1; // if move found return 1
            }
        }
    }

    return 0; // no move found
}

void final_score()
{
    int black_area = 0;
    int white_area = 0;
    int black_stones = 0;
    int white_stones = 0;

    // local array to check if block isnt empty
    int visited[25][25];
    memset(visited, 0, sizeof(visited));

    // count stones each color has
    for (int i = 0; i < board_size; i++)
    {
        for (int j = 0; j < board_size; j++)
        {
            if (board[i][j] == BLACK)
                black_stones++;
            else if (board[i][j] == WHITE)
                white_stones++;
        }
    }

    // count empty areas
    for (int i = 0; i < board_size; i++)
    {
        for (int j = 0; j < board_size; j++)
        {
            // empty unvisited region
            if (board[i][j] == EMPTY && !visited[i][j])
            {
                
                int touches_black = 0;
                int touches_white = 0;
                int area_size = 0;

                // row and collumn coordinates and top stack
                int stack_r[25 * 25];
                int stack_c[25 * 25];
                int top = 0;

                // initialize first cell in stack
                stack_r[top] = i;
                stack_c[top] = j;
                top++;
                visited[i][j] = 1;

                // while we search the whole area
                while (top > 0)
                {
                    // remove cell from stack
                    top--;
                    int cr = stack_r[top]; // current row
                    int cc = stack_c[top]; // current collumn
                    area_size++; 

                    // check 4 directions
                    int dr[] = {1, -1, 0, 0};
                    int dc[] = {0, 0, 1, -1};
                    int d;

                    for (d = 0; d < 4; d++)
                    {
                        int nr = cr + dr[d]; // neighbor row
                        int nc = cc + dc[d]; // neighbor collumn
                        // check if its inside the board
                        if (is_valid(nr, nc))
                        {
                            // it either touches black/white or is empty
                            if (board[nr][nc] == BLACK)
                                touches_black = 1;
                            else if (board[nr][nc] == WHITE)
                                touches_white = 1;
                            else if (board[nr][nc] == EMPTY && !visited[nr][nc])
                            {
                                // if not visited add to stack to check it
                                visited[nr][nc] = 1;
                                stack_r[top] = nr;
                                stack_c[top] = nc;
                                top++;
                            }
                        }
                    }
                }

                // assign area to black or white or noone
                if (touches_black && !touches_white)
                    black_area += area_size;
                else if (touches_white && !touches_black)
                    white_area += area_size;
            }
        }
    }

    // calculate final score and print

    float black_total = black_stones + black_area;
    float white_total = white_stones + white_area + komi;

    printf("= ");
    if (black_total > white_total)
        printf("B+%.1f", black_total - white_total);
    else
        printf("W+%.1f", white_total - black_total);
    printf("\n\n");
}