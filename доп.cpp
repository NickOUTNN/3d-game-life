#include <vector>
#include <Windows.h>
#include <list>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <bitset>
using namespace std;
// 0 -------\ Y (m)
//   -------/
// ||
// ||
// ||
// ||
// \/ X (n)
enum TypeCell
{
    env,
    alive
};
struct Cell
{
    TypeCell type;
    friend ostream& operator<<(ostream& out, const Cell& cell)
    {
        if (cell.type == env) out << '.';
        else if (cell.type == alive) out << '#';
        return out;
    }
};
struct Field1D
{
    int n = 0;
    vector<Cell> cells;
    Field1D(int n) :n(n), cells(vector<Cell>(n)) {}
    int getNum(int pos, int radius = 1, TypeCell type = alive) const
    {
        int count = 0;
        for (int i = pos - radius; i <= pos + radius; i++)
            if (cells[(i + n) % n].type == type)
                count++;
        return count;
    }
    Cell& operator[](int i) { return cells[i]; }
    Cell operator[](int i) const { return cells[i]; } // const âàðèàíò äëÿ cout
    friend ostream& operator<<(ostream& out, const Field1D& field)
    {
        for (int i = 0; i < field.n; i++)
            out << field[i];
        return out;
    }
};
struct Field2D
{
    int n = 0;
    int m = 0;
    vector<Field1D> cells;
    Field2D() {}
    Field2D(int n, int m) : n(n), m(m), cells(vector<Field1D>(n, Field1D(m))) {}
    int getNum(int posX, int posY,  int radius = 1, TypeCell type = alive) const
    {
        int count = 0;
        for (int i = posX - radius; i <= posX + radius; i++)
            count += cells[(i + n) % n].getNum(posY, radius, type);
        return count;
    }
    Field1D& operator[](int i) { return cells[i]; }
    Field1D operator[](int i) const { return cells[i]; }
    friend ostream& operator<<(ostream& out, const Field2D& field)
    {
        for (int i = 0; i < field.n; i++)
            out << field[i] << "\n";
        return out;
    }
};
struct Field3D
{
    int n = 0;
    int m = 0;
    int k = 0;
    vector<Field2D> cells;
    Field3D() {}
    Field3D(int n, int m, int k) : n(n), m(m), k(k), cells(vector<Field2D>(k, Field2D(n, m))) {}

    int getNum(int posZ, int posX, int posY,  int radius = 1, TypeCell type = alive) const
    {
        int count = 0;
        for (int i = posZ - radius; i <= posZ + radius; i++)
        {
            count += cells[(i + k) % k].getNum(posX, posY, radius, type);
        }
        return count;
    }
    Field2D& operator[](int i) { return cells[i]; }
    Field2D operator[](int i) const { return cells[i]; }
    friend ostream& operator<<(ostream& out, const Field3D& field)
    {
        for (int i = 0; i < field.k; i++)
            out << i << ":\n" << field[i] << "\n";
        return out;
    }
};
struct iGame
{
    int n = 0;
    int m = 0;
    int k = 0;

    int seed = 0; // случайная величина для генератора
    double probability = 0.0;  // вероятность того, что клетка живая
    int dimension = 1; // размерность

    int radius = 1; // радиус проверки, граница включена
    int loneliness = 2; // с этого числа и меньше клетки умирают от одиночества
    int birth_start = 3; // с этого числа и до birth_end появляется живая клетка
    int birth_end = 3;
    int overpopulation = 5; // с этого числа и дальше клетки погибают от перенаселения
    virtual void runGame(int numIt) = 0;

};

struct Game2D : public iGame
{
    Field2D field;
    Field2D fieldNext;
    Game2D() { dimension = 2; }
    Game2D(int n, int m) {
        this->n = n;
        this->m = m;
        dimension = 2;
        field = fieldNext = Field2D(n, m);
    }
    void setGame(double p, int s = 0)
    {
        probability = p;
        seed = s;
        field = Field2D(n, m);
        vector<int> tmp(n * m);
        iota(tmp.begin(), tmp.end(), 0);
        shuffle(tmp.begin(), tmp.end(), std::mt19937(seed));
        for (int i = 0; i < (int)(p * n * m + 0.5); i++)
        {
            int x = tmp[i] / m;
            int y = tmp[i] % m;
            field[x][y].type = TypeCell::alive;
        }
    }
    void runGame(int numIt) override
    {
        for (int it = 0; it < numIt; it++)
        {
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < m; j++)
                {
                    int count = field.getNum(i, j);
                    fieldNext[i][j].type = field[i][j].type;
                    if (count <= loneliness || count >= overpopulation) fieldNext[i][j].type = TypeCell::env;
                    else if (count >= birth_start && count <= birth_end) fieldNext[i][j].type = TypeCell::alive;
                }
            }
            field = fieldNext;
        }
    }
};
struct Game3D : public iGame
{
    Field3D field;
    Field3D fieldNext;
    Game3D()
    {
        dimension = 3;
    }
    Game3D(int n, int m, int k)
    {
        this->n = n;
        this->m = m;
        this->k = k;
        dimension = 3;
        loneliness = 5; // с этого числа и меньше клетки умирают от одиночества
        birth_start = 6; // с этого числа и до birth_end появляется живая клетка
        birth_end = 10;
        overpopulation = 12; // с этого числа и дальше клетки погибают от перенаселения
        field = fieldNext = Field3D(n, m, k);

    }
    Game3D(Field3D& _field)
    {
        this->n = _field.n;
       this-> m = _field.m;
       this->k = _field.k;
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < n; j++)
            {
                for (int l = 0; l < m; l++)
                    field[i][j][l].type = _field[i][j][l].type;
            }
        }
        dimension = 3;
        fieldNext = Field3D(n, m, k);
    }

    void setGame(double p, int s = 0)
    {
        probability = p;
        seed = s;
        for (int j = 0; j < k; j++) {
            vector<int> tmp(n * m);
            iota(tmp.begin(), tmp.end(), 0);
            shuffle(tmp.begin(), tmp.end(), std::mt19937(seed));
            for (int i = 0; i < (int)(p * n * m + 0.5); i++)
            {
                int x = tmp[i] / m;
                int y = tmp[i] % m;
                field[j][x][y].type = TypeCell::alive;
            }
            seed = (8253729 * seed + 2396403) % 32768;
        }
    }
    void runGame(int numIt) override
    {
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < n; j++)
            {
                for (int l = 0; l < m; l++)
                {

                    fieldNext[i][j][l].type = TypeCell::env;
                }
            }
        }
        for (int tmp = 0; tmp < numIt; tmp++)
        {
            for (int i = 0; i < k; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    for (int l = 0; l < m; l++)
                    {
                        int count = field.getNum(i, j, l, radius);
                        if (count <= loneliness || count >= overpopulation) { fieldNext[i][j][l].type = TypeCell::env; }
                        else if (count >= birth_start && count <= birth_end) { fieldNext[i][j][l].type = TypeCell::alive; }
                        else fieldNext[i][j][l].type = field[i][j][l].type;
                    }
                }
            }
            for (int i = 0; i < k; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    for (int l = 0; l < m; l++)
                    {

                        field[i][j][l].type = fieldNext[i][j][l].type;
                    }
                }
            }
        }
    }
    double proportion()
    {
        double live = 0.0;
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < n; j++)
            {
                for (int l = 0; l < m; l++)
                {
                    if (field[i][j][l].type == TypeCell::alive)
                    {
                        live++;
                    }
                }
            }
        }
        return live / 64;
    }
};
void create1(Game3D& game) {
    game = Game3D(4, 4, 4);
    game.field[1][0][1].type = TypeCell::alive;
    game.field[1][2][3].type = TypeCell::alive;
    game.field[2][0][1].type = TypeCell::alive;
    game.field[2][2][2].type = TypeCell::alive;
    game.field[3][2][2].type = TypeCell::alive;
    game.field[3][3][1].type = TypeCell::alive;
}
void create2(Game3D& game) {
    game = Game3D(4, 4, 4);
    game.setGame(1, 100);
    game.field[2][0][2].type = TypeCell::env;
    game.field[2][1][0].type = TypeCell::env;
    game.field[3][2][1].type = TypeCell::env;
}

int main()
{
    double prop;
    Field3D field(4, 4, 4);
    Game3D game;
    field[1][0][1].type = TypeCell::alive;
    field[1][2][3].type = TypeCell::alive;
    field[2][0][1].type = TypeCell::alive;
    field[2][2][2].type = TypeCell::alive;
    field[3][3][1].type = TypeCell::alive;
    field[3][2][2].type = TypeCell::alive;
    cout << field;
    for (int rad = 1; rad <= 2; rad++)
    {
        for (int lon = 0; lon < 26; lon++)
        {
            for (int bir = lon + 1; bir <= 26; bir++)
            {
                for (int end = bir; end <= 26; end++)
                {
                    for (int over = end + 1; over <= 27; over++)
                    {
                        
                        create1(game);
                        game.radius = rad;
                        game.loneliness = lon;
                        game.birth_start = bir;
                        game.birth_end = end;
                        game.overpopulation = over;
                        game.runGame(20);
                        prop = game.proportion();
                        if (prop > 0.15 && prop < 0.2)
                        {
                            cout << rad << ' ' << lon << ' ' << bir << ' ' << end << ' ' << over << "\n" << endl;

                        }
                    }
                }
            }
        }
    }
    Game3D game2;
    create2(game2);
    cout << "field2 " << endl;
    cout << game2.field << endl;
    for (int rad = 1; rad <= 2; rad++)
    {
        for (int lon = 0; lon < 26; lon++)
        {
            for (int bir = lon + 1; bir <= 26; bir++)
            {
                for (int end = bir; end <= 26; end++)
                {
                    for (int over = end + 1; over <= 27; over++)
                    {

                        create2(game2);
                        game2.radius = rad;
                        game2.loneliness = lon;
                        game2.birth_start = bir;
                        game2.birth_end = end;
                        game2.overpopulation = over;
                        game2.runGame(20);
                        prop = game2.proportion();
                        if (prop > 0.15 && prop < 0.2)
                        {
                            cout << rad << ' ' << lon << ' ' << bir << ' ' << end << ' ' << over << "\n" << endl;

                        }
                    }
                }
            }
        }
    }
}


