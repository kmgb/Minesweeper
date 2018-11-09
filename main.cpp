#include <iostream>
#include <cstdlib>
#include <cassert>
#include <list>
#include <ctime>

constexpr int width = 9,
        height = 9;
//num_bombs = 16;

struct Tile {
    bool is_bomb;
    bool is_revealed;
    bool is_marked;
    int number;

    int x, y;
};

void print_minefield(Tile (&tiles)[height][width]);

bool reveal_tile(int x, int y, Tile (&tiles)[height][width]);

std::list<Tile *> get_tiles_around(int x, int y, Tile (&tiles)[height][width]);

int count_mines_around(int x, int y, Tile (&tiles)[height][width]);

bool is_game_complete(Tile (&tiles)[height][width]);

int main() {
    // Seed std::rand with the current time
    auto seed = static_cast<unsigned int>(time(nullptr));
    std::cout << "Seeding minefield with: " << seed << std::endl;
    std::srand(seed);

    Tile tiles[height][width];

    // Set the mines
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            Tile &tile = tiles[j][i];

            // rand is random enough
            tile.is_bomb = (std::rand() % 100) < 16;
            tile.is_revealed = false;
            tile.number = 9; // Invalid
            tile.is_marked = false;
            tile.x = i;
            tile.y = j;
        }
    }

    while (true) {
        print_minefield(tiles);

        std::string cmd;
        std::cin >> cmd;

        if (cmd == "exit" || cmd == "e")
            break;

        if (cmd == "reveal" || cmd == "r") {
            int x, y;
            std::cin >> x;
            std::cin >> y;

            --x;
            --y;

            if (!reveal_tile(x, y, tiles))
                break;
        }
        else if (cmd == "mark" || cmd == "m") {
            int x, y;
            std::cin >> x;
            std::cin >> y;

            --x;
            --y;

            Tile &tile = tiles[y][x];
            tile.is_marked = !tile.is_marked;
        }
        else {
            std::cout << "Unknown command: " << cmd << std::endl;
        }

        if (is_game_complete(tiles)) {
            std::cout << "** Congratulations, you win! **" << std::endl;
            break;
        }
    }

    return 0;
}

bool is_game_complete(Tile (&tiles)[height][width]) {
    for (auto &j : tiles) {
        for (auto &tile : j) {
            if (!tile.is_revealed && !tile.is_bomb)
                return false;
        }
    }

    return true;
}

bool reveal_tile(int x, int y, Tile (&tiles)[height][width]) {
    std::cout << "Trying to reveal (" << x << ", " << y << ")." << std::endl;
    // You fool, that's out of range
    if (x < 0 || x > width - 1 || y < 0 || y > height - 1) {
        std::cout << "Error: Invalid coordinates" << std::endl;
        return true;
    }

    Tile &tile = tiles[y][x];
    if (tile.is_bomb) {
        std::cout << "Boom" << std::endl;
        return false;
    }

    if (tile.is_revealed) {
        std::cout << "Error: Already revealed tile" << std::endl;
        return true;
    }

    tile.is_revealed = true;
    tile.number = count_mines_around(x, y, tiles);

    // Do the cool recursive reveal
    if (tile.number == 0) {
        std::cout << "Recursive reveal!" << std::endl;
        auto list = get_tiles_around(x, y, tiles);

        for (auto *t : list) {
            if (!t->is_revealed) {
                reveal_tile(t->x, t->y, tiles);
            }
        }
    }

    return true;
}

void print_minefield(Tile (&tiles)[height][width]) {
    for (int j = -1; j < height; j++) {
        for (int i = -1; i < width; i++) {
            if (i == -1 && j == -1) {
                std::cout << "  ";
            }
            else if (i == -1) {
                std::cout << j + 1 << "| ";
            }
            else if (j == -1) {
                std::cout << "_" << i + 1;
            }
            else {
                Tile &tile = tiles[j][i];

                if (tile.is_revealed) {
                    if (tile.number == 0) {
                        std::cout << ". ";
                    }
                    else {
                        // We haven't initialized the number then
                        assert (tile.number != 9);

                        std::cout << tile.number << " ";
                    }
                }
                else {
                    if (tile.is_marked) {
                        std::cout << "x ";
                    }
                    else {
                        std::cout << "? ";
                    }
                }
            }
        }

        std::cout << std::endl;
    }
}

std::list<Tile *> get_tiles_around(int x, int y, Tile (&tiles)[height][width]) {
    assert(x >= 0 && x < width && y >= 0 && y < height);

    int topleft_x = std::max(0, x - 1),
            topleft_y = std::max(0, y - 1),
            topright_x = std::min(x + 1, width - 1),
            topright_y = std::min(y + 1, height - 1);

    std::list<Tile *> list;
    for (int j = topleft_y; j <= topright_y; j++) {
        for (int i = topleft_x; i <= topright_x; i++) {
            if (j == y && i == x)
                continue; // Skip self

            list.push_back(&tiles[j][i]);
        }
    }

    return list;
}

int count_mines_around(int x, int y, Tile (&tiles)[height][width]) {
    int bomb_count{};
    auto list = get_tiles_around(x, y, tiles);

    for (auto *tile : list) {
        if (tile->is_bomb)
            ++bomb_count;
    }

    return bomb_count;
}

int count_unrevealed_around(int x, int y, Tile (&tiles)[height][width]) {
    int unrevealed_count{};
    auto list = get_tiles_around(x, y, tiles);

    for (auto *tile : list) {
        if (!tile->is_revealed)
            ++unrevealed_count;
    }

    return unrevealed_count;
}