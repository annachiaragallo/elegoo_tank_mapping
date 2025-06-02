#include <stdio.h>
#include <math.h>
 #include <assert.h>
 #include <ctype.h>
 #include <glib.h>
 #include <stdlib.h>
 #include <string.h>
 
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define ROUTE_PATH      "route.html"
#define DIRECTIONS_PATH "directions.csv"
#define WIDTH         1280
#define HEIGHT        960
#define PIX_PER_METER 30
#define SPEED 0.12

void svgHeader(FILE *f, const char *title, int width, int height);
void svgFooter(FILE *f);
void svgLine_border(FILE *f, int x0, int y0, int x1, int y1);
void svgLine(FILE *f, int x0, int y0, int x1, int y1);
double d2r(double degrees);
int xGetNext(int xCurrent, double nMeters, int angleCurrent);
int yGetNext(int xCurrent, double nMeters, int angleCurrent);
const char* getfield(char* line, int num);
void svgObstacleDx(FILE *f, int x0, int y0, int x1, int y1);
void svgObstacleSx(FILE *f, int x0, int y0, int x1, int y1);
int create_svg();