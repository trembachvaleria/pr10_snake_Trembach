// pr10_Trembach.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <windows.h>
using namespace std;

struct Snake
{
    int x;
    int y;
    Snake* next;
};

const int W = 40;
const int H = 20;
Snake* head = nullptr;
int score = 0;
char dir = 'd';
int foodX, foodY;
bool gameOver = false;
bool savedAndExit = false;

//отчищаем список
void clearSnake()
{
    while (head != nullptr)
    {
        Snake* temp = head;
        head = head->next;
        delete temp;
    }
}

//доб в начало
void addHead(int x, int y)
{
    Snake* node = new Snake;
    node->x = x;
    node->y = y;
    node->next = head;
    head = node;
}

//доб в конец
void addTail(int x, int y)
{
    Snake* node = new Snake;
    node->x = x;
    node->y = y;
    node->next = nullptr;

    if (head == nullptr)
    {
        head = node;
        return;
    }

    Snake* cur = head;
    while (cur->next != nullptr)
    {
        cur = cur->next;
    }
    cur->next = node;
}

//удалить хвост
void removeTail() {
    if (head == nullptr) return;
    if (head->next == nullptr)
    {
        delete head;
        head = nullptr;
        return;
    }
    Snake* cur = head;
    while (cur->next->next != nullptr)
    {
        cur = cur->next;
    } 
    delete cur->next;
    cur->next = nullptr;
}

//длина
int snakeLength()
{
    int len = 0;
    Snake* cur = head;
    while (cur != nullptr)
    {
        len++;
        cur = cur->next;
    }
    return len;
}

//генир еда
void generateFood()
{
    foodX = rand() % W;
    foodY = rand() % H;
}

//столкновение с собой
bool hitsItself(int x, int y)
{
    Snake* cur = head->next;
    while (cur != nullptr)
    {
        if (cur->x == x && cur->y == y) return true;
        cur = cur->next;
    }
    return false;
}

void draw() {
    system("cls");// очистить

    cout << "Счёт: " << score << endl;

    // верхняя граница
    for (int i = 0; i < W + 2; i++) cout << "/";
    cout << endl;

    for (int y = 0; y < H; y++)
    {
        cout << "/";// левая граница
        for (int x = 0; x < W; x++)
        {
            if (x == foodX && y == foodY)
            {
                cout << "@";// еда
            }
            else
            {
                bool isSnake = false;
                Snake* cur = head;
                while (cur != nullptr)
                {
                    if (cur->x == x && cur->y == y)
                    {
                        isSnake = true;
                        break;
                    }
                    cur = cur->next;
                }
                if (cur == head) cout << "O";// голова
                else if (isSnake) cout << "o";// тело
                else cout << " ";
            }
        }
        cout << "/" << endl;// правая граница
    }

    // нижняя граница
    for (int i = 0; i < W + 2; i++) cout << "/";
    cout << endl;
    cout << "Управление:" << endl << "a - влево" << endl << "w - вверх" << endl << "d - вправо" << endl << "s - вниз" << endl << "q - выход, p - сохранить\n";
}

//сохранить
void saveGame() {
    ofstream f("save.bin", ios::binary);
    if (!f.is_open()) return;

    f.write((char*)&score, sizeof(score));
    f.write((char*)&dir, sizeof(dir));
    f.write((char*)&foodX, sizeof(foodX));
    f.write((char*)&foodY, sizeof(foodY));

    int len = snakeLength();
    f.write((char*)&len, sizeof(len));

    Snake* cur = head;
    while (cur != nullptr)
    {
        f.write((char*)&cur->x, sizeof(cur->x));
        f.write((char*)&cur->y, sizeof(cur->y));
        cur = cur->next;
    }

    f.close();
    cout << "Игра сохранена\n";
    Sleep(1000);
}

//вводят
void input(){
    if (_kbhit()) 
    {
        char key = _getch();
        if (key == 'w' && dir != 's') dir = 'w';
        else if (key == 's' && dir != 'w') dir = 's';
        else if (key == 'a' && dir != 'd') dir = 'a';
        else if (key == 'd' && dir != 'a') dir = 'd';
        else if (key == 'q') gameOver = true;
        else if (key == 'p')
        {
            saveGame();
            savedAndExit = true;
            gameOver = true;
        }
    }
}

//загрузка
void loadGame(){
    ifstream f("save.bin", ios::binary);
    if (!f.is_open())
    {
        cout << "Файл не найден\n";
        Sleep(1000);
        return;
    }

    clearSnake();

    f.read((char*)&score, sizeof(score));
    f.read((char*)&dir, sizeof(dir));
    f.read((char*)&foodX, sizeof(foodX));
    f.read((char*)&foodY, sizeof(foodY));

    int len;
    f.read((char*)&len, sizeof(len));

    for (int i = 0; i < len; i++)
    {
        int x, y;
        f.read((char*)&x, sizeof(x));
        f.read((char*)&y, sizeof(y));
        addTail(x, y);
    }

    f.close();

    if (dir != 'w' && dir != 'a' && dir != 's' && dir != 'd')
    {
        dir = 'd';
    }

    cout << "Игра загружена\n";
    Sleep(1000);
}


//обновление
void update() {
    // новая позиция головы
    int newX = head->x;
    int newY = head->y;

    if (dir == 'w') newY--;
    else if (dir == 's') newY++;
    else if (dir == 'a') newX--;
    else if (dir == 'd') newX++;

    // проверка границ
    if (newX < 0 || newX >= W || newY < 0 || newY >= H)
    {
        gameOver = true;
        return;
    }

    // проверка столкновения с собой
    if (hitsItself(newX, newY))
    {
        gameOver = true;
        return;
    }

    // добавляем новую голову
    addHead(newX, newY);

    // доб очки если съел еду
    if (newX == foodX && newY == foodY)
    {
        score += 10;
        generateFood();
    }
    else
    {
        // убираем хвост
        removeTail();
    }
}

//рекорды
void showRecords() {
    ifstream f("records.bin", ios::binary);
    if (!f.is_open()) 
    {
        cout << "Рекордов нет\n";
        Sleep(1500);
        return;
    }

    cout << "Рекорды:\n";

    while (true)
    {
        int s;
        char name[50];
        f.read((char*)&s, sizeof(s));
        if (f.eof()) break;
        f.read(name, 50);
        cout << name << " - " << s << endl;
    }

    f.close();
    cout << "\nНажмите любую клавишу чтобы выйти в меню.";
    cin.ignore();
    _getch();
}

//сохраниьь рекорды
void saveRecord() {
    ofstream f("records.bin", ios::binary | ios::app);
    if (!f.is_open()) return;

    char name[50];
    cout << "Введите имя: ";
    cin >> name;

    f.write((char*)&score, sizeof(score));
    f.write(name, 50);

    f.close();
}


void newGame() {
    clearSnake();
    score = 0;
    dir = 'd';
    gameOver = false;
    savedAndExit = false;

    addHead(3, 5);// хвост
    addHead(4, 5);// тело
    addHead(5, 5);// голова

    generateFood();

    // игровой цикл
    while (!gameOver)
    {
        draw();
        input();
        if (gameOver) break;
        update();
        Sleep(250);
    }

    if (savedAndExit)
    {
        savedAndExit = false;
        return;
    }

    cout << "\nИгра окончена! Счёт: " << score << endl;

    string save;
    cout << "Сохранить рекорд? (yes/no): ";
    cin >> save;
    if (save == "yes") saveRecord();

    cout << "Нажмите любую клавишу чтобы выйти в меню.";
    cin.ignore();
    _getch();
}

int main() {
    setlocale(NULL, "Russian");
    srand(time(0));

    int otv = 0;
    while (otv != 4) {
        system("cls");
        cout << "Игра змейка!\n";
        cout << "1. Новая игра\n";
        cout << "2. Загрузить игру\n";
        cout << "3. Таблица рекордов\n";
        cout << "4. Выход\n";
        cout << "Выбор: ";
        cin >> otv;

        if (otv == 1) newGame();
        else if (otv == 2) {
            loadGame();
            gameOver = false;
            if (!gameOver && head != nullptr)
            {
                // игровой цикл после загрузки
                while (!gameOver)
                {
                    draw();
                    input();
                    if (gameOver) break;
                    update();
                    Sleep(250);
                }

                if (savedAndExit)
                {
                    savedAndExit = false;
                    continue;
                }
                cout << "\nИгра окончена! Счёт: " << score << endl;

                string save;
                cout << "Сохранить рекорд? (yes/no): ";
                cin >> save;
                if (save == "yes") saveRecord();

                cout << "Нажмите любую клавишу чтобы выйти в меню.";
                cin.ignore();
                _getch();
            }
        }
        else if (otv == 3) showRecords();
    }

    clearSnake();
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
