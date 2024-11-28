/**
 * @section LICENSE
 ******************************************************************************
 *           <b>(C) Copyright 2009 Energy Micro AS, www.energymicro.com</b>
 ******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source/compiled
 * code may not be used, redistributed or modified without a written consent
 * from Energy Micro AS.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************
 * @file glib_bitmap.c
 * @brief Energy Micro Graphics Library: Bitmap Drawing Routines
 * @author Energy Micro AS
 ******************************************************************************
 */

/* Standard C header files */
#include <stdint.h>

/* EM types */
#include "graphics/em_types.h"

/* GLIB header files */
#include "glib.h"

/**************************************************************************//**
*  @brief
*  Draws a bitmap
*
*  Sets up a bitmap that starts at x0,y0 and draws bitmap.
*  The picture is a 24-bit bitmap. It is organized as 8-bit
*  per color per pixel, and the color order is RGB. So picData is supposed to contain
*  at least (width * height * 3) entries. picData has be organized like this:
*  picData = { R, G, B, R, G, B, R, G, B ... }
*
*  @param pContext
*  Pointer to a GLIB_Context in which the bitmap is drawn.
*  @param x
*  Start x-coordinate for bitmap
*  @param y
*  Start y-coordinate for bitmap
*  @param width
*  Width of picture
*  @param height
*  Height of picture
*  @param pixData
*  Bitmap data 24-bit RGB
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawBitmap(const GLIB_Context *pContext, uint16_t x, uint16_t y,
                         uint16_t width, uint16_t height, uint8_t *picData)
{
  EMSTATUS status;

  /* Set display clipping area for bitmap */
  status = DMD_setClippingArea(x, y, width, height);
  if (status != DMD_OK) return status;

  /* Write bitmap to display */
  status = DMD_writeData(0, 0, picData, width * height);
  if (status != DMD_OK) return status;

  /* Reset display clipping area to the whole display */
  GLIB_resetDisplayClippingArea(pContext);
  if (status != GLIB_OK) return status;

  return GLIB_OK;
}
