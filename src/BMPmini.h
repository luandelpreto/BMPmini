//-----------------------------------------------------------------------------
// C Header file:
//              BMPmini.h
//
// Interface for BMPmini library
//-----------------------------------------------------------------------------
#ifndef _BMPMINI_H_
#define _BMPMINI_H_ 1

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>

enum {
    BMPmini_SUCCESS=0,
    BMPmini_FOPEN_ERR,
    BMPmini_FWRITE_ERR,
};

typedef struct _BMPmini_header BMPmini_header;
typedef struct _BMPmini_image BMPmini_image;

/***************************************************************
 * \brief  Reads a BMP image given its file path.
 *
 * \param filename  the path of the BMP image
 *
 * \return  a new BMPmini_image if successful
 * \return  NULL if an error occurs
 ***************************************************************/
extern BMPmini_image *BMPmini_read(const char *restrict filename);

/***************************************************************
 * \brief  Check if the header is a valid BMP header.
 *
 * \param  header   the header to be checked
 * \param  imgfp    the file ptr to the image  corresponding  to
 *                  this header
 *
 * \return  true    if the header is a valid BMP header
 * \return  false   otherwise
 ***************************************************************/
extern bool BMPmini_check_header(BMPmini_header *restrict header, FILE *imgfp);

/***************************************************************
 * \brief  Writes to a file the BMPmini_image.
 *
 * \param filename  the path name of the file to  be  create for
 *                  writing
 * \param img       the BMPmini_image to be written
 *
 * \return BMPmini_SUCCESS     if the writing is successful
 * \return BMPmini_FOPEN_ERR   if fails to open the file for writing
 * \return BMPmini_FWRITE_ERR  if fails to write to the file
 ***************************************************************/
extern int BMPmini_write(const char *restrict filename, BMPmini_image *img);

/***************************************************************
 * \brief  Deallocate the heap memory used  by the BMPmini_image
 *         object.
 *
 * \param  img   the BMPmini_image object to be deallocated
 ***************************************************************/
extern void BMPmini_free(BMPmini_image *img);

/***************************************************************
 * \brief  Crops the BMP image starting in (x, y)  with  w width
 *         and h height.
 *
 * \param  img  the image to be cropped
 * \param  x    the image x coordinate from which start the crop
 * \param  y    the image y coordinate from which start the crop
 * \param  w    the width of the cropped image
 * \param  h    the height of the cropped image
 *
 * \return  a new heap allocated image if successful
 * \return  a NULL pointer otherwise
 ***************************************************************/
extern BMPmini_image *BMPmini_crop(BMPmini_image *img, int32_t x, int32_t y, int32_t w, int32_t h);

#endif
