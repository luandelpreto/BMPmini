#include "BMPmini.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "[USAGE]:\n%s <BMP image path>\n", argv[0]);
        return EXIT_FAILURE;
    }
    BMPmini_image *img = BMPmini_read(argv[1]);

    if (img) {
        int res = BMPmini_write("images/results/testeBMP.bmp", img);
        if (res != BMPmini_SUCCESS) {
            fprintf(stderr, "FAILED TO WRITE '%s'\n", argv[1]);
        }
    }
    else {
      fprintf(stderr, "FAILED TO READ '%s'\n", argv[1]);
    }

    BMPmini_image *newimg = BMPmini_crop(img, 0, 0, 500, 500);
    if (newimg) {
        int res = BMPmini_write("images/results/testeCroppedBMP.bmp", newimg);
        if (res != BMPmini_SUCCESS) {
            fprintf(stderr, "FAILED TO WRITE '%s'\n", argv[1]);
        }
    }
    else {
        fprintf(stderr, "FAILED TO READ '%s'\n", argv[1]);
    }

    BMPmini_free(img);
    BMPmini_free(newimg);

    return EXIT_SUCCESS;
}
