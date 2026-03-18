// constants
#define EMPTY 0
#define BLACK 1
#define WHITE 2

// global variables are extern so they can be used
extern int board[25][25];
extern int board_size;
extern float komi;

// functions
void clear_board();
int play_move(int color, int r, int c);
void show_board();
void final_score();
int genmove(int color, int *r, int *c);