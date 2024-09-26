#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <getopt.h> 
#include <stdint.h> 
#include <math.h> 
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#pragma pack(push, 1)
 
typedef struct BitmapFileHeader{ 
    char signature[2]; 
    uint32_t file_size; 
    uint16_t reserved1; 
    uint16_t reserved2; 
    uint32_t  pixelArrOffset; 
}BitmapFileHeader; 
 
typedef struct { 
    uint32_t headerSize; 
    uint32_t width; 
    uint32_t height; 
    uint16_t planes; 
    uint16_t bitsPerPixel; 
    uint32_t compression; 
    uint32_t imageSize; 
    uint32_t xPixelsPerMeter; 
    uint32_t yPixelsPerMeter; 
    uint32_t colorsInColorTable; 
    uint32_t importantColorCount; 
}BitmapInfoHeader; 
 
typedef struct RGB{ 
    unsigned char b; 
    unsigned char g; 
    unsigned char r; 
} RGB; 
 
 
typedef struct BMPFile{ 
    BitmapFileHeader bheader; 
    BitmapInfoHeader dheader; 
    RGB** arr; 
}BMPFile; 
 
 
#pragma pack(pop)
 
 
void printFileHeader (BitmapFileHeader header){ 
    printf("signature:\t%s\n", header.signature);
    printf("filesize:\t%x (%u)\n", header.file_size, header.file_size); 
    printf("reserved1:\t%x (%hu)\n", header.reserved1, header.reserved1); 
    printf("reserved2:\t%x (%hu)\n", header.reserved2, header.reserved2); 
    printf("pixelArrOffset:\t%x (%u)\n", header.pixelArrOffset, header.pixelArrOffset); 
} 
 
 
void printInfoHeader(BitmapInfoHeader header){ // выводит соответствующую инфформацию по файлу bmp (структура BitmapInfoHeader) 
    printf("headerSize:\t%x (%u)\n", header.headerSize, header.headerSize); 
    printf("width:     \t%x (%u)\n", header.width, header.width); 
    printf("height:    \t%x (%u)\n", header.height, header.height); 
    printf("planes:    \t%x (%hu)\n", header.planes, header.planes); 
    printf("bitsPerPixel:\t%x (%hu)\n", header.bitsPerPixel, header.bitsPerPixel); 
    printf("compression:\t%x (%u)\n", header.compression, header.compression); 
    printf("imageSize:\t%x (%u)\n", header.imageSize, header.imageSize); 
    printf("xPixelsPerMeter:\t%x (%u)\n", header.xPixelsPerMeter, header.xPixelsPerMeter); 
    printf("yPixelsPerMeter:\t%x (%u)\n", header.yPixelsPerMeter, header.yPixelsPerMeter); 
    printf("colorsInColorTable:\t%x (%u)\n", header.colorsInColorTable, header.colorsInColorTable); 
    printf("importantColorCount:\t%x (%u)\n", header.importantColorCount, header.importantColorCount); 
 
} 
 
 
BMPFile* readBMPfile(const char* file_name){ 
    FILE* fp = fopen(file_name, "rb"); 
    if (!fp)return NULL; 
 
    BMPFile* bmp_file = (BMPFile*)malloc(sizeof(BMPFile)); // выделяем память для основной структуры (указатель на структуру) 
    fread(&(bmp_file->bheader), sizeof(BitmapFileHeader), 1, fp);
    fread(&(bmp_file->dheader), sizeof(BitmapInfoHeader), 1, fp); 
 
    int w = bmp_file->dheader.width;  
    int h = bmp_file->dheader.height; 
    bmp_file->arr = (RGB**)malloc(h * sizeof(RGB*)); 
    for (int i = 0; i < h; i++){ 
        int padding = (4 - (w*sizeof(RGB)) % 4) % 4;//количество байт в конце для выравнивания 
        bmp_file->arr[i] = (RGB*)malloc(w * sizeof(RGB) + padding); 
        fread(bmp_file->arr[i], 1, w * sizeof(RGB) + padding, fp); 
    } 
    fclose(fp); 
    return bmp_file; 
     
} 
 
 
void writeBMPfile(const char* name_file, BMPFile* input_file){ 
    FILE* file = fopen(name_file, "wb"); 
    fwrite(&input_file->bheader, sizeof(BitmapFileHeader), 1, file); 
    fwrite(&input_file->dheader, sizeof(BitmapInfoHeader), 1, file); 
    int width = input_file->dheader.width; 
    int height = input_file->dheader.height; 
    unsigned char padding = (4 - (width * sizeof(RGB)) % 4) % 4; 
    unsigned char paddingBytes[3] = {0}; 
    for (int i = 0; i < height; i++) { 
        fwrite(input_file->arr[i], sizeof(RGB), width, file); 
        fwrite(paddingBytes, sizeof(uint8_t), padding, file); // Записываем выравнивающие байты
    } 
    fclose(file); 
}

void set_color(RGB* cell, RGB color){ 
    cell->r = color.r; 
    cell->g = color.g; 
    cell->b = color.b; 
} 
 
 
void fill_circle(BMPFile* bmp_file, RGB color, int x0, int y0, int radius){  
    int h = bmp_file->dheader.height; 
    int w = bmp_file->dheader.width; 
    for (int i = -radius; i <= radius; i++){ 
        for (int j = -radius; j <= radius; j++){ 
            int x_r = x0 + j; 
            int y_r = y0 - i; 
            if (i * i + j * j <= radius * radius && 
            x_r >= 0 && x_r < w && y_r >= 0 && y_r < h){ 
                set_color(&((bmp_file->arr)[y_r][x_r]), color); 
            } 
        } 
    } 
 
} 
 
 
void fill_rectagle(BMPFile* bmp_file, RGB color, int x0, int y0, int radius){ 
    int h = bmp_file->dheader.height; 
    int w = bmp_file->dheader.width; 
    for (int i = -radius; i <= radius; i++){ 
        for (int j = -radius; j <= radius; j++){ 
            int x_r = x0 + j; 
            int y_r = y0 - i; 
            if (x_r >= 0 && x_r < w && y_r >= 0 && y_r < h){ 
                set_color(&((bmp_file->arr)[y_r][x_r]), color); 
            } 
        } 
    } 
} 

void draw_line(BMPFile* bmp_file, RGB color,  
            int x1, int y1, int x2, int y2, int thickness){ 
    thickness = thickness - 1; 
    if (thickness < 0){ 
        //printf("Error"); 
        return; 
    } 
 
    const int deltaX = abs(x2 - x1); 
    const int deltaY = abs(y2 - y1); 
    const int signX = x1 < x2 ? 1 : -1; 
    const int signY = y1 < y2 ? 1 : -1; 
    int error = deltaX - deltaY;
    set_color(&((bmp_file->arr)[y2][x2]), color); 
    //printf("%d / %d, %d / %d\n", x1,  bmp_file->dheader.width, y1,  bmp_file->dheader.height);
    fill_circle(bmp_file, color, x2, y2, thickness); 
    while((x1 != x2 || y1 != y2) && x1 <  bmp_file->dheader.width && y1 <  bmp_file->dheader.height)  
    { 
        set_color(&((bmp_file->arr)[y1][x1]), color); 
        fill_circle(bmp_file, color, x1, y1, thickness); 
        int error2 = error * 2; 
        if(error2 > -deltaY)  
        { 
            error -= deltaY; 
            x1 += signX; 
        } 
        if(error2 < deltaX)  
        { 
            error += deltaX; 
            y1 += signY; 
        } 
    } 
    //printf("fff");
 
} 

void draw_rectangle(BMPFile* bmp_file, RGB color, int x0, int y0, 
                    int x1, int y1, int th, int is_fill, RGB color_fill){
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    for (int i = 0; i < th; i++){
        draw_line(bmp_file, color, x0 + i, y0 - i, x0 + dx - i, y0 - i, 1); // top
        draw_line(bmp_file, color, x0 + i, y0 - i, x0 + i, y0 - dy + i, 1); // left
        draw_line(bmp_file, color, x0 + i, y0 - dy + i, x0 + dx - i, y0 - dy + i, 1); // bottom
        draw_line(bmp_file, color, x0 - i + dx, y0 - i, x0 + dx - i, y0 - dy + i, 1); // right
    }
    
    if (is_fill == 1){
        for (int y = y0 - th; y >= (y1 + th); y--){
            for (int x = x0 + th; x <= (x1 - th); x++){
                set_color(&((bmp_file->arr)[y][x]), color_fill);
            }
        }
    }
    else if (is_fill == 2){
        int h = bmp_file->dheader.height;
        int w = bmp_file->dheader.width;
        for (int y = 0; y < h; y++){
            for (int x = 0; x < w; x++){
                if (!(x > x0 && y < y0 && x < x1 && y > y1)){
                    set_color(&((bmp_file->arr)[y][x]), color_fill);
                }
            }
        }
    }
}

void rect_ornament(BMPFile * image, int thickness, int cnt, RGB color) {
    int h = image->dheader.height; 
    int w = image->dheader.width; 
    int m = h < w ? h : w; 
    int count = m / (4 * thickness); 
    if (cnt < count){ 
        count = cnt; 
    } 
    h--;  
    w--; 
    int x0 = 0; 
    int y0 = h; 
    int xn = w; 
    int yn = 0; 
    for (int i = 0; i < count; i++){ 
        draw_rectangle(image, color, x0, y0, xn, yn, thickness, 0, color); 
        x0 += thickness * 2; 
        y0 -= thickness * 2; 
        xn -= thickness * 2; 
        yn += thickness * 2; 
    } 
}



void circ_ornament(BMPFile * image, RGB color) {
    int h = image->dheader.height;
    int w = image->dheader.width;
    int radius = h/2;
    if(w < h) radius = w/2;
    int center_x = w/2;
    int center_y = h/2;

    for (int i = 0; i < h; i++){ 
        for (int j = 0; j < w; j++){ 
            if ((i - center_y) * (i - center_y) + (j - center_x)*(j - center_x) > radius * radius){ 
                set_color(&((image->arr)[i][j]), color); 
            } 
        } 
    } 
}

void circ_line(BMPFile * image, int thickness, int x, int y, int radius, RGB color) {
    int h = image->dheader.height;
    int w = image->dheader.width;
    int x0 = x - radius;
    if(x0 < 0) x0 = 0;
    int y0 = y - radius;
    if(y0 < 0) y0 = 0;
    int xn = x + radius;
    if(xn > w) xn = w;
    int yn = y + radius;
    if(yn > h) yn = h;
    for(int i = y0; i < yn; i++) {
        for(int j = x0; j < xn; j++) {
            if ((i - y) * (i - y) + (j - x)*(j - x) <= radius * radius && (i - y) * (i - y) + (j - x)*(j - x) >= (radius - thickness) * (radius - thickness) ){ 
                set_color(&((image->arr)[i][j]), color); 
            } 
        }
    }
}
void semicircles_ornament(BMPFile * image, int thickness, int count, RGB color) {
    int h = image->dheader.height;
    int w = image->dheader.width;
    if(count == 1) {
        int xradius = (w - (thickness + 1)/2) / (2 * count);
        int yradius = (h - (thickness + 1)/2) / (2 * count);
        circ_line(image, thickness, 0, h/2, yradius + thickness, color);
        circ_line(image, thickness, w - 1, h/2, yradius + thickness, color);

        circ_line(image, thickness, w/2, 0, xradius + thickness, color);
        circ_line(image, thickness, w/2, h - 1, xradius + thickness, color);

    } else if(count % 2 == 0){
        int xradius = (w - count  * thickness) / (2 * count);
        int yradius = (h - count  * thickness) / (2 * count);
        int x = w - xradius - (thickness + 1)/2, y = h - yradius - (thickness+1)/2;
        for(int c = 0; c < count; c++) {
            circ_line(image, thickness, 0, y, yradius + thickness, color);
            circ_line(image, thickness, w, y, yradius + thickness, color);

            circ_line(image, thickness, x, 0, xradius + thickness, color);
            circ_line(image, thickness, x, h, xradius + thickness, color);

            x -= 2 * xradius + thickness;
            y -= 2 * yradius + thickness;
        }
    } else {
        int xradius = (w - (count - 1) * thickness + thickness/2) / (2 * count);
        int yradius = (h - (count - 1) * thickness + thickness/2) / (2 * count);
        int x = w - xradius - (thickness + 1)/2, y = h - yradius - (thickness+1)/2;
        for(int c = 0; c < count; c++) {
            circ_line(image, thickness, 0, y, yradius + thickness, color);
            circ_line(image, thickness, w, y, yradius + thickness, color);

            circ_line(image, thickness, x, 0, xradius + thickness, color);
            circ_line(image, thickness, x, h, xradius + thickness, color);

            x -= 2 * xradius + thickness;
            y -= 2 * yradius + thickness;
        }
    }
}


void recolor(BMPFile* image, RGB old_color, RGB new_color) {
	for (int i = 0; i < image->dheader.height; i++) {
		for (int j = 0; j < image->dheader.width; j++) {
			RGB *sptr = &image->arr[i][j];
			if (sptr->r == old_color.r 
				&& sptr->g == old_color.g 
				&& sptr->b == old_color.b ) {
				sptr->r = new_color.r;
                sptr->g = new_color.g;
                sptr->b = new_color.b;
			}
		}
	}
}


bool valid(int ** a, int r, int c, int w, int h) {
    for(int i = r; i < h + r; i++) {
        for(int j = c; j < w + c; j++) {
            if(a[i][j] != 1)
                return false;
        }
    } 
    return true;
}

void fill_a(int ** a, int x1, int x2, int y1, int y2) {
    for(int i = y1; i < y2; i++) {
        for(int j = x1; j < x2; j++) {
            a[i][j] = 2;
        }
    } 
}

void find_rect(int ** a, int i, int j, int n, int m, int (*coords)[4], int * count) {
    int x1 = j, x2 = j, y1 = i, y2 = i;
    for(int h = n - i; h > 1; h--) {
        for( int w = m - j; w > 1; w--) {
            if(valid(a, i, j, w, h)) {
                x2 = x1 + w;
                y2 = y1 + h;
                
                fill_a(a, x1, x2, y1, y2);
                
                coords[*count][0] = x1;
                coords[*count][1] = y1;
                coords[*count][2] = x2;
                coords[*count][3] = y2;
                *count = *count + 1;
                
                return;
            } 
        }
    }
}

void refill_rectangle(BMPFile* image, RGB color_line, RGB color_fill, int thickness) {
    const int n = image->dheader.height;
    const int m = image->dheader.width;
    int coords[10000][4]; 
    int count = 0;
    int** a =  (int **)calloc( n + 1, sizeof( int* ) );
    for (int i = 0; i < n; i++)
        a[i] = (int *)calloc( m + 1, sizeof( int ) );
    for (int i = n - 1; i >= 0; i--)
        for (int j = 0; j < m; j++)
            if (image->arr[i][j].r == color_fill.r 
                && image->arr[i][j].g == color_fill.g
                && image->arr[i][j].b == color_fill.b)
                a[i][j] = 1;
            else
                a[i][j] = 0;
    // for(int i = 0; i < n; i++) {
    //     for(int j = 0; j < m; j++) {
    //         printf("%d", a[i][j]);
    //     }
    //     printf("\n");
    for (int y = 0; y < n - 1; y++){ 
        for (int x = 0; x < m - 1; x++){ 
            if (a[y][x] == 1){
                find_rect(a, y, x, n, m, coords, &count);
            } 
        } 
    } 
    for (int i = 0; i < count; i++) { 
        int x1 =  coords[i][0], y1 = coords[i][1], x2 = coords[i][2],y2 = coords[i][3];
        int r_x1 = x1 - thickness, r_y1 = y1 - thickness,   r_x2 = x2 + thickness, r_y2 = y2 + thickness;
        if(r_x1 < 0) r_x1 = 0;
        if(r_y1 < 0) r_y1 = 0;
        if(r_x2 > m) r_x2 = m;
        if(r_y2 > n) r_y2 = n;
        for (int y = r_y1; y < r_y2; y++){ 
            for (int x = r_x1; x < r_x2; x++){ 
                RGB *sptr = &image->arr[y][x];
				sptr->r = color_line.r;
                sptr->g = color_line.g;
                sptr->b = color_line.b;
            } 
        }
        for (int y = y1; y < y2; y++){ 
            for (int x = x1; x < x2; x++){ 
                RGB *sptr = &image->arr[y][x];
				sptr->r = color_fill.r;
                sptr->g = color_fill.g;
                sptr->b = color_fill.b;
            } 
        }
    } 
    
}

RGB read_color(char *opt) {
    RGB color = {0, 0, 0}; // Инициализируем значения по умолчанию
    char *token = strtok(opt, ".");
    for (int i = 0; i < 3; i++) {
        if (token == NULL) {
            //printf("Ошибка: недостаточно компонент в цвете\n");
            exit(45);
        }
        // Проверяем, состоит ли токен только из цифр
        for (int j = 0; j < strlen(token); j++) {
            if (token[j] < '0' || token[j] > '9') {
                //printf("Ошибка: неверный формат числа\n");
                exit(45);
            }
        }
        // Преобразуем строку в число и сохраняем в структуру
        int value = atoi(token);
        if (value < 0 || value > 255) {
            //printf("Ошибка: компонент цвета должен быть в диапазоне от 0 до 255\n");
            exit(45);
        }
        switch (i) {
            case 0:
                color.r = value;
                break;
            case 1:
                color.g = value;
                break;
            case 2:
                color.b = value;
                break;
        }
        token = strtok(NULL, ".");
    }
    if (token != NULL) {
        //printf("Ошибка: слишком много компонент в цвете\n");
        exit(45);
    }
    return color;
}

int get_pattern(char * pattern) {
    if (!strcmp("rectangle", pattern)) {
        return 1;
    } else if (!strcmp("circle", pattern)) {
        return 2;
    } else if (!strcmp("semicircles", pattern)) {
        return 3;
    } else {
        //printf("Неверный паттерн\n");
        exit(45);
    }
}

int get_int(char * token) {
    for (int j = 0; j < strlen(token); j++) {
        if (token[j] < '0' || token[j] > '9') {
            //printf("Ошибка: неверный формат числа\n");
            exit(45);
        }
    }
    int value = atoi(token);
    if(value <= 0) {
        //printf("Аргумент должен быть больше 0\n");
        exit(45);
    }
    return value;
}

struct Configs {
    char * func;
    RGB old_color;
    RGB new_color;

    int pattern;
    RGB color;
    int thickness;
    int count;
    RGB border_color;
    char *input;
    char *output; // указатель на строку с именем файла, куда будет сохранено изображение
};

// расстановка настроек getopt
void choice(struct Configs *config, int opt) {
    switch (opt) {
        case 'i':
            config->input = optarg;
            break;
        case 'o':
            config->output = optarg;
            break;
        case 'z':
            config->func = "info";
            break;
        case 'h':
            config->func = "help";
            break;
        case 'r':
            config->func = "color_replace";
            break;
        case 'u':
            config->func = "ornament";
            break;
        case 'f':
            config->func = "filled_rects";
            break;
        case 's':
            config->old_color = read_color(optarg);
            break;
        case 'n':
            config->new_color = read_color(optarg);
            break;
        case 'c':
            config->color = read_color(optarg);
            break;
        case 'b':
            config->border_color = read_color(optarg);
            break;
        case 'p':
            config->pattern = get_pattern(optarg);
            break;
        case 't':
            config->thickness = get_int(optarg);
            break;
        case 'C':
            config->count = get_int(optarg);
            break;
        default: 
            //printf("Неверный флаг\n");
            exit(45);
    }
}

void printHelp() {
    printf("Длинная команда --input  короткая комана -i\n");
    printf("Требует параметр строку - название входного файла \n\n");
    printf("Длинная команда --output  короткая комана -o\n");
    printf("Требует параметр строку - название выходного файла \n\n");
    printf("Длинная команда --info  короткая комана -z\n");
    printf("Длинная команда --help  короткая комана -h\n\n");

    printf("Длинная команда --color_replace  короткая комана -r\n");
    printf("Вызов задания 1. Требует флаги --old_color --new_color \n\n");

    printf("Длинная команда --old_color короткая комана -s\n");
    printf("Требует параметр - строку вида rrr.ggg.bbb \n\n");
    printf("Длинная команда --new_color короткая комана -n");
    printf("Требует параметр - строку вида rrr.ggg.bbb \n\n");

    printf("Длинная команда --ornament  короткая комана -u\n");
    printf("Вызов функции 2.Требует флаги --pattern --color --thickness --count\n\n");
    printf("Длинная команда --pattern  короткая комана -p\n");
    printf("Требует памаретр - строку rectangle/circle/semicircles \n\n");
    printf("Длинная команда --color  короткая комана -c\n");
    printf("Требует параметр - строку вида rrr.ggg.bbb \n\n");
    printf("Длинная команда --thickness  короткая комана -t\n");
    printf("Требует параметр - целое число, толщина рисуемых линий \n\n");
    printf("Длинная команда --count        короткая комана -C\n");
    printf("Требует параметр - целое число, количество для орнамента \n\n");

    printf("Длинная команда --filled_rects  короткая комана -f\n");
    printf("Вызов задания 3.Требует флаги --border_color --color --thickness\n\n");
    printf("Длинная команда --border_color  короткая комана -b\n");
    printf("Требует параметр - строку вида rrr.ggg.bbb \n");
}

int main(int argc, char **argv) {
    char *opts = "i:o:zhrs:n:up:c:t:C:fb:";
    RGB defcolor;
    defcolor.r = -1;
    defcolor.g = -1;
    defcolor.b = -1;
    struct Configs config = {"", defcolor, defcolor, 0, defcolor, 0, 0, defcolor, "", ""};
    BMPFile * image;
    struct option longOpts[] = {
            {"input",    required_argument, NULL, 'i'},
            {"output", required_argument, NULL, 'o'},
            {"info",   no_argument, NULL, 'z'},
            {"help",    no_argument, NULL, 'h'},
            //Поменять цвет
            {"color_replace",   no_argument, NULL, 'r'},
            {"old_color",  required_argument, NULL, 's'},
            {"new_color",  required_argument, NULL, 'n'},
            //Рамка
            {"ornament",    no_argument, NULL, 'u'},
            {"pattern", required_argument, NULL, 'p'},
            {"color",   required_argument, NULL, 'c'},
            {"thickness",    required_argument, NULL, 't'},
            {"count",    required_argument,       NULL, 'C'},
            //Поиск залитых прямоугольников
            {"filled_rects",    no_argument, NULL, 'f'},
            {"border_color",   required_argument, NULL, 'b'},

            {NULL,      no_argument,       NULL, 0}
    };
    int opt;
    int longIndex;
    opt = getopt_long(argc, argv, opts, longOpts, &longIndex);
    while (opt != -1) {
        choice(&config, opt);
        opt = getopt_long(argc, argv, opts, longOpts, &longIndex);
    }
    if (!strcmp(config.func, "help")) {
        printf("Course work for option 4.10, created by Anenkov Ivan\n");
        printHelp();
        return 0;
    } 

    if(strcmp(config.input, "") == 0) {
        config.input = argv[argc - 1];
    }
    // int len2 = strlen( config.input);
    // if(len2 < 5 || config.input[len2 - 1] != 'p' || 
    //     config.input[len2 - 2] != 'm' ||
    //     config.input[len2 - 3] != 'b' ||
    //     config.input[len2 - 4] != '.') {
    //     return 0;
    // }
    image = readBMPfile(config.input);
    if (image == NULL) {
        exit(45);
    } 
    if(!strcmp(config.output, "")) {
        config.output = config.input;
    }
    // len2 = strlen( config.output);
    // if(len2 < 5 || config.output[len2 - 1] != 'p' || 
    //     config.output[len2 - 2] != 'm' ||
    //     config.output[len2 - 3] != 'b' ||
    //     config.output[len2 - 4] != '.') {
    //     return 0;
    // }
    if (!strcmp(config.func, "info")) {
        //printf("Course work for option 4.10, created by Anenkov Ivan\n");
        printInfoHeader(image->dheader);
        return 0;
    } else if (!strcmp(config.func, "color_replace")) {
        if(config.old_color.r == -1 ||
            config.old_color.g == -1 ||
            config.old_color.b == -1 ||
            config.new_color.r == -1 ||
            config.new_color.g == -1 ||
            config.new_color.b == -1 ) {
            exit(45);
        }
        recolor(image, config.old_color, config.new_color);
    } else if (!strcmp(config.func, "ornament")) {
        if(config.pattern == 1) {
            if(config.thickness <= 0 || config.count <= 0 ||
                config.color.r == -1 ||
                config.color.g == -1 ||
                config.color.b == -1 ) {
                exit(45);
            }
            rect_ornament(image, config.thickness, config.count, config.color);
        } else if(config.pattern == 2) {
            if(config.color.r == -1 ||
                config.color.g == -1 ||
                config.color.b == -1 ) {
                exit(45);
            }
            circ_ornament(image, config.color);
        } else if(config.pattern == 3) {
            if(config.thickness <= 0 || config.count <= 0 || 
                config.color.r == -1 ||
                config.color.g == -1 ||
                config.color.b == -1 ) {
                exit(45);
            }
            semicircles_ornament(image, config.thickness, config.count, config.color);
        }
    } else if (!strcmp(config.func, "filled_rects")) {
        if(config.thickness <= 0 || 
                config.border_color.r == -1 ||
                config.border_color.g == -1 ||
                config.border_color.b == -1 ||
                config.color.r == -1 ||
                config.color.g == -1 ||
                config.color.b == -1 ) {
            exit(45);
        }
        refill_rectangle(image, config.border_color, config.color,config.thickness);
    } else  {
        //printf("Команда не найдена");
        exit(45);
    }
    //printf("Course work for option 4.10, created by Anenkov Ivan\n");
    writeBMPfile(config.output, image);
    return 0;
}