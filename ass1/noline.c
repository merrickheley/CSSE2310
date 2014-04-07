/** 
 *  \file   noline.c
 *  \author Merrick Heley (merrick.heley@uqconnect.edu.au)
 *  \version 1.0
 *  \brief  noline is naughts and crosses in reverse.
 * 
 *  \details
 *
 * noline is a command line c program that behaves similar to the game
 * 'naughts and crosses' but differs in that the first player to form a
 * line of three markers loses. The board size can be set using the dim
 * arg (must be an odd integer greater than three), and the player types
 * can be set (0 - human, 1 - AI from top left, 2 - AI from bottom right).
 * It is also possible to get input from a files for human players (Oin, Xin)
 * and write output to files regardless of player type.
 * 
 * '-' is the standard io file (i.e. if '-' is specified for Oin, it will
 * read from stdin).
 *
 * All commenting is designed to be compatible with Doxygen.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** \struct PlayerStruct
 *  \brief Creates a structure that manages each players 
 *          individual variables 
 */
typedef struct {
    char cursor;    /**< The players cursor, X or O  */
    int  numMoves;  /**< The number of moves the player has attempted  */
    int  type;      /**< The type of player, 0 human, 1 AI1, 2 AI2 */
    int usein;      /**< 0 if the player is using stdin, else 1 */
    int endoffile;  /**< Stores 1 if end of file has been reached, else 0 */
    FILE *in;       /**< Stores the input file for the player */
    FILE *out;      /**< Stores the output file for the player */
} PlayerStruct;

/**
 * Function prototypes 
 * Detailed usage instruction can be found in the comments within 
 * the function 
 */

/** Checks if the board is full */
int     check_board_full(int dim, int numMoves);

/** Checks if someone has formed 3 markers in a row */    
int     check_loser     (int dim, char **grid, char playerCursor, 
        int x, int y);

/** Checks if the game ends */   
int     check_end       (PlayerStruct *player, int curPlayer, int numMoves,
        int dim, char **grid, int *validCoords);

/** Creates the playing grid */
char  **create_grid     (int dim);

/** Sets up the players structures */
void    create_players  (PlayerStruct *player);
    
/** Destroys the grid and the players */
void    destroy_grid    (int dim, char **grid, PlayerStruct *player);
    
/** Draws the grid */
void    draw_grid       (FILE *out, int dim, char **grid);
    
/** Gives the end game message */
void    end_game        (PlayerStruct *player, int curPlayer, char *message);
    
/** Gets the player input */
char   *get_input       (PlayerStruct *player, int dim);

/** Runs the main loop code */
void    main_loop (int curPlayer, int numMoves, int dim, 
        PlayerStruct *player, char **grid);

 /** Makes a move on the grid */
char  **make_move        (char **grid, char playerCursor, int x, int y);
   
/** Prints a line of a character */ 
void    print_line       (FILE *out, int length, char input);
       
/** Validates the arguments given to the program */
int     validate_args    (int argc, char** argv, int *dim, 
        PlayerStruct *player);

/** Valides the files given to the program */
int     validate_files   (char **argv, PlayerStruct *player);

/** Validates the input given by the player */
int    *validate_input   (char *playerInput, int dim, char **grid);

/** Validates the player arguments */
int     validate_players (int argc, char **argv, PlayerStruct *player);

int main(int argc, char **argv) {
    
    int dim = 0;            /* The grid dimension */
    int numMoves = 0;       /* The total move counter */
    int validArgs;          /* Stores the return of validArgs */
    int curPlayer = 0;      /* The current player, 0 for O, 1 for X */
    char **grid;            /* The grid that players see */
    PlayerStruct player[2]; /* The structures that store the players vars */

    create_players(player);

    validArgs = validate_args(argc, argv, &dim, player);

    if (validArgs > 0 ) {
        return validArgs;
    }

    grid = create_grid(dim);

    draw_grid(player[curPlayer].out, dim, grid);

    main_loop(curPlayer, numMoves, dim, player, grid);

    destroy_grid(dim, grid, player);
    return 0;
}

/**\details
 * Compares the size of dim^2 to numMoves+1
 * \param dim (positive integer)
 * \param numMoves (positive integer)
 *
 * \return 1 if the board is full
 * \return 0 otherwise
 */
int check_board_full (int dim, int numMoves) {

    return (dim*dim == numMoves + 1 ? 1 : 0);
}

/**\details
 * Check if the game has been lost because end of file has been reached, 
 * if the current player has formed three markers in a row or if the
 * current player has filled the board.
 *
 * \param player (array containing two PlayerStruct values)
 * \param curPlayer (integer (0 or 1) that designates the current player)
 * \param numMoves (the total amount of successful moves)
 * \param dim (the dimension of the grid)
 * \param grid (a dim*dim size array of chars, created with create_grid)
 * \param validCoords (an array of 3 integers of the format [valid, x, y])
 *
 * \return 1 if game has ended
 * \return 0 if game should continue
 */
int check_end (PlayerStruct *player, int curPlayer, int numMoves, 
        int dim, char **grid, int *validCoords) {

    if (player[curPlayer].endoffile == 1) {
        if (player[0].out != stdout || player[1].out != stdout) {
            draw_grid(player[curPlayer].out, dim, grid);
        }

        end_game(player, curPlayer, "Player %c loses due to EOF.\n");
        return 1;

    } else if (check_loser(dim, grid, player[curPlayer].cursor, 
                            validCoords[1], validCoords[2]) == 0) {
        end_game(player, curPlayer, "Player %c loses.\n");
        return 1;

    } else if (check_board_full(dim, numMoves) == 1) {
        end_game(player, curPlayer, "The game is a draw.\n");
        return 1;
    }

    return 0;
}

/**
 * \details
 * Checks to see if there are three markers in a row by checking to 2
 * squares in each direction from the player, and then by checking both
 * squares on either side of the player.
 *
 * \param dim (positive integer)
 * \param grid (a dim*dim size array of chars, created with create_grid)
 * \param playerCursor (a character, 'X' or 'O')
 * \param x (positive integer < dim),
 * \param y (positive integer < dim)
 *
 * \return 1 if no player has lost
 * \return 0 if the player designated by playerCursor has lost
 */
int check_loser (int dim, char **grid, char playerCursor, int x, int y) {

    char n = playerCursor;

    /* If valid, check to 2 moves to the left of the cursor */
    if ((x > 1) && ((grid[x-2][y] == n && grid[x-1][y] == n)
            || (y > 1 && grid[x-2][y-2] == n && grid[x-1][y-1] == n)
            || (y < dim-1 && grid[x-2][y+2] == n && grid[x-1][y+1] == n))) {
        return 0;
    }
    
    /* If valid, check to 2 moves to the right of the cursor */
    if ((x < dim-2) && ((grid[x+2][y] == n && grid[x+1][y] == n)
            || (y > 1 && grid[x+2][y-2] == n && grid[x+1][y-1] == n)
            || (y < dim-1 && grid[x+2][y+2] == n && grid[x+1][y+1] == n))) {
        return 0;
    }

    /* If valid, check to 2 moves to the top and bottom of the cursor */
    if ((y > 1 && grid[x][y-2] == n && grid[x][y-1] == n)
            || (y < dim-2 && grid[x][y+2] == n && grid[x][y+1] == n)
            || (y > 0 && y < dim-1 && grid[x][y-1] == n && grid[x][y+1] == n)
            || (x > 0 && x < dim-1 && grid[x-1][y] == n 
            && grid[x+1][y] == n)) {
        return 0;
    }

    /* If valid, check to either side of the cursor*/
    if ((y > 0 && y < dim-1 && x > 0 && x < dim-1) 
            && ((grid[x-1][y-1] == n && grid[x+1][y+1] == n)
            || (grid[x+1][y-1] == n && grid[x-1][y+1] == n))){
        return 0;
    }

    return 1;
}

/**\details
 * Allocates the memory for the playing grid and fills it with dots.
 *
 * \param dim (positive integer)
 *
 * \return grid (a dim*dim size array of chars)
 */

char **create_grid (int dim) {

    int i, j;
    char** grid = (char**) malloc(sizeof(char*)*dim);
    
    for (i = 0; i<dim; ++i) {
        grid[i] = (char*) malloc(sizeof(char)*dim);
    }

    for (i = 0; i<dim; ++i) {
        for (j = 0; j<dim; ++j) {
            grid[i][j] = '.';
        }
    }

    return grid;
}

/**\details
 * Writes the relevant initial values to the players array
 *
 * \param player (array containing two PlayerStruct values)
 */
void create_players (PlayerStruct *player) {

    int i;
    player[0].cursor = 'O';
    player[1].cursor = 'X';
    
    for (i = 0; i<2; ++i) {
        player[i].numMoves = 0;
        player[i].type = 0;
        player[i].usein = 0;
        player[i].endoffile = 0;
        player[i].in = stdin;
        player[i].out = stdout;
    }

}

/**\details
 * Frees the memory used by the grid and closes the players i/o files
 *
 * \param dim (positive integer)
 * \param grid (a dim*dim size array of chars, created with create_grid)
 * \param player (array containing two PlayerStruct values)
 */
void destroy_grid (int dim, char **grid, PlayerStruct *player) {

    int i;
    for (i = 0; i<dim; ++i) {
        free(grid[i]);
    }
    free(grid);

    fclose(player[0].in);
    fclose(player[0].out);
    fclose(player[1].in);
    fclose(player[1].out);
}

/**\details
  * Prints the grid to 'out' in the specified format, the top row containing
  * all '-' symbols and the bottom row containing all '=' symbols.
  * 
  * \param out (The file to direct the output to)
  * \param dim (positive integer)
  * \param grid (a dim*dim size array of chars, created with create_grid)
  */
void draw_grid (FILE *out, int dim, char **grid) {

    int i, j;

    print_line(out, dim, '-');

    for (i = 0; i<dim; ++i) {
        for (j = 0; j<dim; ++j) {
            fprintf(out, "%c", grid[i][j]);
        }
        fprintf(out, "\n");
    }

    print_line(out, dim, '=');
}

/**\details
 * Print the end game message to player 0. If player 1 uses a seperate input
 * file that is not stdout, print the message to player 1 as well.
 *
 * \param player (array containing two PlayerStruct values)
 * \param curPlayer (integer (0 or 1) that designates the current player)
 * \param message (string contain the end game message)
 */
void end_game (PlayerStruct *player, int curPlayer, char *message) {

    fprintf(player[0].out, message, player[curPlayer].cursor);

    if (player[0].out != stdout || player[1].out != stdout) {
        fprintf(player[1].out, message, player[curPlayer].cursor);
    }
}

/**\details
 * If the player is human, check for end of file. If the file has not ended,
 * Get 81 characters of player input, and end the string at the newline, and
 * clear the buffer of any overflow.
 *
 * If player type is 1 or two, the moves are determined by the 
 * following formula:
 *
 * i = (player->numMoves * (dim + 2)) % (dim*dim)
 * type 1: [i/dim, i%dim]
 * type 2: [dim-(1+i/dim), dim-(1+i%dim)]
 *
 * \param player (array containing two PlayerStruct values)
 * \param dim (positive integer)
 *
 * \return playerInput (A single line string with a max strlen of 81)
 */
char *get_input (PlayerStruct *player, int dim) {

    static char playerInput[82];
    int i;

    if (player->type == 0) {
        if (feof(player->in) != 0) {
            player->endoffile = 1;
            return "";
        }

        fprintf(player->out, "%c> ", player->cursor);

        fgets(playerInput, 82, player->in);
        
        /* Terminate the string at the newline character */
        for (i = 0; i<83; i++) {
            if (playerInput[i] == '\n') {
                playerInput[i] = '\0';
                break;
            }
        }

        /* Clear the buffer to prevent overflow */
        if (strlen(playerInput) > 80) {
            while (fgetc(player->in) != '\n' || feof(player->in) != 0);
        }
    }

    i = (player->numMoves * (dim + 2)) % (dim * dim);

    if (player->type == 1) {
        sprintf( playerInput, "%d %d", i/dim, i%dim);
    } 
    
    if (player->type == 2) {
        sprintf( playerInput, "%d %d", dim-(1+i/dim), dim-(1+i%dim));
    }

    return playerInput;
}

/**\details
 * Sets the current player, gets their input, and validates it. If the 
 * player hasn't reached the end of file, check if the coords are valid
 * and make the move if they are, otherwise increase the move count and
 * get new input.
 * 
 * After the move has been made, draw the grid for the opposing player,
 * and check if the game has finished. If it has, tell the players this,
 * otherwise increase the move count and continue.
 *
 * \param curPlayer (integer (0 or 1) that designates the current player)
 * \param numMoves (positive integer)
 * \param dim (positive integer)
 * \param player (array containing two PlayerStruct values)
 * \param grid (a dim*dim size array of chars, created with create_grid)
 */
void main_loop (int curPlayer, int numMoves, int dim, 
        PlayerStruct *player, char **grid) {

    char *playerInput;      /* The player input string */
    int *validCoords;       /* Array of 3 ints: [valid (0), x, y] */

    while (1) {

        curPlayer = numMoves%2;

        playerInput = get_input(&player[curPlayer], dim);

        validCoords = validate_input(playerInput, dim, grid);
        
        if (player[curPlayer].endoffile == 0) {

            /* If invalid coordinates, try again */
            if (validCoords[0] == -1) {
                player[curPlayer].numMoves++;
                continue;
            } else if (player[curPlayer].type > 0) {
                fprintf(player[curPlayer].out, "%c> %d %d\n", 
                        player[curPlayer].cursor, validCoords[1], 
                        validCoords[2]);
            }

            grid = make_move(grid, player[curPlayer].cursor, 
                    validCoords[1], validCoords[2]);
        }

        draw_grid(player[(curPlayer == 1 ? 0 : 1)].out, dim, grid);

        if (check_end(player, curPlayer, numMoves, dim, 
                grid, validCoords) == 1) {
            break;
        }

        player[curPlayer].numMoves++;
        numMoves++;                      
    }
}

/**\details
 * Changes the grid value of [x, y] to playerCursor
 *
 * \param grid (a dim*dim size array of chars, created with create_grid)
 * \param playerCursor (a character, 'X' or 'O')
 * \param x (a positive integer)
 * \param y (a positive integer)
 *
 * \return grid (a dim*dim size array of chars, created with create_grid)
 */
char **make_move (char** grid, char playerCursor, int x, int y) {
    grid[x][y] = playerCursor;
    return grid;
}

/**\details
 * Prints a line of a single characters 'length' long  
 *
 * \param out (The file to direct the output to)
 * \param length (The length of the output)
 * \param input (The input character)
 */
void print_line (FILE *out, int length, char input) {
    int i;
    for (i = 0; i<length; ++i) {
        fprintf(out, "%c", input);                       
    }
    fprintf(out, "\n");
}

/**\details
 * Check that the right number of arguments have been given.
 * If they have, set the dim variable and check if it is a postive
 * odd integer.
 * If it is, check that the player type is either 0, 1 or 2 and set the
 * relevant player type to this number.
 * If this has been done successfully, check that the given files can 
 * be opened and set the relevant player in/out files.
 *
 * \param argc (the number of arguments given to the program at runtime)
 * \param argv (an array of arguments given to the program of length argc)
 * \param dim (positive integer)
 * \param player (array containing two PlayerStruct values)
 *
 * \return 1 if incorrect number of arguments given
 * \return 2 if invalid dim argument give
 * \return 3 if invalid player type given
 * \return 4 if invalid files given
 * \return 0 otherwise
 */
int validate_args (int argc, char **argv, int *dim, PlayerStruct *player) {

    int i;
    char c;

    /* Check for correct amount of args */
    if (argc != 2 && argc != 3 && argc != 4 && argc != 8){

        fprintf(stderr, "Usage: noline dim [playerXtype [playerOtype ");
        fprintf(stderr, "[Oin Oout Xin Xout]]]\n");

        return 1;
    }
    
    /*Check that 'dim' is a number*/
    for (i = 0; i < strlen(argv[1]); ++i) {
        if (argv[1][i] < 48 || argv[1][i] > 57) {
            fprintf(stderr, "Invalid board dimension.\n");
            return 2;
        }
    }

    /* Check for valid board dimension */
    if (sscanf(argv[1], "%d%c", dim, &c) != 1 || *dim < 3 || *dim%2 == 0) {
        fprintf(stderr, "Invalid board dimension.\n");
        return 2;
    }

    /* Check for valid player types */
    if (validate_players(argc, argv, player) == 3) {
        return 3;
    }

    /* Attempt to open player input/output files */
    if (argc > 4 && validate_files(argv, player) == 4) {
        return 4;
    }

    return 0;
}

/**\details
 * Validates all 4 of the input files. If valid, set the corresponding
 * variable, if not return an error.
 *
 * \param argv (an array of arguments given to the program of length argc)
 * \param player (array containing two PlayerStruct values)
 *
 * \return 0 if no errors
 * \return 4 if any file is invalid
 */
int validate_files (char **argv, PlayerStruct *player) {

    int i;

    for (i = 4; i<8; ++i) {
        if (strcmp(argv[i], "-") != 0) {
            if (i%2 == 0) {
                player[(i==4 ? 0 : 1)].in = fopen(argv[i], "r");
                player[(i==4 ? 0 : 1)].usein = 1;
            } else {
                player[(i==5 ? 0 : 1)].out = fopen(argv[i], "w");
            }
        }
    }

    /* Check if file opening failed */
    if (player[0].in == NULL || player[0].out == NULL 
            || player[1].in == NULL || player[1].out == NULL){
        fprintf(stderr, "Invalid files.\n");
        return 4;
    }

    return 0;
}

/**\details
 * Check if the move is within the expected length, if so attempt
 * to extract two integers using sscanf("%d %d",...).
 * Check that the integers are greater than 0 and less than the dimension
 * of the grid, and check that the position is not already taken on the
 * grid.
 *
 * \param playerInput (A single line string of a maximum length of 81 chars)
 * \param dim (positive integer)
 * \param grid (a dim*dim size array of chars, created with create_grid)
 *
 * \return validCoords (Array of 3 ints: [valid (0 if valid), x, y] )
 */
int *validate_input (char *playerInput, int dim, char **grid) {

    static int validCoords[3];
    int sf;

    validCoords[1] = 0;
    validCoords[2] = 0;

    /* Check if move is a valid coordinate set, and is on the board */

    if (strlen(playerInput) > 80 || strlen(playerInput) < 3) {
        validCoords[0] = -1;
        return validCoords;
    }

    sf = sscanf(playerInput, "%d %d", &validCoords[1], &validCoords[2]);

    /* Check if move is within the acceptable rangle */
    if (sf != 2 || validCoords[1] >= dim || validCoords[2] >= dim
        || validCoords[1] < 0 || validCoords[2] < 0) {
        validCoords[0] = -1;
        return validCoords;
    }

    /* Check if move does not collide with anything on the board */

    if (grid[validCoords[1]][validCoords[2]] != '.') {
        validCoords[0] = -1;
        return validCoords;
    }

    validCoords[0] = 0;

    return validCoords;
}

/**\details
 * Check both argument 2 and argument 3 are both single digit integers.
 * If so, set the corresponding player type, otherwise give an error.
 *
 * \param argc (the number of arguments given to the program at runtime)
 * \param argv (an array of arguments given to the program of length argc)
 * \param player (array containing two PlayerStruct values)
 *
 * \return 0 if no errors
 * \return 3 if player type is invalid
 */
int validate_players (int argc, char **argv, PlayerStruct *player) {
    
    int i;

    for (i = 2; i<4; ++i) {
        if (argc > i) {
            if (strlen(argv[i]) == 1 && argv[i][0] > 47 && argv[i][0] < 51) {
                player[(i+1)%2].type = (int) atoi(argv[i]);
            } else {
                fprintf(stderr, "Invalid player type.\n");
                return 3;
            }
        }
    }

    return 0;
}