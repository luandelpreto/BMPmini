#include "BMPmini.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    BMPmini_image *img = BMPmini_read(argv[1]);

    if (img) {
        int res = BMPmini_write("images/testeBMP.bmp", img);
        if (res != BMPmini_SUCCESS) {
            fprintf(stderr, "FAIL TO WRITE\n");
        }
    }
    else {
        fprintf(stderr, "NULL IMG\n");
    }

    BMPmini_image *newimg = BMPmini_crop(img, 2500, 2000, 2500, 1000);
    if (newimg) {
        int res = BMPmini_write("images/testeCroppedBMP.bmp", newimg);
        if (res != BMPmini_SUCCESS) {
            fprintf(stderr, "FAIL TO WRITE 2\n");
        }
    }
    else {
        fprintf(stderr, "NULL IMG 2\n");
    }

    BMPmini_free(img);
    BMPmini_free(newimg);

    return EXIT_SUCCESS;
}
