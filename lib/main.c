#include <assert.h>
#include "png_parser.h"

png_uint_32 get_pixel_weight(png_bytepp* row_ptr, png_uint_32 h, png_uint_32 w, png_uint_32 height, png_uint_32 width) {
    // get weight of pixel in [h][w] position

    png_uint_32 weight = 0;

    for (int h_ = -1; h_ <= 1; ++h_) {

        if ((h_ == -1 && h == 0) || (h_ == 1 && h == height - 1)) {
            continue;
        }

        for (int w_ = -1; w_ <= 1; ++w_) {

            if (h_ == 0 && w_ == 0) {
                continue;
            }
            if ((w_ == -1 && w == 0) || (w_ == 1 && w == width - 1)) {
                continue;
            }

            if (*row_ptr[h + h_] [w + w_] < *row_ptr[h] [w]) {
                weight += (*row_ptr[h] [w] - *row_ptr[h + h_] [w + w_]) * (*row_ptr[h] [w] - *row_ptr[h + h_] [w + w_]);
            } else {
                weight += (*row_ptr[h + h_] [w + w_] - *row_ptr[h] [w]) * (*row_ptr[h + h_] [w + w_] - *row_ptr[h] [w]);
            }

        }

    }

    return weight;

}

void decrease_png_width(png_info_t* png_info, png_uint_32 new_width, png_bytepp* row_ptr) {

    for (; png_info->width > new_width; --png_info->width) {

        png_uint_32 min_column_weight = 0 - 1;
        png_uint_32 min_column = 0;
        for (png_uint_32 w = 0; w < png_info->width; ++w) {
            png_uint_32 column_weight = 0;
            for (png_uint_32 h = 0; h < png_info->height; ++h) {
                column_weight += get_pixel_weight(row_ptr, h, w, png_info->height, png_info->width);
            }
            if (column_weight < min_column_weight) {
                min_column_weight = column_weight;
                min_column = w;
            }
        }

        png_bytepp* new_row_ptr = NULL;
        alloc_rows(new_row_ptr, png_info->height, png_info->width - 1);
        for (png_uint_32 w = 0; w < png_info->width - 1; ++w) {
            if (w != min_column) {
                for (png_uint_32 h = 0; h < png_info->height; ++h) {
                    *new_row_ptr[h] [w] = *row_ptr[h] [w > min_column ? w + 1 : w];
                }
            }
        }

        free_rows(row_ptr, png_info->height);
        row_ptr = new_row_ptr;

    }

}

int main(int argc, char* argv[]) {

    // parse arguments
    assert(argc >= 2);
    char* file_name = argv[1];
    char* new_file_name = argv[2];

    // read new image width
    png_uint_32 new_width;
    scanf("%u", &new_width);

    // read png
    png_info_t png_info;
    png_bytepp* row_ptr = NULL;
    read_png(file_name, &png_info, row_ptr);

    decrease_png_width(&png_info, new_width, row_ptr);

    write_png(new_file_name, &png_info, row_ptr);

    return 0;

}
