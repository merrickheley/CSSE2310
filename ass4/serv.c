/** 
 * \file   serv.c
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  server program that reads trivia questions from a file for clients
 *
 * \details
 * 
 * serv is a command line c program that acts as a multi-port trivia server. 
 * It reads questions from a file, and then sends the questions to 
 * clients connected to the relevant port, and tallies the responses.
 *
 * It also includes a scores feature, such that when scores is run the total
 * score, wins, disconnects and games played across all ports is given.
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include "servSupport.h"

int main(int argc, char *argv[]) {

    restartGame = 1; // GLOBAL: When a game finishes, restart it
    int min = 0;
    int max = 0;
    int roundTime = 0;
    int numGames = (argc-4)/2;

    // Validate args
    int args_valid = validate_args(argc, argv, &min, &max, &roundTime);
    if (args_valid) {
        return args_valid;
    }

    handle_signals();
    
    // Set up the game data
    GameData *games = (GameData *) malloc(sizeof(GameData) * numGames);
    initialise_game(games, numGames, min, max, roundTime);

    // Check all ports are valid
    if (get_ports(argc, argv, games)) {
        free_memory(games, numGames);
        fprintf(stderr, "Invalid Port\n");
        return ERR_PORT;
    }

    // Check questions are valid
    if (get_questions(argc, argv, games) == ERR_FILE) {
        free_memory(games, numGames);
        fprintf(stderr, "Invalid File\n");
        return ERR_FILE;
    }

    // Start the games
    for (int i = 0; i < numGames; ++i) {
        pthread_create(&games[i].gameThread, NULL, game_thread, 
                (void*) &games[i]);
    }

    // Continue and clean up when all threads are done
    for (int i = 0; i < numGames; ++i) {
        pthread_join(games[i].gameThread, NULL);
    }

    free_memory(games, numGames);
    
    return 0;
}