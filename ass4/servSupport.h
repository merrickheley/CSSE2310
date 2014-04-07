/** 
 * \file   server_support.h
 * \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 * \version 1.0
 * \brief  
 *
 * \details
 * 
 * All commenting is designed to be compatible with Doxygen.
 */

#include "misc.h"

#define FALSE 0
#define TRUE 1

#define MAXHOSTNAMELEN 128
#define IN_GAME -1 //NOTE: INGAME > DISCON
#define DISCON -2

/** \struct PlayerData
 *  \brief Holds data for each player
 */
typedef struct {
    char *name;         /**< Players name  */
    int fd;             /**< File descriptor for player IO  */
    int answer;         /**< The players last answer/IN_GAME/DISCON */
    char *buffer;       /**< Buffer for reading player input */
    pthread_t pThread;  /**< Thread ID for getting player input */

    int wins;           /**< Records players win  */
    int discs;          /**< Records players disconnection */
    int score;          /**< Records players score */
    int inRound;        /**< Records if player is current in a round */ 
} PlayerData;

/** \struct QuestionData
 *  \brief Holds data for each question
 */
typedef struct {
    char *qBuffer;      /**< Question pre-formatted to send to client */
    int correct;        /**< The correct answer for the question */
    int options;        /**< The number of options in the question */
} QuestionData;

/** \struct QuestionData
 *  \brief Holds data for each game
 */
typedef struct GameDataStruct {
    int port;           /**< The port the game is running on */
    int min;            /**< The minimum number of players for the game */
    int max;            /**< The maximum number of players for the game */
    int roundTime;      /**< The round time for the game */
    
    int running;        /**< Whether the game is currently running or not */
    int fdServer;       /**< File descriptor for the server  */

    QuestionData *question; /**< The array question data for the game */
    int numQuestions;   /**< The number of questions in the array */
    
    PlayerData *player; /**< Array of players that have been in the game */
    int numPlayers;     /**< The number of players in the array */

    pthread_t *joinThread; /**< Array of threads used for joining games */
    int numJoinThreads; /**< The number of join threads in the array */

    pthread_t gameThread;   /**< The game threads ID */
    pthread_t listenThread; /**< The listen threads ID */

    sem_t playerSem;    /**< for blocking access to player data */
    sem_t fdSem;        /**< for blocking access to pass fd */
    
    int numGames;       /**< The total number of games on the server */
    struct GameDataStruct *allGames; /**< Array of all games on server */
} GameData;

/** \struct TempUserData
 *  \brief Holds data for passing to a join thread temporarily.
 */
typedef struct {
    int fd;             /**< fd to be read */
    GameData *game;     /**< Information about the current game */
} TempUserData;

int restartGame; //!< Global var. Whether to restart games that are finished

/**\details
 * 
 * Extracts the arguments and puts them into the given points. Throw an error
 * if arguments are outside acceptable ranges.
 *
 * \param argc (int of the number of arguments)
 * \param argv (string array of arguments)
 * \param min (pointer to min number of players [modified])
 * \param max (pointer to max number of players [modified])
 * \param roundTime (pointer to roundTime [modified])
 *
 * \return error (error number corresponding to either usage or bad number)
 */
int validate_args(int argc, char **argv, int *min, int *max, int *roundTime);

/**\details
 * 
 * Get the ports from the arguments and validate them. Return port error 
 * if bad.
 *
 * \param argc (int of the number of arguments)
 * \param argv (string array of arguments)
 * \param games (array of all game data for the server)
 *
 * \return error (error number corresponding port error)
 */
int get_ports(int argc, char **argv, GameData *games);

/**\details
 * 
 * Extract the questions from the files given and place the pre-formatted data
 * in the relevants games question structure.
 *
 * This is accomplished using a marker that indicates the part of the question
 * being read. If one or more questions can be read, the file is valid, 
 * otherwise return an error
 *
 * \param argc (int of the number of arguments)
 * \param argv (string array of arguments)
 * \param games (array of all game data for the server)
 *
 * \return error (error number corresponding to file error)
 */
int get_questions(int argc,  char **argv, GameData *games);

void get_question(char **buffer, int *len, int *start, QuestionData *temp, 
        int *marker, int *bufLen);

int get_num(char **buffer, int *len, int *count, int *marker, 
        GameData *games, int i, QuestionData *temp);

int open_listen(int port);
int connect_to(struct in_addr* ipAddress, int port);
int name_to_IP_addr(char* host, struct in_addr *ipAddress);
void free_memory(GameData *games, int numGames);
void send_game_scores(GameData *game, int pNum);
void send_correct(GameData *game, int pNum, int answer);
void send_winner(GameData *game, int pNum, int topScore);
int get_top_score(GameData *game);
void *process_connections(void *arg);
void *client_connect(void *arg);
int get_active_players(GameData *game);
void get_scores(GameData *games, int numGames, int fd);
void add_players(PlayerData dest, PlayerData src);
void send_scores(int fd, int sNumPlayers, PlayerData *superPlayer);
void clean(void *arg);
void *game_thread(void *arg);
void *client_thread(void* arg);

void initialise_game(GameData *games, int numGames, int min, int max, 
        int roundTime);

/**\details
 * Sets up handling for sighup and sigpipe
 */
void handle_signals(void);

/**\details
 * Handles sigphup by setting the global restartGame to 0.
 *
 * Games that have not started will exit, games that are currently playing
 * will not start again
 *
 * \param s (integer passed by sigaction)
 */
void sighup_received(int s);

/**\details
 * Handles sigpipe by ignoring it
 *
 * \param s (integer passed by sigaction)
 */
void sigpipe_received(int s);