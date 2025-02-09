#include <plotinc/plotinc.h>

void draw(plotincFrame *frame, cairo_t *cairo)
{
  cairo_set_source_rgb( cairo, 0.8, 0.5, 0.0 );
  plotincFramePlotFunction( frame, cairo, sin, 1000 );
  cairo_set_source_rgb( cairo, 0.0, 0.5, 0.8 );
  plotincFramePlotFunction( frame, cairo, cos, 1000 );
}

int main(int argc, char** argv)
{
  plotincCanvas canvas;

  plotincCanvasOpenSVG( &canvas, PLOTINC_CANVAS_DEFAULT_WIDTH, PLOTINC_CANVAS_DEFAULT_HEIGHT, "test.svg" );
  canvas.frame_last->draw = draw;
  plotincFrameSetXLabel( canvas.frame_last, "Label x" );
  plotincFrameSetYLabel( canvas.frame_last, "Label y" );
  plotincFrameSetXRange( canvas.frame_last, -2*M_PI, 2*M_PI );
  plotincFrameSetYRange( canvas.frame_last, -2, 2 );
  plotincFrameEnableXGrid( canvas.frame_last );
  plotincFrameEnableYGrid( canvas.frame_last );
  plotincCanvasDraw( &canvas );
  plotincCanvasCloseSVG( &canvas );
  return 0;
}
