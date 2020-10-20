/**
 * image.c
 * Talib Pierson
 * 19 October 2020
 * Generate and write a PPM image to stdout.
 */
#include <libgen.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC_NUMBER "P6"
#define DEPTH 3u
#define MAXVAL 255u
#define MAXVAL_STR "255"
#define BASE 0.5

/**
 * Use malloc to allocate size bytes of memory.
 * Prints an error and exits if malloc returns NULL.
 * @param size bytes of memory to allocate
 * @return value pointer to allocated memory
 */
static void *xmalloc(size_t size) {
    void *value = malloc(size);
    if (value == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    return value;
}

/**
 * Use fwrite to write binary output to stdout.
 * Prints an error and exits if fwrite does not return nitems.
 * @param buff pointer to buffer to write from
 * @param bytes number of bytes to write
 */
static void xwrite(const void *buff, size_t bytes) {
    if (fwrite(buff, 1, bytes, stdout) != bytes) {
        perror("fwrite");
        exit(EXIT_FAILURE);
    }
}

/**
 * Writes the color for a pixel to image.
 * Color conversion formula from HSV to RGB.
 * @param pix The pixel of the image to write to
 * @param h The hue of the color to write
 * @param s The saturation of the color to write
 * @param v The value of the color to write
 */
static void hsv(unsigned char *pix, unsigned h, double s, double v) {
    unsigned char max = (unsigned char) lround(255 * v);
    unsigned char min = (unsigned char) lround(255 * (v - (double) s * v));
    switch (h % 6) {
        default:
            pix[0] = max;
            pix[1] = min;
            pix[2] = min;
            return;
        case 1:
        case 2:
            pix[0] = min;
            pix[1] = max;
            pix[2] = min;
            return;
        case 3:
        case 4:
            pix[0] = min;
            pix[1] = min;
            pix[2] = max;
            return;
    }
}

/**
 * Writes the color for a pixel to image.
 * @param pix The pixel of the image to write to
 * @param cols The width of the image (number of columns)
 * @param row The row of the pixel to get the color for
 * @param col The column of the pixel to get the color for
 */
static void color(unsigned char *pix, size_t cols, size_t row, size_t col) {
    // change to baseline for saturation and value
    double shift = BASE * (double) col / (double) (cols - 1);

    // values to pass to hsv
    double sat = BASE + shift;
    double val = (2 * BASE) - shift;

    hsv(pix, (unsigned) row, sat, val);
}

int main(int argc, char *argv[]) {
    // Validate the argument count.
    if (argc != 3) {
        fprintf(stderr, "Usage %s <width> <height>\n", basename(argv[0]));
        return EXIT_FAILURE;
    }

    // Read the arguments.
    char *cols_str = argv[1];
    char *rows_str = argv[2];
    /// The width of the image (number of columns).
    const size_t cols = strtoul(cols_str, NULL, 10);
    /// The height of the image (number of rows).
    const size_t rows = strtoul(rows_str, NULL, 10);
    /// The size of the image (number of bytes).
    const size_t size = rows * cols * DEPTH;

    /// The image.
    unsigned char *img = (unsigned char *) xmalloc(size);

    // Generate the image.
    unsigned char *pix = img;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            color(pix, cols, row, col);
            pix += 3;
        }
    }

    int header_size = (int) (strlen(MAGIC_NUMBER) + strlen(cols_str) +
                             strlen(rows_str) + strlen(MAXVAL_STR) + 4);
    // Write the header of the image to stdout.
    if (printf("%s\n%s %s\n%u\n", MAGIC_NUMBER, cols_str, rows_str, MAXVAL) !=
        header_size) {
        perror("printf");
        free(img);
        return EXIT_FAILURE;
    }

    // Write the image.
    xwrite(img, size);

    // Free the block allocated by xmalloc.
    free(img);

    // Return successful exit status.
    return EXIT_SUCCESS;
}
