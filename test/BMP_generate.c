//-----------------------------------------------------------------------------
// C file:
//       BMP_generate.c
//
// Generates a new BMP image.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>

#define BMP_BYTES_PER_PIXEL 3U  // RGB
#define BMP_FILE_HEADER_SIZE 14U
#define BMP_INFO_HEADER_SIZE 40U

#define ERROR_MSG(func, msg, ...)                                                  \
    fprintf(stderr, "%s:%s:%lu: " msg, __FILE__, func, __LINE__+0UL, __VA_ARGS__); \
    exit(EXIT_FAILURE)

static unsigned char *BMP_info_header(size_t height, size_t width);
static unsigned char *BMP_file_header(size_t height, size_t stride);
static void BMP_generate(unsigned char *img, size_t height, size_t width, const char *restrict filename);

static unsigned char *BMP_info_header(size_t height, size_t width)
{
    static unsigned char info_header[] = {
        0,0,0,0,   // header size
        0,0,0,0,   // image width
        0,0,0,0,   // image height
        0,0,       // number of color planes
        0,0,       // bits per pixel
        0,0,0,0,   // compression
        0,0,0,0,   // image size
        0,0,0,0,   // horizontal resolution
        0,0,0,0,   // vertical resolution
        0,0,0,0,   // colors in color table
        0,0,0,0,   // important color count
    };

    info_header[0] = (unsigned char) BMP_INFO_HEADER_SIZE;
    info_header[4] = (unsigned char) width;
    info_header[5] = (unsigned char) (width >> 8);
    info_header[6] = (unsigned char) (width >> 16);
    info_header[7] = (unsigned char) (width >> 24);
    info_header[8] = (unsigned char) height;
    info_header[9] = (unsigned char) (height >> 8);
    info_header[10] = (unsigned char) (height >> 16);
    info_header[11] = (unsigned char) (height >> 24);
    info_header[12] = (unsigned char) 1;
    info_header[14] = (unsigned char) (BMP_BYTES_PER_PIXEL * CHAR_BIT);

    return info_header;
}

static unsigned char *BMP_file_header(size_t height, size_t stride)
{
    uintmax_t filesz = BMP_FILE_HEADER_SIZE  + BMP_INFO_HEADER_SIZE + (stride * height);

    static unsigned char file_header[] = {
        0,0,      // signature
        0,0,0,0,  // image file size in bytes
        0,0,0,0,  // reserved
        0,0,0,0,  // start of pixel array
    };

    file_header[0] = (unsigned char) 'B';
    file_header[1] = (unsigned char) 'M';
    file_header[2] = (unsigned char) filesz;
    file_header[3] = (unsigned char) (filesz >> 8);
    file_header[4] = (unsigned char) (filesz >> 16);
    file_header[5] = (unsigned char) (filesz >> 24);
    file_header[10] = (unsigned char) (BMP_FILE_HEADER_SIZE + BMP_INFO_HEADER_SIZE);

    return file_header;
}

void BMP_generate(unsigned char *img, size_t height, size_t width, const char *restrict filename)
{
    size_t byte_width = width * BMP_BYTES_PER_PIXEL;

    unsigned char padding[3] = {0, 0, 0};
    size_t paddingsz = (4 - byte_width % 4) % 4;
    size_t stride = byte_width + paddingsz;

    FILE *imgfp = fopen(filename, "wb");
    if (!imgfp) {
        ERROR_MSG(__func__, "[ERROR]: opening %s\nexiting...\n", filename);
    }

    unsigned char *file_header = BMP_file_header(height, stride);
    size_t nmemb = fwrite(file_header, 1, BMP_FILE_HEADER_SIZE, imgfp);
    if (nmemb != BMP_FILE_HEADER_SIZE) {
        ERROR_MSG(__func__, "[ERROR]: writing to %s\nexiting...\n", filename);
    }

    unsigned char *info_header = BMP_info_header(height, width);
    nmemb = fwrite(info_header, 1, BMP_INFO_HEADER_SIZE, imgfp);
    if (nmemb != BMP_INFO_HEADER_SIZE) {
        ERROR_MSG(__func__, "[ERROR]: writing to %s\nexiting...\n", filename);
    }

    for (size_t i = 0; i < height; i++) {
        fwrite(img + i * byte_width, BMP_BYTES_PER_PIXEL, width, imgfp);
        fwrite(padding, 1, paddingsz, imgfp);
    }

    if (fclose(imgfp) == EOF) {
        ERROR_MSG(__func__, "[WARN]: failed to close %s\nexiting...\n", filename);
    }
}

int main(void)
{
    const size_t height = 420;
    const size_t width = 860;
    unsigned char img[height][width][BMP_BYTES_PER_PIXEL];
    const char *filename = "BMP_testimg.bmp";

    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            img[i][j][2] = (unsigned char) (i * 255 / height);
            img[i][j][1] = (unsigned char) (j * 255 / width);
            img[i][j][0] = (unsigned char) ((i+j) * 255 / (width+height));
        }
    }

    BMP_generate((unsigned char *) img, height, width, filename);
    return 0;
}
