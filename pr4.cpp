#include <iostream>
#include <cstdlib>
#include <windows.h>

using namespace std;

int main()
{
    system("mode con cols=100 lines=51");
    system("color F0");
    ShowCursor(FALSE);
    
    POINT p;
    int i, j;
    float n = 50;
    string gradient = " .,*oO0$";

    float t = 0;
    while (1) {
        system("cls");
        GetCursorPos(&p);
        float x = p.x;
        float y = p.y;

        string l = "";
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                float a = (j / n) * 4 - 2;
                float b = (i / n) * 4 - 2;
                int n = 0;
                while (n < 10) {
                    float aa = pow(a, 2);
                    float bb = pow(b, 2);
                    float twoab = 2 * a * b;

                    /*a = aa - bb + sin(t)*1.5;
                    b = twoab - cos(t/1.2) * 1.5;*/
                    a = aa - bb + (x/GetSystemMetrics(SM_CXSCREEN)*2 - 1) * 3;
                    b = twoab + (y/GetSystemMetrics(SM_CYSCREEN)*2 - 1) * 3;
                    if (abs(a) + abs(b) > 3) break;

                    n++;
                }
                l += gradient[min(n, gradient.length()-1)]; l += " ";
            }
            l += "\n";
        }
        cout << l << endl << " c = " << floor((x/GetSystemMetrics(SM_CXSCREEN)*2 - 1)*3 * 1000)/1000 << ", " << floor((y/GetSystemMetrics(SM_CYSCREEN)*2 - 1)*3 * 1000)/1000;
        t += 0.05;

        Sleep(30); 
    }
}
