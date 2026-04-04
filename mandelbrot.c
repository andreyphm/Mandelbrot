#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#include "TXLib.h"
#pragma GCC diagnostic pop

#include <stdio.h>
#include <immintrin.h>

const int nMax = 256;
const __m256 r2Max = _mm256_set1_ps(10.f);
const __m256 _255  = _mm256_set1_ps(255.f);
const __m256 _7_0  = _mm256_set_ps(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);
const __m256 nmax  = _mm256_set1_ps(nMax);
const __m256 one = _mm256_set1_ps(1.0f);

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
        if (txGetAsyncKeyState(VK_DOWN))        yC -= dy * (txGetAsyncKeyState(VK_SHIFT) ? 50.f : 10.f);
        if (txGetAsyncKeyState(VK_UP))          yC += dy * (txGetAsyncKeyState(VK_SHIFT) ? 50.f : 10.f);
        if (txGetAsyncKeyState(VK_OEM_PLUS))    scale *= (txGetAsyncKeyState(VK_SHIFT) ? 0.9f : 0.95f);
        if (txGetAsyncKeyState(VK_OEM_MINUS))   scale *= (txGetAsyncKeyState(VK_SHIFT) ? 1.1f : 1.05f);

        for (int iy = 0; iy < 600; iy++)
        {
            if (txGetAsyncKeyState(VK_ESCAPE)) break;

            float y0 = yC + ((float)iy - 300.f) * dy * scale;
            float x0 = xC + (-400.f) * dx * scale;

            for (int ix = 0; ix < 800; ix += 8, x0 += dx*scale*8)
            {
                __m256 X0 = _mm256_add_ps(_mm256_set1_ps(x0), _mm256_mul_ps(_7_0, _mm256_set1_ps(dx*scale)));
                __m256 Y0 = _mm256_set1_ps(y0);
                __m256 X  = X0;
                __m256 Y  = Y0;

                __m256 N = _mm256_setzero_ps();
                for (int n = 0; n < nMax; n++)
                {
                    __m256 X2 = _mm256_mul_ps(X, X); 
                    __m256 Y2 = _mm256_mul_ps(Y, Y); 
                    __m256 XY = _mm256_mul_ps(X, Y);
                    
                    __m256 r2 = _mm256_add_ps(X2, Y2);
                    
                    __m256 cmp = _mm256_cmp_ps(r2, r2Max, _CMP_LE_OQ);
                    
                    int mask = _mm256_movemask_ps(cmp); // mask in binary: first bit = cmp[3], second is cmp[2], ...
                    if (!mask) break;

                    __m256 new_cmp = _mm256_and_ps(cmp, one); // because 0xFFFFFFF float is NaN
                    N = _mm256_add_ps(N, new_cmp);  // fix the iteration number (cmp[i] = 0 if element is out of range)

                    __m256 new_X = _mm256_add_ps(_mm256_sub_ps(X2, Y2), X0);
                    __m256 new_Y = _mm256_add_ps(_mm256_add_ps(XY, XY), Y0);
                    X = _mm256_blendv_ps(X, new_X, cmp);    // if (cmp[i]) X[i] = new_X[i], else continue
                    Y = _mm256_blendv_ps(Y, new_Y, cmp);
                }

                float N_array[8] = {};
                _mm256_storeu_ps(N_array, N);            // N to array
                __m256 I = _mm256_mul_ps(_mm256_sqrt_ps(_mm256_sqrt_ps(_mm256_div_ps(N, nmax))), _255);

                float I_array[8] = {};
                _mm256_storeu_ps(I_array, I);

                for (int i = 0; i < 8; i++)
                {
                    BYTE c = (BYTE) I_array[i];
                    RGBQUAD color = (N_array[i] < nMax) ? RGBQUAD {(BYTE)(255 - c), (BYTE)(c%3 * 64), c, 0} : RGBQUAD {0, 0, 0, 0};

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
