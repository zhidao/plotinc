/* plotinc is a library to plot graphs in C programs. */

#ifndef __PLOTINC_H__
#define __PLOTINC_H__

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <cairo/cairo-svg.h>

#define PLOTINC_CANVAS_DEFAULT_WIDTH  960
#define PLOTINC_CANVAS_DEFAULT_HEIGHT 640

#define PLOTINC_DEFAULT_FONT_SIZE      16
#define PLOTINC_BASELINE_MARGIN         4
#define PLOTINC_DEFAULT_TICS_NUM        4

#define PLOTINC_BORDER_LINEWIDTH        1.0
#define PLOTINC_AXIS_LINEWIDTH          1.0
#define PLOTINC_TICS_LINEWIDTH          1.5
#define PLOTINC_GRID_LINEWIDTH          0.5

#define PLOTINC_TICSLENGTH              6
#define PLOTINC_LABEL_MAXSIZE         100

#define PLOTINC_FONTNAME_MAXSIZE      125
#define PLOTINC_DEFAULT_FONT            "Times New Roman"

/* frame */

typedef struct _plotincFrame{
  int ox;
  int oy;
  int width;
  int height;
  int plot_ox;
  int plot_oy;
  int plot_width;
  int plot_height;
  int plot_xtics_num;
  int plot_ytics_num;
  char font_name[PLOTINC_FONTNAME_MAXSIZE];
  int font_pts;
  int baseline_skip;

  double range_xmin;
  double range_xmax;
  double range_ymin;
  double range_ymax;
  char label_x[PLOTINC_LABEL_MAXSIZE];
  char label_y[PLOTINC_LABEL_MAXSIZE];
  void (* draw)(struct _plotincFrame *, cairo_t *);

  bool flag_border;
  bool flag_xaxis;
  bool flag_yaxis;
  bool flag_xtics;
  bool flag_ytics;
  bool flag_xlabel;
  bool flag_ylabel;

  struct _plotincFrame *next;
} plotincFrame;

void plotincFrameInit(plotincFrame *frame);

void plotincFrameResize(plotincFrame *frame, int ox, int oy, int width, int height);

void plotincFrameSetFont(plotincFrame *frame, int size, char *fontname);

void plotincFrameSetXRange(plotincFrame *frame, double xmin, double xmax);
void plotincFrameSetYRange(plotincFrame *frame, double ymin, double ymax);

int plotincFrameXCoord(const plotincFrame *frame, double x);
int plotincFrameYCoord(const plotincFrame *frame, double y);

void plotincFrameSetXLabel(plotincFrame *frame, const char *label);
void plotincFrameSetYLabel(plotincFrame *frame, const char *label);

void plotincFrameDrawBorder(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawXAxis(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawYAxis(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawXTics(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawYTics(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawYTics(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawXLabel(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawYLabel(const plotincFrame *frame, cairo_t *cairo);

void plotincFrameDraw(plotincFrame *frame, cairo_t *cairo);

void plotincFrameDrawPoint(const plotincFrame *frame, cairo_t *cairo, double x, double y, double size);
void plotincFrameDrawLine(const plotincFrame *frame, cairo_t *cairo, double x0, double y0, double x1, double y1);

void plotincFrameSetRangeByData1D(plotincFrame *frame, const double data[], int size);
void plotincFramePlotData1D(const plotincFrame *frame, cairo_t *cairo, const double data[], int size);

void plotincFrameSetRangeByData2D(plotincFrame *frame, const double xdata[], const double ydata[], int size);
void plotincFramePlotData2D(const plotincFrame *frame, cairo_t *cairo, const double xdata[], const double ydata[], int size);

void plotincFramePlotFunction(const plotincFrame *frame, cairo_t *cairo, double (* function)(double), int sample_num);

/* canvas */

typedef struct{
  Display *display;
  Window win;
  XEvent event;

  cairo_surface_t *surface;
  cairo_t *cairo;

  int width;
  int height;

  int row_size;
  int col_size;
  int frame_num;
  plotincFrame *frame_list;
  plotincFrame *frame_last;
} plotincCanvas;

void plotincCanvasDestroyFrame(plotincCanvas *canvas);

void plotincCanvasResize(plotincCanvas *canvas, int width, int height);

bool plotincCanvasAddRowFrame(plotincCanvas *canvas);
bool plotincCanvasAddColFrame(plotincCanvas *canvas);

void plotincCanvasClear(plotincCanvas *canvas);
void plotincCanvasDraw(plotincCanvas *canvas);

bool plotincCanvasOpenX11(plotincCanvas *canvas, int width, int height);
void plotincCanvasCloseX11(plotincCanvas *canvas);

bool plotincCanvasOpenSVG(plotincCanvas *canvas, int width, int height, const char *filename);
void plotincCanvasCloseSVG(plotincCanvas *canvas);

#endif /* __PLOTINC_H__ */
