#include <vector>
#include <Windows.h>
#include <list>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <bitset>
#include <locale.h>
#include <conio.h>
using namespace std;
enum GameEvent
{
    FieldEmpty,
    FullField
};
struct Observer
{
    virtual void newEvent(GameEvent event) = 0;
};
struct Subject
{
    vector<Observer*> obs;
    void subscribe(Observer* tmp)
    {
        obs.push_back(tmp);
    }
    void update(GameEvent event) 
    {
        for (auto sub : obs)
            sub->newEvent(event);
    }
};
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
    int getNum(int pos, TypeCell type = alive, int radius = 1) const
    {
        int count = 0;
        for (int i = pos - radius; i <= pos + radius; i++)
            if (cells[(i + n) % n].type == type)
                count++;
        return count;
    }
    int getLive()
    {
        int count = 0;
        for (int i = 0; i < n; i++)
        {
            if (cells[i].type == alive)
            {
                count++;
            }
        }
        return count;
    }
    Cell& operator[](int i) { return cells[i]; }
    Cell operator[](int i) const { return cells[i]; } 
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
    int getNum(int posX, int posY, TypeCell type = alive, int radius = 1) const
    {
        int count = 0;
        for (int i = posX - radius; i <= posX + radius; i++)
            count += cells[(i + n) % n].getNum(posY, type, radius);
        return count;
    }
    int getLive()
    {
        int count = 0;
        for (int i = 0; i < n; i++)
        {
            count += cells[i].getLive();
        }
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

    int getNum(int posZ, int posX, int posY, TypeCell type = alive, int radius = 1) const
    {
        int count = 0;
        for (int i = posZ - radius; i <= posZ + radius; i++)
        {
            count += cells[(i + k) % k].getNum(posX, posY, type, radius);
        }
        return count;
    }
    int getLive()
    {
        int count = 0;
        for (int i = 0; i < k; i++)
        {
            count += cells[i].getLive();
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
struct iGame:public Subject
{
    int n = 0;
    int m = 0;
    int k = 0;

    int seed = 0; 
    double probability = 0.0;  
    int dimension = 1; 

    int radius = 1; 
    int loneliness = 2; 
    int birth_start = 3; 
    int birth_end = 3;
    int overpopulation = 5; 
    virtual void runGame(int numIt) = 0;
   virtual void setGame(double p, int s = 0) = 0;
   virtual void print(ostream& out) const = 0;
   friend ostream& operator<<(ostream& out, iGame& g) 
   {
       g.print(out);
       return out;
   }
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
            int alliveCount = field.getLive();
            if (alliveCount == 0)
                update(FieldEmpty);
            if (alliveCount == n*m)
                update(FullField);
        }

    }
    
    void print(ostream& out)const
    {
        out << field;
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
        loneliness = 5; 
        birth_start = 6; 
        birth_end = 9;
        overpopulation = 10; 
        field = fieldNext = Field3D(n, m, k);
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
        while (numIt)
        {
            for (int i = 0; i < k; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    for (int l = 0; l < m; l++)
                    {
                        int count = field.getNum(i, j, l);
                        if (count <= loneliness || count >= overpopulation) { fieldNext[i][j][l].type = TypeCell::env; }
                        else if (count >= birth_start && count <= birth_end) { fieldNext[i][j][l].type = TypeCell::alive; }
                        else fieldNext[i][j][l].type = field[i][j][l].type;
                    }
                }
            }
            field = fieldNext;
            int alliveCount = field.getLive();
            if (alliveCount == 0)
                update(FieldEmpty);
            if (alliveCount == n * m * k)
                update(FullField);
            numIt--;
        }
    }
    void print(ostream& out)const
    {
        out << field;
    }

};
class view : public Observer
{
public:
    void newEvent(GameEvent event)
    {
        if (event == FieldEmpty || event == FullField)
        {
            status = gameOver;
        }
    }
    iGame* game = 0;
    int game_dim = 0;
    int button = 0;
    enum statusgame
    {
        gameSetDim,
        gameSetSize,
        gameSetP,
        gameReady,
        gameRun,
        gamePause,
        gameOver,

    };
    statusgame status = gameSetDim;
    void gotocoord(int x, int y = 0) {
        COORD position = { y, x };
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleCursorPosition(hConsole, position);
    }
    view()
    {
        setlocale(LC_ALL, "Russian");
    }
    void start()
    {
        while (1)
        {
            if (status == gameSetDim)
            {
                system("cls");
                cout << "выберите размерность игры, пожалуйста\n" << "1\n" << "2\n" << "3\n";
                int pos = 0;
                gotocoord(pos + 1);
                while (1)
                {
                    if (_kbhit())
                    {
                        button = _getch();
                        if (button == 'w')
                        {
                            pos = (pos - 1 + 3) % 3;
                            gotocoord(pos + 1);
                        }
                        else if (button == 's')
                        {
                            pos = (pos + 1) % 3;
                            gotocoord(pos + 1);
                        }
                        else if (button == 13 && pos != 0)
                        {
                            game_dim = pos + 1;
                            gotocoord(4);
                            system("cls");
                            status = gameSetSize;
                            int size;
                            int count;
                            double p;
                            if (status == gameSetSize)
                            {
                                if (game_dim == 3)
                                {

                                    cout << "выберите количество полей, пожалуйста\n";
                                    cin >> count;
                                    cout << "выберите размер игры, пожалуйста\n";
                                    cin >> size;
                                }
                                else {
                                    cout << "выберите размер игры, пожалуйста\n";
                                    cin >> size;
                                }
                            }
                            status = gameSetP;
                            if (status == gameSetP)
                            {
                                cout << "выберите probability игры, пожалуйста\n";
                                cin >> p;
                            }
                            if (game_dim == 2)
                            {
                                game = new Game2D(size, size);
                                game->setGame(0.34, p);
                                game->subscribe(this);
                            }
                            if (game_dim == 3)
                            {
                                game = new Game3D(size, size, count);
                                game->setGame(0.34, p);
                                game->subscribe(this);
                            }
                            break;
                        }
                    }
                }
                system("cls");
                cout << *game;
                status = gameReady;
            }
            if (_kbhit()&& (status != gamePause))
            {
                    int c = _getch();
                    if (c == 'p')
                    {
                        system("cls");
                        status = gamePause;
                    }
            }
            else if (status == gamePause)
            {
                cout << "вы поставили игру на паузу,выберите l, чтобы продолжить или j, чтобы начать заново\n";
                int c = _getch();
                if (c == 'l')
                    status = gameRun;
                if (c == 'j')
                    status = gameSetDim;
            }
            else if (status == gameReady)
            {
                status = gameRun;
            }
            else if (status == gameRun)
            {
                system("cls");
                game->runGame(1);
                cout << *game;
                Sleep(500);
            }
            else if (status == gameOver)
            {
                system("cls");
                cout << "Ќажмите r чтобы начать игру заново или f чтобы выйти\n";
                int c = _getch();
                if (c == 'r')
                    status = gameSetDim;
                if (c == 'f')
                    break;
            }
            else
            {
                cout << "Unknown\n";
                break;
            }
        }
    }
};
int main()
{
    view b;
    b.start();
}