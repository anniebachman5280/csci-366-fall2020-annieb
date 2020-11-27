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


    // set otherplayer to be 0 or 1 depending on what player is
    int otherPlayer;
    if (player == 1)
    {
        otherPlayer = 0;
    }
    if (player == 0)
    {
        otherPlayer = 1;
    }

    // get the bitval for the fire coordniate
    unsigned long long fire = xy_to_bitval(x, y);

    // bit set the shot position for the fire
    game->players[player].shots = game->players[player].shots | fire;

    // check for bit set of the ships and fired shot, if != 0, then it was a hit
    if(((game->players[otherPlayer].ships) & fire) != 0)
    {
        //printf("HIT\n");

        // update hits and ships
        game->players[player].hits = game->players[player].hits | fire;
        game->players[otherPlayer].ships = game->players[otherPlayer].ships ^ fire;

        // change game status for turns depending on who is the current player
        if(player == 0)
        {
            game->status = PLAYER_1_TURN;
        }
        else if (player == 1)
        {
            game->status = PLAYER_0_TURN;
        }

        // change game staatus for if a player ships is 0, no more ships left
        if(game->players[otherPlayer].ships == 0)
        {
            // player 0 wins
            if(player == 0)
            {
                game->status = PLAYER_0_WINS;
            }
            // player 1 wins
            else if (player == 1)
            {
                game->status = PLAYER_1_WINS;
            }

        }
        return 1;
    }

    // case for a miss shot
    else if (((game->players[otherPlayer].ships) & fire) == 0)
    {
          //  printf("MISS\n");

        // change player turns
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

    //case for player wining game, checking other player this time
    if(game->players[otherPlayer].ships == 0)
    {
        // player 1 wins
        if(player == 0)
        {
            game->status = PLAYER_1_WINS;
            return 0;
        }
        // player 0 wins
        else if (player == 1)
        {
            game->status = PLAYER_0_WINS;
            return 0;
        }
    }
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
    // return unsigned long long value for x,y coord
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

    // cases for wrong specs
    // empty spec
    if(spec == NULL)
    {
        //printf("Spec Null \n");
        return -1;
    }

    // not correct length spec
    if (strlen(spec) != 15)
   {
        //printf("Not 15 length \n");
        return -1;
    }
    // does not begin with a character
    else if(!isalpha(spec[0]))
    {
        //printf("Not Char \n");
        return -1;
    }
    // next to numbers are not numbers
    else if (!isdigit(spec[1]) || !isdigit(spec[2]))
    {
        //printf("Not digit \n");
        return -1;
    }
    // check char in the right place
    else if(!isalpha(spec[3]))
    {
        //printf("Not Char \n");
        return -1;
    }
    // next to numbers are not numbers
    else if (!isdigit(spec[4]) || !isdigit(spec[5]))
    {
        //printf("Not digit \n");
        return -1;
    }
    else if(!isalpha(spec[6]))
    {
        //printf("Not Char \n");
        return -1;
    }
        // next to numbers are not numbers
    else if (!isdigit(spec[7]) || !isdigit(spec[8]))
    {
        //printf("Not digit \n");
        return -1;
    }
    else if(!isalpha(spec[9]))
    {
        //printf("Not Char \n");
        return -1;
    }
        // next to numbers are not numbers
    else if (!isdigit(spec[10]) || !isdigit(spec[11]))
    {
        //printf("Not digit \n");
        return -1;
    }
    else if(!isalpha(spec[12]))
    {
        //printf("Not Char \n");
        return -1;
    }
        // next to numbers are not numbers
    else if (!isdigit(spec[13]) || !isdigit(spec[14]))
    {
        //printf("Not digit \n");
        return -1;
    }

    // array to keep track of which char has been used, in order c, b, d, s, p
    int usedL [] = {0 ,0, 0, 0, 0};

    // create pointer to what is at address of game player
    player_info *player_info = &game->players[player];

    // vars
    int j = 0; // iterator
    int x; // x cord
    int y; // y cord
    int length; // length of ship
    int check1; // check for valid, return -1 if not valid

    // begin for loop, checking three array index at a time checks for which letter is given, sets
    // length depending on the letter, calls the add hor/vert depending on letter case
    // also checks for off board ships
    for (int i = 0; i < 5; i++)
    {
        if (spec[j] == 'C' || spec[j] == 'c')
        {
           // printf("Length = 5\n" );
            length = 5;
            // check to see if 'c' is clready used in the spec
            if (usedL[0] == 1)
            {
                //printf("C/c already used \n");
                return -1;
            }
            // else, set c to be used now
            else
            {
                usedL[0] = 1;
            }
           // printf("+1: %d, +2: %d \n", spec[j+1]-'0', spec[j+2]-'0');
            if(spec[j] == 'C')
            {
                // length makes ship off the board
                if((spec[j+1]-'0') > 3)
                {
                    return -1;
                }
            }
            else if (spec[j] == 'c')
            {
                // length makes ship off the board
                if((spec[j+2]-'0') > 3)
                {
                    return -1;
                }
            }
        }
        else if (spec[j] == 'B' || spec[j] == 'b')
        {
            length = 4;
            // check if b is used yet
            if (usedL[1] == 1)
            {
                //printf("B/b already used \n");
                return -1;
            }
            // if not use, mark as used
            else
            {
                usedL[1] = 1;
            }
           // printf("+1: %d, +2: %d \n", spec[j+1]-'0', spec[j+2]-'0');
            if(spec[j] == 'B')
            {
                // length makes ship off the board
                if((spec[j+1]-'0') > 4)
                {
                    return -1;
                }
            }
            else if (spec[j] == 'b')
            {
                // length makes ship off the board
                if((spec[j+2]-'0') > 4)
                {
                    return -1;
                }
            }
        }
        else if (spec[j] == 'D' || spec[j] == 'd')
        {
            length = 3;
            // check is d is used yet
            if (usedL[2] == 1)
            {
                //printf("D/d already used \n");
                return -1;
            }
            // set d to be used
            else
            {
                usedL[2] = 1;
            }
            //printf("+1: %d, +2: %d \n", spec[j+1]-'0', spec[j+2]-'0');
            if(spec[j] == 'D')
            {
                // length makes ship off the board
                if((spec[j+1]-'0') > 5)
                {
                    return -1;
                }
            }
            else if (spec[j] == 'd')
            {
                // length makes ship off the board
                if((spec[j+2]-'0') > 5)
                {
                    return -1;
                }
            }
        }
        else if (spec[j] == 'S' || spec[j] == 's')
        {
            length = 3;
            // check if s is used yet
            if (usedL[3] == 1)
            {
                //printf("S/s already used \n");
                return -1;
            }
            // set s to be used now
            else
            {
                usedL[3] = 1;
            }
           // printf("+1: %d, +2: %d \n", spec[j+1]-'0', spec[j+2]-'0');
            if(spec[j] == 'S')
            {
                // length makes ship off the board
                if((spec[j+1]-'0') > 5)
                {
                    return -1;
                }
            }
            else if (spec[j] == 's')
            {
                // length makes ship off the board
                if((spec[j+2]-'0') > 5)
                {
                    return -1;
                }
            }
        }
        else if (spec[j] == 'P' || spec[j] == 'p')
        {
            length = 2;
            // chekc if p has been used yet
            if (usedL[4] == 1)
            {
                //printf("P/p already used \n");
                return -1;
            }
            // set p to be used
            else
            {
                usedL[4] = 1;
            }
           // printf("+1: %d, +2: %d \n", spec[j+1]-'0', spec[j+2]-'0');
            if(spec[j] == 'P')
            {
                // length makes ship off the board
                if((spec[j+1]-'0') > 6)
                {
                    return -1;
                }
            }
            else if (spec[j] == 'p')
            {
                // length makes ship off the board
                if((spec[j+2]-'0') > 6)
                {
                    return -1;
                }
            }
        }
        // wrong spec, length of ship is 0
        else
        {
            length = 0;
            return -1;
        }

        // next spec index
        j++;

        // get x into int form
        x = spec[j] - '0';

        // next spec index
        j++;

        // get y into int form
        y = spec[j] - '0';

        // next spec index
        j++;

        // If capital, add horizontal with valid check
        if (spec[j-3] == 'C' || spec[j-3] == 'B' || spec[j-3] == 'D' || spec[j-3] == 'S' || spec[j-3] == 'P')
        {
            check1 = add_ship_horizontal(player_info, x, y, length);
            if (check1 == -1)
            {
                return -1;
            }
        }
        // if lower case, add verticly, with valid check
        else if (spec[j-3] == 'c' || spec[j-3] == 'b' || spec[j-3] == 'd' || spec[j-3] == 's' || spec[j-3] == 'p')
        {
            check1 = add_ship_vertical(player_info, x, y, length);
            if (check1 == -1)
            {
                return -1;
            }
        }
        // else error
        else{
        }

    }
    // if player 1 has loaded a board, start game with player 0 going first.
    // Here player 0 must load before player 1 for game to work
    if (player == 1)
    {
        game->status = PLAYER_0_TURN;
    }
    // valid return
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

    // vars
    int newx = x;
    int newy = y;
    unsigned long long t;
    // new var for ship, set ships to this new var at the end
    unsigned long long p = player->ships; // temp var

    // for each length of the ship, get bitval and check to make sure there is no ship there already
    // if no new ship, set bitval position and set p to be set with new ship posotion
    for (int i = x; i < (x + length); i++)
    {
        t = xy_to_bitval(newx,newy);
        newx += 1;
        if ((t & p) != 0)
        {
            return -1;
        }
        else
        {
            p = p | t;
        }
    }
    // set ships to the new value
    player->ships = p;
    return 1;

}

int add_ship_vertical(player_info *player, int x, int y, int length) {
    // implement this as part of Step 2
    // returns 1 if the ship can be added, -1 if not
    // hint: this can be defined recursively

    // vars
    int newx = x;
    int newy = y;
    unsigned long long t;
    // new var for ship, set ships to this new var at the end
    unsigned long long p = player->ships; // temp var

    // for each length of the ship, get bitval and check to make sure there is no ship there already
    // if no new ship, set bitval position
    for (int i = y; i < (y + length); i++)
    {
        t = xy_to_bitval(newx,newy);
        newy += 1;
        if ((t & p) != 0)
        {
            return -1;
        }
        else
        {
            p = p | t;
        }
    }
    // set ships now
    player->ships = p;
    return 1;

}