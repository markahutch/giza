/* giza - a scientific plotting layer built on cairo
 *
 * This file is (or was) part of GIZA, a scientific plotting
 * layer built on cairo.
 * GIZA comes with ABSOLUTELY NO WARRANTY.
 * This is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
 *
 * Copyright (C) 2010 James Wetter & Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */
#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-driver-svg-private.h"
#include "giza-drivers-private.h"

#include <giza.h>
#include <string.h>
#include <cairo/cairo-svg.h>

#define GIZA_DEFAULT_WIDTH 612
#define GIZA_DEFAULT_HEIGHT 459 /* use 4:3 aspect ratio */
#define GIZA_DEVICE_UNITS_PER_MM 2.8346
#define GIZA_DEVICE_UNITS_PER_PIXEL 0.765 /* so pixel resolution is same as pixel devices */
#define GIZA_DEVICE_INTERACTIVE 0
#define GIZA_DEVICE_EXTENSION ".svg"

/**
 * Opens an svg device for drawing to.
 *
 * The routine does the following:
 *  -Sets variables in the GIZA_Device structure Dev
 *  -Creates the cairo svg surface which is associated with a file
 *
 * Input:
 *  vert :- If set to one the created surface is portrait
 */
int
_giza_open_device_svg (int vert)
{
  int lenext = strlen (GIZA_DEVICE_EXTENSION);
  int length = strlen (Dev.prefix) + lenext + 5;
  char fileName[length + 1];
  _giza_get_filename_for_device(fileName,Dev.prefix,Dev.pgNum,GIZA_DEVICE_EXTENSION);

  Dev.deviceUnitsPermm    = GIZA_DEVICE_UNITS_PER_MM;
  Dev.deviceUnitsPerPixel = GIZA_DEVICE_UNITS_PER_PIXEL;
  Dev.isInteractive       = GIZA_DEVICE_INTERACTIVE;
  Dev.defaultBackgroundAlpha = 0.;

  /* set all device specific settings */
  if (_giza_sizeSpecified() )
    {
      _giza_get_specified_size(&Dev.width, &Dev.height);
    }
  else if (vert)
    {
      Dev.height = GIZA_DEFAULT_WIDTH;
      Dev.width  = GIZA_DEFAULT_HEIGHT;
    }
  else
    {
      Dev.width  = GIZA_DEFAULT_WIDTH;
      Dev.height = GIZA_DEFAULT_HEIGHT;
    }

  surface = cairo_svg_surface_create (fileName, Dev.width, Dev.height);

  if (!surface)
    {
      _giza_error ("_giza_open_device_svg", "Could not create cairo svg surface");
      return -1;
    }

  return 0;
}

/**
 * Flush the svg
 */
void
_giza_flush_device_svg (void)
{
  cairo_surface_flush (surface);
}

/**
 * Closes the svg device
 */
void
_giza_close_device_svg (void)
{
  cairo_surface_finish (surface);
  cairo_status_t status = cairo_surface_status (surface);
  if (status != CAIRO_STATUS_SUCCESS)
     _giza_error("giza_close_device_svg",cairo_status_to_string(status));
  cairo_surface_destroy (surface);
}

/**
 * Change the page
 */
void
_giza_change_page_svg (void)
{
  /* Close the old svg */
  cairo_destroy (context);
  _giza_close_device_svg();

  /* name the new svg */
  int lenext = strlen (GIZA_DEVICE_EXTENSION);
  int length = strlen (Dev.prefix) + lenext + 5;
  char fileName[length + 1];
  _giza_get_filename_for_device(fileName,Dev.prefix,Dev.pgNum + 1,GIZA_DEVICE_EXTENSION);

  /* Open it */
  surface = cairo_svg_surface_create (fileName, Dev.width, Dev.height);

  if (!surface)
    {
      _giza_error ("giza_change_page_svg", "Could not create cairo svg surface");
      return;
    }

  context = cairo_create (surface);
  if (!context)
    {
      _giza_error ("giza_change_page_svg", "Could not create cairo context");
      return;
    }
}
