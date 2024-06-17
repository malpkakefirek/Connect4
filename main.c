#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define ROWS 6
#define COLS 7
#define MAXLINE 1024
#define RED_COIN "\e[31m●\e[0m"
#define RED_TEXT "\e[31mRED\e[0m"
#define BLUE_COIN "\e[96m●\e[0m"
#define BLUE_TEXT "\e[96mBLUE\e[0m"
#define WON_TEXT "\e[93mYOU WON!\e[0m"
#define LOST_TEXT "\e[31mYou Lost :(\e[0m"

extern int startClient(int argc, char **argv);
extern int startServer(int argc, char **argv);

char board[ROWS][COLS];

void initialize_board() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i][j] = ' ';
        }
    }
}

void clear_terminal() {
    const char *CLEAR_SCREEN_ANSI = "\e[2J\e[H";
    write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 8);
}

void print_players(char player) {
    printf("«CONNECT 4»\n");
    // printf("player 1 vs player 2\n");
    printf("You are ");
    if (player == 'X') {
        printf(RED_TEXT);
    } else {
        printf(BLUE_TEXT);
    }
    printf("!\n\n");
}

void print_board() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("| ");
            if (board[i][j] == 'X') {
                printf(RED_COIN);
            } else if (board[i][j] == 'O') {
                printf(BLUE_COIN);
            } else {
                printf(" ");
            }
            printf(" ");
        }
        printf("|\n");
    }
    printf(" ");
    for (int j = 0; j < COLS; j++) {
        printf("--- ");
    }
    printf("\n");
}

void print_gamescreen(char player) {
    clear_terminal();
    print_players(player);
    print_board();
}

int make_move(int col, char player) {
    for (int i = ROWS - 1; i >= 0; i--) {
        if (board[i][col] == ' ') {
            board[i][col] = player;
            return 1;
        }
    }
    return 0;
}

int check_draw() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (board[i][j] == ' ') {
                return 0;
            }
        }
    }
    return 1;
}

int check_winner(char player) {
    // Check horizontal
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS - 3; j++) {
            if (board[i][j] == player && board[i][j+1] == player && board[i][j+2] == player && board[i][j+3] == player) {
                return 1;
            }
        }
    }
    // Check vertical
    for (int i = 0; i < ROWS - 3; i++) {
        for (int j = 0; j < COLS; j++) {
            if (board[i][j] == player && board[i+1][j] == player && board[i+2][j] == player && board[i+3][j] == player) {
                return 1;
            }
        }
    }
    // Check diagonal
    for (int i = 0; i < ROWS - 3; i++) {
        for (int j = 0; j < COLS - 3; j++) {
            if (board[i][j] == player && board[i+1][j+1] == player && board[i+2][j+2] == player && board[i+3][j+3] == player) {
                return 1;
            }
        }
    }
    for (int i = 0; i < ROWS - 3; i++) {
        for (int j = 3; j < COLS; j++) {
            if (board[i][j] == player && board[i+1][j-1] == player && board[i+2][j-2] == player && board[i+3][j-3] == player) {
                return 1;
            }
        }
    }
    return 0;
}

void play_game(int sockfd, int is_server) {
    char buffer[MAXLINE];
    int col;
    int is_your_turn = is_server;
    char player = is_server ? 'X' : 'O';
    char opponent = is_server ? 'O' : 'X';
    initialize_board();
    print_gamescreen(player);

    while (1) {
        if (is_your_turn) {
            printf("Your turn. Enter column (0-6): ");
            scanf("%d", &col);
            while (col < 0 || col >= COLS || !make_move(col, player)) {
                printf("Invalid move. Enter column (0-6): ");
                scanf("%d", &col);
            }
            snprintf(buffer, sizeof(buffer), "%d", col);
            send(sockfd, buffer, strlen(buffer), 0);
            print_gamescreen(player);

            if (check_winner(player)) {
                printf(WON_TEXT);
                printf("\n");
                break;
            }

            if (check_draw()) {
                printf("You Drew :o");
                printf("\n");
                break;
            }
        } else {
            printf("Waiting for opponent's move...\n");
            int n = read(sockfd, buffer, MAXLINE);
            if (n == 0) {
                printf("Opponent disconnected. ");
                printf(WON_TEXT);
                printf("\n");
                break;
            }
            buffer[n] = '\0';
            col = atoi(buffer);
            make_move(col, opponent);
            print_gamescreen(player);

            if (check_winner(opponent)) {
                printf(LOST_TEXT);
                printf("\n");
                break;
            }

            if (check_draw()) {
                printf("You Drew :o");
                printf("\n");
                break;
            }
        }

        is_your_turn = !is_your_turn; // Switch turns
    }
}

int main(int argc, char **argv) {
    char choice;
    char ipAddress[100];
    int sockfd;

    printf("Do you want to host a game or join a game? (h/j): ");
    scanf(" %c", &choice);

    if (choice == 'h') {
        printf("Hosting the game...\n");
        sockfd = startServer(argc, argv);
    } else if (choice == 'j') {
        printf("Enter the IP address of the host: ");
        scanf("%s", ipAddress);

        char *clientArgs[] = {argv[0], ipAddress};
        sockfd = startClient(2, clientArgs);
    } else {
        printf("Invalid choice. Please run the program again and choose 'h' to host or 'j' to join.\n");
        return 1;
    }

    play_game(sockfd, choice == 'h');
    close(sockfd);
    return 0;
}
