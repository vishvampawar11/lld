/*
Connect Four is a two-player connection game where the players
take turns placing their pieces in a 7x6 grid.The first player
to connect four of their pieces in a row, column, or diagonal wins.

Requirements
- Player will place its piece inside the grid by selecting a coloumn (lowest possible row will be automatically selected).
- After each step, system will check if there is a winner.
- The game is over if there is a winnner or if there is a draw.
- Error handling
    - Selected column should be valid (within range and should have at least one empty spot)
    - Player will play only when its turn is valid.

Entities
- System
- Player
- Board
*/

#include <iostream>
#include <array>

enum class GAME_STATE
{
    WIN_P1,
    WIN_P2,
    IN_PROGRESS,
    DRAW
};

enum class DISC_COLOR
{
    NONE,
    RED,
    YELLOW
};

enum class MOVE_RESULT
{
    SUCCESS,
    GAME_FINISHED,
    PLAYER_NOT_VALID,
    COLUMN_INVALID
};

class Player
{
    int id;
    DISC_COLOR color;

public:
    explicit Player(int id, DISC_COLOR color) : id(id), color(color) {}

    [[nodiscard]] int getId() const { return id; }
    [[nodiscard]] DISC_COLOR getColor() const { return color; }

    bool operator==(const Player &other) const { return id == other.id; }
    bool operator!=(const Player &other) const { return id != other.id; }
};

class Board
{
    static constexpr int ROWS = 6;
    static constexpr int COLS = 7;
    static constexpr int WIN_TARGET = 4;

    std::array<std::array<DISC_COLOR, COLS>, ROWS> grid;
    std::array<int, COLS> availableRow; // {column => lowest avaialbe row}

    int fullColumns = 0;

    [[nodiscard]] bool checkDirn(int row, int col, int dr, int dc) const
    {
        DISC_COLOR color = grid[row][col];

        int cnt = 0;
        for (int i = 0; i < WIN_TARGET; i++)
        {
            int r = row + (i * dr);
            int c = col + (i * dc);
            if (r >= 0 && r < ROWS && c >= 0 && c < COLS && grid[r][c] == color) cnt++;
            else break;
        }

        for (int i = 1; i < WIN_TARGET; i++)
        {
            int r = row - (i * dr);
            int c = col - (i * dc);
            if (r >= 0 && r < ROWS && c >= 0 && c < COLS && grid[r][c] == color) cnt++;
            else break;
        }

        return cnt >= WIN_TARGET;
    }

public:
    Board()
    {
        for (auto &row : grid) row.fill(DISC_COLOR::NONE);
        availableRow.fill(ROWS - 1);
    }

    [[nodiscard]] bool canPlace(int column) const
    {
        if (column < 0 || column >= COLS || availableRow[column] == -1) return false;
        return true;
    }

    [[nodiscard]] int placeDisc(DISC_COLOR color, int column)
    {
        int row = availableRow[column];
        grid[row][column] = color;

        availableRow[column]--;
        if (availableRow[column] == -1) fullColumns++;

        return row;
    }

    [[nodiscard]] bool checkWin(int row, int col) const
    {
        if (checkDirn(row, col, 1, 0)) return true;     // vertical
        if (checkDirn(row, col, 0, 1)) return true;     // horizontal
        if (checkDirn(row, col, 1, 1)) return true;     // diagonal 1 (\)
        if (checkDirn(row, col, 1, -1)) return true;    // diagonal 2 (/)

        return false;
    }

    [[nodiscard]] bool isFull() const { return fullColumns == COLS; }
};

class System
{
    Board board;
    Player p1, p2;
    Player *currentPlayer;
    GAME_STATE gameState;

public:
    explicit System(Player player1, Player player2) : board(), p1(player1), p2(player2), currentPlayer(&p1), gameState(GAME_STATE::IN_PROGRESS) {}

    [[nodiscard]] MOVE_RESULT makeMove(const Player &p, int column)
    {
        if (gameState != GAME_STATE::IN_PROGRESS) return MOVE_RESULT::GAME_FINISHED;
        if (p != *currentPlayer) return MOVE_RESULT::PLAYER_NOT_VALID;
        if (!board.canPlace(column)) return MOVE_RESULT::COLUMN_INVALID;

        int row = board.placeDisc(p.getColor(), column);

        if (board.checkWin(row, column)) gameState = (p == p1) ? GAME_STATE::WIN_P1 : GAME_STATE::WIN_P2;
        else if (board.isFull()) gameState = GAME_STATE::DRAW;
        else currentPlayer = (p == p1) ? &p2 : &p1;

        return MOVE_RESULT::SUCCESS;
    }

    [[nodiscard]] const Player *getWinner() const
    {
        if (gameState == GAME_STATE::WIN_P1) return &p1;
        if (gameState == GAME_STATE::WIN_P2) return &p2;
        return nullptr;
    }

    [[nodiscard]] Player &getCurrentplayer() const { return *currentPlayer; }
    [[nodiscard]] GAME_STATE getGameState() const { return gameState; }
    [[nodiscard]] const Board &getBoard() const { return board; }
};

int main()
{
    Player p1(1, DISC_COLOR::RED);
    Player p2(2, DISC_COLOR::YELLOW);

    System s(p1, p2);

    while (s.getGameState() == GAME_STATE::IN_PROGRESS)
    {
        std::cout << "Player " << s.getCurrentplayer().getId() << " move: ";

        int column = -1;
        std::cin >> column;
        MOVE_RESULT result = s.makeMove(s.getCurrentplayer(), column);

        if (result != MOVE_RESULT::SUCCESS) std::cout << "Invalid move (Error: " << static_cast<int>(result) << ")" << "\n";

        GAME_STATE gameState = s.getGameState();
        
        if (gameState == GAME_STATE::WIN_P1) std::cout << "Winner: P1";
        if (gameState == GAME_STATE::WIN_P2) std::cout << "Winner: P2";
    }

    std::cout << "Result: " << static_cast<int>(s.getGameState()) << "\n";

    return 0;
}
