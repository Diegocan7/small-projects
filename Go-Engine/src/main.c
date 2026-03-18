#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "goteam.h"

void process_command(char *line);
int coordinates(char *coord, int *r, int *c);
const char *cmds[] = {
    "protocol_version", "name", "version", "known_command", "list_commands",
    "quit", "boardsize", "clear_board", "komi", "play", "genmove",
    "showboard", "final_score", NULL};
int main(void)
{
    char line[256];

    // loops that reads from stdin
    while (fgets(line, sizeof(line), stdin) != NULL)
    {
        process_command(line);
    }

    return 0;
}

void process_command(char *line)
{
    char command[256], arg1[256], arg2[256];
    // read first word
    // check so we dont overflow
    if (sscanf(line, "%255s", command) != 1)
    {
        return;
    }
    if (strcmp(command, "protocol_version") == 0)
    {
        printf("= 2\n\n");
    }
    else if (strcmp(command, "name") == 0)
    {
        printf("= kalinixta\n\n");
    }
    else if (strcmp(command, "version") == 0)
    {
        printf("= 1.0\n\n");
    }
    else if (strcmp(command, "list_commands") == 0)
    {
        printf("=   ");
        for (int i = 0; cmds[i] != NULL; i++)
        {
            printf("%s\n", cmds[i]);
        }
        printf("\n");
    }
    else if (strcmp(command, "known_command") == 0)
    {
        int found = 0;
        if (sscanf(line, "%*s %255s", arg1) == 1)
        {
            for (int i = 0; cmds[i] != NULL; i++)
            {
                if (strcmp(arg1, cmds[i]) == 0)
                {
                    found = 1;
                    break;
                }
            }
            if (found)
                printf("= true\n\n");
            else
                printf("= false\n\n");

        }
        else
            printf("? syntax error\n\n");
        
    }
    // boardsize
    else if (strcmp(command, "boardsize") == 0)
    {
        int sz;
        if (sscanf(line, "%*s %d", &sz) == 1 && sz > 0 && sz <= 256)
        {
            board_size = sz;
            clear_board();
            printf("=\n\n");
        }
        else
        {
            printf("? invalid size\n\n");
        }
    }
    // clear board
    else if (strcmp(command, "clear_board") == 0)
    {
        clear_board();
        printf("=\n\n");
    }
    // komi
    else if (strcmp(command, "komi") == 0)
    {
        float new_komi;
        if (sscanf(line, "%*s %f", &new_komi) == 1)
        {
            komi = new_komi; // save to global variable
            printf("=\n\n");
        }
    }
    // play
    else if (strcmp(command, "play") == 0)
    {
        // play <color> <coordinate>
        if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
        {
            int color = 0;
            if (strcasecmp(arg1, "black") == 0)
            {
                color = BLACK;
            }
            else if (strcasecmp(arg1, "white") == 0)
            {
                color = WHITE;
            }
            if (color == 0)
            {
                printf("? invalid color\n\n");
                return;
            }

            int r, c;
            int res = coordinates(arg2, &r, &c);
            if (res == 2)
            {
                printf("=\n\n");
            }
            else if (res == 1)
            {
                if (play_move(color, r, c))
                {
                    printf("=\n\n");
                }
                else
                {
                    printf("? illegal move\n\n");
                }
            }
            else
            {
                printf("? invalid coordinate\n\n");
            }
        }
        else
        {
            printf("? syntax error\n\n");
        }
    }
    // show board
    else if (strcmp(command, "showboard") == 0)
    {
        show_board();
    }
    else if (strcmp(command, "genmove") == 0)
    {
        char color_str[256];

        // read color to play for
        if (sscanf(line, "%*s %255s", color_str) == 1)
        {
            int color = 0;
            if (strcasecmp(color_str, "black") == 0)
                color = BLACK;
            else if (strcasecmp(color_str, "white") == 0)
                color = WHITE;

            if (color == 0)
            {
                printf("? invalid color\n\n");
                return;
            }

            int r, c;
            // call genmove
            if (genmove(color, &r, &c))
            {
                // convert to letters
                char col_char = 'A' + c;
                if (col_char >= 'I')
                {
                    col_char++; // skip I
                }
                printf("= %c%d\n\n", col_char, r + 1);
            }
            else
            {
                // if genmove returns 0 it means there is no room
                printf("= PASS\n\n");
            }
        }
        else
        {
            printf("? syntax error\n\n");
        }
    }
    else if (strcmp(command, "final_score") == 0)
    {
        final_score();
    }
    // quit
    else if (strcmp(command, "quit") == 0)
    {
        printf("=\n\n");
        exit(0);
    }
    // unexpected command
    else
    {
        printf("? unknown command\n\n");
    }
}

// convert coordinates
int coordinates(char *coord, int *r, int *c)
{
    char col_char;
    int row_num;

    if (strcasecmp(coord, "pass") == 0)
        return 2;

    if (sscanf(coord, "%c%d", &col_char, &row_num) != 2)
        return 0;

    col_char = toupper(col_char);
    if (col_char >= 'J')
        col_char--;

    *c = col_char - 'A';
    *r = row_num - 1;

    // bound check
    if (*r < 0 || *r >= board_size || *c < 0 || *c >= board_size)
    {
        return 0;
    }
    return 1;
}