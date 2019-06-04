/*
 * sample code partially based on 
 * https://www.lemoda.net/c/write-png/
 */

#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "libnumberpic.h"


#define B {0,0,0}
#define V {255,255,0}

#define S 6
#define S2 12

typedef const pixel_t number_t[16384*7];

const number_t SP =
{B, B, B, B, B, B,
 B, B, B, B, B, B,
 B, B, B, B, B, B,
 B, B, B, B, B, B,
 B, B, B, B, B, B,
 B, B, B, B, B, B,};
const number_t ZERO =
{B, B, V, V, B, B,
 B, V, B, B, V, B,
 B, V, B, B, V, B,
 B, V, B, B, V, B,
 B, V, B, B, V, B,
 B, B, V, V, B, B};
const number_t ONE =
{B, B, B, V, B, B,
 B, B, V, V, B, B,
 B, V, B, V, B, B,
 B, B, B, V, B, B,
 B, B, B, V, B, B,
 B, B, V, V, V, B,};
const number_t TWO =
{B, B, V, V, B, B,
 B, V, B, B, V, B,
 B, B, B, V, B, B,
 B, B, V, B, B, B,
 B, V, B, B, B, B,
 B, V, V, V, V, B};
const number_t THREE =
{B, B, V, V, B, B,
 B, V, B, B, V, B,
 B, B, B, V, B, B,
 B, B, B, V, B, B,
 B, V, B, B, V, B,
 B, B, V, V, B, B};
const number_t FOUR =
{B, B, B, V, B, B,
 B, B, V, B, B, B,
 B, V, B, V, B, B,
 B, V, V, V, V, B,
 B, B, B, V, B, B,
 B, B, B, V, B, B};
const number_t FIVE =
{B, V, V, V, V, B,
 B, V, B, B, B, B,
 B, V, V, V, B, B,
 B, B, B, B, V, B,
 B, V, B, B, V, B,
 B, B, V, V, B, B};
const number_t SIX =
{B, B, V, V, B, B,
 B, V, B, B, V, B,
 B, V, B, B, B, B,
 B, V, V, V, B, B,
 B, V, B, B, V, B,
 B, B, V, V, B, B};
const number_t SEVEN =
{B, V, V, V, V, B,
 B, B, B, B, V, B,
 B, B, B, V, B, B,
 B, B, B, V, B, B,
 B, B, V, B, B, B,
 B, B, V, B, B, B};
const number_t EIGHT =
{B, B, V, V, B, B,
 B, V, B, B, V, B,
 B, B, V, V, B, B,
 B, V, B, B, V, B,
 B, V, B, B, V, B,
 B, B, V, V, B, B};
const number_t NINE =
{B, B, V, V, B, B,
 B, V, B, B, V, B,
 B, B, V, V, V, B,
 B, B, B, B, V, B,
 B, V, B, B, V, B,
 B, B, V, V, B, B};
const number_t UNDEF =
{B, B, V, V, B, B,
 B, V, B, B, V, B,
 B, B, V, V, V, B,
 B, B, B, V, V, B,
 B, V, B, B, V, B,
 B, B, V, V, B, B};

const number_t *numbers[]={&SP, &ZERO, &ONE, &TWO, &THREE, &FOUR, &FIVE, &SIX, &SEVEN, &EIGHT, &NINE, &UNDEF};

/* Given "bitmap", this returns the pixel of bitmap at the point ("x", "y"). */
pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}

static void mk(int nr, pixel_t *p, int w) {
   for(int i=0; i<S; i++) {
      for(int j=0; j<S; j++) {
	 number_t *num = numbers[nr+1];
	 //const number_t *num = numbers[nr];
	 const pixel_t pix = (*num)[S*j+i];
 	 p[w*j*2+2*i] = pix;
 	 p[w*j*2+2*i+1] = pix;
 	 p[w*(1+j*2)+2*i] = pix;
 	 p[w*(1+j*2)+2*i+1] = pix;
      }
   }
}



/// Public functions

bitmap_t *numberpic_mkbitmap(int w, int h) {
	bitmap_t *bm = malloc(sizeof(bitmap_t));
	if(!bm) return NULL;
	bm->width=w;
	bm->height=h;
	bm->pixels =calloc( w*h, sizeof(pixel_t));
	if(!bm->pixels) {
		free(bm);
		return NULL;
	}
	return bm;
}

void placenum(int nr, bitmap_t *bm, int x, int y) {
   mk(nr, pixel_at(bm, x, y), bm->width);
}
    
/* Write "bitmap" to a PNG file specified by "path"; returns 0 on
   success, non-zero on error. */
int save_png_to_file (bitmap_t *bitmap, const char *path)
{
    FILE * fp;
    
    fp = fopen (path, "wb");
    if (! fp) {
	return -1;
    }
    int retval = save_png_to_FILE(bitmap, fp);
    fclose(fp);
    return retval;
}

int save_png_to_FILE (bitmap_t *bitmap, FILE *fp)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int pixel_size = 3;
    int depth = 8;

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto png_create_write_struct_failed;
    }
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        goto png_create_info_struct_failed;
    }
    
    /* Set up error handling. */

    if (setjmp (png_jmpbuf (png_ptr))) {
        goto png_failure;
    }
    /* Set image attributes. */
    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap->width,
                  bitmap->height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    
    /* Initialize rows of PNG. */
    row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
    for (y = 0; y < bitmap->height; y++) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; x++) {
            pixel_t * pixel = pixel_at (bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }
    
    /* Write the image data to "fp". */
    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    status = 0;
    for (y = 0; y < bitmap->height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
 png_failure:
 png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
    return status;
}

