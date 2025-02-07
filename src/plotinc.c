#include <plotinc/plotinc.h>

/* frame */

/* initialize a frame. */
void plotincFrameInit(plotincFrame *frame)
{
  frame->plot_xtics_num = PLOTINC_DEFAULT_TICS_NUM;
  frame->plot_ytics_num = PLOTINC_DEFAULT_TICS_NUM;
  plotincFrameSetFont( frame, PLOTINC_DEFAULT_FONT_SIZE, PLOTINC_DEFAULT_FONT );
  plotincFrameResize( frame, 0, 0, 0, 0 );
  plotincFrameSetXRange( frame, -10, 10 );
  plotincFrameSetYRange( frame, -10, 10 );
  frame->label_x[0] = frame->label_y[0] = '\0';
  frame->draw = NULL;

  frame->flag_border = true;
  frame->flag_xaxis = true;
  frame->flag_yaxis = true;
  frame->flag_xtics = true;
  frame->flag_ytics = true;
  frame->flag_xlabel = true;
  frame->flag_ylabel = true;

  frame->next = NULL;
}

/* resize a frame. */
void plotincFrameResize(plotincFrame *frame, int ox, int oy, int width, int height)
{
  frame->baseline_skip = frame->font_pts + PLOTINC_BASELINE_MARGIN;
  frame->ox = ox;
  frame->oy = oy;
  frame->width = width;
  frame->height = height;
  frame->plot_ox = ox + frame->baseline_skip * 2;
  frame->plot_oy = oy + frame->baseline_skip * 0.5;
  frame->plot_width = width - frame->baseline_skip * 3;
  frame->plot_height = height - frame->baseline_skip * 3;
}

/* set font of a frame. */
void plotincFrameSetFont(plotincFrame *frame, int size, char *fontname)
{
  frame->font_pts = size;
  strncpy( frame->font_name, fontname, PLOTINC_FONTNAME_MAXSIZE-1 );
}

/* set x-range of a frame. */
void plotincFrameSetXRange(plotincFrame *frame, double xmin, double xmax)
{
  frame->range_xmin = xmin;
  frame->range_xmax = xmax;
}

/* set y-range of a frame. */
void plotincFrameSetYRange(plotincFrame *frame, double ymin, double ymax)
{
  frame->range_ymin = ymin;
  frame->range_ymax = ymax;
}

/* convert a double-precision value to x-component of coordinates. */
int plotincFrameXCoord(const plotincFrame *frame, double x)
{
  double r;

  r = ( x - frame->range_xmin ) / ( frame->range_xmax - frame->range_xmin );
  if( r < 0 || r > 1 ) return -1;
  return frame->plot_ox + frame->plot_width * r;
}

/* convert a double-precision value to y-component of coordinates. */
int plotincFrameYCoord(const plotincFrame *frame, double y)
{
  double r;

  r = ( frame->range_ymax - y ) / ( frame->range_ymax - frame->range_ymin );
  if( r < 0 || r > 1 ) return -1;
  return frame->plot_oy + frame->plot_height * r;
}

/* recall font of a frame. */
static void _plotincFrameRecallFont(const plotincFrame *frame, cairo_t *cairo)
{
  cairo_set_font_size( cairo, frame->font_pts );
  cairo_select_font_face( cairo, frame->font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL );
}

/* set x-label of a frame. */
void plotincFrameSetXLabel(plotincFrame *frame, const char *label)
{
  strncpy( frame->label_x, label, PLOTINC_LABEL_MAXSIZE-1 );
}

/* set y-label of a frame. */
void plotincFrameSetYLabel(plotincFrame *frame, const char *label)
{
  strncpy( frame->label_y, label, PLOTINC_LABEL_MAXSIZE-1 );
}

/* draw border lines of a frame. */
void plotincFrameDrawBorder(const plotincFrame *frame, cairo_t *cairo)
{
  cairo_set_line_width( cairo, PLOTINC_BORDER_LINEWIDTH );
  cairo_set_source_rgb( cairo, 0, 0, 0 ); /* black */
  cairo_rectangle( cairo, frame->plot_ox, frame->plot_oy, frame->plot_width, frame->plot_height );
  cairo_stroke( cairo );
}

/* draw x-axis of a frame. */
void plotincFrameDrawXAxis(const plotincFrame *frame, cairo_t *cairo)
{
  int zero;

  if( ( zero = plotincFrameYCoord( frame, 0 ) ) > 0 ){
    cairo_set_line_width( cairo, PLOTINC_AXIS_LINEWIDTH );
    cairo_move_to( cairo, frame->plot_ox, zero );
    cairo_line_to( cairo, frame->plot_ox + frame->plot_width, zero );
    cairo_stroke( cairo );
  }
}

/* draw y-axis of a frame. */
void plotincFrameDrawYAxis(const plotincFrame *frame, cairo_t *cairo)
{
  int zero;

  if( ( zero = plotincFrameXCoord( frame, 0 ) ) > 0 ){
    cairo_set_line_width( cairo, PLOTINC_AXIS_LINEWIDTH );
    cairo_move_to( cairo, zero, frame->plot_oy );
    cairo_line_to( cairo, zero, frame->plot_oy + frame->plot_height );
    cairo_stroke( cairo );
  }
}

static void _plotincFrameDrawXTicsOne(const plotincFrame *frame, cairo_t *cairo, int xtics, int y, int dy)
{
  cairo_move_to( cairo, xtics, frame->plot_oy + y );
  cairo_rel_line_to( cairo, 0, dy );
  cairo_stroke( cairo );
}
static void _plotincFrameDrawBottomXTicsOne(const plotincFrame *frame, cairo_t *cairo, int xtics)
{
  cairo_set_line_width( cairo, PLOTINC_TICS_LINEWIDTH );
  _plotincFrameDrawXTicsOne( frame, cairo, xtics, frame->plot_height, -PLOTINC_TICSLENGTH );
}
static void _plotincFrameDrawTopXTicsOne(const plotincFrame *frame, cairo_t *cairo, int xtics)
{
  cairo_set_line_width( cairo, PLOTINC_TICS_LINEWIDTH );
  _plotincFrameDrawXTicsOne( frame, cairo, xtics, 0, PLOTINC_TICSLENGTH );
}
static void _plotincFrameDrawXTicsVal(const plotincFrame *frame, cairo_t *cairo, double val, int xtics)
{
  cairo_text_extents_t te;
  char str[BUFSIZ];

  _plotincFrameRecallFont( frame, cairo );
  sprintf( str, "%g", val );
  cairo_text_extents( cairo, str, &te );
  cairo_move_to( cairo, xtics - te.width / 2, frame->plot_oy + frame->plot_height + frame->baseline_skip );
  cairo_show_text( cairo, str );
}
static void _plotincFrameSetGridLine(cairo_t *cairo)
{
  double dash[] = { 6.0, 10.0 };
  cairo_set_line_width( cairo, PLOTINC_GRID_LINEWIDTH );
  cairo_set_dash( cairo, dash, sizeof(dash)/sizeof(double), 0 );
}
static void _plotincFrameDrawXGridOne(const plotincFrame *frame, cairo_t *cairo, int xtics)
{
  _plotincFrameSetGridLine( cairo );
  _plotincFrameDrawXTicsOne( frame, cairo, xtics, 0, frame->plot_height );
  cairo_set_dash( cairo, NULL, 0, 0 );
}
static void _plotincFrameDrawXTics(const plotincFrame *frame, cairo_t *cairo, double val, int tics)
{
  _plotincFrameDrawTopXTicsOne( frame, cairo, tics );
  _plotincFrameDrawBottomXTicsOne( frame, cairo, tics );
  _plotincFrameDrawXTicsVal( frame, cairo, val, tics );
  _plotincFrameDrawXGridOne( frame, cairo, tics );
}

static void _plotincFrameDrawYTicsOne(const plotincFrame *frame, cairo_t *cairo, int ytics, int x, int dx)
{
  cairo_move_to( cairo, frame->plot_ox + x, ytics );
  cairo_rel_line_to( cairo, dx, 0 );
  cairo_stroke( cairo );
}
static void _plotincFrameDrawLeftYTicsOne(const plotincFrame *frame, cairo_t *cairo, int ytics)
{
  cairo_set_line_width( cairo, PLOTINC_TICS_LINEWIDTH );
  _plotincFrameDrawYTicsOne( frame, cairo, ytics, 0, PLOTINC_TICSLENGTH );
}
static void _plotincFrameDrawRightYTicsOne(const plotincFrame *frame, cairo_t *cairo, int ytics)
{
  cairo_set_line_width( cairo, PLOTINC_TICS_LINEWIDTH );
  _plotincFrameDrawYTicsOne( frame, cairo, ytics, frame->plot_width, -PLOTINC_TICSLENGTH );
}
static void _plotincFrameDrawYTicsVal(const plotincFrame *frame, cairo_t *cairo, double val, int ytics)
{
  cairo_text_extents_t te;
  char str[BUFSIZ];

  _plotincFrameRecallFont( frame, cairo );
  sprintf( str, "%g", val );
  cairo_text_extents( cairo, str, &te );
  cairo_move_to( cairo, frame->plot_ox - te.width - PLOTINC_BASELINE_MARGIN, ytics + te.height/2 );
  cairo_show_text( cairo, str );
}
static void _plotincFrameDrawYGridOne(const plotincFrame *frame, cairo_t *cairo, int ytics)
{
  _plotincFrameSetGridLine( cairo );
  _plotincFrameDrawYTicsOne( frame, cairo, ytics, 0, frame->plot_width );
  cairo_set_dash( cairo, NULL, 0, 0 );
}
static void _plotincFrameDrawYTics(const plotincFrame *frame, cairo_t *cairo, double val, int tics)
{
  _plotincFrameDrawLeftYTicsOne( frame, cairo, tics );
  _plotincFrameDrawRightYTicsOne( frame, cairo, tics );
  _plotincFrameDrawYTicsVal( frame, cairo, val, tics );
  _plotincFrameDrawYGridOne( frame, cairo, tics );
}

/* draw x-tics of a frame. */
void plotincFrameDrawXTics(const plotincFrame *frame, cairo_t *cairo)
{
  double tics_width, val;
  int n, tics;

  tics_width = ( frame->range_xmax - frame->range_xmin ) / frame->plot_xtics_num;
  n = floor( log10( tics_width ) );
  tics_width = ceil( tics_width / exp10( n ) ) * exp10( n );
  if( frame->range_xmax > 0 && frame->range_xmin < 0 ){
    for( n=0; ( tics = plotincFrameXCoord( frame, ( val = -n * tics_width ) ) ) >= 0; n++ )
      _plotincFrameDrawXTics( frame, cairo, val, tics );
    for( n=0; ( tics = plotincFrameXCoord( frame, ( val = n * tics_width ) ) ) >= 0; n++ )
      _plotincFrameDrawXTics( frame, cairo, val, tics );
  } else{
    for( n=0; ; n++ ){
      tics = plotincFrameXCoord( frame, ( val = ceil( frame->range_xmin / tics_width + n ) * tics_width ) );
      if( tics >= 0 )
        _plotincFrameDrawXTics( frame, cairo, val, tics );
      if( n > frame->plot_xtics_num ) break;
    }
  }
}

/* draw y-tics of a frame. */
void plotincFrameDrawYTics(const plotincFrame *frame, cairo_t *cairo)
{
  double tics_width, val;
  int n, tics;

  tics_width = ( frame->range_ymax - frame->range_ymin ) / frame->plot_ytics_num;
  n = floor( log10( tics_width ) );
  tics_width = ceil( tics_width / exp10( n ) ) * exp10( n );
  if( frame->range_ymax > 0 && frame->range_ymin < 0 ){
    for( n=0; ( tics = plotincFrameYCoord( frame, ( val = -n * tics_width ) ) ) >= 0; n++ )
      _plotincFrameDrawYTics( frame, cairo, val, tics );
    for( n=0; ( tics = plotincFrameYCoord( frame, ( val = n * tics_width ) ) ) >= 0; n++ )
      _plotincFrameDrawYTics( frame, cairo, val, tics );
  } else{
    for( n=0; ; n++ ){
      tics = plotincFrameYCoord( frame, ( val = ceil( frame->range_ymin / tics_width + n ) * tics_width ) );
      if( tics >= 0 )
        _plotincFrameDrawYTics( frame, cairo, val, tics );
      if( n > frame->plot_ytics_num ) break;
    }
  }
}

/* draw x-label of a frame. */
void plotincFrameDrawXLabel(const plotincFrame *frame, cairo_t *cairo)
{
  cairo_text_extents_t te;

  if( frame->label_x[0] ){
    _plotincFrameRecallFont( frame, cairo );
    cairo_text_extents( cairo, frame->label_x, &te );
    cairo_move_to( cairo,
      frame->plot_ox + ( frame->plot_width - te.width ) / 2,
      frame->plot_oy + frame->plot_height + frame->baseline_skip*2 );
    cairo_show_text( cairo, frame->label_x );
  }
}

/* draw y-label of a frame. */
void plotincFrameDrawYLabel(const plotincFrame *frame, cairo_t *cairo)
{
  cairo_text_extents_t te;

  if( frame->label_y[0] ){
    _plotincFrameRecallFont( frame, cairo );
    cairo_text_extents( cairo, frame->label_y, &te );
    cairo_save( cairo );
    cairo_translate( cairo, frame->plot_ox, frame->plot_oy + frame->plot_height/2 );
    cairo_rotate( cairo, -M_PI/2 );
    cairo_translate( cairo, -te.width/2, -frame->baseline_skip - PLOTINC_BASELINE_MARGIN );
    cairo_show_text( cairo, frame->label_y );
    cairo_restore( cairo );
  }
}

/* draw a frame. */
void plotincFrameDraw(plotincFrame *frame, cairo_t *cairo)
{
  if( frame->flag_border ) plotincFrameDrawBorder( frame, cairo );
  if( frame->flag_xaxis  ) plotincFrameDrawXAxis(  frame, cairo );
  if( frame->flag_xtics  ) plotincFrameDrawXTics(  frame, cairo );
  if( frame->flag_xlabel ) plotincFrameDrawXLabel( frame, cairo );
  if( frame->flag_yaxis  ) plotincFrameDrawYAxis(  frame, cairo );
  if( frame->flag_ytics  ) plotincFrameDrawYTics(  frame, cairo );
  if( frame->flag_ylabel ) plotincFrameDrawYLabel( frame, cairo );
  if( frame->draw )
    frame->draw( frame, cairo );
}

/* draw a 2D point on a frame. */
void plotincFrameDrawPoint(const plotincFrame *frame, cairo_t *cairo, double x, double y, double size)
{
  int px, py;

  if( ( px = plotincFrameXCoord( frame, x ) ) < 0 ) return;
  if( ( py = plotincFrameYCoord( frame, y ) ) < 0 ) return;
  cairo_move_to( cairo, px, py );
  cairo_arc( cairo, px, py, size, 0, 360 );
  cairo_fill( cairo );
}

/* draw a 2D line segment on a frame. */
void plotincFrameDrawLine(const plotincFrame *frame, cairo_t *cairo, double x0, double y0, double x1, double y1)
{
  int px, py;

  px = plotincFrameXCoord( frame, x0 );
  py = plotincFrameYCoord( frame, y0 );
  cairo_move_to( cairo, px, py );
  px = plotincFrameXCoord( frame, x1 );
  py = plotincFrameYCoord( frame, y1 );
  cairo_line_to( cairo, px, py );
  cairo_stroke( cairo );
}

static void _plotincFrameFindMinMax(const double data[], int size, double *min, double *max)
{
  int i;

  *min = *max = data[0];
  for( i=1; i<size; i++ ){
    if( data[i] < *min ) *min = data[i];
    if( data[i] > *max ) *max = data[i];
  }
}

/* set y-range of a frame based on 1-dimensional data. */
void plotincFrameSetRangeByData1D(plotincFrame *frame, const double data[], int size)
{
  double ymin, ymax;

  plotincFrameSetXRange( frame, 0, size-1 );
  _plotincFrameFindMinMax( data, size, &ymin, &ymax );
  if( ymax > ymin )
    plotincFrameSetYRange( frame, ymin, ymax );
}

/* plot 1-dimensional data on a frame. */
void plotincFramePlotData1D(const plotincFrame *frame, cairo_t *cairo, const double data[], int size)
{
  int i, px, py;

  px = plotincFrameXCoord( frame, 0 );
  py = plotincFrameYCoord( frame, data[0] );
  cairo_move_to( cairo, px, py );
  for( i=1; i<size; i++ ){
    px = plotincFrameXCoord( frame, i );
    py = plotincFrameYCoord( frame, data[i] );
    cairo_line_to( cairo, px, py );
  }
  cairo_stroke( cairo );
}

/* set x- and y-ranges of a frame based on 2-dimensional data. */
void plotincFrameSetRangeByData2D(plotincFrame *frame, const double xdata[], const double ydata[], int size)
{
  double xmin, xmax, ymin, ymax;

  _plotincFrameFindMinMax( xdata, size, &xmin, &xmax );
  if( xmax > xmin )
    plotincFrameSetXRange( frame, xmin, xmax );
  _plotincFrameFindMinMax( ydata, size, &ymin, &ymax );
  if( ymax > ymin )
    plotincFrameSetYRange( frame, ymin, ymax );
}

/* plot 2-dimensional data on a frame. */
void plotincFramePlotData2D(const plotincFrame *frame, cairo_t *cairo, const double xdata[], const double ydata[], int size)
{
  int i, px, py;

  px = plotincFrameXCoord( frame, xdata[0] );
  py = plotincFrameYCoord( frame, ydata[0] );
  cairo_move_to( cairo, px, py );
  for( i=1; i<size; i++ ){
    px = plotincFrameXCoord( frame, xdata[i] );
    py = plotincFrameYCoord( frame, ydata[i] );
    cairo_line_to( cairo, px, py );
  }
  cairo_stroke( cairo );
}

/* plot a function on a frame. */
void plotincFramePlotFunction(const plotincFrame *frame, cairo_t *cairo, double (* function)(double), int sample_num)
{
  double *xdata, *ydata;
  int i;

  xdata = malloc( sizeof(double)*sample_num );
  ydata = malloc( sizeof(double)*sample_num );
  if( !xdata || !ydata ){
    fprintf( stderr, "cannot allocate buffer for sampling." );
    goto TERMINATE;
  }
  for( i=0; i<sample_num; i++ ){
    xdata[i] = frame->range_xmin + ( frame->range_xmax - frame->range_xmin ) * (double)i / sample_num;
    ydata[i] = function( xdata[i] );
  }
  plotincFramePlotData2D( frame, cairo, xdata, ydata, sample_num );
 TERMINATE:
  free( xdata );
  free( ydata );
}

/* canvas */

static void _plotincCanvasSetSize(plotincCanvas *canvas, int width, int height)
{
  canvas->width = width;
  canvas->height = height;
}

static bool _plotincCanvasInitFrame(plotincCanvas *canvas)
{
  canvas->row_size = 1;
  canvas->col_size = 1;
  canvas->frame_num = 0;
  canvas->frame_list = canvas->frame_last = NULL;
  if( !plotincCanvasAddRowFrame( canvas ) ) return false;
  return true;
}

static void _plotincCanvasDestroyFrame(plotincCanvas *canvas)
{
  plotincFrame *frame_ptr;

  while( canvas->frame_list ){
    frame_ptr = canvas->frame_list->next;
    free( canvas->frame_list );
    canvas->frame_list = frame_ptr;
  }
}

static bool _plotincCanvasAllocFrame(plotincCanvas *canvas)
{
  plotincFrame *new_frame;

  if( !( new_frame = malloc( sizeof(plotincFrame) ) ) ){
    fprintf( stderr, "cannot allocate memory for a new canvas." );
    return false;
  }
  plotincFrameInit( new_frame );
  if( !canvas->frame_list )
    canvas->frame_list = canvas->frame_last = new_frame;
  else{
    canvas->frame_last->next = new_frame;
    canvas->frame_last = new_frame;
  }
  canvas->frame_num++;
  return true;
}

static void _plotincCanvasResizeFrame(plotincCanvas *canvas)
{
  int i, j, frame_width, frame_height;
  plotincFrame *frame_ptr;

  frame_width  = canvas->width  / canvas->col_size;
  frame_height = canvas->height / canvas->row_size;
  frame_ptr = canvas->frame_list;
  for( i=0; i<canvas->row_size; i++ )
    for( j=0; j<canvas->col_size; j++ ){
      if( !frame_ptr ) return;
      plotincFrameResize( frame_ptr,
        (double)canvas->width  * (double)j / canvas->col_size,
        (double)canvas->height * (double)i / canvas->row_size,
        frame_width, frame_height );
      frame_ptr = frame_ptr->next;
    }
}

/* resize a canvas. */
void plotincCanvasResize(plotincCanvas *canvas, int width, int height)
{
  _plotincCanvasSetSize( canvas, width, height );
  _plotincCanvasResizeFrame( canvas );
}

/* add a frame in row-direction to a canvas. */
bool plotincCanvasAddRowFrame(plotincCanvas *canvas)
{
  if( !_plotincCanvasAllocFrame( canvas ) ) return false;
  if( canvas->row_size * canvas->col_size < canvas->frame_num )
    canvas->row_size++;
  _plotincCanvasResizeFrame( canvas );
  return true;
}

/* add a frame in column-direction to a canvas. */
bool plotincCanvasAddColFrame(plotincCanvas *canvas)
{
  if( !_plotincCanvasAllocFrame( canvas ) ) return false;
  if( canvas->row_size * canvas->col_size < canvas->frame_num )
    canvas->col_size++;
  _plotincCanvasResizeFrame( canvas );
  return true;
}

/* clear background of a canvas. */
void plotincCanvasClear(plotincCanvas *canvas)
{
  cairo_set_source_rgb( canvas->cairo, 1, 1, 1 ); /* white */
  cairo_rectangle( canvas->cairo, 0, 0, canvas->width, canvas->height );
  cairo_fill( canvas->cairo );
}

/* draw a canvas. */
void plotincCanvasDraw(plotincCanvas *canvas)
{
  plotincFrame *frame_ptr;

  plotincCanvasClear( canvas );
  for( frame_ptr=canvas->frame_list; frame_ptr; frame_ptr=frame_ptr->next ){
    plotincFrameDraw( frame_ptr, canvas->cairo );
  }
  cairo_show_page( canvas->cairo );
}

static void _plotincCanvasClose(plotincCanvas *canvas)
{
  _plotincCanvasDestroyFrame( canvas );

  cairo_destroy( canvas->cairo );
  cairo_surface_destroy( canvas->surface );
}

/* open a canvas on X-Window system. */
bool plotincCanvasOpenX11(plotincCanvas *canvas, int width, int height)
{
  /* connect to X server */
  canvas->display = XOpenDisplay( NULL );
  canvas->win = XCreateSimpleWindow( canvas->display, RootWindow( canvas->display, DefaultScreen(canvas->display) ),
    0, 0, width, height, 0,
    WhitePixel( canvas->display, DefaultScreen(canvas->display) ),
    BlackPixel( canvas->display, DefaultScreen(canvas->display) ) );
  XMapWindow( canvas->display, canvas->win );
  XSelectInput( canvas->display, canvas->win, ExposureMask );
  while( 1 ){
    XNextEvent( canvas->display, &canvas->event );
    if( canvas->event.type == Expose ) break;
  }
  /* assign cairo surface and context */
  canvas->surface = cairo_xlib_surface_create( canvas->display, canvas->win, DefaultVisual(canvas->display,0), width, height );
  canvas->cairo = cairo_create( canvas->surface );
  /* size */
  _plotincCanvasSetSize( canvas, width, height );
  return _plotincCanvasInitFrame( canvas );
}

/* close a canvas on X-Window system. */
void plotincCanvasCloseX11(plotincCanvas *canvas)
{
  _plotincCanvasClose( canvas );

  XDestroyWindow( canvas->display, canvas->win );
  XCloseDisplay( canvas->display );
}

/* open a canvas on a SVG file. */
bool plotincCanvasOpenSVG(plotincCanvas *canvas, int width, int height, const char *filename)
{
  /* assign cairo surface and context */
  canvas->surface = cairo_svg_surface_create( filename, width, height );
  canvas->cairo = cairo_create( canvas->surface );
  /* size */
  _plotincCanvasSetSize( canvas, width, height );
  return _plotincCanvasInitFrame( canvas );
}

/* close a canvas on a SVG file. */
void plotincCanvasCloseSVG(plotincCanvas *canvas)
{
  _plotincCanvasClose( canvas );
}
