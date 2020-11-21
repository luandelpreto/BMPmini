//-----------------------------------------------------------------------------
// C file:
//       BMPmini.c
//
// Implementation for BMPmini library
//-----------------------------------------------------------------------------
#include "BMPminidef.h"
#include <string.h>
#include <assert.h>

//-------------------------------------
// Inline functions prototypes
//-------------------------------------
static long __get_file_size(FILE *);
static uint32_t __get_bytes_per_pixel(BMPmini_header *restrict);
static uint32_t __get_position_x_row(int32_t, BMPmini_header *restrict);
static uint32_t __get_padding(BMPmini_header *restrict);
static uint32_t __get_image_row_size_bytes(BMPmini_header *restrict);
static uint32_t __get_image_size_bytes(BMPmini_header *restrict);
static void BMPmini_perror(const char *restrict, bool);
//-------------------------------------

BMPmini_image *BMPmini_read(const char *restrict filename)
{
    FILE *imgfp = fopen(filename, "rb");
    if (!imgfp) {
        BMPmini_PERROR(__func__, "[ERROR]: fopen", 1);
        return NULL;
    }

    // Read BMP header
    BMPmini_header header;
    rewind(imgfp);
    size_t nbytes = fread(&header, sizeof(header), 1, imgfp);
    if (nbytes != 1) {
        BMPmini_PERROR(__func__, "[ERROR]: fread", 1);
        goto CLEANUP_R1;
    }
    // Check BMP header
    bool validheader = BMPmini_check_header(&header, imgfp);
    if (!validheader) {
        BMPmini_PERROR(__func__, "[ERROR]: invalid BMP header", 0);
        goto CLEANUP_R1;
    }

    // Account for data offset from header
    if (header.image_size_bytes > SIZE_MAX - (header.offset - sizeof(header))) {
        BMPmini_PERROR(__func__, "[OVERFLOW]: size_t overflow encountered", 0);
        goto CLEANUP_R1;
    }

    size_t imgszbytes_and_offset = header.image_size_bytes + (header.offset - sizeof(header));
    if (sizeof(BMPmini_image) > SIZE_MAX - imgszbytes_and_offset) {
        BMPmini_PERROR(__func__, "[OVERFLOW]: size_t overflow encountered", 0);
        goto CLEANUP_R1;
    }

    BMPmini_image *img = malloc(sizeof(*img) + imgszbytes_and_offset);
    if (!img) {
        BMPmini_PERROR(__func__, "[ERROR]: malloc", 1);
        goto CLEANUP_R1;
    }
    memcpy(&img->header, &header, sizeof(img->header));

    // Read BMP image data
    nbytes = fread(img->data, imgszbytes_and_offset, 1, imgfp);

    //nbytes = fread(img->data, img->header.image_size_bytes, 1, imgfp);
    if (nbytes != 1) {
        BMPmini_PERROR(__func__, "[ERROR]: fread", 1);
        goto CLEANUP_R0;
    }

    if (fclose(imgfp) == EOF) {
        BMPmini_PERROR(__func__, "[WARN]: fclose", 1);
    }
    return img;
CLEANUP_R0:
    free(img);
    img = NULL;
CLEANUP_R1:
    if (fclose(imgfp) == EOF) {
        BMPmini_PERROR(__func__, "[WARN]: fclose", 1);
    }
    return NULL;
}

int BMPmini_write(const char *restrict filename, BMPmini_image *img)
{
    assert(img);

    FILE *imgfp = fopen(filename, "wb");
    if (!imgfp) {
        BMPmini_PERROR(__func__, "[ERROR]: fopen", 1);
        return BMPmini_FOPEN_ERR;
    }

    rewind(imgfp);
    size_t nbytes = fwrite(&img->header, sizeof(img->header), 1, imgfp);
    if (nbytes != 1) {
        BMPmini_PERROR(__func__, "[ERROR]: fwrite", 1);
        return BMPmini_FWRITE_ERR;
    }

    nbytes = fwrite(img->data, img->header.image_size_bytes, 1, imgfp);
    if (nbytes != 1) {
        BMPmini_PERROR(__func__, "[ERROR]: fwrite", 1);
        return BMPmini_FWRITE_ERR;
    }

    if (fclose(imgfp) == EOF) {
        BMPmini_PERROR(__func__, "[ERROR]: fclose", 1);
    }
    return BMPmini_SUCCESS;
}

BMPmini_image *BMPmini_crop(BMPmini_image *img, int32_t x, int32_t y, int32_t w, int32_t h)
{
    assert(img);

}

bool BMPmini_check_header(BMPmini_header *restrict header, FILE *imgfp)
{
    assert(header);
    // A BMP header is valid if:
    //   * It's magic number is 0x4d42
    //   * There is only one image plane
    //   * There is no compression
    //   * num_colors and important_colors are both 0
    //   * The image has BMP_BITS_PER_PIXEL bits per pixel
    //   * The 'size' and 'image_size_bytes' fields are correct in  relation
    //     to the bits, width, and height fields and in relation to the file
    //     size
    return header->type == BMP_MAGIC_VALUE
    && header->num_planes == BMP_NUM_PLANES
    && header->compression == BMP_COMPRESSION
    && header->num_colors == BMP_NUM_COLORS
    && header->important_colors == BMP_IMPORTANT_COLORS
    && header->bitsperpixel == BMP_BITS_PER_PIXEL
    && header->size == __get_file_size(imgfp)
    && header->image_size_bytes == __get_image_size_bytes(header);
}

void BMPmini_free(BMPmini_image *img)
{
    if (img) {
        free(img);
        img = NULL;
    }
}
