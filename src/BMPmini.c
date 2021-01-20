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

/* TODO check endianness */
static void __parse_bytes2hdr(BMPmini_header *header, unsigned char *hdrdata)
{
    unsigned char *ptr = hdrdata;
    header->type             = (ptr[1] << 8) + ptr[0];
    header->size             = (ptr[5] << 24) + (ptr[4] << 16) + (ptr[3] << 8) + ptr[2];
    header->reserved1        = (ptr[7] << 8) + ptr[6];
    header->reserved2        = (ptr[9] << 8) + ptr[8];
    header->offset           = (ptr[13] << 24) + (ptr[12] << 16) + (ptr[11] << 8) + ptr[10];
    header->dib_header_size  = (ptr[17] << 24) + (ptr[16] << 16) + (ptr[15] << 8) + ptr[14];
    header->width_px         = (ptr[21] << 24) + (ptr[20] << 16) + (ptr[19] << 8) + ptr[18];
    header->height_px        = (ptr[25] << 24) + (ptr[24] << 16) + (ptr[23] << 8) + ptr[22];
    header->num_planes       = (ptr[27] << 8) + ptr[26];
    header->bitsperpixel     = (ptr[29] << 8) + ptr[28];
    header->compression      = (ptr[33] << 24) + (ptr[32] << 16) + (ptr[31] << 8) + ptr[30];
    header->image_size_bytes = (ptr[37] << 24) + (ptr[36] << 16) + (ptr[35] << 8) + ptr[34];
    header->x_res_ppm        = (ptr[41] << 24) + (ptr[40] << 16) + (ptr[39] << 8) + ptr[38];
    header->y_res_ppm        = (ptr[45] << 24) + (ptr[44] << 16) + (ptr[43] << 8) + ptr[42];
    header->num_colors       = (ptr[49] << 24) + (ptr[48] << 16) + (ptr[47] << 8) + ptr[46];
    header->important_colors = (ptr[53] << 24) + (ptr[52] << 16) + (ptr[51] << 8) + ptr[50];
}

static void __parse_hdr2bytes(BMPmini_header header, unsigned char *hdrdata)
{
    unsigned char *ptr = hdrdata;
    ptr[0] = (unsigned char) header.type;
    ptr[1] = (unsigned char) (header.type >> 8);
    ptr[2] = (unsigned char) header.size;
    ptr[3] = (unsigned char) (header.size >> 8);
    ptr[4] = (unsigned char) (header.size >> 16);
    ptr[5] = (unsigned char) (header.size >> 24);
    ptr[6] = (unsigned char) header.reserved1;
    ptr[7] = (unsigned char) (header.reserved1 >> 8);
    ptr[8] = (unsigned char) header.reserved2;
    ptr[9] = (unsigned char) (header.reserved2 >> 8);
    ptr[10] = (unsigned char) header.offset;
    ptr[11] = (unsigned char) (header.offset >> 8);
    ptr[12] = (unsigned char) (header.offset >> 16);
    ptr[13] = (unsigned char) (header.offset >> 24);
    ptr[14] = (unsigned char) header.dib_header_size;
    ptr[15] = (unsigned char) (header.dib_header_size >> 8);
    ptr[16] = (unsigned char) (header.dib_header_size >> 16);
    ptr[17] = (unsigned char) (header.dib_header_size >> 24);
    ptr[18] = (unsigned char) header.width_px;
    ptr[19] = (unsigned char) (header.width_px >> 8);
    ptr[20] = (unsigned char) (header.width_px >> 16);
    ptr[21] = (unsigned char) (header.width_px >> 24);
    ptr[22] = (unsigned char) header.height_px;
    ptr[23] = (unsigned char) (header.height_px >> 8);
    ptr[24] = (unsigned char) (header.height_px >> 16);
    ptr[25] = (unsigned char) (header.height_px >> 24);
    ptr[26] = (unsigned char) header.num_planes;
    ptr[27] = (unsigned char) (header.num_planes >> 8);
    ptr[28] = (unsigned char) header.bitsperpixel;
    ptr[29] = (unsigned char) (header.bitsperpixel >> 8);
    ptr[30] = (unsigned char) header.compression;
    ptr[31] = (unsigned char) (header.compression >> 8);
    ptr[32] = (unsigned char) (header.compression >> 16);
    ptr[33] = (unsigned char) (header.compression >> 24);
    ptr[34] = (unsigned char) header.image_size_bytes;
    ptr[35] = (unsigned char) (header.image_size_bytes >> 8);
    ptr[36] = (unsigned char) (header.image_size_bytes >> 16);
    ptr[37] = (unsigned char) (header.image_size_bytes >> 24);
    ptr[38] = (unsigned char) header.x_res_ppm;
    ptr[39] = (unsigned char) (header.x_res_ppm >> 8);
    ptr[40] = (unsigned char) (header.x_res_ppm >> 16);
    ptr[41] = (unsigned char) (header.x_res_ppm >> 24);
    ptr[42] = (unsigned char) header.y_res_ppm;
    ptr[43] = (unsigned char) (header.y_res_ppm >> 8);
    ptr[44] = (unsigned char) (header.y_res_ppm >> 16);
    ptr[45] = (unsigned char) (header.y_res_ppm >> 24);
    ptr[46] = (unsigned char) header.num_colors;
    ptr[47] = (unsigned char) (header.num_colors >> 8);
    ptr[48] = (unsigned char) (header.num_colors >> 16);
    ptr[49] = (unsigned char) (header.num_colors >> 24);
    ptr[50] = (unsigned char) header.important_colors;
    ptr[51] = (unsigned char) (header.important_colors >> 8);
    ptr[52] = (unsigned char) (header.important_colors >> 16);
    ptr[53] = (unsigned char) (header.important_colors >> 24);
}

BMPmini_image *BMPmini_read(const char *restrict filename)
{
    FILE *imgfp = fopen(filename, "rb");
    if (!imgfp) {
        BMPmini_PERROR(__func__, "[ERROR]: fopen", 1);
        return NULL;
    }

    /* Read BMP header */
    unsigned char hdrbytes[BMP_HEADER_SIZE];
    rewind(imgfp);
    size_t nbytes = fread(hdrbytes, BMP_HEADER_SIZE, 1, imgfp);
    if (nbytes != 1) {
        BMPmini_PERROR(__func__, "[ERROR]: fread", 1);
        goto CLEANUP_R1;
    }
    BMPmini_header header;
    __parse_bytes2hdr(&header, hdrbytes);

    /* Check BMP header */
    if (!BMPmini_check_header(&header, imgfp)) {
        BMPmini_PERROR(__func__, "[ERROR]: invalid BMP header", 0);
        goto CLEANUP_R1;
    }

    /* Account for data offset from header */
    if (__st_overflow(header.image_size_bytes, header.offset - BMP_HEADER_SIZE)) {
        BMPmini_PERROR(__func__, "[OVERFLOW]: size_t overflow encountered", 0);
        goto CLEANUP_R1;
    }

    size_t imgszbytes_and_offset = header.image_size_bytes + (header.offset - BMP_HEADER_SIZE);
    if (__st_overflow(sizeof(BMPmini_image), imgszbytes_and_offset)) {
        BMPmini_PERROR(__func__, "[OVERFLOW]: size_t overflow encountered", 0);
        goto CLEANUP_R1;
    }

    BMPmini_image *img = malloc(sizeof(*img) + imgszbytes_and_offset);
    if (!img) {
        BMPmini_PERROR(__func__, "[ERROR]: malloc", 1);
        goto CLEANUP_R1;
    }
    //memcpy(&img->header, &header, BMP_HEADER_SIZE);
    img->header = header;

    /* Read BMP image data */
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
    unsigned char hdrbytes[BMP_HEADER_SIZE];
    __parse_hdr2bytes(img->header, hdrbytes);
    size_t nbytes = fwrite(hdrbytes, BMP_HEADER_SIZE, 1, imgfp);
    if (nbytes != 1) {
        BMPmini_PERROR(__func__, "[ERROR]: fwrite", 1);
        return BMPmini_FWRITE_ERR;
    }

    nbytes = fwrite(img->data, img->header.image_size_bytes + (img->header.offset - BMP_HEADER_SIZE), 1, imgfp);
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

    /* Update new header size and dimensions info */
    BMPmini_header newheader = img->header;
    newheader.width_px = w;
    newheader.height_px = h;
    newheader.image_size_bytes = __get_image_size_bytes(&newheader);

    if (__st_overflow(newheader.image_size_bytes, newheader.offset - BMP_HEADER_SIZE)) {
        BMPmini_PERROR(__func__, "[OVERFLOW]: size_t overflow encountered", 0);
        return NULL;
    }
    size_t imgszbytes_and_offset = newheader.image_size_bytes + (newheader.offset - BMP_HEADER_SIZE);
    newheader.size = imgszbytes_and_offset + BMP_HEADER_SIZE;

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

    /* Get final crop positions */
    int32_t width = x + w;
    int32_t height = y + h;

    /* Bottom-Up DIB */
    if (img->header.height_px > 0) {
        y = img->header.height_px - height;
        height = y + h;
    }
    /* Top-Down DIB - nothing to do */

    int32_t newidx = newimg->header.offset - BMP_HEADER_SIZE;
    uint32_t bpp = __get_bytes_per_pixel(&img->header);
    uint32_t width_and_padding = img->header.width_px + __get_padding(&img->header);
    int32_t newimg_padding = __get_padding(&newimg->header);
    /* Size of offset is allocated within data array */
    uint8_t *data = img->data + (img->header.offset - BMP_HEADER_SIZE);
    for (int32_t i = y; i < height; i++) {
        for (int32_t j = x; j < width; j++) {
            uint32_t offset = j + width_and_padding * i;
            uint8_t *pixeloffset = data + offset * bpp;
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
    /* A BMP header is valid if:
     *   * It's magic number is 0x4d42
     *   * There is only one image plane
     *   * There is no compression
     *   * num_colors and important_colors are both 0
     *   * The image has BMP_BITS_PER_PIXEL bits per pixel
     *   * The 'size' and 'image_size_bytes' fields are correct in  relation
     *     to the bits, width, and height fields and in relation to the file
     *     size
     */
#if defined(NDEBUG)
    printf("TYPE:             Received: %"PRIu16" , expected: %d\n", header->type, BMP_MAGIC_VALUE);
    printf("NUM_PLANES:       Received: %"PRIu16" , expected: %d\n", header->num_planes, BMP_NUM_PLANES);
    printf("COMPRESSION:      Received: %"PRIu32" , expected: %d\n", header->compression, BMP_COMPRESSION);
    printf("NUM_COLORS:       Received: %"PRIu32" , expected: %d\n", header->num_colors, BMP_NUM_COLORS);
    printf("IMPORTANT_COLORS: Received: %"PRIu32" , expected: %d\n", header->important_colors, BMP_IMPORTANT_COLORS);
    printf("BITSPERPIXEL:     Received: %"PRIu16" , expected: %d\n", header->bitsperpixel, BMP_BITS_PER_PIXEL);
    printf("SIZE:             Received: %"PRIu32" , expected: %"PRIu32"\n", header->size, __get_file_size(imgfp));
    printf("IMAGE_SIZE_BYTES: Received: %"PRIu32" , expected: %"PRIu32"\n", header->image_size_bytes, __get_image_size_bytes(header));
    fflush(stdout);
#endif
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
