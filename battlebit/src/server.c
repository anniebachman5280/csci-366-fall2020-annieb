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

    printf("\nplayer %d\n", player);
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


    printf("Otherplayer %d\n", otherPlayer);


        int client_socket_fd = SERVER->player_sockets[player];
        char raw_buffer[2000];
        char_buff *input_buffer = cb_create(2000);
        char_buff *output_buffer = cb_create(2000);

        char_buff *p = cb_create(2000);

        int read_size;
        cb_append(output_buffer, "\nbattleBit (? for help) >");
        cb_write(client_socket_fd, output_buffer);

        while((read_size = recv(client_socket_fd, raw_buffer, 2000, 0)) > 0)
        {
            cb_reset(output_buffer);
            cb_reset(input_buffer);
            if (read_size > 0) {
                raw_buffer[read_size] = '\0';

                cb_append(input_buffer, raw_buffer);

                printf("\n\n1\n%s", input_buffer->buffer);

                char *command = cb_tokenize(input_buffer, " \r\n");
                char* arg1 = cb_next_token(input_buffer);
                char* arg2 = cb_next_token(input_buffer);
                int n1;
                int n2;
                int n3;


                //printf("\no\n%s", command);
                if (player == 0)
                {
                    printf("%d", player);
                    cb_append(output_buffer, "you are player 0\n");
                    cb_write(client_socket_fd, output_buffer);
                }
                else if (player == 1)
                {
                    printf("%d", player);
                    cb_append(output_buffer, "you are player 1\n");
                    cb_write(client_socket_fd, output_buffer);
                }
                else
                {
                    printf("%d", player);
                    cb_append(output_buffer, "you are player not 1 or 0\n");
                    cb_write(client_socket_fd, output_buffer);
                }

                if(strcmp(command, "help") == 0)
                {

                    cb_append(output_buffer, "A useful help message...");

                    cb_append(output_buffer, "? - show help\n");
                    cb_append(output_buffer, "load <string> - load a ship layout file for the given player\n");
                    cb_append(output_buffer, "show - shows the board for the given player\n");
                    cb_append(output_buffer, "fire [0-7] [0-7] - fire at the given position\n");
                    cb_append(output_buffer, "say <string> - Send the string to all players as part of a chat\n");
                    cb_append(output_buffer, "\"exit - quit the server\\n\"");

                    cb_append(output_buffer, command);

                    cb_write(client_socket_fd, output_buffer);
                }
                else if(strcmp(command, "?") == 0)
                {
                    cb_append(output_buffer, "? - show help\n");
                    cb_append(output_buffer, "load <string> - load a ship layout file for the given player\n");
                    cb_append(output_buffer, "show- shows the board for the given player\n");
                    cb_append(output_buffer, "fire [0-7] [0-7] - fire at the given position\n");
                    cb_append(output_buffer, "say <string> - Send the string to all players as part of a chat\n");
                    cb_append(output_buffer, "\"exit - quit the server\\n\"");

                    cb_append(output_buffer, command);

                    cb_write(client_socket_fd, output_buffer);
                }
                else if (strcmp(command, "quit") == 0){
                    close(client_socket_fd);
                }
                else if (strcmp(command, "exit") == 0){
                    close(client_socket_fd);
                }
                else if (strcmp(command, "show") == 0){
                    //game_get_current()->players[1].ships;
                   // game_load_board(game_get_current(), , );
                   // n1 = atoi(arg1);
                   repl_print_board(game_get_current(), player,  output_buffer);
                    //cb_print(output_buffer);
                    //cb_append(output_buffer, p);
                    cb_write(client_socket_fd, output_buffer);
                    //repl_print_ships();
                }
                else if (strcmp(command, "load") == 0){
                    //game_get_current()->players[1].ships;
                    n1 = atoi(arg1);
                    cb_append(output_buffer, arg1);
                    cb_write(client_socket_fd, output_buffer);
                    game_load_board(game_get_current(), player, arg1);
                    //repl_print_ships();
                }
                else if (strcmp(command, "fire") == 0){

                n1 = atoi(arg1);
                n2 = atoi(arg2);
                int f = game_fire(game_get_current(), player, n1,n2);
                    if (f == 0){
                        cb_append(output_buffer, "\nMISS\n");
                        cb_write(client_socket_fd, output_buffer);
                    }
                    else if (f == 1)
                    {
                        cb_append(output_buffer, "\nHIT\n");
                        cb_write(client_socket_fd, output_buffer);
                    }
                game *pGame = game_get_current();
                printf("%u", pGame->status);
                printf("%u", game_get_current()->status);

                }
                else if (strcmp(command, "say") == 0){
                   char *string = input_buffer->buffer;

                    cb_append(p, "\n");
                    while (cb_next_token(input_buffer) != NULL)
                    {
                        printf("done");
                        char *n = cb_next_token(input_buffer);
                        cb_append(p, n);
                        cb_append(p, " ");
                        server_broadcast(p);
                      //  printf("1%s\n", n);
                        //printf("2%s", );
                        //cb_write(client_socket_fd, output_buffer);

                    }

                   // server_broadcast(string);

                  // printf("2%s", string);
                //   printf("3%s", input_buffer);
               //     printf("3%s", input_buffer->buffer);
                //  char *string = &input_buffer->buffer;
                   // printf("%s", string->buffer);
                   // printf("%s", *string);

                  // server_broadcast(input_buffer);
                 // cb_print(input_buffer);
                 // cb_print();
                   // printf("%s", *command);
                  // char *str = input_buffer;
                 //  printf("%s", str);

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
                cb_append(output_buffer, "\nbattleBit (? for hekp) > ");
                cb_write(client_socket_fd, output_buffer);
            }
        }

}

void server_broadcast(char_buff *msg) {
    // send message to all players
 //   for (int i =0; i < 1; ++i){
        //SERVER->player_sockets[i];
     //   cb_write(SERVER->player_sockets[i], msg);
   // }
   // char_buff *m = cb_create(2000);
      // cb_append(m, msg->buffer);
  //  cb_append(m, msg->buffer);
  // cb
    cb_write(SERVER->player_sockets[0], msg);
    cb_write(SERVER->player_sockets[1], msg);
    printf("1%s", msg->buffer);
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

        struct sockaddr_in client;
        socklen_t size_from_connect;
        int client_socket_fd;
        int player = 0;
        printf("%1d\n", player);
        while ((client_socket_fd = accept(server_socket_fd,
                                          (struct sockaddr *) &client,
                                          &size_from_connect)) > 0) {
            printf("%2d\n", player);
            SERVER->player_sockets[player] = client_socket_fd;
            pthread_create(&SERVER->player_threads[player], NULL,  handle_client_connect, player);
            printf("%3d\n", player);
            player++;
            printf("%4d\n", player);
            //SERVER->player_sockets[player] = client_socket_fd;
           // pthread_create(&SERVER->player_threads, NULL, handle_client_connect, player);
            printf("%5d\n", player);
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


    /* Testing with one terminal */

/*    int server_socket_fd = socket(AF_INET,
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

        struct sockaddr_in client;
        socklen_t size_from_connect;
        int client_socket_fd;

        while ((client_socket_fd = accept(server_socket_fd,
                                          (struct sockaddr *) &client,
                                          &size_from_connect)) > 0) {
            char raw_buffer[2000];
            char_buff *input_buffer = cb_create(2000);
            char_buff *output_buffer = cb_create(2000);

            int read_size;
            cb_append(output_buffer, "\nbattleBit (? for help) > ");
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
                        cb_append(output_buffer, "A useful help message...");
                        cb_append(output_buffer, command);

                        cb_write(client_socket_fd, output_buffer);
                    }
                    else if (strcmp(command, "quit") == 0){
                        close(client_socket_fd);
                    }
                    else if (command != NULL){
                        cb_append(output_buffer, "Command was :");
                        cb_append(output_buffer, command);

                        cb_write(client_socket_fd, output_buffer);
                    }
                    cb_reset(output_buffer);
                    cb_append(output_buffer, "\nbattleBit (? for hekp) > ");
                    cb_write(client_socket_fd, output_buffer);
                }
            }
        }
    }*/
}
