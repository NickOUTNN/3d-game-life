#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <locale.h>
#include <iostream>
#include <locale.h>
#include <iostream>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;
COORD position;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
struct iView
{
    char livingCell;
    char dyingCell;
    virtual void draw() = 0;
    iView(char b1 = '#', char b2 = '.')
    {
        livingCell = b1;
        dyingCell = b2;
    }
};
struct View2d : iView
{
    char** fielddraw;
    int n;
    int m;
    void setField(char** field1, int n1, int m1) {
        n = n1;
        m = m1;
        fielddraw = new char* [n];
        for (int i = 0; i < n; i++) {
            fielddraw[i] = new char[m];
            for (int j = 0; j < m; j++)
                fielddraw[i][j] = field1[i][j];
        }

    }
    void draw()
    {
        for (int i = 0; i < n; i++)
        {
            fwrite(fielddraw[i], sizeof(char), m, stdout);
            printf_s("\n");
        }
    }
};
struct View3d : iView
{
    int k;
    int n;
    int m;
    char*** fielddraw;
    void setField(char*** field1, int k1, int n1, int m1) {
        k = k1;
        n = n1;
        m = m1;
        fielddraw = new char** [k];
        for (int i = 0; i < k; i++) {
            fielddraw[i] = new char* [n];
            for (int j = 0; j < n; j++) {
                fielddraw[i][j] = new char[m];
                for (int l = 0; l < m; l++)
                    fielddraw[i][j][l] = field1[i][j][l];
            }
        }

    }
    void draw()
    {
        for (int i = 1; i < k-1; i++)
        {
            for (int j = 0; j < n; j++)
            {
                fwrite(fielddraw[i][j], sizeof(char), m, stdout);
                printf_s("\n");
            }
            cout << endl;
        }
    }
};

enum Rools
{
    rool12d = 2,
    rool22d = 3,
    rool13d = 6,
    rool23d = 9
};
struct iGame
{
    virtual void runGame(int numIt) = 0;
    double probability;
    int seed;
    iGame() {
        seed = 0;
        probability = 0;
    }
};

struct Game2d :iGame
{
    int n;
    int m;
    View2d view;
    char** field;
    char** field_next;
    int getCount(int x, int y)
    {
        int count = 0;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (field[x + i][y + j] == view.livingCell)
                    count++;
            }
        }
        return count;
    }
    Game2d(int n1, int m1, int seed, double probability)
    {
        int tmp = seed;
        n = n1;
        m = m1;
        field = new char* [n + 2];
        field_next = new char* [n + 2];
        for (int i = 0; i < n + 2; i++)
        {
            field[i] = new char[m + 2];
            field_next[i] = new char[m + 2];
            for (int j = 0; j < m + 2; j++)
            {

                if ((tmp) <= (100 * probability))
                {
                    field[i][j] = view.livingCell;
                }
                else
                {
                    field[i][j] = view.dyingCell;
                }
                field_next[i][j] = view.dyingCell;
                tmp = (8253729 * tmp + 2396403) % 32768;
            }
        }
    }
    void runGame(int numIt) override
    {

        while (numIt)
        {
            for (int i = 1; i < n + 1; i++)
            {
                for (int j = 1; j < m + 1; j++)
                {

                    if (field[i][j] == view.livingCell && ((getCount(i, j) - 1) < rool12d || (getCount(i, j) - 1) > rool22d))
                        field_next[i][j] = view.dyingCell;
                    else if (field[i][j] == view.dyingCell && getCount(i, j) == rool22d)
                        field_next[i][j] = view.livingCell;
                    else
                        field_next[i][j] = field[i][j];
                }
            }
            for (int i = 0; i < n + 2; i++)
            {
                for (int j = 0; j < m + 2; j++)
                {
                    field[i][j] = field_next[i][j];

                }
            }
            numIt--;
        }
        view.setField(field, n + 2, m + 2);
        view.draw();
    }
};
struct Game3d :iGame
{
    int n;
    int m;
    int k;
    View3d view;
    char*** field;
    char*** field_next;
    int getCount(int x, int y, int z)
    {
        int count = 0;
      for (int i=-1;i<=1;i++)
         for (int j = -1; j <= 1; j++)
             for (int k = -1; k <= 1; k++)
                if (field[x+i][y + j][z + k] == view.livingCell)
                    count++;
          return count;
    }
    Game3d(int k1, int n1, int m1, int seed, double probability)
    {
        int tmp = seed;
        n = n1;
        m = m1;
        k = k1;
        field = new char** [k+2];
        field_next = new char** [k+2];
        for (int i = 0; i < k+2; i++)
        {
            field[i] = new char* [n + 2];
            field_next[i] = new char* [n + 2];
            for (int j = 0; j < n + 2; j++)
            {
                field[i][j] = new char[m + 2];
                field_next[i][j] = new char[m + 2];
                for (int l = 0; l < m + 2; l++)
                {
                    if ((tmp) <= (100 * probability))
                    {
                        field[i][j][l] = view.livingCell;
                    }
                    else
                    {
                        field[i][j][l] = view.dyingCell;
                    }
                    field_next[i][j][l] = view.dyingCell;
                    tmp = (8253729 * tmp + 2396403) % 32768;
                }
            }
        }
    }
    void runGame(int numIt) override
    {
        while (numIt)
        {
            for (int i = 1; i < k+1; i++)
            {
                for (int j = 1; j < n + 1; j++)
                {
                    for (int l = 1; l < m + 1; l++)
                    {
                        if (field[i][j][l] == view.livingCell && ((getCount(i, j, l)-1) < rool13d || (getCount(i, j, l)-1) > rool23d))
                            field_next[i][j][l] = view.dyingCell;
                        else if (field[i][j][l] == view.dyingCell && getCount(i, j, l) == rool23d)
                            field_next[i][j][l] = view.livingCell;
                    }
                }
            }
            for (int i = 0; i < k+2; i++)
            {
                for (int j = 0; j < n + 2; j++)
                {
                    for (int l = 0; l < m + 2; l++)
                    {
                        field[i][j][l] = field_next[i][j][l];

                    }
                }
            }
            numIt--;
        }
        view.setField(field, k+2, n + 2, m + 2);
        view.draw();
    }
};
int main()
{
    iGame* k;
    Game3d* b = new Game3d(3, 5, 5, 1 , 10);
    k = b;
    k->runGame(1);

    return 0;
}