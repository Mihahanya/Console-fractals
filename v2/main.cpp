#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <ctime>
#include <windows.h>
#include "m.h"

#pragma execution_character_set( "utf-8" )

using namespace std;
using namespace sf;

#define ws 800
#define s 70

int n[ws][ws]; sf::Color c[ws][ws]; 

int main()
{
    // Some Variables //
    POINT p;
    long int t = 0;
    unsigned int next = clock() + 1000;
    int fps = 0, k=round(ws/s), iterations=70, pixelization=1;
    double x, y, cx, cy, map=1, nx=0, ny=0, compression=0.05;

    //string gradient = " .....----,,,**ooOO0$";
    string gradient = " .-,*oO0$";
    //string gradient = ".':,\"!!/r(l1Z4H9W8$@";
    double sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);

    m m;
    sf::Font font; font.loadFromFile("font1.ttf");
    sf::Event event;
    sf::Image imgGrad; imgGrad.loadFromFile("gradient.png");

    // Window Setting //
    sf::RenderWindow window(sf::VideoMode(ws, ws), "Julia", sf::Style::Close | sf::Style::Titlebar);
    window.setVerticalSyncEnabled(true); window.setFramerateLimit(55);
    
    sf::Color colorBase[ws]; int colGradSize = imgGrad.getSize().x - 1;
    for (int i = 0; i < colGradSize+1; i++) {
        colorBase[i] = imgGrad.getPixel(i, 0);
    }
    sf::Color background = imgGrad.getPixel(0, 0);

    /// Main Cycle ///
    while (window.isOpen()) {
        window.clear();
        window.sf::RenderTarget::clear(background);

        // Button thinks //
        if (GetKeyState('W') & 0x8000) nx -= 0.05 * map;
        if (GetKeyState('S') & 0x8000) nx += 0.05 * map;
        if (GetKeyState('A') & 0x8000) ny -= 0.05 * map;
        if (GetKeyState('D') & 0x8000) ny += 0.05 * map;

        if (GetKeyState('Q') & 0x8000) map *= 1.05;
        if (GetKeyState('E') & 0x8000) map *= 0.95; 

        if (GetKeyState('X') & 0x8000) iterations += 1;
        if (GetKeyState('Z') & 0x8000 && iterations > 3) iterations -= 1;

        if (GetKeyState('R') & 0x8000) compression += 0.01;
        if (GetKeyState('F') & 0x8000 && compression > 0) compression -= 0.01; 

        if (GetKeyState('V') & 0x8000 && pixelization < 5) pixelization += 1;
        if (GetKeyState('C') & 0x8000 && pixelization > 1) pixelization -= 1;

        // Getting C //
        GetCursorPos(&p); 
        double mx = p.x; cx = (mx / sw * 2 - 1) * 3;
        double my = p.y; cy = (my / sh * 2 - 1) * 3;

        // Capture Shaders //
        for (int i = 0; i < s*2; i+=pixelization) {
            for (int j = 0; j < s; j+=pixelization) {
                x = ((i / (double)s)/2 * 4 - 2) * map + ny;
                y = ((j / (double)s) * 4 - 2) * map + nx;

                n[i][j] = m.julia(x, y, cx, cy, iterations);
                //c[i][j] = (double)n[i][j] / (gradient.length()-1) * 255;
                c[i][j] = colorBase[(int)((float)n[i][j] / (iterations) * colGradSize)];

                for (int ip = 0; ip < pixelization; ip++) {
                    for (int jp = 0; jp < pixelization; jp++) {
                        n[i+ip][j+jp] = n[i][j];
                        c[i+ip][j+jp] = c[i][j];
                    }
                }
            }
        }

        // Rendering and Optimizing //
        for (int i = 0; i < s*2; i++) {
            string l = ""; int lj = 0;
            for (int j = 0; j < s; j++) {
                if ((float)(abs(c[i][lj].r - c[i][j + 1].r) + abs(c[i][lj].g - c[i][j + 1].g) + abs(c[i][lj].b - c[i][j + 1].b)) / 255 / 3 > compression || j == s - 1) {
                    if (l != "") {
                        sf::Text text;
                        text.setString(l); text.setCharacterSize(k); text.setPosition(i * k / 2, lj * k); text.setFont(font);
                        text.setFillColor(c[i][lj]); window.draw(text);
                    }
                    l = ""; lj = j + 1;
                }
                l += gradient[(int)(((float)n[i][j + 1] / iterations) * (gradient.length() - 1))]; l += "\n";
            }
        }

        // Printing FPS and C //
        if (clock() >= next) {
            next = clock() + 1000;
            fps = t;
            t = 0;
        } t++;

        sf::Text infoText; 
        infoText.setString( "FPS: " + std::to_string(fps) + 
                            " | c = " + std::to_string(cx) + ", " + std::to_string(cy) + 
                            " | Zoom: x" + std::to_string(1/map) + 
                            " | Iterations: " + std::to_string(iterations) +
                            " | Compression: " + std::to_string(compression) +
                            " | Pixelization: x" + std::to_string(pixelization));
        infoText.setCharacterSize(10); infoText.setPosition(0, 0); infoText.setFont(font); window.draw(infoText);

        // Window thinks //
        while (window.pollEvent(event)) if (event.type == sf::Event::Closed) window.close();
        window.display();
    }
}
