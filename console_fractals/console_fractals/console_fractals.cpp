#include <iostream>
#include <string>
#include <windows.h>

#define KEY_STATE 0x8000
#define KeyPress(x) GetKeyState(x) & KEY_STATE

namespace csl 
{
    const WORD cols[] = {8, 1, 2, 3, 4, 5, 6, 9, 10, 11, 12, 13, 14, 7, 15};
    const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

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
    constexpr int size_w = 128/1.5, size_h = 72/1.5, out_len = size_w*size_h*2;
    constexpr double size_aspect = (double)size_w / size_h;

    csl::set_window(size_w*2, size_h+2, L"Console fractals");

    //const char gradient[] = " .,:oO0$";
    //const char gradient[] = " .:!/r(l1Z4H9W8$@";
    const char gradient[] = " `:,!*zTY]kGm%NQ@";
    //const char gradient[] = " `.,*+xoOX#0@";
    const size_t gr_len = strlen(gradient);

    const double move_spd = 1e-3, c_spd = 0.5e-3, scale_spd = 1e-3;
    double nx=0, ny=0, scale=1, cx=0, cy=0;
    int iter_n=10, mode=0;

    auto timer = clock();
    double delta_time = 1./60;
    std::ios_base::sync_with_stdio(false);

    CHAR_INFO* frame = new CHAR_INFO[out_len];
    COORD buff_size {size_w*2, size_h}, buff_coord {0, 0};
    SMALL_RECT write_region { 0, 0, size_w*2-1, size_h-1 };


    while (1) 
    {
        if (KeyPress('W')) ny -= move_spd * scale * delta_time;
        if (KeyPress('S')) ny += move_spd * scale * delta_time;
        if (KeyPress('A')) nx -= move_spd * scale / size_aspect * delta_time;
        if (KeyPress('D')) nx += move_spd * scale / size_aspect * delta_time;

        if (KeyPress('Q')) scale *= 1+(scale_spd*delta_time);
        if (KeyPress('E')) scale *= 1-(scale_spd*delta_time);

        if (KeyPress('X')) iter_n += 1;
        if (KeyPress('Z') && iter_n > 2) iter_n -= 1;

        if (KeyPress('1')) mode = 0;
        if (KeyPress('2')) mode = 1;

        if (KeyPress('I')) cy -= c_spd * scale * delta_time;
        if (KeyPress('K')) cy += c_spd * scale * delta_time;
        if (KeyPress('J')) cx -= c_spd * scale / size_aspect * delta_time;
        if (KeyPress('L')) cx += c_spd * scale / size_aspect * delta_time;


        int n;
        for (int i = 0; i < size_h; i++) {
            for (int j = 0; j < size_w*2; j++) {
                double x = (double)(j - size_w) / size_w * scale + nx;
                double y = (double)(i * 2 - size_h) / size_h * scale + ny;
                x *= size_aspect;

                if (mode == 0) n = julia(x, y, x, y, iter_n); // Mandelbrot
                if (mode == 1) n = julia(x, y, cx, cy, iter_n); // Julia

                int grdi = (double)n/iter_n*(gr_len-1);
                int coli = (double)n/iter_n*(sizeof(csl::cols)/sizeof(WORD)-1);
                
                frame[i*size_w*2+j].Char.UnicodeChar = gradient[grdi];
                frame[i*size_w*2+j].Attributes = csl::cols[coli];
            }
        }

        delta_time = clock()-timer;
        timer = clock();

        WriteConsoleOutput(csl::handle, frame, buff_size, buff_coord, &write_region);

        csl::gotoxy(0, size_h-1);
        printf("fps: %.1f, scale: x%f, number of iterations: %i, cx: %f, cy: %f \n",
            1000/delta_time, 1/scale, iter_n, cx, cy);
        printf("WASD - move, QE - zoom, XZ - change number of iterations, 1..2 - mods, IJKL - change component");
    }

    return 0;
}
