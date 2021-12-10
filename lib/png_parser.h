#include <png.h>  // requires libpng
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    png_uint_32 height;
    png_uint_32 width;
    int bit_depth;
    int color_type;
} png_info_t;

void alloc_rows(png_bytepp* row_ptr, png_uint_32 height, png_uint_32 width) {

    *row_ptr = (png_bytepp)malloc(sizeof(png_bytepp) * height);
    for (png_uint_32 i = 0; i < height; i++) {
        *row_ptr[i] = (png_bytep)malloc(sizeof(png_bytepp) * width);
    }

}

void free_rows(png_bytepp* row_ptr, png_uint_32 height) {

    for (png_uint_32 i = 0; i < height; i++) {
        free(row_ptr[i]);
    }

    free(row_ptr);

}

void read_png(char* file_name, png_info_t* png_info, png_bytepp* row_ptr) {

    FILE *file_stream = fopen(file_name, "rb");

    // check if it's png
    unsigned char header[9];
    fread(header, 1, 8, file_stream);
    if(png_sig_cmp(header, 0, 8)){
        exit(1);
    }

    // create png struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_infop end_info = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, file_stream);
    png_set_sig_bytes(png_ptr, 8);

    // read png info
    png_read_info(png_ptr, info_ptr);
    int interlace_type, compression_type, filter_method;
    png_get_IHDR(png_ptr, info_ptr, &png_info->width, &png_info->height, &png_info->bit_depth, &png_info->color_type,
        &interlace_type, NULL, NULL);

    // read image data
    *row_ptr = (png_bytepp)png_malloc(png_ptr, sizeof(png_bytepp) * png_info->height);
    for (png_uint_32 i = 0; i < png_info->height; i++) {
        *row_ptr[i] = (png_bytep)png_malloc(png_ptr, sizeof(png_bytepp) * png_info->width);
    }

    png_set_rows(png_ptr, info_ptr, *row_ptr);
    png_read_image(png_ptr, *row_ptr);
    png_read_end(png_ptr, end_info);

    // destroy struct and close file
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(file_stream);

}

void write_png(char* file_name, png_info_t* png_info, png_bytepp* row_data) {

    FILE* file_stream = fopen(file_name, "wb");

    // create png struct
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    // set png info
    png_init_io(png_ptr, file_stream);
    png_set_IHDR(png_ptr, info_ptr, png_info->width, png_info->height, png_info->bit_depth, png_info->color_type,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_bytepp row_ptr = (png_bytepp)png_malloc(png_ptr, sizeof(png_bytepp) * png_info->height);
    for (int i = 0; i < png_info->height; i++) {
        row_ptr[i] = (png_bytep)png_malloc(png_ptr, png_info->width);
    }
    for (png_uint_32 h = 0; h < png_info->height; h++) {
        for (png_uint_32 w = 0; w < png_info->width; w++) {
            row_ptr[h] [w] = *row_data[h] [w];
        }
    }

    // write png
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, row_ptr);
    png_write_end(png_ptr, info_ptr);

    // free memory
    for (png_uint_32 i = 0; i < png_info->height; i++) {
        png_free(png_ptr, row_ptr[i]);
    }
    png_free(png_ptr, *row_ptr);

    // destroy struct and close file
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(file_stream);

}
