#include <iostream>
#include <cstdlib>
#include <cassert>
#include <list>
#include <ctime>
#include <vector>

struct Tile
{
    bool is_bomb;
    bool is_revealed;
    bool is_marked;
    int number;

    int x, y;
};

class Minesweeper
{
public:
    Minesweeper(int width, int height);

    void Run();

private:
    void printMinefield() const;
    bool isGameComplete() const;

    bool revealTile(int x, int y);
    std::list<Tile *> getTilesAround(int x, int y);
    int countMinesAround(int x, int y);

    int m_width, m_height;
    std::vector<std::vector<Tile>> m_Tiles;
};

int main()
{
    constexpr int kMinefieldWidth = 9,
                  kMinefieldHeight = 9;

    // Seed std::rand with the current time
    auto seed = static_cast<unsigned int>(time(nullptr));
    std::cout << "Seeding minefield with: " << seed << std::endl;
    std::srand(seed);

    Minesweeper game(kMinefieldWidth, kMinefieldHeight);

    game.Run();

    return 0;
}

Minesweeper::Minesweeper(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_Tiles(m_height, std::vector<Tile>(m_width))
{

}

void Minesweeper::Run()
{
    // Set the mines
    for (int j = 0; j < m_height; j++)
    {
        for (int i = 0; i < m_width; i++)
        {
            Tile &tile = m_Tiles[j][i];

            // rand is random enough
            tile.is_bomb = (std::rand() % 100) < 16;
            tile.is_revealed = false;
            tile.number = 9; // Invalid
            tile.is_marked = false;
            tile.x = i;
            tile.y = j;
        }
    }

    while (true)
    {
        printMinefield();

        std::string cmd;
        std::cin >> cmd;

        if (cmd == "exit" || cmd == "e")
        {
            break;
        }

        if (cmd == "reveal" || cmd == "r")
        {
            int x, y;
            std::cin >> x;
            std::cin >> y;

            --x;
            --y;

            if (!revealTile(x, y))
            {
                break;
            }
        }
        else if (cmd == "mark" || cmd == "m")
        {
            int x, y;
            std::cin >> x;
            std::cin >> y;

            --x;
            --y;

            Tile &tile = m_Tiles[y][x];
            tile.is_marked = !tile.is_marked;
        }
        else
        {
            std::cout << "Unknown command: " << cmd << std::endl;
        }

        if (isGameComplete())
        {
            std::cout << "** Congratulations, you win! **" << std::endl;
            break;
        }
    }
}

bool Minesweeper::isGameComplete() const
{
    for (auto &j : m_Tiles)
    {
        for (auto &tile : j)
        {
            if (!tile.is_revealed && !tile.is_bomb)
            {
                return false;
            }
        }
    }

    return true;
}

bool Minesweeper::revealTile(int x, int y)
{
    // You fool, that's out of range
    if (x < 0 || x > m_width - 1 || y < 0 || y > m_height - 1)
    {
        std::cout << "Error: Invalid coordinates" << std::endl;
        return true;
    }

    Tile &tile = m_Tiles[y][x];
    if (tile.is_bomb)
    {
        std::cout << "Boom" << std::endl;
        return false;
    }

    if (tile.is_revealed)
    {
        std::cout << "Error: Already revealed tile" << std::endl;
        return true;
    }

    tile.is_revealed = true;
    tile.number = countMinesAround(x, y);

    // Do the cool recursive reveal
    if (tile.number == 0)
    {
        std::cout << "Recursive reveal!" << std::endl;
        auto list = getTilesAround(x, y);

        for (auto *t : list)
        {
            if (!t->is_revealed)
            {
                revealTile(t->x, t->y);
            }
        }
    }

    return true;
}

void Minesweeper::printMinefield() const
{
    for (int j = -1; j < m_height; j++)
    {
        for (int i = -1; i < m_width; i++)
        {
            if (i == -1 && j == -1)
            {
                std::cout << "  ";
            }
            else if (i == -1)
            {
                std::cout << j + 1 << "| ";
            }
            else if (j == -1)
            {
                std::cout << "_" << i + 1;
            }
            else
            {
                Tile const &tile = m_Tiles[j][i];

                if (tile.is_revealed)
                {
                    if (tile.number == 0)
                    {
                        std::cout << ". ";
                    }
                    else
                    {
                        // We haven't initialized the number then
                        assert (tile.number != 9);

                        std::cout << tile.number << " ";
                    }
                }
                else
                {
                    if (tile.is_marked)
                    {
                        std::cout << "x ";
                    }
                    else
                    {
                        std::cout << "? ";
                    }
                }
            }
        }

        std::cout << std::endl;
    }
}

std::list<Tile *> Minesweeper::getTilesAround(int x, int y)
{
    assert(x >= 0 && x < m_width && y >= 0 && y < m_height);

    int topleft_x = std::max(0, x - 1),
        topleft_y = std::max(0, y - 1),
        topright_x = std::min(x + 1, m_width - 1),
        topright_y = std::min(y + 1, m_height - 1);

    std::list<Tile *> list;
    for (int j = topleft_y; j <= topright_y; j++)
    {
        for (int i = topleft_x; i <= topright_x; i++)
        {
            if (j == y && i == x)
            {
                continue; // Skip self
            }

            list.push_back(&m_Tiles[j][i]);
        }
    }

    return list;
}

int Minesweeper::countMinesAround(int x, int y)
{
    int bomb_count{};
    auto list = getTilesAround(x, y);

    for (auto *tile : list)
    {
        if (tile->is_bomb)
        {
            ++bomb_count;
        }
    }

    return bomb_count;
}