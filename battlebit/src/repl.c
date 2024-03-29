//
// Created by carson on 5/20/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "repl.h"
#include "server.h"
#include "char_buff.h"


extern void nasm_hello_world();

struct char_buff * repl_read_command(char * prompt) {
    printf("%s", prompt);
    char *line = NULL;
    size_t buffer_size = 0; // let getline autosize it
    if (getline(&line, &buffer_size, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);  // We received an EOF
        } else  {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }
    if (strcmp(line, "") == 0) {
        free(line);
        return NULL;
    } else {
        struct char_buff *buffer = cb_create(2000);
        cb_append(buffer, line);
        free(line);
        return buffer;
    }
}

void repl_execute_command(struct char_buff * buffer) {
    char* command = cb_tokenize(buffer, " \n");

    // char_buff for holding strings
    char_buff *n = cb_create(2000);
    if (command) {
        char* arg1 = cb_next_token(buffer);
        char* arg2 = cb_next_token(buffer);
        char* arg3 = cb_next_token(buffer);
        int n1;
        int n2;
        int n3;

        if (strcmp(command, "exit") == 0) {
            printf("goodbye!");
            exit(EXIT_SUCCESS);
        } else if(strcmp(command, "?") == 0) {
            printf("? - show help\n");
            printf("load [0-1] <string> - load a ship layout file for the given player\n");
            printf("show [0-1] - shows the board for the given player\n");
            printf("fire [0-1] [0-7] [0-7] - the given player fires at the given position\n");
            printf("say <string> - Send the string to all players as part of a chat\n");
            printf("reset - reset the game\n");
            printf("server - start the server\n");
            printf("exit - quit the server\n");
        } else if(strcmp(command, "server") == 0) {

            server_start();

        } else if(strcmp(command, "show") == 0) {

            // work with repl_print_board
            n1 = atoi(arg1); // int for input
            repl_print_board(game_get_current(), n1, n); // print board, send n as buff
            cb_print(n); // print buff

        } else if(strcmp(command, "reset") == 0) {

            game_init();

        } else if (strcmp(command, "load") == 0) {

            // work with game_load_board
            n1 = atoi(arg1); // to int
            game_load_board(game_get_current(), n1, arg2); // load board with inputs

        } else if (strcmp(command, "fire") == 0) {

            // work with game_fire
            n1 = atoi(arg1); // to int
            n2 = atoi(arg2); // to int
            n3 = atoi(arg3); // to int

          // call fire
          int f = game_fire(game_get_current(), n1, n2, n3);

          // if 0, miss. could print
          if (f == 0){
            //  printf("\nMISS\n");
          }
          // if 1, hit, could print
          else if (f == 1)
          {
              //printf("\nHIT\n");
          }

        } else if (strcmp(command, "nasm") == 0) {
            nasm_hello_world();
        } else if (strcmp(command, "shortcut") == 0) {
            // update player 1 to only have a single ship in position 0, 0
            game_get_current()->players[1].ships = 1ull;
        }
        else if (strcmp(command, "say") == 0){
/*            char_buff *sayrepl = cb_create(2000);
            cb_append(sayrepl, command);
            cb_write(SERVER->player_sockets[0], sayrepl);
            cb_write(SERVER->player_sockets[1], sayrepl);*/
        }
        else {
            printf("Unknown Command: %s\n", command);
        }
    }
}

void repl_print_board(game *game, int player, char_buff * buffer) {
    player_info player_info = game->players[player];
    cb_append(buffer, "battleBit.........\n");
    cb_append(buffer, "-----[ ENEMY ]----\n");
    repl_print_hits(&player_info, buffer);
    cb_append(buffer, "==================\n");
    cb_append(buffer, "-----[ SHIPS ]----\n");
    repl_print_ships(&player_info, buffer);
    cb_append(buffer, ".........battleBit\n\n");
}

void repl_print_ships(player_info *player_info, char_buff *buffer) {
    // Step 4 - Implement this to print out the visual ships representation
    //  for the console.  You will need to use bit masking for each position
    //  to determine if a ship is at the position or not.  If it is present
    //  you need to print an X.  If not, you need to print a space character ' '

   // printf("\n%llu \n", player_info->ships);
    //unsigned long long test1 = 8070451636549550111;

    // vars
    unsigned long long test;
    cb_append(buffer , "  0 1 2 3 4 5 6 7");
    char str[0];
    // loop over the 8x8 grid and check for if  there is a ship using &
    // * if there is, space if nothing
    for (int j = 0; j < 8; j++)
    {
        sprintf(str, "%d", j);
        cb_append(buffer, " \n");
        cb_append(buffer, str);
        // inner for loop
        for (int k = 0; k < 8; k++)
        {
            // get bit val for testing of each x,y
            test = xy_to_bitval(k,j);
            // there is a ship if and with test and ships
            if ((test & player_info->ships) != 0)
            {
                cb_append(buffer, " *");
            }
            // there is no ship if and with test and ships = 0
            else if ((test & player_info->ships) == 0)
            {
                cb_append(buffer, "  ");
            }
        }
    }
    cb_append(buffer, " \n");
}

void repl_print_hits(struct player_info *player_info, struct char_buff *buffer) {
    // Step 6 - Implement this to print out a visual representation of the shots
    // that the player has taken and if they are a hit or not.  You will again need
    // to use bit-masking, but this time you will need to consult two values: both
    // hits and shots values in the players game struct.  If a shot was fired at
    // a given spot and it was a hit, print 'H', if it was a miss, print 'M'.  If
    // no shot was taken at a position, print a space character ' '

   // printf("\n%llu \n", player_info->ships);
   // printf("%llu \n", player_info->shots);
   // printf("%llu \n", player_info->hits);

   // vars
    unsigned long long test;
    cb_append(buffer , "  0 1 2 3 4 5 6 7");
    char str[0];
    // loop over the 8x8 grid and check for if  there is a shot using &
    // H if there is a hit , M for a miss
    for (int j = 0; j < 8; j++)
    {
        sprintf(str, "%d", j);
        cb_append(buffer, " \n");
        cb_append(buffer, str);
        // inner for loop
        for (int k = 0; k < 8; k++)
        {
            // get bit val of each x,y
            test = xy_to_bitval(k,j);
            // if and bit val of location and shots, there is a shot
            if (((player_info->shots & test) != 0) && ((player_info->hits & test) != 0))
            {
                cb_append(buffer, " H");
            }
            // if and with shots and test is -, there is a miss, but a shot
            else if (((player_info->shots & test) != 0) && ((player_info->hits & test) == 0))
            {
                cb_append(buffer, " M");
            }
            else
            {
                cb_append(buffer, "  ");
            }
        }
    }
    cb_append(buffer, " \n");
}