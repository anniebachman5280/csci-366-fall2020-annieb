//
// Created by carson on 5/20/20.
//

#include "stdio.h"
#include "stdlib.h"
#include "server.h"
#include "char_buff.h"
#include "game.h"
#include "repl.h"
#include "pthread.h"
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h>    //inet_addr
#include<unistd.h>    //write

static game_server *SERVER;

void init_server() {
    if (SERVER == NULL) {
        SERVER = calloc(1, sizeof(struct game_server));
    } else {
        printf("Server already started");
    }
}

int handle_client_connect(int player) {
    // STEP 9 - This is the big one: you will need to re-implement the REPL code from
    // the repl.c file, but with a twist: you need to make sure that a player only
    // fires when the game is initialized and it is there turn.  They can broadcast
    // a message whenever, but they can't just shoot unless it is their turn.
    //
    // The commands will obviously not take a player argument, as with the system level
    // REPL, but they will be similar: load, fire, etc.
    //
    // You must broadcast informative messages after each shot (HIT! or MISS!) and let
    // the player print out their current board state at any time.
    //
    // This function will end up looking a lot like repl_execute_command, except you will
    // be working against network sockets rather than standard out, and you will need
    // to coordinate turns via the game::status field.

    int otherPlayer;
    if (player == 1)
    {
        otherPlayer = 0;
    }
    else if (player == 0)
    {
        otherPlayer = 1;
    }
    else{
        printf("player error");
    }

        int client_socket_fd = SERVER->player_sockets[player];
        char raw_buffer[2000];
        char_buff *input_buffer = cb_create(2000);
        char_buff *output_buffer = cb_create(2000);

        char_buff *p = cb_create(2000);
        char_buff *saybuff = cb_create(2000);

        int read_size;
        if (player == 0) {
            cb_append(output_buffer, "Welcome to the battleBit server Player 0 \n");
            cb_append(output_buffer, "battleBit (? for help) > ");
        }
        else if (player == 1) {
        cb_append(output_buffer, "Welcome to the battleBit server Player 1 \n");
        cb_append(output_buffer, "battleBit (? for help) > ");
    }
        cb_write(client_socket_fd, output_buffer);

        while((read_size = recv(client_socket_fd, raw_buffer, 2000, 0)) > 0)
        {
            cb_reset(output_buffer);
            cb_reset(input_buffer);
            if (read_size > 0) {
                raw_buffer[read_size] = '\0';

                cb_append(input_buffer, raw_buffer);

                char *command = cb_tokenize(input_buffer, " \r\n");

                if(strcmp(command, "help") == 0)
                {
                    cb_append(output_buffer, "? - show help\n");
                    cb_append(output_buffer, "load <string> - load a ship layout \n");
                    cb_append(output_buffer, "show - shows the board \n");
                    cb_append(output_buffer, "fire [0-7] [0-7] - fire at the given position\n");
                    cb_append(output_buffer, "say <string> - Send the string to all players as part of a chat\n");
                    cb_append(output_buffer, "exit \n");

                    cb_write(client_socket_fd, output_buffer);
                }
                else if(strcmp(command, "?") == 0)
                {
                    cb_append(output_buffer, "? - show help\n");
                    cb_append(output_buffer, "load <string> - load a ship layout \n");
                    cb_append(output_buffer, "show - shows the board \n");
                    cb_append(output_buffer, "fire [0-7] [0-7] - fire at the given position\n");
                    cb_append(output_buffer, "say <string> - Send the string to all players as part of a chat\n");
                    cb_append(output_buffer, "exit \n");

                    cb_write(client_socket_fd, output_buffer);
                }
                else if (strcmp(command, "quit") == 0){
                    close(client_socket_fd);
                }
                else if (strcmp(command, "exit") == 0){
                    close(client_socket_fd);
                }
                else if (strcmp(command, "show") == 0){
                   repl_print_board(game_get_current(), player,  output_buffer);
                   cb_write(client_socket_fd, output_buffer);

                    cb_reset(output_buffer);
                    cb_append(output_buffer, "\nbattleBit (? for hekp) > ");
                    cb_write(client_socket_fd, output_buffer);
                }
                else if (strcmp(command, "load") == 0){
                    char* arg1 = cb_next_token(input_buffer);

                    if ((game_get_current()->status > 1)) {
                        cb_append(output_buffer, "Already Loaded Board");
                        cb_write(client_socket_fd, output_buffer);
                    }
                    else {
                        if (player == 0) {
                            game_load_board(game_get_current(), player, arg1);
                            cb_append(output_buffer, "Waiting On Player 1");
                            cb_write(client_socket_fd, output_buffer);
                        }
                        else if (player == 1){
                            game_load_board(game_get_current(), player, arg1);
                            cb_append(output_buffer, "All Player Boards Loaded\n");
                            cb_append(output_buffer, "Player 0 Turn");
                            cb_write(client_socket_fd, output_buffer);
                        }
                    }
                    cb_reset(output_buffer);
                    cb_append(output_buffer, "\nbattleBit (? for hekp) > ");
                    cb_write(client_socket_fd, output_buffer);
                }
                else if (strcmp(command, "fire") == 0) {
                    char* arg1 = cb_next_token(input_buffer);
                    char* arg2 = cb_next_token(input_buffer);
                    int n1;
                    int n2;
                    if ((player == 1) && (game_get_current()->status == 2)) {
                        //printf("Its Player's 0 turn");
                        cb_append(output_buffer, "Player 0 Turn");
                        cb_write(client_socket_fd, output_buffer);
                    } else if ((player == 0) && (game_get_current()->status == 3)) {
                        //printf("Its Player's 1 turn");
                        cb_append(output_buffer, "Player 1 Turn");
                        cb_write(client_socket_fd, output_buffer);
                    }
                    else if ((game_get_current()->status < 2)) {
                        //printf("Its Player's 1 turn");
                        cb_append(output_buffer, "Game Has Not Begun!");
                        cb_write(client_socket_fd, output_buffer);
                    }
                    else {
                        n1 = atoi(arg1);
                        n2 = atoi(arg2);
                        char_buff *firem = cb_create(2000);
                        int f = game_fire(game_get_current(), player, n1, n2);
                        cb_append(firem, "\n");
                        if (f == 0) {
                            if(player == 0)
                            {
                                cb_append(firem, "Player 0 fires at ");
                                cb_append(firem, arg1);
                                cb_append(firem, " ");
                                cb_append(firem, arg2);
                                cb_append(firem, " - MISS\n");
                                server_broadcast(firem);
                            }
                            if (player == 1)
                            {
                                cb_append(firem, "Player 1 fires at ");
                                cb_append(firem, arg1);
                                cb_append(firem, " ");
                                cb_append(firem, arg2);
                                cb_append(firem, " - MISS\n");
                                server_broadcast(firem);
                            }
                        } else if (f == 1) {
                            if(player == 0)
                            {
                                cb_append(firem, "Player 0 fires at ");
                                cb_append(firem, arg1);
                                cb_append(firem, " ");
                                cb_append(firem, arg2);
                                cb_append(firem, " - HIT\n");
                                server_broadcast(firem);
                            }
                            if (player == 1)
                            {
                                cb_append(firem, "Player 1 fires at ");
                                cb_append(firem, arg1);
                                cb_append(firem, " ");
                                cb_append(firem, arg2);
                                cb_append(firem, " - HIT\n");
                                server_broadcast(firem);
                            }
                        }
                        cb_reset(firem);
                        if (game_get_current()->status == 4)
                        {
                            cb_append(firem, "PLAYER 0 WINS!");
                            server_broadcast(firem);
                            puts(" ");

                        } else if (game_get_current()->status == 5)
                        {
                            cb_append(firem, "PLAYER 1 WINS!");
                            server_broadcast(firem);
                        }
                    }
                    cb_reset(output_buffer);
                    cb_append(output_buffer, "\nbattleBit (? for hekp) > ");
                    cb_write(client_socket_fd, output_buffer);
                }
                else if (strcmp(command, "say") == 0){
                   char *string = input_buffer->buffer;
                    cb_append(p, "\n");
                    char *n = "";
                    if (player == 1){
                        cb_append(p, "Player 1 says: ");
                    }
                    else if (player == 0)
                    {
                        cb_append(p, "Player 0 says: ");
                    }
                    char *test = cb_next_token(input_buffer);
                    while (test != NULL)
                   {
                        cb_append(p, test);
                        cb_append(p, " ");
                        test = cb_next_token(input_buffer);
                   }
                    cb_append(p, "\n");
                    server_broadcast(p);

                    cb_reset(p);

                    cb_reset(output_buffer);
                    cb_append(output_buffer, "\nbattleBit (? for hekp) > ");
                    cb_write(client_socket_fd, output_buffer);

                }
                else if (command != NULL){
                    cb_append(output_buffer, "Command was :");
                    cb_append(output_buffer, command);
                    cb_write(client_socket_fd, output_buffer);
                }
                else
                {
                    cb_reset(output_buffer);
                    cb_append(output_buffer, "\nbattleBit (? for hekp) > ");
                    cb_write(client_socket_fd, output_buffer);
                }
                cb_reset(output_buffer);
            }
        }
}

void server_broadcast(char_buff *msg) {
    cb_write(SERVER->player_sockets[0], msg);
    cb_write(SERVER->player_sockets[1], msg);
    printf("%s", msg->buffer);
}

int run_server() {
    // STEP 8 - implement the server code to put this on the network.
    // Here you will need to initalize a server socket and wait for incoming connections.
    //
    // When a connection occurs, store the corresponding new client socket in the SERVER.player_sockets array
    // as the corresponding player position.
    //
    // You will then create a thread running handle_client_connect, passing the player number out
    // so they can interact with the server asynchronously

    int server_socket_fd = socket(AF_INET,
                                  SOCK_STREAM,
                                  IPPROTO_TCP);
    if (server_socket_fd == -1) {
        printf("Could not create socket\n");
    }

    int yes = 1;
    setsockopt(server_socket_fd,
               SOL_SOCKET,
               SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in server;

    // fill out the socket information
    server.sin_family = AF_INET;
    // bind the socket on all available interfaces
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(9876);

    int request = 0;
    if (bind(server_socket_fd,
            // Again with the cast
             (struct sockaddr *) &server,
             sizeof(server)) < 0) {
        puts("Bind failed");
    } else {
        puts("Bind worked!");
        listen(server_socket_fd, 88);

        //Accept an incoming connection
        puts("Waiting for incoming connections...");
        //puts("battleBit (? for help) > ");

        struct sockaddr_in client;
        socklen_t size_from_connect;
        int client_socket_fd;
        int player = 0;
        //printf("%1d\n", player);
        while ((client_socket_fd = accept(server_socket_fd,
                                          (struct sockaddr *) &client,
                                          &size_from_connect)) > 0) {
            SERVER->player_sockets[player] = client_socket_fd;
            pthread_create(&SERVER->player_threads[player], NULL,  handle_client_connect, player);
            player++;
            if (player > 1) {
                break;
            }
        }
    }
}

int server_start() {
    // STEP 7 - using a pthread, run the run_server() function asynchronously, so you can still
    // interact with the game via the command line REPL
    init_server();
    pthread_create(&SERVER->server_thread, NULL, (void *) run_server, NULL);
}
