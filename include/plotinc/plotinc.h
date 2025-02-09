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

#define PLOTINC_CANVAS_DEFAULT_WIDTH   960
#define PLOTINC_CANVAS_DEFAULT_HEIGHT  640
#define PLOTINC_CANVAS_DEFAULT_PADDING   4

#define PLOTINC_DEFAULT_FONT_SIZE       16
#define PLOTINC_BASELINE_MARGIN          4
#define PLOTINC_DEFAULT_TICS_NUM         4

#define PLOTINC_BORDER_LINEWIDTH         1.0
#define PLOTINC_AXIS_LINEWIDTH           1.0
#define PLOTINC_TICS_LINEWIDTH           1.5
#define PLOTINC_GRID_LINEWIDTH           0.5

#define PLOTINC_TICSLENGTH               6
#define PLOTINC_FRAMESTR_MAXSIZE       100

#define PLOTINC_FONTNAME_MAXSIZE       125
#define PLOTINC_DEFAULT_FONT             "Times New Roman"

/* axis */

typedef struct{
  double range_min;
  double range_max;
  int tics_num;
  char label[PLOTINC_FRAMESTR_MAXSIZE];
  /* flags to draw components */
  bool flag_tics;
  bool flag_grid;
  bool flag_label;
} plotincAxis;

/* frame */

typedef struct _plotincFrame{
  char title[PLOTINC_FRAMESTR_MAXSIZE];
  /* drawable region */
  int ox;
  int oy;
  int width;
  int height;
  /* plot region */
  int plot_ox;
  int plot_oy;
  int plot_width;
  int plot_height;
  /* font */
  char font_name[PLOTINC_FONTNAME_MAXSIZE];
  int font_pts;
  int baseline_skip;
  /* plot coordinate region */
  plotincAxis xaxis;
  plotincAxis yaxis;
  plotincAxis y2axis;
  /* drawing method */
  void (* draw)(struct _plotincFrame *, cairo_t *);
  /* flags to draw components */
  bool flag_title;
  /* list */
  struct _plotincFrame *next;
} plotincFrame;

void plotincFrameInit(plotincFrame *frame);
void plotincFrameSetTitle(plotincFrame *frame, const char *title);

void plotincFrameEnableXTics(plotincFrame *frame);
void plotincFrameEnableXGrid(plotincFrame *frame);
void plotincFrameEnableXLabel(plotincFrame *frame);
void plotincFrameEnableYTics(plotincFrame *frame);
void plotincFrameEnableYGrid(plotincFrame *frame);
void plotincFrameEnableYLabel(plotincFrame *frame);
void plotincFrameEnableY2Tics(plotincFrame *frame);
void plotincFrameEnableY2Grid(plotincFrame *frame);
void plotincFrameEnableY2Label(plotincFrame *frame);

void plotincFrameDisableXTics(plotincFrame *frame);
void plotincFrameDisableXGrid(plotincFrame *frame);
void plotincFrameDisableXLabel(plotincFrame *frame);
void plotincFrameDisableYTics(plotincFrame *frame);
void plotincFrameDisableYGrid(plotincFrame *frame);
void plotincFrameDisableYLabel(plotincFrame *frame);
void plotincFrameDisableY2Tics(plotincFrame *frame);
void plotincFrameDisableY2Grid(plotincFrame *frame);
void plotincFrameDisableY2Label(plotincFrame *frame);

void plotincFrameResize(plotincFrame *frame, int ox, int oy, int width, int height);

void plotincFrameSetFont(plotincFrame *frame, int size, char *fontname);

void plotincFrameSetXRange(plotincFrame *frame, double min, double max);
void plotincFrameSetYRange(plotincFrame *frame, double min, double max);
void plotincFrameSetY2Range(plotincFrame *frame, double min, double max);

void plotincFrameSetXLabel(plotincFrame *frame, const char *label);
void plotincFrameSetYLabel(plotincFrame *frame, const char *label);
void plotincFrameSetY2Label(plotincFrame *frame, const char *label);

int plotincFrameXCoord(const plotincFrame *frame, double x);
int plotincFrameYCoord(const plotincFrame *frame, double y);
int plotincFrameY2Coord(const plotincFrame *frame, double y);

void plotincFrameDrawTitle(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawBorder(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawXTics(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawYTics(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawY2Tics(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawXLabel(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawYLabel(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawY2Label(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawXGrid(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawYGrid(const plotincFrame *frame, cairo_t *cairo);
void plotincFrameDrawY2Grid(const plotincFrame *frame, cairo_t *cairo);

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
