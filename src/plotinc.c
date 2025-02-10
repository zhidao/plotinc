#include <plotinc/plotinc.h>
#include <unistd.h>

/* axis */

static bool _plotincAxisZeroIsIncluded(const plotincAxis *axis){
  return axis->range_min < 0 && axis->range_max > 0;
}

static void _plotincAxisSetRange(plotincAxis *axis, double min, double max){
  axis->range_min = min;
  axis->range_max = max;
}

static void _plotincAxisSetTicsNum(plotincAxis *axis, int num){
  axis->tics_num = num;
}

static void _plotincAxisSetLabel(plotincAxis *axis, const char *label){
  if( label && label[0] ){
    strncpy( axis->label, label, PLOTINC_FRAMESTR_MAXSIZE-1 );
    axis->flag_label = true;
  } else{
    axis->label[0] = '\0';
    axis->flag_label = false;
  }
}

static void _plotincAxisInit(plotincAxis *axis){
  _plotincAxisSetRange( axis, -10, 10 );
  _plotincAxisSetTicsNum( axis, PLOTINC_DEFAULT_TICS_NUM );
  _plotincAxisSetLabel( axis, NULL );
  axis->flag_tics = false;
  axis->flag_grid = false;
}

static double _plotincAxisVal(const plotincAxis *axis, double ratio){
  return axis->range_min + ( axis->range_max - axis->range_min ) * ratio;
}

static double _plotincAxisValRatio(const plotincAxis *axis, double val){
  return ( val - axis->range_min ) / ( axis->range_max - axis->range_min );
}

static double _plotincAxisTicsWidth(const plotincAxis *axis){
  double w;
  int n;

  w = ( axis->range_max - axis->range_min ) / axis->tics_num;
  n = floor( log10( w ) );
  return ceil( w / exp10( n ) ) * exp10( n );
}

static double _plotincAxisTicsVal(const plotincAxis *axis, double tics_width, int i){
  return ceil( axis->range_min / tics_width + i ) * tics_width;
}

/* frame */

/* initialize a frame. */
void plotincFrameInit(plotincFrame *frame)
{
  frame->title[0] = '\0';
  plotincFrameSetFont( frame, PLOTINC_DEFAULT_FONT_SIZE, PLOTINC_DEFAULT_FONT );
  plotincFrameResize( frame, 0, 0, 0, 0 );
  _plotincAxisInit( &frame->xaxis );
  _plotincAxisInit( &frame->yaxis );
  _plotincAxisInit( &frame->y2axis );
  plotincFrameEnableXTics( frame );
  plotincFrameEnableYTics( frame );
  frame->draw = NULL;
  frame->flag_title = false;
  frame->next = NULL;
}

void plotincFrameSetTitle(plotincFrame *frame, const char *title)
{
  if( title && title[0] ){
    strncpy( frame->title, title, PLOTINC_FRAMESTR_MAXSIZE-1 );
    frame->flag_title = true;
  } else{
    frame->title[0] = '\0';
    frame->flag_title = false;
  }
}

void plotincFrameEnableXTics(plotincFrame *frame){ frame->xaxis.flag_tics = true; }
void plotincFrameEnableXGrid(plotincFrame *frame){ frame->xaxis.flag_grid = true; }
void plotincFrameEnableYTics(plotincFrame *frame){ frame->yaxis.flag_tics = true; }
void plotincFrameEnableYGrid(plotincFrame *frame){ frame->yaxis.flag_grid = true; }
void plotincFrameEnableY2Tics(plotincFrame *frame){ frame->y2axis.flag_tics = true; }
void plotincFrameEnableY2Grid(plotincFrame *frame){ frame->y2axis.flag_grid = true; }

void plotincFrameDisableXTics(plotincFrame *frame){ frame->xaxis.flag_tics = false; }
void plotincFrameDisableXGrid(plotincFrame *frame){ frame->xaxis.flag_grid = false; }
void plotincFrameDisableYTics(plotincFrame *frame){ frame->yaxis.flag_tics = false; }
void plotincFrameDisableYGrid(plotincFrame *frame){ frame->yaxis.flag_grid = false; }
void plotincFrameDisableY2Tics(plotincFrame *frame){ frame->y2axis.flag_tics = false; }
void plotincFrameDisableY2Grid(plotincFrame *frame){ frame->y2axis.flag_grid = false; }

/* resize a frame. */
void plotincFrameResize(plotincFrame *frame, int ox, int oy, int width, int height)
{
  frame->baseline_skip = frame->font_pts * 4.0 / 3.0; /* assuming 96ppi */
  frame->ox = ox;
  frame->oy = oy;
  frame->width = width;
  frame->height = height;
  frame->plot_ox = ox + frame->baseline_skip * 2 + PLOTINC_BASELINE_MARGIN;
  frame->plot_oy = oy + frame->baseline_skip + PLOTINC_BASELINE_MARGIN;
  frame->plot_width = frame->width - frame->baseline_skip * 4 - PLOTINC_BASELINE_MARGIN * 2;
  frame->plot_height = frame->height - frame->baseline_skip * 3 - PLOTINC_BASELINE_MARGIN * 2;
}

/* set font of a frame. */
void plotincFrameSetFont(plotincFrame *frame, int size, char *fontname)
{
  frame->font_pts = size;
  strncpy( frame->font_name, fontname, PLOTINC_FONTNAME_MAXSIZE-1 );
}

/* set x-range of a frame. */
void plotincFrameSetXRange(plotincFrame *frame, double min, double max)
{
  _plotincAxisSetRange( &frame->xaxis, min, max );
}

/* set y-range of a frame. */
void plotincFrameSetYRange(plotincFrame *frame, double min, double max)
{
  _plotincAxisSetRange( &frame->yaxis, min, max );
}

/* set y2-range of a frame. */
void plotincFrameSetY2Range(plotincFrame *frame, double min, double max)
{
  _plotincAxisSetRange( &frame->y2axis, min, max );
}

/* recall font of a frame. */
static void _plotincFrameRecallFont(const plotincFrame *frame, cairo_t *cairo)
{
  cairo_set_source_rgb( cairo, 0, 0, 0 ); /* black */
  cairo_set_font_size( cairo, frame->font_pts );
  cairo_select_font_face( cairo, frame->font_name, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL );
}

/* set x-label of a frame. */
void plotincFrameSetXLabel(plotincFrame *frame, const char *label)
{
  _plotincAxisSetLabel( &frame->xaxis, label );
}

/* set y-label of a frame. */
void plotincFrameSetYLabel(plotincFrame *frame, const char *label)
{
  _plotincAxisSetLabel( &frame->yaxis, label );
}

/* set y2-label of a frame. */
void plotincFrameSetY2Label(plotincFrame *frame, const char *label)
{
  _plotincAxisSetLabel( &frame->y2axis, label );
}

/* convert a double-precision value to x-component of coordinates. */
int plotincFrameXCoord(const plotincFrame *frame, double x)
{
  return frame->plot_ox + frame->plot_width * _plotincAxisValRatio( &frame->xaxis, x );
}

/* convert a double-precision value to y-component of coordinates. */
int plotincFrameYCoord(const plotincFrame *frame, double y)
{
  return frame->plot_oy + frame->plot_height * ( 1 - _plotincAxisValRatio( &frame->yaxis, y ) );
}

/* convert a double-precision value to y2-component of coordinates. */
int plotincFrameY2Coord(const plotincFrame *frame, double y)
{
  return frame->plot_oy + frame->plot_height * ( 1 - _plotincAxisValRatio( &frame->y2axis, y ) );
}

/* draw title of a frame. */
void plotincFrameDrawTitle(const plotincFrame *frame, cairo_t *cairo)
{
  cairo_text_extents_t te;

  if( frame->title[0] ){
    _plotincFrameRecallFont( frame, cairo );
    cairo_text_extents( cairo, frame->title, &te );
    cairo_move_to( cairo,
      frame->plot_ox + ( frame->plot_width - te.width ) / 2,
      frame->plot_oy - PLOTINC_BASELINE_MARGIN );
    cairo_show_text( cairo, frame->title );
  }
}

/* draw border lines of a frame. */
void plotincFrameDrawBorder(const plotincFrame *frame, cairo_t *cairo)
{
  int zero;

  cairo_set_source_rgb( cairo, 0, 0, 0 ); /* black */
  /* border */
  cairo_set_line_width( cairo, PLOTINC_BORDER_LINEWIDTH );
  cairo_rectangle( cairo, frame->plot_ox, frame->plot_oy, frame->plot_width, frame->plot_height );
  /* x-axis */
  cairo_set_line_width( cairo, PLOTINC_AXIS_LINEWIDTH );
  if( _plotincAxisZeroIsIncluded( &frame->yaxis ) ){
    zero = plotincFrameYCoord( frame, 0 );
    cairo_move_to( cairo, frame->plot_ox, zero );
    cairo_rel_line_to( cairo, frame->plot_width, 0 );
  }
  /* y-axis */
  if( _plotincAxisZeroIsIncluded( &frame->xaxis ) ){
    zero = plotincFrameXCoord( frame, 0 );
    cairo_move_to( cairo, zero, frame->plot_oy );
    cairo_rel_line_to( cairo, 0, frame->plot_height );
  }
  cairo_stroke( cairo );
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
static void _plotincFrameDrawXTics(const plotincFrame *frame, cairo_t *cairo, double val, int tics)
{
  _plotincFrameDrawTopXTicsOne( frame, cairo, tics );
  _plotincFrameDrawBottomXTicsOne( frame, cairo, tics );
  _plotincFrameDrawXTicsVal( frame, cairo, val, tics );
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
static void _plotincFrameDrawYTics(const plotincFrame *frame, cairo_t *cairo, double val, int tics)
{
  _plotincFrameDrawLeftYTicsOne( frame, cairo, tics );
  _plotincFrameDrawYTicsVal( frame, cairo, val, tics );
}

static void _plotincFrameDrawY2TicsVal(const plotincFrame *frame, cairo_t *cairo, double val, int ytics)
{
  cairo_text_extents_t te;
  char str[BUFSIZ];

  _plotincFrameRecallFont( frame, cairo );
  sprintf( str, "%g", val );
  cairo_text_extents( cairo, str, &te );
  cairo_move_to( cairo, frame->plot_ox +frame->plot_width + PLOTINC_BASELINE_MARGIN, ytics + te.height/2 );
  cairo_show_text( cairo, str );
}
static void _plotincFrameDrawY2Tics(const plotincFrame *frame, cairo_t *cairo, double val, int tics)
{
  _plotincFrameDrawRightYTicsOne( frame, cairo, tics );
  _plotincFrameDrawY2TicsVal( frame, cairo, val, tics );
}

/* draw x-tics of a frame. */
void plotincFrameDrawXTics(const plotincFrame *frame, cairo_t *cairo)
{
  double tics_width, val;
  int i, tics;

  cairo_set_source_rgb( cairo, 0, 0, 0 ); /* black */
  tics_width = _plotincAxisTicsWidth( &frame->xaxis );
  if( frame->xaxis.range_max > 0 && frame->xaxis.range_min < 0 ){
    for( i=0; ( tics = plotincFrameXCoord( frame, ( val =-i * tics_width ) ) ) >= frame->plot_ox; i++ )
      _plotincFrameDrawXTics( frame, cairo, val, tics );
    for( i=1; ( tics = plotincFrameXCoord( frame, ( val = i * tics_width ) ) ) <= frame->plot_ox + frame->plot_width; i++ )
      _plotincFrameDrawXTics( frame, cairo, val, tics );
  } else{
    for( i=0; ; i++ ){
      tics = plotincFrameXCoord( frame, ( val = _plotincAxisTicsVal( &frame->xaxis, tics_width, i ) ) );
      if( tics >= frame->plot_ox && tics <= frame->plot_ox + frame->plot_width )
        _plotincFrameDrawXTics( frame, cairo, val, tics );
      if( i > frame->xaxis.tics_num ) break;
    }
  }
}

/* draw y-tics of a frame. */
void plotincFrameDrawYTics(const plotincFrame *frame, cairo_t *cairo)
{
  double tics_width, val;
  int i, tics;

  cairo_set_source_rgb( cairo, 0, 0, 0 ); /* black */
  tics_width = _plotincAxisTicsWidth( &frame->yaxis );
  if( frame->yaxis.range_max > 0 && frame->yaxis.range_min < 0 ){
    for( i=0; ( tics = plotincFrameYCoord( frame, ( val =-i * tics_width ) ) ) <= frame->plot_oy + frame->plot_height; i++ )
      _plotincFrameDrawYTics( frame, cairo, val, tics );
    for( i=1; ( tics = plotincFrameYCoord( frame, ( val = i * tics_width ) ) ) >= frame->plot_oy; i++ )
      _plotincFrameDrawYTics( frame, cairo, val, tics );
  } else{
    for( i=0; ; i++ ){
      tics = plotincFrameYCoord( frame, ( val = _plotincAxisTicsVal( &frame->yaxis, tics_width, i ) ) );
      if( tics >= frame->plot_oy && tics <= frame->plot_oy + frame->plot_height )
        _plotincFrameDrawYTics( frame, cairo, val, tics );
      if( i > frame->yaxis.tics_num ) break;
    }
  }
}

/* draw y2-tics of a frame. */
void plotincFrameDrawY2Tics(const plotincFrame *frame, cairo_t *cairo)
{
  double tics_width, val;
  int i, tics;

  cairo_set_source_rgb( cairo, 0, 0, 0 ); /* black */
  tics_width = _plotincAxisTicsWidth( &frame->y2axis );
  if( frame->y2axis.range_max > 0 && frame->y2axis.range_min < 0 ){
    for( i=0; ( tics = plotincFrameY2Coord( frame, ( val =-i * tics_width ) ) ) <= frame->plot_oy + frame->plot_height; i++ )
      _plotincFrameDrawY2Tics( frame, cairo, val, tics );
    for( i=1; ( tics = plotincFrameY2Coord( frame, ( val = i * tics_width ) ) ) >= frame->plot_oy; i++ )
      _plotincFrameDrawY2Tics( frame, cairo, val, tics );
  } else{
    for( i=0; ; i++ ){
      tics = plotincFrameY2Coord( frame, ( val = _plotincAxisTicsVal( &frame->y2axis, tics_width, i ) ) );
      if( tics >= frame->plot_oy && tics <= frame->plot_oy + frame->plot_height )
        _plotincFrameDrawY2Tics( frame, cairo, val, tics );
      if( i > frame->y2axis.tics_num ) break;
    }
  }
}

/* set grid line property of a frame. */
static void _plotincFrameSetGridLine(cairo_t *cairo)
{
  double dash[] = { 6.0, 10.0 };
  cairo_set_source_rgb( cairo, 0, 0, 0 ); /* black */
  cairo_set_line_width( cairo, PLOTINC_GRID_LINEWIDTH );
  cairo_set_dash( cairo, dash, sizeof(dash)/sizeof(double), 0 );
}

/* draw x-grid of a frame. */
void plotincFrameDrawXGrid(const plotincFrame *frame, cairo_t *cairo)
{
  double tics_width;
  int i, tics;

  _plotincFrameSetGridLine( cairo );
  cairo_set_source_rgb( cairo, 0, 0, 0 ); /* black */
  tics_width = _plotincAxisTicsWidth( &frame->xaxis );
  if( frame->xaxis.range_max > 0 && frame->xaxis.range_min < 0 ){
    for( i=1; ( tics = plotincFrameXCoord( frame,-i * tics_width ) ) > frame->plot_ox; i++ )
      _plotincFrameDrawXTicsOne( frame, cairo, tics, 0, frame->plot_height );
    for( i=1; ( tics = plotincFrameXCoord( frame, i * tics_width ) ) < frame->plot_ox + frame->plot_width; i++ )
      _plotincFrameDrawXTicsOne( frame, cairo, tics, 0, frame->plot_height );
  } else{
    for( i=0; ; i++ ){
      tics = plotincFrameXCoord( frame,  _plotincAxisTicsVal( &frame->xaxis, tics_width, i ) );
      if( tics > frame->plot_ox && tics < frame->plot_ox + frame->plot_width )
        _plotincFrameDrawXTicsOne( frame, cairo, tics, 0, frame->plot_height );
      if( i > frame->xaxis.tics_num ) break;
    }
  }
  cairo_set_dash( cairo, NULL, 0, 0 );
}

/* draw y-grid of a frame. */
void plotincFrameDrawYGrid(const plotincFrame *frame, cairo_t *cairo)
{
  double tics_width;
  int i, tics;

  _plotincFrameSetGridLine( cairo );
  tics_width = _plotincAxisTicsWidth( &frame->yaxis );
  if( frame->yaxis.range_max > 0 && frame->yaxis.range_min < 0 ){
    for( i=1; ( tics = plotincFrameYCoord( frame,-i * tics_width ) ) < frame->plot_oy + frame->plot_height; i++ )
      _plotincFrameDrawYTicsOne( frame, cairo, tics, 0, frame->plot_width );
    for( i=1; ( tics = plotincFrameYCoord( frame, i * tics_width ) ) > frame->plot_oy; i++ )
      _plotincFrameDrawYTicsOne( frame, cairo, tics, 0, frame->plot_width );
  } else{
    for( i=0; ; i++ ){
      tics = plotincFrameYCoord( frame, _plotincAxisTicsVal( &frame->yaxis, tics_width, i ) );
      if( tics > frame->plot_oy && tics < frame->plot_oy + frame->plot_height )
        _plotincFrameDrawYTicsOne( frame, cairo, tics, 0, frame->plot_width );
      if( i > frame->yaxis.tics_num ) break;
    }
  }
  cairo_set_dash( cairo, NULL, 0, 0 );
}

/* set grid line property of a frame. */
static void _plotincFrameSetY2GridLine(cairo_t *cairo)
{
  double dash[] = { 3.0, 5.0 };
  cairo_set_source_rgb( cairo, 0, 0, 0 ); /* black */
  cairo_set_line_width( cairo, PLOTINC_GRID_LINEWIDTH );
  cairo_set_dash( cairo, dash, sizeof(dash)/sizeof(double), 0 );
}

/* draw y2-grid of a frame. */
void plotincFrameDrawY2Grid(const plotincFrame *frame, cairo_t *cairo)
{
  double tics_width;
  int i, tics;

  _plotincFrameSetY2GridLine( cairo );
  tics_width = _plotincAxisTicsWidth( &frame->y2axis );
  if( frame->y2axis.range_max > 0 && frame->y2axis.range_min < 0 ){
    for( i=1; ( tics = plotincFrameY2Coord( frame,-i * tics_width ) ) < frame->plot_oy + frame->plot_height; i++ )
      _plotincFrameDrawYTicsOne( frame, cairo, tics, 0, frame->plot_width );
    for( i=1; ( tics = plotincFrameY2Coord( frame, i * tics_width ) ) > frame->plot_oy; i++ )
      _plotincFrameDrawYTicsOne( frame, cairo, tics, 0, frame->plot_width );
  } else{
    for( i=0; ; i++ ){
      tics = plotincFrameY2Coord( frame, _plotincAxisTicsVal( &frame->y2axis, tics_width, i ) );
      if( tics > frame->plot_oy && tics < frame->plot_oy + frame->plot_height )
        _plotincFrameDrawYTicsOne( frame, cairo, tics, 0, frame->plot_width );
      if( i > frame->y2axis.tics_num ) break;
    }
  }
  cairo_set_dash( cairo, NULL, 0, 0 );
}

/* draw label of a frame. */
static void _plotincFrameDrawLabel(const plotincFrame *frame, cairo_t *cairo, const char *label, int x, int y, double angle)
{
  cairo_text_extents_t te;

  if( !label[0] ) return;
  _plotincFrameRecallFont( frame, cairo );
  cairo_text_extents( cairo, label, &te );
  cairo_save( cairo );
  cairo_translate( cairo, x, y );
  cairo_rotate( cairo, angle );
  cairo_translate( cairo, -te.width/2, 0 );
  cairo_move_to( cairo, 0, 0 );
  cairo_show_text( cairo, label );
  cairo_restore( cairo );
}

/* draw label in TeX format of a file. */
#define PATHSIZ 512
static void _plotincFrameDrawTexLabel(const plotincFrame *frame, cairo_t *cairo, const char *label, int x, int y, double angle)
{
  FILE *fp;
  char tmpfile[PATHSIZ], outfile[PATHSIZ];
  char cmd[BUFSIZ];
  cairo_surface_t *label_image;
  int label_width, label_height;
  double scale;

  if( !label[0] ) return;
  strcpy( tmpfile, "_plotinc.XXXXXX" );
  if( mkstemp( tmpfile ) < 0 ){
    fprintf( stderr, "cannot create temprary file name." );
    return;
  }
  sprintf( outfile, "%s.tex", tmpfile );
  if( !( fp = fopen( outfile, "w" ) ) ){
    fprintf( stderr, "cannot open a temprary file." );
    return;
  }
  fprintf( fp, "\\documentclass{jarticle}\n" );
  fprintf( fp, "\\usepackage{amsmath,amssymb,bm}\n" );
  fprintf( fp, "\\begin{document}\n" );
  fprintf( fp, "%s\n", label );
  fprintf( fp, "\\thispagestyle{empty}\n" );
  fprintf( fp, "\\end{document}\n" );
  fclose( fp ); /* temprary TeX file */

  sprintf( cmd, "platex %s > /dev/null", outfile );
  if( system( cmd ) == 0x7f ){
    fprintf( stderr, "failed to make a DVI file." );
    return;
  }
  sprintf( cmd, "dvips -E %s.dvi > /dev/null", tmpfile );
  if( system( cmd ) == 0x7f ){
    fprintf( stderr, "failed to make an EPS file." );
    return;
  }
  sprintf( cmd, "pstopnm -portrait -pgm %s.ps > /dev/null", tmpfile );
  if( system( cmd ) == 0x7f ){
    fprintf( stderr, "failed to make a PGM file." );
    return;
  }
  sprintf( cmd, "convert %s001.pgm %s.png > /dev/null", tmpfile, tmpfile );
  if( system( cmd ) == 0x7f ){
    fprintf( stderr, "failed to make a PNG file." );
    return;
  }
  sprintf( outfile, "%s.png", tmpfile );
  label_image = cairo_image_surface_create_from_png( outfile );
  label_width = cairo_image_surface_get_width( label_image );
  label_height = cairo_image_surface_get_height( label_image );
  cairo_save( cairo );
  scale = (double)frame->baseline_skip / label_height;
  cairo_translate( cairo, x, y );
  cairo_rotate( cairo, angle );
  cairo_scale( cairo, scale, scale );
  cairo_translate( cairo,-label_width/2, 0 );
  cairo_set_source_surface( cairo, label_image, 0, 0 );
  cairo_paint( cairo );
  cairo_restore( cairo );
  cairo_surface_destroy( label_image );
  sprintf( cmd, "rm %s* > /dev/null", tmpfile );
  if( system( cmd ) < 0 ){
    fprintf( stderr, "cannot remove temporary files." );
  }
}

/* draw x-label of a frame. */
void plotincFrameDrawXLabel(const plotincFrame *frame, cairo_t *cairo)
{
  if( strchr( frame->xaxis.label, '$' ) ){
    _plotincFrameDrawTexLabel( frame, cairo, frame->xaxis.label,
      frame->plot_ox + frame->plot_width/2,
      frame->plot_oy + frame->plot_height + frame->baseline_skip + PLOTINC_BASELINE_MARGIN*2,
      0 );
  } else{
    _plotincFrameDrawLabel( frame, cairo, frame->xaxis.label,
      frame->plot_ox + frame->plot_width/2,
      frame->plot_oy + frame->plot_height + frame->baseline_skip*2,
      0 );
  }
}

/* draw y-label of a frame. */
void plotincFrameDrawYLabel(const plotincFrame *frame, cairo_t *cairo)
{
  if( strchr( frame->yaxis.label, '$' ) ){
    _plotincFrameDrawTexLabel( frame, cairo, frame->yaxis.label,
      frame->plot_ox - frame->baseline_skip*2 - PLOTINC_BASELINE_MARGIN,
      frame->plot_oy + frame->plot_height/2,
      -M_PI/2 );
  } else{
    _plotincFrameDrawLabel( frame, cairo, frame->yaxis.label,
      frame->plot_ox - frame->baseline_skip - PLOTINC_BASELINE_MARGIN,
      frame->plot_oy + frame->plot_height/2,
     -M_PI/2 );
  }
}

/* draw y2-label of a frame. */
void plotincFrameDrawY2Label(const plotincFrame *frame, cairo_t *cairo)
{
  if( strchr( frame->y2axis.label, '$' ) ){
    _plotincFrameDrawTexLabel( frame, cairo, frame->y2axis.label,
      frame->plot_ox + frame->plot_width + frame->baseline_skip + PLOTINC_BASELINE_MARGIN,
      frame->plot_oy + frame->plot_height/2,
      -M_PI/2 );
  } else{
    _plotincFrameDrawLabel( frame, cairo, frame->y2axis.label,
      frame->plot_ox + frame->plot_width + frame->baseline_skip*2 + PLOTINC_BASELINE_MARGIN,
      frame->plot_oy + frame->plot_height/2,
     -M_PI/2 );
  }
}

/* draw a frame. */
void plotincFrameDraw(plotincFrame *frame, cairo_t *cairo)
{
  if( frame->xaxis.flag_grid  )  plotincFrameDrawXGrid(   frame, cairo );
  if( frame->xaxis.flag_tics  )  plotincFrameDrawXTics(   frame, cairo );
  if( frame->xaxis.flag_label )  plotincFrameDrawXLabel(  frame, cairo );
  if( frame->yaxis.flag_grid  )  plotincFrameDrawYGrid(   frame, cairo );
  if( frame->yaxis.flag_tics  )  plotincFrameDrawYTics(   frame, cairo );
  if( frame->yaxis.flag_label )  plotincFrameDrawYLabel(  frame, cairo );
  if( frame->y2axis.flag_grid  ) plotincFrameDrawY2Grid(  frame, cairo );
  if( frame->y2axis.flag_tics  ) plotincFrameDrawY2Tics(  frame, cairo );
  if( frame->y2axis.flag_label ) plotincFrameDrawY2Label( frame, cairo );
  if( frame->flag_title )        plotincFrameDrawTitle(   frame, cairo );
  plotincFrameDrawBorder( frame, cairo );
  if( frame->draw ){
    cairo_rectangle( cairo, frame->plot_ox, frame->plot_oy, frame->plot_width, frame->plot_height );
    cairo_clip( cairo );
    frame->draw( frame, cairo );
    cairo_reset_clip( cairo );
  }
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

/* plot a parametric function on a frame. */
void plotincFramePlotParametricFunction(const plotincFrame *frame, cairo_t *cairo, double (* xfunction)(double), double (* yfunction)(double), double param_min, double param_max, int sample_num)
{
  double *xdata, *ydata, param;
  int i;

  xdata = malloc( sizeof(double)*sample_num );
  ydata = malloc( sizeof(double)*sample_num );
  if( !xdata || !ydata ){
    fprintf( stderr, "cannot allocate buffer for sampling." );
    goto TERMINATE;
  }
  for( i=0; i<sample_num; i++ ){
    param = ( param_max - param_min ) * (double)i / ( sample_num - 1 ) + param_min;
    xdata[i] = xfunction( param );
    ydata[i] = yfunction( param );
  }
  plotincFramePlotData2D( frame, cairo, xdata, ydata, sample_num );
 TERMINATE:
  free( xdata );
  free( ydata );
}

/* plot a function on a frame. */
void plotincFramePlotFunction(const plotincFrame *frame, cairo_t *cairo, double (* function)(double), int sample_num)
{
  double xfunction(double param){ return _plotincAxisVal( &frame->xaxis, param ); }
  double yfunction(double param){ return function( xfunction( param ) ); }
  plotincFramePlotParametricFunction( frame, cairo, xfunction, yfunction, 0, 1, sample_num );
}

/* canvas */

static void _plotincCanvasSetSize(plotincCanvas *canvas, int width, int height)
{
  canvas->width  = width;
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
  double w, h;
  plotincFrame *frame_ptr;

  frame_width  = ( w = canvas->width  - PLOTINC_CANVAS_DEFAULT_PADDING * 2 ) / canvas->col_size;
  frame_height = ( h = canvas->height - PLOTINC_CANVAS_DEFAULT_PADDING * 2 ) / canvas->row_size;
  frame_ptr = canvas->frame_list;
  for( i=0; i<canvas->row_size; i++ )
    for( j=0; j<canvas->col_size; j++ ){
      if( !frame_ptr ) return;
      plotincFrameResize( frame_ptr,
        PLOTINC_CANVAS_DEFAULT_PADDING + w * (double)j / canvas->col_size,
        PLOTINC_CANVAS_DEFAULT_PADDING + h * (double)i / canvas->row_size,
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
