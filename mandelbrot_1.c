#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#include "TXLib.h"
#pragma GCC diagnostic pop

#include <stdio.h>

const int nMax = 256;
const float r2Max = 4;

int main()
{
    txCreateWindow(800, 600);
    
    float dy = 0.005f, dx = 0.005f;
    float xC = 0.f, yC = 0.f, scale = 1.f;

    for( ; ; )
    {
        if (txGetAsyncKeyState(VK_ESCAPE)) break;

        txBegin();

        RGBQUAD* pixel = txVideoMemory(); // pixel = array of structures with color
        int width  = txGetExtentX();

        if (txGetAsyncKeyState(VK_RIGHT))       xC += dx * (txGetAsyncKeyState(VK_SHIFT) ? 50.f : 10.f);
        if (txGetAsyncKeyState(VK_LEFT))        xC -= dx * (txGetAsyncKeyState(VK_SHIFT) ? 50.f : 10.f);
        if (txGetAsyncKeyState(VK_DOWN))        yC += dy * (txGetAsyncKeyState(VK_SHIFT) ? 50.f : 10.f);
        if (txGetAsyncKeyState(VK_UP))          yC -= dy * (txGetAsyncKeyState(VK_SHIFT) ? 50.f : 10.f);
        if (txGetAsyncKeyState(VK_OEM_PLUS))    scale -= dx * (txGetAsyncKeyState(VK_SHIFT) ? 50.f : 10.f);
        if (txGetAsyncKeyState(VK_OEM_MINUS))   scale += dx * (txGetAsyncKeyState(VK_SHIFT) ? 50.f : 10.f);

        for (int iy = 0; iy < 600; iy++)
        {
            if (txGetAsyncKeyState(VK_ESCAPE)) break;

            float y0 = (((float)iy - 300.f) * dy + yC) * scale;
            float x0 = ((-490.f) * dx + xC) * scale;

            for (int ix = 0; ix < 800; ix += 4, x0 += dx*4)
            {
                float X0[4] = {x0, x0+dx, x0+2*dx, x0+3*dx};
                float Y0[4] = {y0, y0, y0, y0};
                float X[4] = {}; for (int i = 0; i < 4; i++) X[i] = X0[i];
                float Y[4] = {}; for (int i = 0; i < 4; i++) Y[i] = Y0[i];

                int N[4] = {0, 0, 0, 0};
                for (int n = 0; n < nMax; n++)
                {
                    float X2[4] = {}; for (int i = 0; i < 4; i++) X2[i] = X[i] * X[i]; 
                    float Y2[4] = {}; for (int i = 0; i < 4; i++) Y2[i] = Y[i] * Y[i]; 
                    float XY[4] = {}; for (int i = 0; i < 4; i++) XY[i] = X[i] * Y[i];
                    
                    float r2[4] = {}; for (int i = 0; i < 4; i++) r2[i] = X2[i] + Y2[i];
                    
                    int cmp[4] = {};
                    for (int i = 0; i < 4; i++) if (r2[i] <= r2Max) cmp[i] = 1;
                    
                    int mask = 0;
                    for (int i = 0; i < 4; i++) mask |= (cmp[i] << i);  // mask in binary: first bit = cmp[3], second is cmp[2], ...
                    if (!mask) break;

                    for (int i = 0; i < 4; i++) N[i] = N[i] + cmp[i];   // fix the iteration number (cmp[i] = 0 if element is out of range)

                    for (int i = 0; i < 4; i++)
                    {
                        if (cmp[i])
                        {
                            X[i] = X2[i] - Y2[i] + X0[i];
                            Y[i] = XY[i] + XY[i] + Y0[i];
                        }
                    }
                }

                for (int i = 0; i < 4; i++)
                {
                    float I = sqrtf(sqrtf((float)N[i] / (float)nMax)) * 255.f;

                    BYTE c = (BYTE) I;
                    RGBQUAD color = (N[i] < nMax) ? RGBQUAD {(BYTE)(255 - c), (BYTE)(c%2 * 64), c, 0} : RGBQUAD {0, 0, 0, 0};

                    pixel[iy * width + ix + i] = color;
                }
            }
        }
        txEnd();
        printf("\rFPS: %6.1f   ", txGetFPS());
        fflush(stdout);
    }
    return 0;
}
