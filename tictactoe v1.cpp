#include <iostream>
#include <vector>

using namespace std;

// Function to draw the tic-tac-toe board
void drawBoard(vector<char> board) {
    cout << " " << board[0] << " | " << board[1] << " | " << board[2] << endl;
    cout << "---|---|---" << endl;
    cout << " " << board[3] << " | " << board[4] << " | " << board[5] << endl;
    cout << "---|---|---" << endl;
    cout << " " << board[6] << " | " << board[7] << " | " << board[8] << endl;
}

// Check if a player has won
bool checkWin(vector<char> board, char player) {
    // Rows, columns, and diagonals
    if ((board[0] == player && board[1] == player && board[2] == player) ||
        (board[3] == player && board[4] == player && board[5] == player) ||
        (board[6] == player && board[7] == player && board[8] == player) ||
        (board[0] == player && board[3] == player && board[6] == player) ||
        (board[1] == player && board[4] == player && board[7] == player) ||
        (board[2] == player && board[5] == player && board[8] == player) ||
        (board[0] == player && board[4] == player && board[8] == player) ||
        (board[2] == player && board[4] == player && board[6] == player))
        return true;
    return false;
}

int main() {
    vector<char> board = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    char currentPlayer = 'X';
    int move;
    int turns = 0;

    while (turns < 9) {
        drawBoard(board);
        cout << "Player " << currentPlayer << ", enter move (1-9): ";
        cin >> move;
        move--; // Convert to 0-indexed

        if (move < 0 || move > 8 || board[move] == 'X' || board[move] == 'O') {
            cout << "Invalid move, try again." << endl;
            continue;
        }

        board[move] = currentPlayer;
        turns++;

        if (checkWin(board, currentPlayer)) {
            drawBoard(board);
            cout << "Player " << currentPlayer << " wins!" << endl;
            return 0;
        }

        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }

    drawBoard(board);
    cout << "It's a draw!" << endl;
    return 0;
}
