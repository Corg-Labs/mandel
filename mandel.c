/*
 * mandel.c - Mandelbrot infinite zoom animation
 *
 * Renders the Mandelbrot set in real time, smoothly zooming
 * into a self-similar Misiurewicz-point region until the limits
 * of double precision are reached, then resets and zooms again.
 *
 * Uses smooth iteration count for continuous color and a cosine
 * RGB palette for a rich, looping color cycle. Output is full
 * 256-color ANSI.
 *
 * Build:  gcc mandel.c -o mandel -lm
 * Run:    ./mandel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define WIDTH      100
#define HEIGHT     36
#define MAX_ITER_BASE 200
#define MAX_ITER_CAP  800

/* Zoom target - a Misiurewicz point in seahorse valley */
static const double CX = -0.74364386269;
static const double CY =  0.13182590271;

static int rgb_to_ansi(float r, float g, float b) {
    if (r < 0) r = 0;
    if (r > 1) r = 1;
    if (g < 0) g = 0;
    if (g > 1) g = 1;
    if (b < 0) b = 0;
    if (b > 1) b = 1;
    int ri = (int)(r * 5.0f + 0.5f);
    int gi = (int)(g * 5.0f + 0.5f);
    int bi = (int)(b * 5.0f + 0.5f);
    return 16 + 36 * ri + 6 * gi + bi;
}

/* Cosine palette - smooth, looping color cycle */
static int palette(float t) {
    float r = 0.5f + 0.5f * cosf(6.28318f * (t + 0.00f));
    float g = 0.5f + 0.5f * cosf(6.28318f * (t + 0.33f));
    float b = 0.5f + 0.5f * cosf(6.28318f * (t + 0.66f));
    return rgb_to_ansi(r, g, b);
}

int main(void) {
    printf("\x1b[?25l\x1b[2J");

    double scale = 3.0;
    float  color_shift = 0.0f;
    char   line[WIDTH * 24 + 32];

    for (;;) {
        /* Higher iter count as we zoom in for sharper detail */
        int max_iter = MAX_ITER_BASE + (int)(-log(scale / 3.0) * 30.0);
        if (max_iter > MAX_ITER_CAP) max_iter = MAX_ITER_CAP;
        if (max_iter < MAX_ITER_BASE) max_iter = MAX_ITER_BASE;

        double dx = scale / WIDTH;
        double dy = scale / WIDTH * 2.0;  /* compensate terminal aspect */

        printf("\x1b[H");
        for (int py = 0; py < HEIGHT; py++) {
            int prev = -1;
            char *q = line;
            for (int px = 0; px < WIDTH; px++) {
                double real = CX + (px - WIDTH / 2)  * dx;
                double imag = CY + (py - HEIGHT / 2) * dy;

                double zr = 0.0, zi = 0.0;
                int iter = 0;
                while (iter < max_iter) {
                    double zr2 = zr * zr;
                    double zi2 = zi * zi;
                    if (zr2 + zi2 > 256.0) break;
                    zi = 2.0 * zr * zi + imag;
                    zr = zr2 - zi2 + real;
                    iter++;
                }

                int col;
                if (iter >= max_iter) {
                    col = 16; /* inside the set: black */
                } else {
                    /* smooth iteration count */
                    double mag = sqrt(zr * zr + zi * zi);
                    double nu  = iter + 1.0 - log(log(mag)) / log(2.0);
                    float t = (float)(nu * 0.025) + color_shift;
                    t -= floorf(t);
                    col = palette(t);
                }

                if (col != prev) {
                    q += sprintf(q, "\x1b[48;5;%dm", col);
                    prev = col;
                }
                *q++ = ' ';
            }
            q += sprintf(q, "\x1b[0m\n");
            fwrite(line, 1, q - line, stdout);
        }
        fflush(stdout);

        scale *= 0.965;
        if (scale < 1e-13) scale = 3.0;   /* loop the zoom */
        color_shift += 0.004f;

        usleep(30000);
    }
    return 0;
}
