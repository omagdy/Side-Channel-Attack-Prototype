/*
 * sample code partially based on 
 * https://www.lemoda.net/c/write-png/
 */

#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "libnumberpic.h"


int LOOPFOREVER=0;
// Can be set to 1 for testing purposes (inifite loop accessing the shared library)

static int pix (int value, int max)
{
    if (value < 0) {
        return 0;
    }
    return (int) (256.0 *((double) (value)/(double) max));
}

int main (int argc, char *argv[1])
{
    bitmap_t *test;
    int x;
    int y;
    int status;

    status = 0;
    if(argc>0 && atoi(argv[1])) { LOOPFOREVER=1; }

    /* Create an image. */
    test = numberpic_mkbitmap(100, 100);
    if (!test) {
	return -1;
    }

    for (y = 0; y < test->height; y++) {
        for (x = 0; x < test->width; x++) {
            pixel_t * pixel = pixel_at (test, x, y);
            pixel->red = pix (x, test->width);
            pixel->green = pix (y, test->height);
        }
    }
    do {
       placenum(1, test, 10, 20);
       placenum(4, test, 40, 44);
       sleep(1);
       // usleep(1); //For part 2a
    } while(LOOPFOREVER);

    /* Write the image to a file 'test.png'. */
    if (save_png_to_file (test, "test.png")) {
	fprintf (stderr, "Error writing file.\n");
	status = -1;
    }

    free(test->pixels);
    free(test);

    return status;
}

