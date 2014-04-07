/** 
 * \file   server_support.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  
 *
 * \details
 * 
 * 
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "servSupport.h"

int validate_args(int argc, char **argv, int *min, int *max, int *roundTime) {

    if (argc < 6 || argc%2 == 1) {
        fprintf(stderr, "Usage: serv round_time minplayers maxplayers port ");
        fprintf(stderr, "qfile [port qfile ...]\n");
        return ERR_USAGE;
    }

    if (!str_is_num(argv[1], strlen(argv[1]))
            || !str_is_num(argv[2], strlen(argv[2])) 
            || !str_is_num(argv[3], strlen(argv[3]))
            || !sscanf(argv[1], "%d", roundTime) 
            || !sscanf(argv[2], "%d", min)
            || !sscanf(argv[3], "%d", max) || *min < 0 || *max < 0
            || *roundTime < 0) {
        fprintf(stderr, "Bad Number\n");
        return ERR_NUM;
    }

    return 0;
}

int get_ports(int argc, char **argv, GameData *games) {
    int port = 0;

    for (int i = 4; i < argc; i += 2) {
        if (str_is_num(argv[i], strlen(argv[1])) 
                && sscanf(argv[i], "%d", &port) && port > 0
                && port < 65536) {
            games[(i-4)/2].port = port;

        } else {
            return ERR_PORT;
        }
    }

    return 0;
}

int get_questions(int argc,  char **argv, GameData *games) {

    int fd;

    char *buffer = (char*) malloc(sizeof(char));

    for (int i = 0; i < (argc-4)/2; ++i) {
        QuestionData temp;

        if ((fd = open(argv[i*2+5], O_RDONLY)) == -1) {
            free(buffer);
            return ERR_FILE;
        }

        int marker = 0;
        int start = 0;
        int count = 0;
        int err = 0;
        int len = 0;
        int bufLen = 0;

        while ((bufLen = get_line(fd, &buffer)) > 0) {
            switch (marker) {
                case 0: // get question
                    get_question(&buffer, &len, &start, &temp, &marker, 
                            &bufLen);
                    
                    break;
                case 1: // get num options and correct 
                    err = get_num(&buffer, &len, &count, &marker, games, 
                            i, &temp);
                    break;
                case 2: // get question options
                    len += bufLen + 2;
                    temp.qBuffer = (char *) realloc(temp.qBuffer, len);
                    if (bufLen > 1) {
                        strcat(temp.qBuffer, buffer);
                    } else {
                        strcat(temp.qBuffer, "");
                    }
                    strcat(temp.qBuffer, "\n");
                    count++;

                    if (count == temp.options) {
                        marker = 3;
                    }
                    break;

                case 3: //last line of question
                    if (bufLen != 1) {
                        free(temp.qBuffer);
                        free(buffer);
                        return ERR_FILE + games[i].numQuestions;
                    }

                    marker = 0;
                    start = 0;
                    games[i].numQuestions++;
                    games[i].question = (QuestionData *)  
                            realloc(games[i].question, sizeof(QuestionData) * 
                            games[i].numQuestions);

                    games[i].question[games[i].numQuestions-1] = temp;

                    break;
            }

            if (err) {
                return err;
            }
        }

        close(fd);

        if (marker != 0) {
            free(temp.qBuffer);
            free(buffer);
            return ERR_FILE + games[i].numQuestions;
        }
    }

    free(buffer);
    return 0;
}

void get_question (char **buffer, int *len, int *start, QuestionData *temp, 
        int *marker, int *bufLen) {
    if (*start == 0) {
        *len = strlen(*buffer) + 2;
        temp->qBuffer = (char *) malloc(sizeof(char) * *len);
        temp->qBuffer[0] = '\0';
        strcat(temp->qBuffer, *buffer);
        strcat(temp->qBuffer, "\n");
        *start = 1;
    } else {
        if (!strcmp(*buffer, "----")) {
            *len += 3;
            temp->qBuffer = (char *) realloc(temp->qBuffer, *len);
            strcat(temp->qBuffer, ".");
            *marker = 1;
        } else if (*bufLen == 1) {
            *len += 2;
            temp->qBuffer = (char *) realloc(temp->qBuffer, *len);
        } else {
            *len += strlen(*buffer) + 2;
            temp->qBuffer = (char *) realloc(temp->qBuffer, *len);
            strcat(temp->qBuffer, *buffer);
        }

        strcat(temp->qBuffer, "\n");
    }
}

int get_num (char **buffer, int *len, int *count, int *marker, 
        GameData *games, int i, QuestionData *temp) {
    if (sscanf(*buffer, "%d %d", &temp->options, 
            &temp->correct) != 2) {
        free(*buffer);
        free(temp->qBuffer);
        return ERR_FILE + games[i].numQuestions;
    }
    *len +=  num_digits(temp->options) + 2;
    temp->qBuffer = (char*) realloc(temp->qBuffer, *len);

    sprintf(temp->qBuffer, "%s%d\n", temp->qBuffer, 
        temp->options);

    *count = 0;

    if (temp->options == 0) {
        *marker = 3;
    } else {
        *marker = 2;
    }

    return 0;
}

int open_listen (int port) {
    int fd;
    struct sockaddr_in serverAddr;
    int optVal;

    /* Create TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        exit(ERR_LISTEN);
    }

    /* Allow our local address to be reused immediately */
    optVal = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(int)) < 0) {
        exit(ERR_LISTEN);
    }

    /* Populate address (IP Address & port number) to listen on */
    serverAddr.sin_family = AF_INET;	/* IP v4 */
    serverAddr.sin_port = htons(port);	/* Port number - converted to network
                                        ** byte order */
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);	/* Any local IP  address*/

    /* Bind socket to this particular address */
    if(bind(fd, (struct sockaddr*)&serverAddr, 
            sizeof(struct sockaddr_in)) < 0) {
        exit(ERR_LISTEN);
    }

    /* Start listening on that socket - willing to accept connections.
    * Second argument is maximum queue length of pending connections
    * (SOMAXCONN is 128)
    */
    if(listen(fd, SOMAXCONN) < 0) {
        exit(ERR_LISTEN);
    } 

    return fd;
}

void free_memory (GameData *games, int numGames) {

    for (int i = 0; i < numGames; ++i) {
        for (int j = 0; j < games[i].numQuestions; ++j) {
            free(games[i].question[j].qBuffer);
        }
        free(games[i].question);

        for (int j = 0; j < games[i].numPlayers; ++j) {
            free(games[i].player[j].buffer);
            free(games[i].player[j].name);
            if (games[i].player[j].answer >= IN_GAME) {
                close(games[i].player[j].fd);
            }
        }
        free(games[i].player);

        for (int j = 0; j < games[i].numJoinThreads; ++j) {
            pthread_cancel(games[i].joinThread[j]);
            pthread_join(games[i].joinThread[j], NULL);
        }
        free(games[i].joinThread);

        sem_destroy(&games[i].playerSem);
        sem_destroy(&games[i].fdSem);
    }

    free(games);
}

void send_game_scores (GameData *game, int pNum) {

    int first = 1;

    for (int i = 0; i < game->numPlayers; ++i) {
        if (!game->player[i].inRound) {
            continue;
        }

        send_message(game->player[pNum].fd, (first ? "S" : " "));

        send_message(game->player[pNum].fd, game->player[i].name);
        send_message(game->player[pNum].fd, ":");
        send_integer(game->player[pNum].fd, game->player[i].score);

        first = 0;
    }

    send_message(game->player[pNum].fd, "\n");
}

void send_correct (GameData *game, int pNum, int answer) {

    int first = 1;

    for (int i = 0; i < game->numPlayers; ++i) {
        if (!game->player[i].inRound) {
            continue;
        }

        send_message(game->player[pNum].fd, (first ? "C" : " "));

        send_message(game->player[pNum].fd, game->player[i].name);
        send_message(game->player[pNum].fd, ":");

        if (game->player[i].answer == answer) {
            send_message(game->player[pNum].fd, "Correct");
        }  else if (game->player[i].answer == DISCON
                || game->player[i].answer == IN_GAME) { // In game, no answer
            send_message(game->player[pNum].fd, "TimedOut");
        } else {
            send_message(game->player[pNum].fd, "Incorrect");
        }
        
        first = 0;
    }

    send_message(game->player[pNum].fd, "\n");
}

void send_winner (GameData *game, int pNum, int topScore) {

    int first = 1;

    for (int i = 0; i < game->numPlayers; ++i) {
        if (game->player[i].inRound && game->player[i].score == topScore) {
            send_message(game->player[pNum].fd, (first ? "W" : " "));
            send_message(game->player[pNum].fd, game->player[i].name);
            first = 0;
        }
    }

    send_message(game->player[pNum].fd, "\n");
}

int get_top_score(GameData *game) {
    int topScore = 0;

    for (int i = 0; i < game->numPlayers; ++i) {

        if (game->player[i].answer >= IN_GAME 
                && game->player[i].score > topScore) {
            topScore = game->player[i].score;
        }
    }
    
    return topScore;
}

void *process_connections(void *arg) {
    GameData *game = (GameData *) arg;
    struct sockaddr_in fromAddr;
    socklen_t fromAddrSize;
    int error;
    char hostname[MAXHOSTNAMELEN];
    TempUserData temp;

    temp.game = game;

    while(1) {
        fromAddrSize = sizeof(struct sockaddr_in);

        // Prevent a thread from stealing an existing threads fd without it 
        // getting it first 
        sem_wait(&game->fdSem); 

        /* Block, waiting for a connection request then accept it */
        /* fromAddr will get populated with client address */
        temp.fd = accept(game->fdServer, (struct sockaddr*)&fromAddr, 
                &fromAddrSize);
        if (temp.fd < 0) {
            fprintf(stderr, "Bad Client\n");
            exit(ERR_CLIENT);
        }

        /* Turn client IP address into hostname */
        error = getnameinfo((struct sockaddr*)&fromAddr, fromAddrSize, 
                hostname, MAXHOSTNAMELEN, NULL, 0, 0);
        if(error) {
            fprintf(stderr, "Bad Client\n");
            exit(ERR_CLIENT);
        }
        
        game->joinThread = (pthread_t *) realloc(game->joinThread, 
                sizeof(pthread_t) * (game->numJoinThreads + 1));
        
        pthread_create(&game->joinThread[game->numJoinThreads], NULL, 
                client_connect, (void*) &temp);

        game->numJoinThreads++;
        
    }

    return NULL;
}

void *client_connect(void *arg) {

    //TempUserData *freeme = ;
    GameData *game = ((TempUserData *) arg)->game;
    int fd = ((TempUserData *) arg)->fd;

    sem_post(&game->fdSem);

    char *buffer = (char *) malloc(sizeof(char));
    pthread_cleanup_push(clean, &buffer);

    if (get_line(fd, &buffer)) { 
        if (!strcmp(buffer, "scores")) {
            get_scores(game->allGames, game->numGames, fd);
            close(fd);
            free(buffer);
            return NULL;
        }
    } else {
        close (fd);
        return NULL;
    }

    sem_wait(&game->playerSem);
    int i = game->numPlayers;

    if (get_active_players(game) == game->max) {
        send_message(fd, "$\n");
        close(fd);
        free(buffer);
        sem_post(&game->playerSem);
        return NULL;
    }

    game->player = (PlayerData*) realloc(game->player, 
            sizeof(PlayerData) * (game->numPlayers + 1));
    
    game->player[i].fd = fd;
    game->player[i].name = (char *) malloc(sizeof(char) 
           * ((strlen(buffer) + 1)));
    memcpy(game->player[i].name, buffer, sizeof(char) * (strlen(buffer) + 1));

    game->player[i].buffer = (char *) malloc(sizeof(char));
    game->player[i].score = 0;
    game->player[i].answer = IN_GAME;
    game->player[i].inRound = 0;
    game->player[i].wins = 0;
    game->player[i].discs = 0;
    game->numPlayers++;

    send_message(game->player[i].fd, "Hello Player ");
    send_integer(game->player[i].fd, get_active_players(game));
    send_message(game->player[i].fd, "/");
    send_integer(game->player[i].fd, game->min);
    send_message(game->player[i].fd, ".\n");
    sem_post(&game->playerSem);

    pthread_cleanup_pop(1);

    return NULL;
}

int get_active_players(GameData *game) {
    int tempNumPlayers = 0;

    for (int i = 0; i < game->numPlayers; ++i) {
        if (game->player[i].answer >= IN_GAME) {
            tempNumPlayers++;
        }
    }

    return tempNumPlayers;
}

void get_scores(GameData *games, int numGames, int fd) {

    PlayerData *superPlayer = (PlayerData *) malloc(sizeof(PlayerData));
    int sNumPlayers = 0;

    int super;
    int s;
    
    for (int i = 0; i < numGames; ++i) {
        sem_wait(&games[i].playerSem);
        for (int j = 0; j < games[i].numPlayers; ++j) {
            super = -1;

            for (int k = 0; k < sNumPlayers; ++k) {
                if (!strcmp(superPlayer[k].name, games[i].player[j].name)) {
                    super = k;
                    break;
                }
            }

            if (super > -1) {
                superPlayer[super].inRound++;
                superPlayer[super].discs += games[i].player[j].discs;
                superPlayer[super].wins  += games[i].player[j].wins;

                if (games[i].player[i].answer == DISCON) {
                    superPlayer[super].score += games[i].player[j].score;
                }
            } else {
                s = sNumPlayers;

                superPlayer[s].name = (char *) malloc(sizeof(char) * 
                        (strlen(games[i].player[j].name) + 1));
                memcpy(superPlayer[s].name, games[i].player[j].name, 
                        sizeof(char) * (strlen(games[i].player[j].name) + 1));

                superPlayer[s].discs = games[i].player[j].discs;
                superPlayer[s].wins = games[i].player[j].wins;
                superPlayer[s].inRound = 1;

                if (games[i].player[i].answer == DISCON) {
                    superPlayer[s].score = games[i].player[j].score;
                } else {
                    superPlayer[s].score = 0;
                }

                sNumPlayers++;
                superPlayer = (PlayerData *) realloc(superPlayer, 
                        sizeof(PlayerData) * (sNumPlayers + 1));
            }
        }
        sem_post(&games[i].playerSem);
    }
    

    send_scores(fd, sNumPlayers, superPlayer);

    for (int k = 0; k < sNumPlayers; ++k) {
        free(superPlayer[k].name);
    }

    free(superPlayer);
}

void send_scores(int fd, int sNumPlayers, PlayerData *superPlayer) {
    for (int i = 0; i < sNumPlayers; ++i) {
        send_message(fd, superPlayer[i].name);
        send_message(fd, " played:");
        send_integer(fd, superPlayer[i].inRound);
        send_message(fd, " won:");
        send_integer(fd, superPlayer[i].wins);
        send_message(fd, " disc:");
        send_integer(fd, superPlayer[i].discs);
        send_message(fd, " score:");
        send_integer(fd, superPlayer[i].score);
        send_message(fd, "\n");
    }
}

void clean(void *arg) {
    char **p = arg;
    free(*p);
}

void *client_thread(void* arg) {
    PlayerData *player = (PlayerData *) arg;

    int c = get_line(player->fd, &player->buffer);

    if (c > 1 && str_is_num(player->buffer, c)) {
        sscanf(player->buffer, "%d", &player->answer);
    }

    if (c < 1 || !str_is_num(player->buffer, c)) {
        player->answer = DISCON;
        close(player->fd);
    }

    return NULL;
}

void *game_thread(void *arg) {
    GameData *game = (GameData *) arg;
    game->numPlayers = 0;

    game->fdServer = open_listen(game->port);
    pthread_create(&game->listenThread, NULL, process_connections, 
            (void *) game);

    int qNum = 0;
    int tempNumPlayers = 0;

    while(qNum < game->numQuestions) {

        if (qNum == 0 && restartGame == 0) {
            break;
        }

        if (game->min > get_active_players(game)) {
            continue;
        }

        sem_wait(&game->playerSem);

        tempNumPlayers = game->numPlayers; // So we only close threads created
        for (int i = 0; i < game->numPlayers; ++i) {
            if (game->player[i].answer >= IN_GAME) {
                game->player[i].inRound = TRUE;
            }
        }

        for (int i = 0; i < game->numPlayers; ++i) {
            if (game->player[i].inRound == TRUE) {
                send_game_scores(game, i);

                send_message(game->player[i].fd, 
                        game->question[qNum].qBuffer);

                pthread_create(&game->player[i].pThread, NULL, 
                        client_thread, (void*) &game->player[i]);
            }
        }

        sem_post(&game->playerSem);

        sleep(game->roundTime);

        // This must be calculated before output is displayed
        // Do not merge into 1 loop.
        for (int i = 0; i < tempNumPlayers; ++i) {
            if (!game->player[i].inRound) {
                continue;
            }

            pthread_cancel(game->player[i].pThread);
            pthread_join(game->player[i].pThread, NULL);

            if (game->player[i].answer == DISCON) {
                game->player[i].discs++;
                close(game->player[i].fd);
            }  

            if (game->player[i].answer 
                    == game->question[qNum].correct) {
                game->player[i].score++;
            }
        }

        int topScore = get_top_score(game);

        if (qNum + 1 == game->numQuestions) {
            for (int i = 0; i < tempNumPlayers; ++i) {
                if (game->player[i].inRound
                        && game->player[i].score == topScore) {
                    game->player[i].wins++;
                }
            }
        }

        // Displays output
        for (int i = 0; i < tempNumPlayers; ++i) {
            if (!game->player[i].inRound 
                    || game->player[i].answer == DISCON) {
                continue;
            }

            if (qNum + 1 == game->numQuestions) {
                send_winner(game, i, topScore);
            }

            send_correct(game, i, game->question[qNum].correct);

            if (qNum + 1 == game->numQuestions) {
                send_game_scores(game, i);
            }
        }

        if (qNum + 1 == game->numQuestions) {
            qNum = -1; // Will be incremented to 0 later

            for (int i = 0; i < game->numPlayers; ++i) {
                if (game->player[i].inRound) {
                    game->player[i].answer = DISCON;
                    close(game->player[i].fd);
                }
            }
        }

        // Quits round
        for (int i = 0; i < game->numPlayers; ++i) {
            if (game->player[i].inRound) {
                game->player[i].inRound = FALSE;
            }
        }

        qNum++;

    }

    pthread_cancel(game->listenThread);
    pthread_join(game->listenThread, NULL);

    game->running = 0;
    return NULL;
}

void initialise_game(GameData *games, int numGames, int min, int max, 
        int roundTime) {

    for (int i = 0; i < numGames; ++i) {
        games[i].min = min;
        games[i].max = max;
        games[i].roundTime = roundTime;

        games[i].question = (QuestionData *) malloc(sizeof(QuestionData));
        games[i].numQuestions = 0;

        games[i].player = (PlayerData *) malloc(sizeof(PlayerData));
        games[i].numPlayers = 0;

        games[i].joinThread = (pthread_t *) malloc(sizeof(pthread_t));
        games[i].numJoinThreads = 0;

        games[i].running = 1;

        games[i].allGames = games;
        games[i].numGames = numGames;

        sem_init(&games[i].playerSem, 0, 1);
        sem_init(&games[i].fdSem, 0, 1);
    }
}

void handle_signals(void) {
    // Set up signal handling for SIGHUP and SIGPIPE
    struct sigaction sa;
    sa.sa_handler = sighup_received;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGHUP, &sa, 0);

    struct sigaction sa2;
    sa2.sa_handler = sigpipe_received;
    sa2.sa_flags = SA_RESTART;
    sigemptyset(&sa2.sa_mask);    
    sigaction(SIGPIPE, &sa2, 0);
}

void sighup_received(int s) {
    restartGame = 0;
}

void sigpipe_received(int s) {
    return;
}

