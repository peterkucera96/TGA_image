#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define PI 3.14
#define MAX_ARGUMENT 9
#ifndef DRAWING


typedef unsigned char byte;


// Struktura popisujuca hlavicku suboru TGA
typedef struct
{
    byte id_length;
    byte color_map_type;
    byte image_type;
    byte color_map[5];
    byte x_origin[2];
    byte y_origin[2];
    byte width[2];
    byte height[2];
    byte depth;
    byte descriptor;
} TGAHeader;


// Struktura popisujuca pixel
typedef struct
{
    byte red;
    byte green;
    byte blue;
    byte alpha;
} Pixel;

// Deklaracia funkcii pre kreslenie a pracu so suborom TGA
void vykreslenie(Pixel* pixels, int width, int height, char *command);
void swapInt(int *a, int *b);
void saveTGA(const char *filename, int width, int height, Pixel *pixels);
void clear(int width, int height, Pixel *pixels);
void drawLine(int x1, int y1, int x2, int y2, int width, int height, Pixel *pixels, byte r, byte g, byte b, byte a);
void drawRectangle(int x, int y, int width, int height, int imgWidth, int imgHeight, Pixel *pixels, byte r, byte g, byte b, byte a);
void drawTriangle(int centerX, int centerY, int sideLength, int width, int height, Pixel *pixels, byte r, byte g, byte b, byte a);
void drawCircle(int centerX, int centerY, int radius, int width, int height, Pixel *pixels, byte r, byte g, byte b, byte a);
void drawRotatedRectangle(float angle, int x, int y, int width, int height, int imgWidth, int imgHeight, Pixel *pixels, byte r, byte g, byte b, byte a);
#endif


// Hlavna funkcia main
int main(int argc, char *argv[])
{
    const char *outputFilename = NULL;
    int width = -1;
    int height = -1;


    // Zpracovanie prikazov
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--output") == 0 && i + 1 < argc)
        {
            outputFilename = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "--width") == 0 && i + 1 < argc)
        {
            width = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc)
        {
            height = atoi(argv[i + 1]);
            i++;
        }
        else
        {   
            // Chybne alebo nadbytocne parametre prikazoveho riadku
            printf("Chybné zadané parametre.\n");
            printf("Spravne zadanie: %s --output <cesta k suboru> --width <cislo> --height <cislo>\n", argv[0]);
            return 1;
        }
    }

    if (outputFilename == NULL || width == -1 || height == -1)
    {
        // Chybajuce alebo chybne zadanie parametre
        printf("Chybné zadané parametre.\n");
        printf("Spravne zadanie: %s --output <cesta k suboru> --width <cislo> --height <cislo>\n", argv[0]);
        return 1;
    }

    if (width <= 0 || height <= 0)
    {   
        // Neplatne rozmery
        printf("Chybné údaje. Šírka a výška musia být kladná celá čísla.\n");
        return 1;
    }


    // Alokacia pixelov
    Pixel *pixels = (Pixel *)malloc(width * height * sizeof(Pixel));
    //  Nastavenie vychodzej farby na bielu
    clear(width,height,pixels);


    //  Zpracovanie uzivatelskych prikazov
    char command[101] = {};
    char *token;
    while (1)
    {
        fgets(command, 101, stdin);

        //  Prikazy clear,save a exit
        if (strcmp(command, "exit\n") == 0)
        {
            saveTGA(outputFilename, width, height, pixels);
            free(pixels);     
            return 0;
        }
        else if (strcmp(command, "save\n") == 0)    
        {
            saveTGA(outputFilename, width, height, pixels);
        }else if (strcmp(command, "clear\n") == 0)
        {
            clear(width,height,pixels);
        }else
        {
            // Vykreslovanie tvarov
            vykreslenie(pixels,width,height,command);
        }
    }
}

// Funkcie pre vymenu dvoch celych cisiel 
void swapInt(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Funkcia pre ulozenie dat do suboru TGA
void saveTGA(const char *filename, int width, int height, Pixel *pixels)
{

    // Otvorenie suboru pre zapis v binarnom mode
    FILE *tgaFile = fopen(filename, "wb");
    if (tgaFile == NULL) {
        printf("Chyba pri otvoreni.\n");
        exit(1);
    }

    // Hlavicka TGA suboru 
    TGAHeader header = {
        .id_length = 0,
        .color_map_type = 0,
        .image_type = 2,
        .color_map = {0, 0, 0, 0, 0},
        .x_origin = {0, 0},
        .y_origin = {0, 0},
        .width = {width & 0xFF, (width >> 8) & 0xFF},
        .height = {height & 0xFF, (height >> 8) & 0xFF},
        .depth = 32,
        .descriptor = 8};


    // Zapis hlavicky do suboru
    fwrite(&header, sizeof(TGAHeader), 1, tgaFile);


    // Zapis pixelov do suboru
    for (int i = 0; i < width * height; ++i)
    {
        fwrite(&pixels[i].blue, sizeof(byte), 1, tgaFile);
        fwrite(&pixels[i].green, sizeof(byte), 1, tgaFile);
        fwrite(&pixels[i].red, sizeof(byte), 1, tgaFile);
        fwrite(&pixels[i].alpha, sizeof(byte), 1, tgaFile);
    }

    // Uzavrenie suboru
    fclose(tgaFile);
}

// Funkcia pre vymazanie platna nastavenim pixelov na farbu bielu
void clear(int width, int height, Pixel *pixels){
     for (int i = 0; i < width * height; ++i)
    {
        pixels[i].red = 255;
        pixels[i].green = 255;
        pixels[i].blue = 255;
        pixels[i].alpha = 255;
    }
}

// Funkcia pre spracovanie uzivatelskych prikazov pre kreslenie 
void vykreslenie(Pixel* pixels, int width, int height, char *command)
{

        // Nazov tvaru
        char* tvar;
        tvar = strtok(command, " ");

        // Pole pre uchovanie parametrov tvaru
        int par[MAX_ARGUMENT];
        for(int i=0;i<MAX_ARGUMENT;i++){
            par[i]=-1;
        }

        // Nacitanie argumnetov tvarov
        char *token; 
        for(int i=0;i<MAX_ARGUMENT;i++){
            token=strtok(NULL, ",");
            if(token!=NULL){
                par[i]=atoi(token);
            }
        }

        // Kontrola parametrov a prevedenie prislusnej funkcie
        if(strcmp(tvar, "line")==0)
        {
             if(par[0]<0|| par[1]<0|| par[2]<0|| par[3]<0|| par[4]<0|| par[5]<0|| par[6]<0||par[7]<0||par[4]>255||par[5]>255||par[6]>255||par[7]>255)
             {
                
                // Neplatne argumenty pre gunkciu line 
                printf("Neplatné argumenty funkcie line!\n");
                printf("Správne zadanie: x1,y1,x2,y2,r,g,b,a\n");  

            }else
            {
                drawLine(par[0], par[1], par[2], par[3], width, height, pixels, par[4], par[5], par[6], par[7]);
            }
        }else if(strcmp(tvar, "rectangle") == 0)
        {
             if(par[0]<0|| par[1]<0|| par[2]<0|| par[3]<0|| par[4]<0|| par[5]<0|| par[6]<0||par[7]<0||par[4]>255||par[5]>255||par[6]>255||par[7]>255)
             {
                
                // Neplatne argumenty pre funkciu rectangle
                printf("Neplatné argumenty funkcie rectangle!\n");
                printf("Správne zadanie: x,y,width,height,r,g,b,a\n");
            }else
            {
                drawRectangle(par[0], par[1], par[2], par[3], width, height, pixels, par[4], par[5], par[6], par[7]);
            }
        }else if(strcmp(tvar, "circle")==0)
        {
            if(par[0]<0|| par[1]<0|| par[2]<0|| par[3]<0|| par[4]<0|| par[5]<0|| par[6]<0||par[3]>255||par[4]>255||par[5]>255||par[6]>255)
            {
                
                // Neplatne argumety pre funkciu circle
                printf("Neplatné argumenty funkcie circle!\n");
                printf("Správne zadanie: x,y,radius,r,g,b,a\n");  
            }else
            {
                drawCircle(par[0], par[1], par[2], width, height, pixels, par[3], par[4], par[5], par[6]);
            }
        }else if(strcmp(tvar, "triangle")==0)
        {
            if(par[0]<0|| par[1]<0|| par[2]<0|| par[3]<0|| par[4]<0|| par[5]<0|| par[6]<0||par[3]>255||par[4]>255||par[5]>255||par[6]>255)
            {
                
                // Neplatne argumenty pre funkciu triangle
                printf("Neplatné argumenty funkcie triangle!\n");
                printf("Správne zadanie: x,y,width,r,g,b,a"); 
            }else
            {
                drawTriangle(par[0], par[1], par[2], width, height, pixels, par[3], par[4], par[5], par[6]);
            }
        }else if(strcmp(tvar, "rotated-rectangle") == 0)
        {
            if(par[0]<0|| par[1]<0|| par[2]<0|| par[3]<0|| par[4]<0|| par[5]<0|| par[6]<0||par[7]<0||par[8]<0||par[5]>255||par[6]>255||par[7]>255||par[8]>255)
            {
                
                // Neplatne argumenty pre funkciu rotated-rectangle
                printf("Neplatné argumenty funkcie rotated-rectangle!\n");
                printf("Správne zadanie: rectangle angle,x,y,width,height,r,g,b,a\n");
            }else
            {
                drawRotatedRectangle(par[0], par[1], par[2], par[3], par[4], width, height, pixels, par[5], par[6], par[7], par[8]);
            }
        }else
        {

            // Nespravny prikaz
            printf("Nesprávny príkaz!\n");
        }
}

// Funkcia pre kreslenie ciary pomocou Bresenhamova algoritmu
void drawLine(int x1, int y1, int x2, int y2, int width, int height, Pixel *pixels, byte r, byte g, byte b, byte a)
{   

    float alpha = a / 255.0;
    // Bresenhamov algoritmus
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1)
    {
        // Osetrenie kreslenia mimo platno
        if (x1 >= 0 && x1 < width && height - 1 - y1 >= 0 && height - 1 - y1 < height)
        {
            int index = (height - 1 - y1) * width + x1;
            pixels[index].red = alpha * r + (1.0 - alpha) * pixels[index].red;
            pixels[index].green = alpha * g + (1.0 - alpha) * pixels[index].green;
            pixels[index].blue = alpha * b + (1.0 - alpha) * pixels[index].blue;
            pixels[index].alpha = 255;
        }

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
  
}

// Funkcia pre kreslenie obdlznika
void drawRectangle(int x, int y, int width, int height, int imgWidth, int imgHeight, Pixel *pixels, byte r, byte g, byte b, byte a)
{
    for (int i = 0; i < height; i++)
    {
        drawLine(x, y + i, x + width - 1, y + i, imgWidth, imgHeight, pixels, r, g, b, a);
    }   
}

// Funkcia pre kreslenie kruhu
void drawCircle(int centerX, int centerY, int radius, int width, int height, Pixel *pixels, byte r, byte g, byte b, byte a)
{   
    float alpha = a / 255.0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int dx = x - centerX;
            int dy = centerY - y;

            if (dx * dx + dy * dy <= radius * radius)
            {
                int index = (height - 1 - y) * width + x;
                pixels[index].red = alpha * r + (1.0 - alpha) * pixels[index].red;
                pixels[index].green = alpha * g + (1.0 - alpha) * pixels[index].green;
                pixels[index].blue = alpha * b + (1.0 - alpha) * pixels[index].blue;
                pixels[index].alpha = 255;
            }
        }
    }
}

// Funkcia pre kreslenie trojuhlnika
void drawTriangle(int centerX, int centerY, int sideLength, int width, int height, Pixel *pixels, byte r, byte g, byte b, byte a)
{
    float alpha = a / 255.0;
    float halfHeight = sideLength * sqrt(3) / 2.0;
    float halfSide = sideLength / 2.0;
    
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            
            float tx = x - centerX;
            float ty = centerY - y +(halfHeight/2.5);

            
            if (tx >= -halfSide && tx <= halfSide &&
                ty >= 0 && ty <= halfHeight &&
                ty <= -sqrt(3) * tx + sideLength * sqrt(3) / 2.0 &&
                ty <= sqrt(3) * tx + sideLength * sqrt(3) / 2.0)
            {
                int index = (height - 1 - y) * width + x;
                pixels[index].red = alpha * r + (1.0 - alpha) * pixels[index].red;
                pixels[index].green = alpha * g + (1.0 - alpha) * pixels[index].green;
                pixels[index].blue = alpha * b + (1.0 - alpha) * pixels[index].blue;
                pixels[index].alpha = 255;
            }
        }
    }
}

// Funkcia pre kreslenie rotacneho trojuholnika
void drawRotatedRectangle(float angle, int x, int y, int width, int height, int imgWidth, int imgHeight, Pixel *pixels, byte r, byte g, byte b, byte a)
{   

    float alpha = a / 255.0;

    
    angle=(int)angle%360;
    angle=360-angle;
    float radians = angle * PI / 180.0;

    float centerX = x + (width / 2.0);
    float centerY = y + (height / 2.0);

    
    for (int i = x; i < x + width; ++i)
    {
        for (int j = y; j < y + height; ++j)
        {
            // Rotacia
            float translatedX = i - centerX;
            float translatedY = centerY - j;

            
            float rotatedX = translatedX * cos(radians) - translatedY * sin(radians);
            float rotatedY = translatedX * sin(radians) + translatedY * cos(radians);

            
            int rotatedPixelX = rotatedX + centerX - (width / 2.0);
            int rotatedPixelY = rotatedY + centerY - (height / 2.0);

            // Kontrola zapisu mimo obrazok
            if (rotatedPixelX >= 0 && rotatedPixelX < imgWidth && rotatedPixelY >= 0 && rotatedPixelY < imgHeight)
            {
                int index = (imgHeight - 1 - rotatedPixelY) * imgWidth + rotatedPixelX;
                pixels[index].red = alpha * r + (1.0 - alpha) * pixels[index].red;
                pixels[index].green = alpha * g + (1.0 - alpha) * pixels[index].green;
                pixels[index].blue = alpha * b + (1.0 - alpha) * pixels[index].blue;
                pixels[index].alpha = 255;
            }
        }
    }
}