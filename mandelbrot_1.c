#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#include "TXLib.h"
#pragma GCC diagnostic pop

#include <stdio.h>

int main()
{
    txCreateWindow(800, 600);
    RGBQUAD* pixel = txVideoMemory();
    
    for (int iy = 0; iy < 600; iy++)
    {
        int y = iy * 800;
        for (int ix = 0; ix < 800; ix++)
        {
            int offset = y + ix;
            pixel[offset].rgbBlue = 0;
            pixel[offset].rgbGreen = 255;
            pixel[offset].rgbRed = 0;
        }
    }

    txSleep();          //now window doesn't close immediately
    return 0;
}
