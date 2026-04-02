#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#include "TXLib.h"
#pragma GCC diagnostic pop

#include <stdio.h>

const int nMax = 256;
const float r2Max = 10;

int main()
{
    txCreateWindow(800, 600);
    
    float dy = 0.005f, dx = 0.005f;
    float xC = 0.f, yC = 0.f, scale = 1.f;

    for( ; ; )
    {
        if (txGetAsyncKeyState(VK_ESCAPE)) break;

        for (int iy = 0; iy < 600; iy++)
        {
            if (txGetAsyncKeyState(VK_ESCAPE)) break;

            float y0 = (((float)iy - 300.f) * dy + yC) * scale;
            float x0 = ((-490.f) * dx + xC) * scale;

            for (int ix = 0; ix < 800; ix++, x0 += dx)
            {
                float x = x0;
                float y = y0;

                int N = 0;
                for (; N < nMax; N++)
                {
                    float   x2 = x*x, 
                            y2 = y*y, 
                            xy = x*y;
                    
                    float r2 = x2 + y2;
                    
                    if (r2 >= r2Max) break; 

                    x = x2 - y2 + x0;
                    y = xy + xy + y0;
                }

                float I = sqrtf(sqrtf((float)N / (float)nMax)) * 255.f;

                BYTE c = (BYTE) I;
                RGBQUAD color = (N < nMax) ? RGBQUAD {(BYTE)(255 - c), (BYTE)(c%2 * 64), c, 0} : RGBQUAD {0, 0, 0, 0};

                RGBQUAD* pixel = txVideoMemory();
                int width  = txGetExtentX();
                pixel[iy * width + ix] = color;
            }
        }
        printf("\t\r%.01f", txGetFPS());
        txSleep();          //now window doesn't close immediately
    }
    return 0;
}
