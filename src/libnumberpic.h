/* This is a shared library for rendering a png picture with numbers in it
 */
#include <inttypes.h>

/* A coloured pixel. */
typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}
pixel_t;

/* A picture. */
typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
}
bitmap_t;

typedef const pixel_t number_t[16384*7];

// could be private to the library, but well... for now its public
extern const number_t *numbers[];

// public functions

// Create a bitmap of size x/y
bitmap_t *numberpic_mkbitmap(int x, int y);

// Place number in bitmap at position x/y. bitmap must have been initialized before
void placenum(int nr, bitmap_t *bm, int x, int y);

// Access a single pixel in the bitmap
pixel_t * pixel_at (bitmap_t * bitmap, int x, int y);

// Write bitmap to png file
int save_png_to_file (bitmap_t *bitmap, const char *path);
int save_png_to_FILE(bitmap_t *bitmap, FILE *f);


