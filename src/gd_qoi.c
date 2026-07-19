#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"

#define QOI_IMPLEMENTATION
#include "gd_qoi.h"

#define GD_QOI_ALLOC_STEP 8192

static unsigned char alpha7BitTo8Bit(int alpha7Bit)
{
    return (unsigned char)(alpha7Bit == gdAlphaTransparent
                               ? 0
                               : 255 - ((alpha7Bit << 1) + (alpha7Bit >> 6)));
}

static int alpha8BitTo7Bit(unsigned char alpha8Bit) { return gdAlphaMax - (alpha8Bit >> 1); }

static void *gdQoiReadCtxToMemory(gdIOCtx *infile, int *size)
{
    unsigned char *data = NULL;
    int logical_size = 0;
    int real_size = 0;

    if (size != NULL) {
        *size = 0;
    }
    if (infile == NULL || size == NULL) {
        return NULL;
    }

    for (;;) {
        int n;

        if (real_size - logical_size < GD_QOI_ALLOC_STEP) {
            unsigned char *temp;
            int new_size;

            if (real_size > INT_MAX - GD_QOI_ALLOC_STEP) {
                gdFree(data);
                return NULL;
            }
            new_size = real_size + GD_QOI_ALLOC_STEP;
            temp = (unsigned char *)gdRealloc(data, new_size);
            if (temp == NULL) {
                gdFree(data);
                return NULL;
            }
            data = temp;
            real_size = new_size;
        }

        n = gdGetBuf(data + logical_size, GD_QOI_ALLOC_STEP, infile);
        if (n <= 0) {
            break;
        }
        logical_size += n;
    }

    *size = logical_size;
    return data;
}

static gdImagePtr gdImageCreateFromQoiBytes(const void *data, int size)
{
    qoi_desc desc;
    unsigned char *pixels;
    gdImagePtr im = NULL;
    unsigned int x, y;
    unsigned char *p;

    if (data == NULL || size <= 0) {
        return NULL;
    }

    memset(&desc, 0, sizeof(desc));
    pixels = (unsigned char *)gdQoiDecode(data, size, &desc, 4);
    if (pixels == NULL) {
        return NULL;
    }

    if (desc.width == 0 || desc.height == 0 || desc.width > INT_MAX || desc.height > INT_MAX) {
        gdFree(pixels);
        return NULL;
    }

    im = gdImageCreateTrueColor((int)desc.width, (int)desc.height);
    if (im == NULL) {
        gdFree(pixels);
        return NULL;
    }
    im->saveAlphaFlag = 1;
    im->alphaBlendingFlag = 0;

    p = pixels;
    for (y = 0; y < desc.height; y++) {
        for (x = 0; x < desc.width; x++) {
            unsigned char r = *(p++);
            unsigned char g = *(p++);
            unsigned char b = *(p++);
            unsigned char a = *(p++);
            im->tpixels[y][x] = gdTrueColorAlpha(r, g, b, alpha8BitTo7Bit(a));
        }
    }

    gdFree(pixels);
    return im;
}

static unsigned char *gdImageToQoiPixels(gdImagePtr im)
{
    unsigned char *pixels, *p;
    int x, y;

    if (im == NULL || gdImageSX(im) <= 0 || gdImageSY(im) <= 0) {
        return NULL;
    }
    if (overflow2(gdImageSX(im), gdImageSY(im)) || overflow2(gdImageSX(im) * gdImageSY(im), 4)) {
        return NULL;
    }

    pixels = (unsigned char *)gdMalloc((size_t)gdImageSX(im) * gdImageSY(im) * 4);
    if (pixels == NULL) {
        return NULL;
    }

    p = pixels;
    if (im->trueColor) {
        for (y = 0; y < gdImageSY(im); y++) {
            for (x = 0; x < gdImageSX(im); x++) {
                int c = im->tpixels[y][x];
                *(p++) = gdTrueColorGetRed(c);
                *(p++) = gdTrueColorGetGreen(c);
                *(p++) = gdTrueColorGetBlue(c);
                *(p++) = alpha7BitTo8Bit(gdTrueColorGetAlpha(c));
            }
        }
    } else {
        for (y = 0; y < gdImageSY(im); y++) {
            for (x = 0; x < gdImageSX(im); x++) {
                int c = im->pixels[y][x];
                *(p++) = (unsigned char)im->red[c];
                *(p++) = (unsigned char)im->green[c];
                *(p++) = (unsigned char)im->blue[c];
                *(p++) = alpha7BitTo8Bit(im->alpha[c]);
            }
        }
    }

    return pixels;
}

static int gdQoiNormalizeColorspace(int colorspace)
{
    return colorspace == GD_QOI_LINEAR ? QOI_LINEAR : QOI_SRGB;
}

static const unsigned char gd_qoi_end_marker[8] = {0, 0, 0, 0, 0, 0, 0, 1};

BGD_DECLARE(void) gdQoiInfoInit(gdQoiInfo *info)
{
    if (info != NULL) {
        memset(info, 0, sizeof(*info));
    }
}

BGD_DECLARE(int) gdQoiGetInfoPtr(int size, const void *data, gdQoiInfo *info)
{
    const unsigned char *bytes = (const unsigned char *)data;
    uint32_t width, height;

    if (info == NULL || data == NULL || size < 14 + 8) {
        return 0;
    }

    gdQoiInfoInit(info);
    if (memcmp(bytes, "qoif", 4) != 0) {
        return 0;
    }

    width = ((uint32_t)bytes[4] << 24) | ((uint32_t)bytes[5] << 16) |
        ((uint32_t)bytes[6] << 8) | bytes[7];
    height = ((uint32_t)bytes[8] << 24) | ((uint32_t)bytes[9] << 16) |
        ((uint32_t)bytes[10] << 8) | bytes[11];

    if (width == 0 || height == 0 || bytes[12] < 3 || bytes[12] > 4 || bytes[13] > 1 ||
        memcmp(bytes + size - sizeof(gd_qoi_end_marker), gd_qoi_end_marker, sizeof(gd_qoi_end_marker)) != 0 ||
        height >= 400000000U / width) {
        return 0;
    }

    info->width = width;
    info->height = height;
    info->channels = bytes[12];
    info->colorspace = bytes[13];
    return 1;
}

BGD_DECLARE(int) gdQoiGetInfoCtx(gdIOCtxPtr infile, gdQoiInfo *info)
{
    void *data;
    int size = 0;
    int result;

    if (infile == NULL || info == NULL) {
        return 0;
    }

    data = gdQoiReadCtxToMemory(infile, &size);
    if (data == NULL) {
        return 0;
    }
    result = gdQoiGetInfoPtr(size, data, info);
    gdFree(data);
    return result;
}

BGD_DECLARE(int) gdQoiGetInfo(FILE *infile, gdQoiInfo *info)
{
    gdIOCtx *in;
    int result;

    if (infile == NULL) {
        return 0;
    }
    in = gdNewFileCtx(infile);
    if (in == NULL) {
        return 0;
    }
    result = gdQoiGetInfoCtx(in, info);
    in->gd_free(in);
    return result;
}

BGD_DECLARE(void) gdQoiWriteOptionsInit(gdQoiWriteOptions *options)
{
    if (options != NULL) {
        options->colorspace = GD_QOI_SRGB;
        options->metadata = NULL;
    }
}

static int _gdImageQoiCtx(gdImagePtr im, gdIOCtx *outfile, const gdQoiWriteOptions *options)
{
    qoi_desc desc;
    unsigned char *pixels;
    void *encoded;
    int encoded_size = 0;
    int result = 0;

    gdQoiWriteOptions default_options;

    if (outfile == NULL) {
        return 0;
    }

    if (options == NULL) {
        gdQoiWriteOptionsInit(&default_options);
        options = &default_options;
    }

    pixels = gdImageToQoiPixels(im);
    if (pixels == NULL) {
        return 0;
    }

    desc.width = (unsigned int)gdImageSX(im);
    desc.height = (unsigned int)gdImageSY(im);
    desc.channels = 4;
    desc.colorspace = (unsigned char)gdQoiNormalizeColorspace(options->colorspace);

    encoded = gdQoiEncode(pixels, &desc, &encoded_size);
    gdFree(pixels);
    if (encoded == NULL || encoded_size <= 0) {
        gdFree(encoded);
        return 0;
    }

    if (gdPutBuf(encoded, encoded_size, outfile) == encoded_size) {
        result = 1;
    } else {
        gd_error("gd-qoi write error\n");
    }

    gdFree(encoded);
    return result;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromQoi(FILE *inFile)
{
    gdImagePtr im;
    gdIOCtx *in = gdNewFileCtx(inFile);
    if (in == NULL) {
        return NULL;
    }
    im = gdImageCreateFromQoiCtx(in);
    in->gd_free(in);
    return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromQoiPtr(int size, void *data)
{
    return gdImageCreateFromQoiBytes(data, size);
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromQoiCtx(gdIOCtx *infile)
{
    void *data;
    int size = 0;
    gdImagePtr im;

    data = gdQoiReadCtxToMemory(infile, &size);
    if (data == NULL) {
        return NULL;
    }
    im = gdImageCreateFromQoiBytes(data, size);
    gdFree(data);
    return im;
}

BGD_DECLARE(gdImagePtr) gdImageCreateFromQoiSource(gdSourcePtr inSource)
{
    gdImagePtr im;
    gdIOCtx *in;
    if (inSource == NULL) {
        return NULL;
    }
    in = gdNewSSCtx(inSource, NULL);
    if (in == NULL) {
        return NULL;
    }
    im = gdImageCreateFromQoiCtx(in);
    in->gd_free(in);
    return im;
}

BGD_DECLARE(int) gdImageQoiWithOptions(gdImagePtr im, FILE *outFile, const gdQoiWriteOptions *options)
{
    gdIOCtx *out = gdNewFileCtx(outFile);
    if (out == NULL) {
        return 0;
    }
    int result = gdImageQoiCtxWithOptions(im, out, options);
    out->gd_free(out);
    return result;
}

BGD_DECLARE(void) gdImageQoi(gdImagePtr im, FILE *outFile)
{
    gdImageQoiWithOptions(im, outFile, NULL);
}

BGD_DECLARE(void) gdImageQoiEx(gdImagePtr im, FILE *outFile, int colorspace)
{
    gdQoiWriteOptions options;
    gdQoiWriteOptionsInit(&options);
    options.colorspace = colorspace;
    gdImageQoiWithOptions(im, outFile, &options);
}

BGD_DECLARE(void *) gdImageQoiPtrWithOptions(gdImagePtr im, int *size, const gdQoiWriteOptions *options)
{
    gdIOCtx *out;
    void *rv = NULL;

    if (size != NULL) {
        *size = 0;
    }
    if (size == NULL) {
        return NULL;
    }

    out = gdNewDynamicCtx(2048, NULL);
    if (out == NULL) {
        return NULL;
    }
    if (_gdImageQoiCtx(im, out, options)) {
        rv = gdDPExtractData(out, size);
    }
    out->gd_free(out);
    return rv;
}

BGD_DECLARE(void *) gdImageQoiPtr(gdImagePtr im, int *size)
{
    return gdImageQoiPtrWithOptions(im, size, NULL);
}

BGD_DECLARE(void *) gdImageQoiPtrEx(gdImagePtr im, int *size, int colorspace)
{
    gdQoiWriteOptions options;
    gdQoiWriteOptionsInit(&options);
    options.colorspace = colorspace;
    return gdImageQoiPtrWithOptions(im, size, &options);
}

BGD_DECLARE(void) gdImageQoiCtx(gdImagePtr im, gdIOCtx *outfile)
{
    gdImageQoiCtxWithOptions(im, outfile, NULL);
}

BGD_DECLARE(int) gdImageQoiCtxWithOptions(gdImagePtr im, gdIOCtx *outfile, const gdQoiWriteOptions *options)
{
    return _gdImageQoiCtx(im, outfile, options);
}

BGD_DECLARE(void)
gdImageQoiCtxEx(gdImagePtr im, gdIOCtx *outfile, int colorspace)
{
    gdQoiWriteOptions options;
    gdQoiWriteOptionsInit(&options);
    options.colorspace = colorspace;
    gdImageQoiCtxWithOptions(im, outfile, &options);
}

BGD_DECLARE(void) gdImageQoiToSink(gdImagePtr im, gdSinkPtr outSink)
{
    gdIOCtx *out;
    if (outSink == NULL) {
        return;
    }
    out = gdNewSSCtx(NULL, outSink);
    if (out == NULL) {
        return;
    }
    gdImageQoiCtx(im, out);
    out->gd_free(out);
}
