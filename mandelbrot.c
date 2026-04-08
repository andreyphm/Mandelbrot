#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#include "TXLib.h"
#pragma GCC diagnostic pop

#include <stdio.h>
#include <immintrin.h> // advanced vector extentions (AVX)

enum mandelbrot_mode
{
    LINEAR      = 0,
    ARRAY       = 1,
    INTRINSICS  = 2 
};

const int WINDOW_WIDTH  = 800;
const int WINDOW_HEIGHT = 600;
const int nMax = 256;
const float r2Max = 10.f;
const __m256 r2Max_vector = _mm256_set1_ps(10.f);
const __m256 _255  = _mm256_set1_ps(255.f);
const __m256 _7_0  = _mm256_set_ps(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);
const __m256 nmax  = _mm256_set1_ps(nMax);
const __m256 one   = _mm256_set1_ps(1.0f);

void draw_linear_mandelbrot(float x0, float y0, int iy, float dx, float scale, int width, RGBQUAD* pixel);
void draw_array_mandelbrot(float x0, float y0, int iy, float dx, float scale, int width, RGBQUAD* pixel);
void draw_intrinsics_mandelbrot(float x0, float y0, int iy, float dx, float scale, int width, RGBQUAD* pixel);

int main()
{
    txCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

    txSelectFont("Arial", 25);
    txSetColor(TX_WHITE);       // set FPS output font and color
    
    float dy = 0.005f, dx = 0.005f;
    float xC = 0.f, yC = 0.f, scale = 1.f;
    mandelbrot_mode mode = INTRINSICS;

    for( ; ; )
    {
        if (txGetAsyncKeyState(VK_ESCAPE)) break;

        txBegin();

        RGBQUAD* pixel = txVideoMemory(); // pixel = array of structures with color
        int width  = txGetExtentX();

        if (txGetAsyncKeyState(VK_RIGHT))       xC += dx * (txGetAsyncKeyState(VK_SHIFT) ? 50.f * scale : 10.f * scale);
        if (txGetAsyncKeyState(VK_LEFT))        xC -= dx * (txGetAsyncKeyState(VK_SHIFT) ? 50.f * scale : 10.f * scale);
        if (txGetAsyncKeyState(VK_DOWN))        yC -= dy * (txGetAsyncKeyState(VK_SHIFT) ? 50.f * scale : 10.f * scale);
        if (txGetAsyncKeyState(VK_UP))          yC += dy * (txGetAsyncKeyState(VK_SHIFT) ? 50.f * scale : 10.f * scale);
        if (txGetAsyncKeyState(VK_OEM_PLUS))    scale *= (txGetAsyncKeyState(VK_SHIFT) ? 0.9f : 0.95f);
        if (txGetAsyncKeyState(VK_OEM_MINUS))   scale *= (txGetAsyncKeyState(VK_SHIFT) ? 1.1f : 1.05f);

        if (txGetAsyncKeyState('1')) mode = LINEAR;
        if (txGetAsyncKeyState('2')) mode = ARRAY;
        if (txGetAsyncKeyState('3')) mode = INTRINSICS;

        for (int iy = 0; iy < WINDOW_HEIGHT; iy++)
        {
            if (txGetAsyncKeyState(VK_ESCAPE)) break;

            float y0 = yC + ((float)iy - 300.f) * dy * scale;
            float x0 = xC + (-400.f) * dx * scale;

            switch(mode)
            {
                case LINEAR:
                    draw_linear_mandelbrot(x0, y0, iy, dx, scale, width, pixel);
                    break;
                case ARRAY:
                    draw_array_mandelbrot(x0, y0, iy, dx, scale, width, pixel);
                    break;
                case INTRINSICS:
                    draw_intrinsics_mandelbrot(x0, y0, iy, dx, scale, width, pixel);
                default:
                    break;
            }
        }
        char buffer[64] = {};
        sprintf(buffer, "FPS: %.1f", txGetFPS());
        txTextOut(10, 10, buffer);
        txEnd();    // bifferization end
    }
    return 0;
}

void draw_linear_mandelbrot(float x0, float y0, int iy, float dx, float scale, int width, RGBQUAD* pixel)
{
    for (int ix = 0; ix < 800; ix++, x0 += dx*scale)
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
        RGBQUAD color = (N < nMax) ? RGBQUAD {(BYTE)(255 - c), (BYTE)(c%5 * 64), c, 0} : RGBQUAD {0, 0, 0, 0};
        pixel[iy * width + ix] = color;
    }
}

void draw_array_mandelbrot(float x0, float y0, int iy, float dx, float scale, int width, RGBQUAD* pixel)
{
    for (int ix = 0; ix < 800; ix += 4, x0 += dx*scale*4)
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
            RGBQUAD color = (N[i] < nMax) ? RGBQUAD {(BYTE)(255 - c), (BYTE)(c%5 * 64), c, 0} : RGBQUAD {0, 0, 0, 0};

            pixel[iy * width + ix + i] = color;
        }
    }
}

void draw_intrinsics_mandelbrot(float x0, float y0, int iy, float dx, float scale, int width, RGBQUAD* pixel)
{
    for (int ix = 0; ix < WINDOW_WIDTH; ix += 8, x0 += dx*scale*8)
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
                    
            __m256 cmp = _mm256_cmp_ps(r2, r2Max_vector, _CMP_LE_OQ);
                    
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
            RGBQUAD color = (N_array[i] < nMax) ? RGBQUAD {(BYTE)(255 - c), (BYTE)(c%5 * 64), c, 0} : RGBQUAD {0, 0, 0, 0};
            pixel[iy * width + ix + i] = color;
        }
    }
}
