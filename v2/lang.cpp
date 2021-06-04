#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <ctime>
#include <windows.h>
#include "m.h"

using namespace std;
using namespace sf;

#define ws 630
#define s 70

int n[ws][ws]; float c[ws][ws];

int main()
{
    // Some Variables //
    POINT p;
    long int t = 0;
    unsigned int next = clock() + 1000;
    int fps = 0;
    double x, y, cx, cy, map=1, nx=0, ny=0, k=ws/s;

    //string gradient = " .....----,,,**ooOO0$";
    string gradient = " .':,\"!/r(l1Z4H9W8$@";
    double sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);

    m m;
    sf::Font font; font.loadFromFile("font1.ttf");
    sf::Event event;

    // Window Setting //
    sf::RenderWindow window(sf::VideoMode(ws, ws), "Julia", sf::Style::Close | sf::Style::Titlebar);
    window.setVerticalSyncEnabled(true); window.setFramerateLimit(55);

    /// Main Cycle ///
    while (window.isOpen()) {
        window.clear();

        // Button thinks //
        if (GetKeyState(VK_UP) & 0x8000) nx -= 0.05 * map;
        if (GetKeyState(VK_DOWN) & 0x8000) nx += 0.05 * map;
        if (GetKeyState(VK_LEFT) & 0x8000) ny -= 0.05 * map;
        if (GetKeyState(VK_RIGHT) & 0x8000) ny += 0.05 * map;
        if (GetKeyState('O') & 0x8000) map *= 1.05;
        if (GetKeyState('P') & 0x8000) map *= 0.95;
        

        // Getting C //
        GetCursorPos(&p); 
        double mx = p.x; cx = (mx / sw * 2 - 1) * 3;
        double my = p.y; cy = (my / sh * 2 - 1) * 3;

        // Capture Shaders //
        for (int i = 0; i < s*2; i++) {
            for (int j = 0; j < s; j++) {
                x = ((i / (double)s)/2 * 4 - 2) * map + ny;
                y = ((j / (double)s) * 4 - 2) * map + nx;

                n[i][j] = m.julia(x, y, cx, cy, gradient.length());
                c[i][j] = (double)n[i][j] / (gradient.length()-1) * 255;
            }
        }

        // Rendering and Optimizing //
        for (int i = 0; i < s*2; i++) {
            string l = ""; int lj = 0;
            for (int j = 0; j < s; j++) {
                if (abs(c[i][lj] - c[i][j + 1]) / 255 > 0.15) {
                    //if (i != s-2) l += gradient[n[i+1][j]];

                    if (c[i][lj] != 0) {
                        sf::Text text;
                        text.setString(l); text.setCharacterSize(k); text.setPosition(i * k / 2, lj * k); text.setFont(font);
                        text.setFillColor(sf::Color(c[i][lj] * 0.1, c[i][lj] * 0.6, c[i][lj])); window.draw(text);
                    }

                    l = ""; lj = j+1;
                } 
                l += gradient[n[i][j+1]]; l += "\n";                
            }
        }

        // Printing FPS and C //
        if (clock() >= next) {
            next = clock() + 1000;
            fps = t;
            t = 0;
        } t++;

        sf::Text infoText; 
        infoText.setString( "FPS: " + std::to_string(fps) + " | c = " + std::to_string(cx) + ", " + std::to_string(cy));
        infoText.setCharacterSize(10); infoText.setPosition(0, 0); infoText.setFont(font); window.draw(infoText);

        // Window thinks //
        while (window.pollEvent(event)) if (event.type == sf::Event::Closed) window.close();
        window.display();
    }
}
