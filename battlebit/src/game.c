//
// Created by carson on 5/20/20.
//

#include <stdlib.h>
#include <stdio.h>
#include "game.h"

#include <string.h>
#include <ctype.h>

// STEP 10 - Synchronization: the GAME structure will be accessed by both players interacting
// asynchronously with the server.  Therefore the data must be protected to avoid race conditions.
// Add the appropriate synchronization needed to ensure a clean battle.

static game * GAME = NULL;

void game_init() {
    if (GAME) {
        free(GAME);
    }
    GAME = malloc(sizeof(game));
    GAME->status = CREATED;
    game_init_player_info(&GAME->players[0]);
    game_init_player_info(&GAME->players[1]);
}

void game_init_player_info(player_info *player_info) {
    player_info->ships = 0;
    player_info->hits = 0;
    player_info->shots = 0;
}

int game_fire(game *game, int player, int x, int y) {
    // Step 5 - This is the crux of the game.  You are going to take a shot from the given player and
    // update all the bit values that store our game state.
    //
    //  - You will need up update the players 'shots' value
    //  - you You will need to see if the shot hits a ship in the opponents ships value.  If so, record a hit in the
    //    current players hits field
    //  - If the shot was a hit, you need to flip the ships value to 0 at that position for the opponents ships field
    //
    //  If the opponents ships value is 0, they have no remaining ships, and you should set the game state to
    //  PLAYER_1_WINS or PLAYER_2_WINS depending on who won.


    int otherPlayer;
    if (player == 1)
    {
        otherPlayer = 0;
    }
    if (player == 0)
    {
        otherPlayer = 1;
    }

    unsigned long long fire = xy_to_bitval(x, y);
    game->players[player].shots = game->players[player].shots | fire;




    if(((game->players[otherPlayer].ships) & fire) != 0)
    {
        //printf("HIT\n");
        game->players[player].hits = game->players[player].hits | fire;
        game->players[otherPlayer].ships = game->players[otherPlayer].ships ^ fire;
        if(player == 0)
        {
            game->status = PLAYER_1_TURN;
        }
        else if (player == 1)
        {
            game->status = PLAYER_0_TURN;
        }
        if(game->players[otherPlayer].ships == 0)
        {
            if(player == 0)
            {
                game->status = PLAYER_0_WINS;
                //return 1;
            }
            else if (player == 1)
            {
                game->status = PLAYER_1_WINS;
                //return 1;
            }

        }
        return 1;
    }
    else if (((game->players[otherPlayer].ships) & fire) == 0)
    {
          //  printf("MISS\n");
        if(player == 0)
        {
            game->status = PLAYER_1_TURN;
        }
        else if (player == 1)
        {
            game->status = PLAYER_0_TURN;
        }
        return 0;
    }

    if(game->players[otherPlayer].ships == 0)
    {
        if(player == 0)
        {
            game->status = PLAYER_1_WINS;
            return 0;
        }
        else if (player == 1)
        {
            game->status = PLAYER_0_WINS;
            return 0;
        }

    }

//    printf("%d\n", player);
//    printf("%d\n", otherPlayer);
//    printf("%llu\n", game->players[0].ships);
//    printf("%llu\n", game->players[1].ships);
//    printf("%llu\n", game->players->ships);
//    printf("%llu\n", game->players[0].shots);
//    printf("%llu\n", game->players[1].shots);
//    printf("%llu\n", game->players->shots);
//    printf("%llu\n", game->players[0].hits);
//    printf("%llu\n", game->players[1].hits);
//    printf("%llu\n\n", game->players->hits);

}

unsigned long long int xy_to_bitval(int x, int y) {
    // Step 1 - implement this function.  We are taking an x, y position
    // and using bitwise operators, converting that to an unsigned long long
    // with a 1 in the position corresponding to that x, y
    //
    // x:0, y:0 == 0b00000...0001 (the one is in the first position)
    // x:1, y: 0 == 0b00000...10 (the one is in the second position)
    // ....
    // x:0, y: 1 == 0b100000000 (the one is in the eighth position)
    //
    // you will need to use bitwise operators and some math to produce the right
    // value.

    // variable declaration
    unsigned long long my_unsigned_int_64 = 0;
    // check for bad cases
    if ((x > -1) && (y > -1) && (x < 8) && (y < 8)) {
        // literal
        my_unsigned_int_64 = 1ull;
        // shift x
        my_unsigned_int_64 = my_unsigned_int_64 << x;
        // shift y
        my_unsigned_int_64 = my_unsigned_int_64 << y * 8;

        // Print Result
        // printf("X = %d, Y = %d, Unsigned Long Long = %llu  \n", x, y, my_unsigned_int_64);
    }
    return my_unsigned_int_64;
}

struct game * game_get_current() {
    return GAME;
}

int game_load_board(struct game *game, int player, char * spec) {
    // Step 2 - implement this function.  Here you are taking a C
    // string that represents a layout of ships, then testing
    // to see if it is a valid layout (no off-the-board positions
    // and no overlapping ships)
    //



    //printf("\n%s\n", spec);

    if(spec == NULL)
    {
        //printf("Spec Null \n");
        return -1;
    }

    if (strlen(spec) != 15)
   {
        //printf("Not 15 length \n");
        return -1;
    }
    else if(!isalpha(spec[0]))
    {
        //printf("Not Char \n");
        return -1;
    }
    else if (!isdigit(spec[1]) || !isdigit(spec[2]))
    {
        //printf("Not digit \n");
        return -1;
    }


    int usedL [] = {0 ,0, 0, 0, 0};

    player_info *player_info = &game->players[player];

    int j = 0;
    int x;
    int y;
    int length;
    int check1;
    for (int i = 0; i < 5; i++)
    {
      //  printf("%c" ,spec[j]);
        if (spec[j] == 'C' || spec[j] == 'c')
        {
           // printf("Length = 5\n" );
            length = 5;
            if (usedL[0] == 1)
            {
                //printf("C/c already used \n");
                return -1;
            }
            else
            {
                usedL[0] = 1;
            }
           // printf("+1: %d, +2: %d \n", spec[j+1]-'0', spec[j+2]-'0');
            if(spec[j] == 'C')
            {
                if((spec[j+1]-'0') > 3)
                {
                    return -1;
                }
            }
            else if (spec[j] == 'c')
            {
                if((spec[j+2]-'0') > 3)
                {
                    return -1;
                }
            }
        }
        else if (spec[j] == 'B' || spec[j] == 'b')
        {
            length = 4;
            if (usedL[1] == 1)
            {
                //printf("B/b already used \n");
                return -1;
            }
            else
            {
                usedL[1] = 1;
            }
           // printf("+1: %d, +2: %d \n", spec[j+1]-'0', spec[j+2]-'0');
            if(spec[j] == 'B')
            {
                if((spec[j+1]-'0') > 4)
                {
                    return -1;
                }
            }
            else if (spec[j] == 'b')
            {
                if((spec[j+2]-'0') > 4)
                {
                    return -1;
                }
            }
        }
        else if (spec[j] == 'D' || spec[j] == 'd')
        {
            length = 3;
            if (usedL[2] == 1)
            {
                //printf("D/d already used \n");
                return -1;
            }
            else
            {
                usedL[2] = 1;
            }
            //printf("+1: %d, +2: %d \n", spec[j+1]-'0', spec[j+2]-'0');
            if(spec[j] == 'D')
            {
                if((spec[j+1]-'0') > 5)
                {
                    return -1;
                }
            }
            else if (spec[j] == 'd')
            {
                if((spec[j+2]-'0') > 5)
                {
                    return -1;
                }
            }
        }
        else if (spec[j] == 'S' || spec[j] == 's')
        {
            length = 3;
            if (usedL[3] == 1)
            {
                //printf("S/s already used \n");
                return -1;
            }
            else
            {
                usedL[3] = 1;
            }
           // printf("+1: %d, +2: %d \n", spec[j+1]-'0', spec[j+2]-'0');
            if(spec[j] == 'S')
            {
                if((spec[j+1]-'0') > 5)
                {
                    return -1;
                }
            }
            else if (spec[j] == 's')
            {
                if((spec[j+2]-'0') > 5)
                {
                    return -1;
                }
            }
        }
        else if (spec[j] == 'P' || spec[j] == 'p')
        {
            length = 2;
            if (usedL[4] == 1)
            {
                //printf("P/p already used \n");
                return -1;
            }
            else
            {
                usedL[4] = 1;
            }
           // printf("+1: %d, +2: %d \n", spec[j+1]-'0', spec[j+2]-'0');
            if(spec[j] == 'P')
            {
                if((spec[j+1]-'0') > 6)
                {
                    return -1;
                }
            }
            else if (spec[j] == 'p')
            {
                if((spec[j+2]-'0') > 6)
                {
                    return -1;
                }
            }
        }
        else
        {
            length = 0;
            return -1;
        }
        j++;

        //printf("%d", j);
        //printf("%c" ,spec[j]);
        x = spec[j] - '0';
        //printf("x = %d \n", x);
        j++;

        //printf("%c" ,spec[j]);
        y = spec[j] - '0';
        //printf("y = %d \n", y);
        j++;

       // printf("%c", spec[j-3]);
        if (spec[j-3] == 'C' || spec[j-3] == 'B' || spec[j-3] == 'D' || spec[j-3] == 'S' || spec[j-3] == 'P')
        {
           // printf("\nHere\n");
            //check1 = add_ship_horizontal(&GAME->players[player], x, y, length);
            //check1 = add_ship_horizontal(&player_info, x, y, length);
            check1 = add_ship_horizontal(player_info, x, y, length);
            //printf("H Updated SHIPS: %llu\n" , player_info->ships);
            //printf("H Updated SHIPS: %llu\n" ,&GAME->players[player].ships);
            //printf("H Updated SHIPS: %llu\n" ,game->players[player].ships);
            //printf("Updated SHIPS: %llu\n" , &GAME->players[player].ships);
            if (check1 == -1)
            {
                return -1;
            }
        }
        else if (spec[j-3] == 'c' || spec[j-3] == 'b' || spec[j-3] == 'd' || spec[j-3] == 's' || spec[j-3] == 'p')
        {
            //add_ship_vertical(&GAME->players[player], x, y, length);
            //check1 = add_ship_vertical(&GAME->players[player], x, y, length);
            check1 = add_ship_vertical(player_info, x, y, length);
            //printf("V Updated SHIPS: %llu\n" , player_info->ships);
            //printf("V Updated SHIPS: %llu\n" ,&GAME->players[player].ships);
            //printf("V Updated SHIPS: %llu\n" ,game->players[player].ships);
            if (check1 == -1)
            {
                return -1;
            }
        }
        else
        {

        }

    }
    if (player == 1)
    {
        game->status = PLAYER_0_TURN;
    }
    return 1;


    // if it is valid, you should write the corresponding unsigned
    // long long value into the Game->players[player].ships data
    // slot and return 1
    //
    // if it is invalid, you should return -1
}

int add_ship_horizontal(player_info *player, int x, int y, int length) {
    // implement this as part of Step 2
    // returns 1 if the ship can be added, -1 if not
    // hint: this can be defined recursively


    int newx = x;
    int newy = y;
    unsigned long long t;
    unsigned long long p = player->ships;
    for (int i = x; i < (x + length); i++)
    {
        //printf("\nSHIPS H: %llu\n" ,p);
        t = xy_to_bitval(newx,newy);
        //printf("XY : %d %d \n" , newx, newy);
        //printf("xy to bit val: %llu\n", t);
        //player->ships = player->ships | t;
        //printf("New SHIPS: %llu\n" ,player->ships);
        newx += 1;
        if ((t & p) != 0)
        {
            //printf("Should be -1\n");
            return -1;
        }
        else
        {
            p = p | t;
            //printf("New SHIPS: %llu\n" ,p);
        }
    }
    //printf("Should be 1\n\n");
    //player->ships = t & player->ships;
    player->ships = p;
    //printf("Updated SHIPS H: %llu\n" ,player->ships);
    return 1;

}

int add_ship_vertical(player_info *player, int x, int y, int length) {
    // implement this as part of Step 2
    // returns 1 if the ship can be added, -1 if not
    // hint: this can be defined recursively


    int newx = x;
    int newy = y;
    unsigned long long t;
    unsigned long long p = player->ships;
    for (int i = y; i < (y + length); i++)
    {
        //printf("\nSHIPS V: %llu\n" ,p);
        t = xy_to_bitval(newx,newy);
       // printf("XY : %d %d \n" , newx, newy);
       // printf("xy to bit val: %llu\n", t);
        //player->ships = player->ships | t;
        //printf("New SHIPS: %llu\n" ,player->ships);
        newy += 1;
        if ((t & p) != 0)
        {
          //  printf("Should be -1\n");
            return -1;
        }
        else
        {
            p = p | t;
         //   printf("New SHIPS: %llu\n" ,p);
        }
    }
   // printf("Should be 1\n");
    //player->ships = t & player->ships;
    player->ships = p;
    //printf("Updated SHIPS V: %llu\n" ,player->ships);
    return 1;

}