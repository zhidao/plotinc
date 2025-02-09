#include <plotinc/plotinc.h>

void draw1(plotincFrame *frame, cairo_t *cairo)
{
  cairo_set_source_rgb( cairo, 0, 0, 1 );
  plotincFrameDrawPoint( frame, cairo, -5, -2, 5 );
  cairo_set_source_rgb( cairo, 1, 0, 0 );
  plotincFrameDrawPoint( frame, cairo,  0,  4, 8 );
  cairo_set_source_rgb( cairo, 0, 0.6, 0 );
  plotincFrameDrawPoint( frame, cairo,  3,  1, 5 );
  cairo_set_source_rgb( cairo, 0, 1, 0 );
  plotincFrameDrawLine( frame, cairo, -8, -2, 3, 3 );
  plotincFrameDrawLine( frame, cairo,  3,  3, 6,-1 );
}

double sample_data[] = { -5, -3, -1, 2, 0, -4 };

void draw2(plotincFrame *frame, cairo_t *cairo)
{
  cairo_set_source_rgb( cairo, 0.5, 0, 0.8 );
  plotincFramePlotData1D( frame, cairo, sample_data, sizeof(sample_data)/sizeof(double) );
}

void draw4(plotincFrame *frame, cairo_t *cairo)
{
  cairo_set_source_rgb( cairo, 0.8, 0.5, 0.0 );
  plotincFramePlotFunction( frame, cairo, sin, 1000 );
  cairo_set_source_rgb( cairo, 0.0, 0.5, 0.8 );
  plotincFramePlotFunction( frame, cairo, cos, 1000 );
}

int main(int argc, char** argv)
{
  plotincCanvas canvas;

  plotincCanvasOpenX11( &canvas, PLOTINC_CANVAS_DEFAULT_WIDTH, PLOTINC_CANVAS_DEFAULT_HEIGHT );
  canvas.frame_last->draw = draw1;
  plotincFrameSetTitle( canvas.frame_last, "Points & Segments" );
  plotincFrameSetXLabel( canvas.frame_last, "Label x" );
  plotincFrameSetYLabel( canvas.frame_last, "Label y" );

  plotincCanvasAddRowFrame( &canvas );
  canvas.frame_last->draw = draw2;
  plotincFrameSetTitle( canvas.frame_last, "Data" );
  plotincFrameSetRangeByData1D( canvas.frame_last, sample_data, sizeof(sample_data)/sizeof(double) );
  plotincFrameSetXLabel( canvas.frame_last, "Label x2" );
  plotincFrameSetYLabel( canvas.frame_last, "Label y2" );
  plotincFrameEnableXGrid( canvas.frame_last );
  plotincFrameEnableYGrid( canvas.frame_last );

  plotincCanvasAddColFrame( &canvas );
  plotincFrameSetTitle( canvas.frame_last, "Fine tics" );
  plotincFrameSetXLabel( canvas.frame_last, "Label x3" );
  plotincFrameSetYLabel( canvas.frame_last, "Label y3" );
  plotincFrameSetY2Label( canvas.frame_last, "Label y3'" );
  plotincFrameSetXRange( canvas.frame_last, 0.4667, 0.4682 );
  plotincFrameSetY2Range( canvas.frame_last, -30, 30 );
  plotincFrameEnableY2Tics( canvas.frame_last );
  plotincFrameEnableXGrid( canvas.frame_last );
  plotincFrameEnableYGrid( canvas.frame_last );
  plotincFrameEnableY2Grid( canvas.frame_last );

  plotincCanvasAddRowFrame( &canvas );
  canvas.frame_last->draw = draw4;
  plotincFrameSetTitle( canvas.frame_last, "Functions (trigonometrics)" );
  plotincFrameSetXRange( canvas.frame_last, -2*M_PI, 2*M_PI );
  plotincFrameSetYRange( canvas.frame_last, -2, 2 );
  plotincFrameEnableYGrid( canvas.frame_last );

  plotincCanvasDraw( &canvas );
  getchar();
  plotincCanvasCloseX11( &canvas );
  return 0;
}
