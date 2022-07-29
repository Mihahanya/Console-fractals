#include <iostream>
#include <string>
#include <windows.h>

using std::cout;
using std::string;

#define KEY_STATE 0x8000
#define KeyPress(x) GetKeyState(x) & KEY_STATE

#define RST "\x1B[0m"
#define CBLK "\x1B[30m"
#define CRED "\x1B[31m"
#define CGRN "\x1B[32m"
#define CYEL "\x1B[33m"
#define CBLU "\x1B[34m"
#define CMGN "\x1B[35m"
#define CKYN "\x1B[36m"
#define CWHT "\x1B[37m"
#define BCBLK "\x1B[90m"
#define BCRED "\x1B[91m"
#define BCGRN "\x1B[92m"
#define BCYEL "\x1B[93m"
#define BCBLU "\x1B[94m"
#define BCMGN "\x1B[95m"
#define BCKYN "\x1B[96m"
#define BCWHT "\x1B[97m"


namespace csl 
{
    const string cols[] = { BCBLK, CBLU, CMGN, CRED, CYEL, CGRN, CKYN, BCBLU, BCMGN, BCRED, BCYEL, BCGRN, BCKYN, CWHT };
    const int cols_n = 14;
    const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

    inline string col_str(const string& s, int c) {
        return cols[c] + s + RST;
    }

    void gotoxy(int x, int y) {
        COORD coords = {x, y};
        SetConsoleCursorPosition(handle, coords);
    }

    void set_window(int w, int h, const WCHAR* title_text) {
        COORD coord = {w, h};
        SMALL_RECT rect = {0, 0, w-1, h-1};
        SetConsoleScreenBufferSize(handle, coord);
        SetConsoleWindowInfo(handle, TRUE, &rect);

        LPCWSTR title{title_text};
        SetConsoleTitle(title);
    }
}

int julia(double x, double y, double cx, double cy, int k) {
    int n = 0;
    while (n < k) {
        double twoab = 2 * x * y;
        x = pow(x, 2) - pow(y, 2) + cx;
        y = twoab + cy;
        if (pow(x, 2) + pow(y, 2) > 3) break;

        n++;
    }
    return n;
}

int main() 
{
    constexpr int size_w = 85, size_h = 50;
    constexpr double size_aspect = double(size_w) / size_h;

    csl::set_window(size_w*2, size_h+2, L"Console fractals");

    //const string gradient = " .,:oO0$";
    //const string gradient = " .:!/r(l1Z4H9W8$@";
    const string gradient = " `:,!*zTY]kGm%NQ@";
    //const string gradient = " `.,*+xoOX#0@";

    const double move_speed = 0.03, c_speed = 0.005;
    double nx=0, ny=0, map=1, cx=0, cy=0;
    int iter_n=10, mode=0;


    auto timer = clock();
    std::ios_base::sync_with_stdio(false);

    while (1) 
    {
        if (KeyPress('W')) ny -= move_speed * map;
        if (KeyPress('S')) ny += move_speed * map;
        if (KeyPress('A')) nx -= move_speed * map / size_aspect;
        if (KeyPress('D')) nx += move_speed * map / size_aspect;

        if (KeyPress('Q')) map *= 1.05;
        if (KeyPress('E')) map *= 0.95;

        if (KeyPress('X')) iter_n += 1;
        if (KeyPress('Z') && iter_n > 2) iter_n -= 1;

        if (KeyPress('1')) mode = 0;
        if (KeyPress('2')) mode = 1;

        if (KeyPress('I')) cy -= c_speed * map;
        if (KeyPress('K')) cy += c_speed * map;
        if (KeyPress('J')) cx -= c_speed * map / size_aspect;
        if (KeyPress('L')) cx += c_speed * map / size_aspect;


        int n;
        string frame{""};
        for (int i = 0; i < size_h; i++) {
            for (int j = 0; j < size_w*2; j++) {
                double x = double(j - size_w) / size_w * map + nx;
                double y = double(i*2 - size_h) / size_h * map + ny;
                x *= size_aspect;

                if (mode == 0) n = julia(x, y, x, y, iter_n); // Mandelbrot
                if (mode == 1) n = julia(x, y, cx, cy, iter_n); // Julia

                int grdi = (double)n/iter_n*(gradient.length()-1);
                int coli = (double)n/iter_n*csl::cols_n;
                
                frame += csl::col_str(string{gradient[grdi]}, coli);
            }
            frame += '\n';
        }

        double dt = clock()-timer;
        timer = clock();

        csl::gotoxy(0, 0);
        cout << frame;
        printf("fps: %.1f, map: x%f, number of iterations: %i, cx: %f, cy: %f ", 
            1000/dt, 1/map, iter_n, cx, cy);
        printf("\nWASD - move, QE - zoom, XZ - change number of iterations, 1..2 - mods, IJKL - change component");
    }
    return 0;
}
