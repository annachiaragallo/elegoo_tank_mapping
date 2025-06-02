#include "draw.h"

int create_svg(){
    int xCurrent, yCurrent, angleCurrent, xNext, yNext;
    FILE *svg_file = fopen(ROUTE_PATH, "w");
    if(!svg_file)
    {
        printf("Could not open file: %s\n", ROUTE_PATH);
        return(1);
    }

    int time_value, dir_value; /*; ang_value;*/
    char line[1024];
    FILE *csv_file = fopen(DIRECTIONS_PATH, "r");

    
    if(!csv_file)
    {
        printf("Could not open file: %s\n", DIRECTIONS_PATH);
        return(1);
    }

    svgHeader(svg_file, "Line Test", WIDTH, HEIGHT);

    svgLine_border(svg_file, 0, 0, 0, HEIGHT);
    svgLine_border(svg_file, 0, HEIGHT, WIDTH, HEIGHT);
    svgLine_border(svg_file, WIDTH, HEIGHT, WIDTH, 0);
    svgLine_border(svg_file, WIDTH, 0, 0, 0);

    /*Starting point */
    xCurrent = WIDTH  / 4;
    yCurrent = HEIGHT / 4;

    /*Angle to walk at*/
    angleCurrent = 0;

    /*Go straight*/
    xNext = xGetNext(xCurrent, 0, angleCurrent);
    yNext = yGetNext(yCurrent, 0, angleCurrent);

    /*svgLine(svg_file, xCurrent, yCurrent, xNext, yNext);*/


    while (fgets(line, 1024, csv_file))
    {
         char* tmp = strdup(line);
         if(strcmp(getfield(tmp, 1), "T") == 0){
            char * time_tmp = strdup(line);
            time_value = atoi(getfield(time_tmp, 2));
            /*multiplt for speed (0,12)*/
            //printf("Field T would be %i\n", time_value);
            free(time_tmp);

            xCurrent = xNext;
            yCurrent = yNext;

            //printf("Walk %i meters with angle %i\n", time_value, angleCurrent);
            xNext = xGetNext(xCurrent, (time_value * SPEED), angleCurrent);
            yNext = yGetNext(yCurrent, (time_value * SPEED), angleCurrent);
        
            svgLine(svg_file, xCurrent, yCurrent, xNext, yNext);

          /* if(dir_value == 0 || dir_value == 2){
                printf("obstacle...\n");
                svgObstacleDx(svg_file, xCurrent, yCurrent, xNext, yNext);
            }

            else if(dir_value == 1){
                printf("obstacle...\n");
                svgObstacleSx(svg_file, xCurrent, yCurrent, xNext, yNext);
            }*/
            //angleCurrent = 0;

            
         } else { //field = "D"
            char * dir_tmp = strdup(line);
            dir_value = atoi(getfield(dir_tmp, 2));
            //printf("Field D would be %i\n", dir_value);
            free(dir_tmp);
            if(dir_value == 0 || dir_value == 2){ 
                angleCurrent += 30;
                printf("giro a sx, angolo: %i\n", angleCurrent);
            } else if (dir_value == 1){
                angleCurrent -= 30;
            }
        }
         free(tmp);
    }

    fclose(csv_file);
    svgFooter(svg_file);
    fclose(svg_file);

}


void svgHeader(FILE *f, const char *title, int width, int height){
    fprintf(
        f,
        "<!doctype html>\n"
        "<html>\n"
        " <head><title>%s</title></head>\n"
        " <body>\n"
        "  <svg width='%d' height='%d'>\n",
        title, width, height
    );
}

void svgFooter(FILE *f){
    fprintf(
        f,
        "  </svg>\n"
        " </body>\n"
        "</html>\n"
    );
}

void svgLine_border(FILE *f, int x0, int y0, int x1, int y1){
    fprintf(
        f,
        "<line x1='%d' y1='%d' x2='%d' y2='%d' stroke='black'/>\n",
        x0, y0, x1, y1
    );
}


void svgLine(FILE *f, int x0, int y0, int x1, int y1){
    fprintf(
        f,
        "<line x1='%d' y1='%d' x2='%d' y2='%d' stroke='brown' stroke-width='%d' stroke-linecap='round'/>\n",
        x0, y0, x1, y1, 12
    );
}

void svgObstacleDx(FILE *f, int x0, int y0, int x1, int y1){
    fprintf(
        f,
    "<polygon points='%d, %d, %d, %d' style='fill:lime;stroke:purple;stroke-width:%i'/>",
    x0 - 15, y0 + 15, x1 - 15, y1 + 15, 12);
}

void svgObstacleSx(FILE *f, int x0, int y0, int x1, int y1){
    fprintf(
        f,
    "<polygon points='%d, %d, %d, %d' style='fill:lime;stroke:purple;stroke-width:%i'/>",
    x0 + 15, y0 - 15, x1 + 15, y1 - 15, 12);
}

double d2r(double degrees){
    return(degrees * M_PI / 180.0);
}

int rnd(double value){
    return(
        (int)(
            value + (value >= 0 ? 0.5 : -0.5)
        )
    );
}

int xGetNext(int xCurrent, double nMeters, int angleCurrent){
    return(
        rnd(xCurrent + nMeters * PIX_PER_METER * cos(d2r(angleCurrent)))
    );
}

int yGetNext(int xCurrent, double nMeters, int angleCurrent){
    return(
        rnd(xCurrent + nMeters * PIX_PER_METER * sin(d2r(angleCurrent)))
    );
}

const char* getfield(char* line, int num){
     const char* tok;
     for (tok = strtok(line, ";");
                tok && *tok;
                tok = strtok(NULL, ";\n"))
     {
          if (!--num)
                return tok;
     }
     return NULL;
}