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
static uint32_t __get_padding(BMPmini_header *restrict);
static uint32_t __get_image_row_size_bytes(BMPmini_header *restrict);
static uint32_t __get_image_size_bytes(BMPmini_header *restrict);
static void BMPmini_perror(const char *restrict, bool);
static bool __st_overflow(size_t, size_t);
static bool __int32_overflow(int32_t, int32_t);
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
    if (__st_overflow(header.image_size_bytes, header.offset - sizeof(header))) {
        BMPmini_PERROR(__func__, "[OVERFLOW]: size_t overflow encountered", 0);
        goto CLEANUP_R1;
    }

    size_t imgszbytes_and_offset = header.image_size_bytes + (header.offset - sizeof(header));
    if (__st_overflow(sizeof(BMPmini_image), imgszbytes_and_offset)) {
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
    assert(x >= 0 && y >= 0 && w > 0 && h > 0);

    if (__int32_overflow(x, w) || __int32_overflow(y, h)) {
        BMPmini_PERROR(__func__, "[OVERFLOW]: int32_t overflow encountered", 0);
        return NULL;
    }

    if (x+w > img->header.width_px || y+h > img->header.height_px) {
        BMPmini_PERROR(__func__, "[ERROR]: size of the new image greater than original", 0);
        return NULL;
    }

    // Update new header size and dimensions info
    BMPmini_header newheader = img->header;
    newheader.width_px = w;
    newheader.height_px = h;
    newheader.image_size_bytes = __get_image_size_bytes(&newheader);
    newheader.size = BMP_HEADER_SIZE + newheader.image_size_bytes;

    if (__st_overflow(newheader.image_size_bytes, newheader.offset - sizeof(newheader))) {
        BMPmini_PERROR(__func__, "[OVERFLOW]: size_t overflow encountered", 0);
        return NULL;
    }
    size_t imgszbytes_and_offset = newheader.image_size_bytes + (newheader.offset - sizeof(newheader));

    if (__st_overflow(sizeof(BMPmini_image), imgszbytes_and_offset)) {
        BMPmini_PERROR(__func__, "[OVERFLOW]: size_t overflow encountered", 0);
        return NULL;
    }

    BMPmini_image *newimg = malloc(sizeof(*newimg) + imgszbytes_and_offset);
    if (!newimg) {
        BMPmini_PERROR(__func__, "[ERROR]: malloc", 1);
        return NULL;
    }
    newimg->header = newheader;

    // Get final crop positions
    int32_t width = x + w;
    int32_t height = y + h;

    // Bottom-Up DIB
    if (img->header.height_px > 0) {
        y = img->header.height_px - height;
        height = y + h;
    }
    // Top-Down DIB - nothing to do

    int32_t newidx = newimg->header.offset - sizeof(newimg->header);
    uint32_t bpp = __get_bytes_per_pixel(&img->header);
    uint32_t width_and_padding = img->header.width_px + __get_padding(&img->header);
    int32_t newimg_padding = __get_padding(&newimg->header);
    // Size of offset is allocated within data array
    uint8_t *data = img->data + (img->header.offset - sizeof(img->header));
    for (int32_t i = y; i < height; i++) {
        for (int32_t j = x; j < width; j++) {
            uint32_t offset = j + width_and_padding * i;
            uint8_t *pixeloffset = data + offset * bpp; // Em algum momento, isso vai para o outro lado
            for (uint8_t k = 0; k < 3; k++) {
                newimg->data[newidx++] = pixeloffset[k];
            }
        }

        memset(newimg->data + newidx, 0, newimg_padding);
        newidx += newimg_padding;
    }

    return newimg;
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
