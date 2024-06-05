#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>


typedef struct {
    unsigned char* data;
    int width;
    int height;
} TGAImage;

TGAImage initializeImage(int width, int height) {
    TGAImage image;
    image.width = width;
    image.height = height;
    image.data = (unsigned char*)malloc(width * height * 4);
    memset(image.data, 0, width * height * 4);
    return image;
}

void freeImage(TGAImage* image) {
    free(image->data);
}

void setPixel(TGAImage* image, int x, int y, int r, int g, int b, int a) {
    if (x >= 0 && x < image->width && y >= 0 && y < image->height) {
        int index = (y * image->width + x) * 4;
        float alpha = a / 255.0;
        image->data[index] = (unsigned char)(alpha * r + (1.0 - alpha) * image->data[index]);
        image->data[index + 1] = (unsigned char)(alpha * g + (1.0 - alpha) * image->data[index + 1]);
        image->data[index + 2] = (unsigned char)(alpha * b + (1.0 - alpha) * image->data[index + 2]);
        image->data[index + 3] = 255; // Alpha kanál nastavený na 255
    }
}

void drawLine(TGAImage* image, int x1, int y1, int x2, int y2, int r, int g, int b, int a) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        setPixel(image, x1, y1, r, g, b, a);
        
        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void drawRectangle(TGAImage* image, int x, int y, int width, int height, int r, int g, int b, int a) {
    int xEnd = x + width;
    int yEnd = y + height;

    for (int i = x; i < xEnd; i++) {
        for (int j = y; j < yEnd; j++) {
            setPixel(image, i, j, r, g, b, a);
        }
    }
}


void drawCircle(TGAImage* image, int x, int y, int radius, int r, int g, int b, int a) {
    int cx = x;
    int cy = y;
    int d = 3 - 2 * radius;
    int x_ = 0;
    int y_ = radius;

    while (x_ <= y_) {
        setPixel(image, cx + x_, cy + y_, r, g, b, a);
        setPixel(image, cx - x_, cy + y_, r, g, b, a);
        setPixel(image, cx + x_, cy - y_, r, g, b, a);
        setPixel(image, cx - x_, cy - y_, r, g, b, a);
        setPixel(image, cx + y_, cy + x_, r, g, b, a);
        setPixel(image, cx - y_, cy + x_, r, g, b, a);
        setPixel(image, cx + y_, cy - x_, r, g, b, a);
        setPixel(image, cx - y_, cy - x_, r, g, b, a);

        if (d < 0) {
            d = d + 4 * x_ + 6;
        } else {
            d = d + 4 * (x_ - y_) + 10;
            y_--;
        }
        x_++;
    }
}


void drawTriangle(TGAImage* image, int x, int y, int side, int r, int g, int b, int a) {
    // Výpočet výšky rovnostranného trojuhlnika
    float height = sqrt(3) * side / 2.0;

    // Vrchol A
    int ax = x;
    int ay = y;

    // Vrchol B
    int bx = x + side;
    int by = y;

    // Vrchol C
    int cx = x + side / 2;
    int cy = y + height;

    // Vykreslenie stran trojuhelnika
    drawLine(image, ax, ay, bx, by, r, g, b, a);
    drawLine(image, bx, by, cx, cy, r, g, b, a);
    drawLine(image, cx, cy, ax, ay, r, g, b, a);
}

void drawRotatedRectangle(TGAImage* image, int angle, int x, int y, int width, int height, int r, int g, int b, int a) {
    // Prevod uhla na radiany
    double radianAngle = (double)angle * M_PI / 180.0;

    // Vypocet stredu obdlznika
    int centerX = x + width / 2;
    int centerY = y + height / 2;

    // Vypocet novych suradnic vrcholov obdzlnika po rotacii
    int x1 = centerX + (int)(cos(radianAngle) * (x - centerX) - sin(radianAngle) * (y - centerY));
    int y1 = centerY + (int)(sin(radianAngle) * (x - centerX) + cos(radianAngle) * (y - centerY));

    int x2 = centerX + (int)(cos(radianAngle) * (x + width - centerX) - sin(radianAngle) * (y - centerY));
    int y2 = centerY + (int)(sin(radianAngle) * (x + width - centerX) + cos(radianAngle) * (y - centerY));

    int x3 = centerX + (int)(cos(radianAngle) * (x + width - centerX) - sin(radianAngle) * (y + height - centerY));
    int y3 = centerY + (int)(sin(radianAngle) * (x + width - centerX) + cos(radianAngle) * (y + height - centerY));

    int x4 = centerX + (int)(cos(radianAngle) * (x - centerX) - sin(radianAngle) * (y + height - centerY));
    int y4 = centerY + (int)(sin(radianAngle) * (x - centerX) + cos(radianAngle) * (y + height - centerY));

    // Vykreslenie stran obdzlnika
    drawLine(image, x1, y1, x2, y2, r, g, b, a);
    drawLine(image, x2, y2, x3, y3, r, g, b, a);
    drawLine(image, x3, y3, x4, y4, r, g, b, a);
    drawLine(image, x4, y4, x1, y1, r, g, b, a);
}

#pragma pack(push, 1)
typedef struct {
    uint8_t idLength;
    uint8_t colorMapType;
    uint8_t imageType;
    uint16_t colorMapOrigin;
    uint16_t colorMapLength;
    uint8_t colorMapDepth;
    uint16_t xOrigin;
    uint16_t yOrigin;
    uint16_t width;
    uint16_t height;
    uint8_t pixelDepth;
    uint8_t imageDescriptor;
} TGAHeader;
#pragma pack(pop)

void saveImage(TGAImage* image, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // TGA hlavicka
    TGAHeader header;
    memset(&header, 0, sizeof(TGAHeader));
    header.imageType = 2;  // Truecolor
    header.width = (uint16_t)image->width;
    header.height = (uint16_t)image->height;
    header.pixelDepth = 32;  // 32-bit obrazky s alfa kanalom

    fwrite(&header, sizeof(TGAHeader), 1, file);

    // Data obrazu
    fwrite(image->data, sizeof(unsigned char), image->width * image->height * 4, file);

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s --output <output_file> --width <width> --height <height>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int width = 200;
    int height = 200;
    char* outputFilename = NULL;

    // Zpracovanie argumentov z prikazoveho riadku
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                outputFilename = argv[i + 1];
                i++;
            } else {
                fprintf(stderr, "Error: Missing output filename\n");
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "--width") == 0) {
            if (i + 1 < argc) {
                width = atoi(argv[i + 1]);
                i++;
            } else {
                fprintf(stderr, "Error: Missing width value\n");
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "--height") == 0) {
            if (i + 1 < argc) {
                height = atoi(argv[i + 1]);
                i++;
            } else {
                fprintf(stderr, "Error: Missing height value\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (width <= 0 || height <= 0 || outputFilename == NULL) {
        fprintf(stderr, "Error: Invalid width, height, or output filename\n");
        exit(EXIT_FAILURE);
    }

    // Inicializacia TGA obrazka s danymi rozmermi
    TGAImage image = initializeImage(width, height);

    // Zpracovanie prikazov
    char command[100];

    while (fgets(command, sizeof(command), stdin) != NULL) {
        if (strncmp(command, "line", 4) == 0) {
            // Nacitenie parametrov a vykreslenie ciary
            // ...
        } else if (strncmp(command, "rectangle", 9) == 0) {
            // Nacitenie parametrov a vykreslenie obdlznika
            // ...
        } else if (strncmp(command, "circle", 6) == 0) {
            // Nacitenie parametrov a vykreslenie kruhu
            // ...
        } else if (strncmp(command, "triangle", 8) == 0) {
            // NNacitenie parametrov a vykreslenie trojuholnika
            // ...
        } else if (strncmp(command, "rotated-rectangle", 17) == 0) {
            // NNacitenie parametrov a vykreslenie rotovaneho obdlznika
            // ...
        } else if (strncmp(command, "save", 4) == 0) {
            // Ulozenie obrazka
            saveImage(&image, outputFilename);
        } else if (strncmp(command, "exit", 4) == 0) {
            // Ulozenie obrazka a ukoncenie programu
            saveImage(&image, outputFilename);
            freeImage(&image);
            return 0;
        } else {
            fprintf(stderr, "Error: Invalid command\n");
        }
    }

    // Uvolnenie pameti a ukoncenie programu
    freeImage(&image);
    return 0;
}