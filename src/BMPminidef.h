//----------------------------------------------------------------------------
// C Header file:
//              BMPminidef.h
//
// Internals for BMPmini
#ifndef _BMPMINI_DEF_H_
#define _BMPMINI_DEF_H_ 1

#include "BMPmini.h"

#if CHAR_BIT != 8
#error "BMPmini supports only 8-bit bytes"
#endif

#define _BMP_FTELL_ERR -1L
#define _BMP_FSEEK_ERR -2L

#define BMP_HEADER_SIZE 54U
#define DIB_HEADER_SIZE 40U
// Values for the header
#define BMP_MAGIC_VALUE      0x4D42
#define BMP_NUM_PLANES       1
#define BMP_COMPRESSION      0
#define BMP_NUM_COLORS       0
#define BMP_IMPORTANT_COLORS 0

struct _BMPmini_header {
    uint16_t type;             // Magic identifier
    uint32_t size;             // File size in bytes
    uint16_t reserved1;        // Not used
    uint16_t reserved2;        // Not used
    uint32_t offset;           // Offset to image data (in bytes) from the beginning of file
    uint32_t dib_header_size;  // DIB header size in bytes
    int32_t width_px;          // Width of the image
    int32_t height_px;         // Height of the image
    uint16_t num_planes;       // Number of color planes
    uint16_t bitsperpixel;     // Bits per pixel
    uint32_t compression;      // Compression type
    uint32_t image_size_bytes; // Image size in bytes
    int32_t x_res_ppm;         // Pixels per meter
    int32_t y_res_ppm;         // Pixels per meter
    uint32_t num_colors;       // Number of colors
    uint32_t important_colors; // Important colors
};

#ifndef FLEX_ARRAY
// Check if the compiler is known to support flexible array members
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && (!defined(__SUNPRO_C) || (_SUNPRO_C > 0x580))
  #define FLEX_ARRAY
#elif defined(__GNUC__)
  #if (__GNUC__ >= 3)
    #define FLEX_ARRAY
  #else
    #define FLEX_ARRAY 0  // Older GNU extension
  #endif
#endif
// Otherwise, default to safer but a bit wasteful traditional style
#ifndef FLEX_ARRAY
  #define FLEX_ARRAY 1
#endif
#endif

struct _BMPmini_image {
    BMPmini_header header;
    uint8_t data[FLEX_ARRAY];
};

//--------------------------------------------------------
// Internals
//--------------------------------------------------------
#define BMP_BITS_PER_PIXEL   24
#define BMP_BITS_PER_BYTE    8
#define BMP_BYTES_PER_PIXEL  (BMP_BITS_PER_PIXEL / BMP_BITS_PER_BYTE);

#define BMPmini_PERROR(func, msg, flag)                           \
    fprintf(stderr, "%s:%s:%lu: ", __FILE__, func, __LINE__+0UL); \
    BMPmini_perror(msg, flag)

static inline void BMPmini_perror(const char *restrict msg, bool flag)
{
    if (flag) {
        perror(msg);
    }
    else {
        fputs(msg, stderr);
    }
}

static inline bool __st_overflow(size_t a, size_t b)
{
    if (a > SIZE_MAX - b) {
        return true;
    }
    return false;
}

static inline bool __int32_overflow(int32_t a, int32_t b)
{
    if (a > INT32_MAX - b) {
        return true;
    }
    return false;
}

//-----------------------------------
// Inline functions
//-----------------------------------
static inline long __get_file_size(FILE *imgfp)
{
    long curr_pos = ftell(imgfp); // Store original file position
    if (curr_pos == -1L) {
        BMPmini_PERROR(__func__, "[ERROR]: an error occurred in ftell: ", 1);
        return _BMP_FTELL_ERR;
    }

    if (fseek(imgfp, 0, SEEK_END)) {
        BMPmini_PERROR(__func__, "[ERROR]: an error occurred in fseek: ", 1);
        return _BMP_FSEEK_ERR;
    }

    long file_size = ftell(imgfp);
    if (file_size == -1L) {
        BMPmini_PERROR(__func__, "[ERROR]: an error occurred in ftell: ", 1);
        return _BMP_FTELL_ERR;
    }

    if (fseek(imgfp, curr_pos, SEEK_SET)) {
        BMPmini_PERROR(__func__, "[ERROR]: an error occurred in fseek: ", 1);
        return _BMP_FSEEK_ERR;
    }

    return file_size;
}

static inline uint32_t __get_bytes_per_pixel(BMPmini_header *restrict header)
{
    return header->bitsperpixel / BMP_BITS_PER_BYTE;
}

static inline uint32_t __get_padding(BMPmini_header *restrict header)
{
    // BMP padding is (4 - <byte width> % 4) % 4
    return (4 - header->width_px * __get_bytes_per_pixel(header) % 4) % 4;
}

static inline uint32_t __get_image_row_size_bytes(BMPmini_header *restrict header)
{
    uint32_t bytes_per_row_no_padding = header->width_px * __get_bytes_per_pixel(header);
    return bytes_per_row_no_padding + __get_padding(header);
}

static inline uint32_t __get_image_size_bytes(BMPmini_header *restrict header)
{
    return __get_image_row_size_bytes(header) * header->height_px;
}

#endif
