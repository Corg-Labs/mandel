# Mandelbrot Infinite Zoom in C

A terminal-based, real-time infinite zoom into the Mandelbrot set, written entirely in C.

The program smoothly zooms toward a self-similar Misiurewicz point in seahorse valley until the limits of double-precision arithmetic are reached — then resets and zooms again. Colors flow continuously via a cosine-based RGB palette and a smooth iteration count.

No graphics library. No GPU. Just complex arithmetic and stdout.

This project focuses on learning:

- complex iteration / dynamical systems
- the Mandelbrot escape criterion
- smooth iteration count
- color palettes from cosine waves
- dynamic precision / iteration budgeting
- ANSI 256-color terminal rendering
- real-time rendering in C

---

# Features

- Real-time zoom into a self-similar region of the Mandelbrot set
- Smooth iteration coloring (no banding)
- Cosine RGB palette with a slowly-drifting color cycle
- Iteration count scales automatically with zoom depth
- Pure terminal rendering
- Written entirely in C
- Standard libraries only

---

# How It Works

For every pixel in the terminal, the program maps the pixel to a complex number `c`, then iterates `z = z^2 + c` starting from `z = 0`. If `|z|` ever exceeds 2, the point *escapes* the set; the number of iterations needed to escape (smoothed for a continuous result) becomes the pixel's color.

For every frame:

- Compute the current zoom scale (shrinks each frame)
- For each pixel, map to a complex number around the zoom target
- Iterate `z = z^2 + c` until escape or max iterations
- Apply smooth iteration count
- Look up color in a cosine palette
- Print the pixel as a colored space character

When the zoom reaches the floating-point precision limit (~1e-13), it resets to the starting scale and begins again.

---

# Tutorial / Rendering Pipeline

## 1. The Mandelbrot Set

The Mandelbrot set is the set of all complex numbers `c` for which the iteration

```
z = 0
z = z^2 + c
z = z^2 + c
z = z^2 + c
...
```

stays bounded forever. Points outside the set escape to infinity at different speeds — that "speed of escape" gives the iconic colored bands.

---

## 2. The Escape Test

For each pixel:

```
zr = 0; zi = 0;
while iter < max_iter:
    zr2 = zr * zr
    zi2 = zi * zi
    if zr2 + zi2 > 256: break       // escaped
    zi = 2*zr*zi + imag(c)
    zr = zr2 - zi2 + real(c)
    iter++
```

We use a larger escape radius (256 instead of 4) because it improves smooth coloring.

---

## 3. Smooth Iteration Count

Plain integer iteration counts produce ugly banded colors. The standard fix is the *smooth iteration count*:

```
nu = iter + 1 - log(log(|z|)) / log(2)
```

This gives a continuous real number instead of an integer, so colors flow smoothly across the entire fractal.

---

## 4. Cosine Color Palette

A simple, beautiful palette comes from three offset cosines:

```
r = 0.5 + 0.5 * cos(2 pi * (t + 0.00))
g = 0.5 + 0.5 * cos(2 pi * (t + 0.33))
b = 0.5 + 0.5 * cos(2 pi * (t + 0.66))
```

Each channel is a cosine wave at a different phase — the result cycles through every hue smoothly. Multiplying `t` by a small factor controls the density of the bands; adding a slowly-growing `color_shift` makes the palette drift over time.

---

## 5. Zooming

Each frame, the scale shrinks by a constant factor:

```
scale *= 0.965;
```

Around the chosen zoom target `(CX, CY)`, this pulls successive frames deeper into the set. The Misiurewicz point in seahorse valley is self-similar — you'll see the same spiral structures appearing at every scale.

When `scale` falls below `1e-13` (the floor of double precision), it resets to `3.0` and the zoom loops.

---

## 6. Dynamic Iteration Budget

Deeper zooms need more iterations to resolve detail. We scale `max_iter` with the log of the zoom level:

```
max_iter = MAX_ITER_BASE + (int)(-log(scale / 3.0) * 30);
```

At full zoom (scale = 3) we use ~200 iters; at deepest zoom we use ~800.

---

## 7. ANSI 256-Color Output

Each pixel's RGB is quantized to the 6x6x6 color cube of ANSI 256 colors:

```
index = 16 + 36 * R + 6 * G + B    (with R, G, B in [0, 5])
```

The pixel is printed as a space with that color as its background:

```
printf("\x1b[48;5;%dm ", index);
```

Adjacent pixels with the same color share one escape code to keep output fast.

---

# Build

Compile using:

```
gcc mandel.c -o mandel -lm
```

Or just:

```
make
```

The `-lm` flag links the math library.

---

# Run

```
./mandel
```

Press `Ctrl+C` to exit.

For best results, run in a 256-color terminal at roughly 100 columns × 36 rows.

---

# Customizing

Edit the constants near the top of `mandel.c`:

- `WIDTH`, `HEIGHT` — resolution
- `CX`, `CY` — zoom target; try some famous spots:
  - Seahorse Valley:  `-0.745,  0.113`
  - Elephant Valley:  `0.275,   0.000`
  - The Spire:        `-0.7458, 0.10499`
  - Mini-Mandelbrot:  `-1.7693831, 0.0042368`
- Zoom speed - change the `0.965` multiplier
- Color density - change the `0.025` factor on `nu`

---

# Concepts Practiced

- Complex iteration
- Dynamical systems
- Escape-time fractals
- Smooth coloring
- Cosine color palettes
- Floating-point precision limits
- Dynamic algorithm parameters
- ANSI 256-color rendering
- Real-time rendering loops
- Terminal graphics

---

# Dependencies

Standard C libraries only:

- `stdio.h`
- `stdlib.h`
- `string.h`
- `math.h`
- `unistd.h`

No graphics engine required.

---

# Inspiration

Benoit Mandelbrot's discovery of the set named after him in 1980 was one of the first iconic visualizations of "computational mathematics" — beautiful, intricate structure emerging from a single line of recursion.

This program is a tiny tribute, fitting that infinite complexity into a terminal window with a few hundred lines of C.
