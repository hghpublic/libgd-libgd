#ifndef GD_H
#define GD_H 1

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version information.  This gets parsed by build scripts as well as
 * gcc so each #define line in this group must also be splittable on
 * whitespace, take the form GD_*_VERSION and contain the magical
 * trailing comment. */
#define GD_MAJOR_VERSION 2      /*version605b5d1778*/
#define GD_MINOR_VERSION 4      /*version605b5d1778*/
#define GD_RELEASE_VERSION 0    /*version605b5d1778*/
#define GD_EXTRA_VERSION "-dev" /*version605b5d1778*/

/* End parsable section. */

/* The version string.  This is constructed from the version number
 * parts above via macro abuse^Wtrickery. */
#define GDXXX_VERSION_STR(mjr, mnr, rev, ext) mjr "." mnr "." rev ext
#define GDXXX_STR(s) GDXXX_SSTR(s) /* Two levels needed to expand args. */
#define GDXXX_SSTR(s) #s

#define GD_VERSION_STRING                                                                          \
    GDXXX_VERSION_STR(GDXXX_STR(GD_MAJOR_VERSION), GDXXX_STR(GD_MINOR_VERSION),                    \
                      GDXXX_STR(GD_RELEASE_VERSION), GD_EXTRA_VERSION)

/* Do the DLL dance: dllexport when building the DLL,
   dllimport when importing from it, nothing when
   not on Silly Silly Windows (tm Aardman Productions). */

/* 2.0.20: for headers */

/* 2.0.24: __stdcall also needed for Visual BASIC
   and other languages. This breaks ABI compatibility
   with previous DLL revs, but it's necessary. */

/* 2.0.29: WIN32 programmers can declare the NONDLL macro if they
   wish to build gd as a static library or by directly including
   the gd sources in a project. */

/* http://gcc.gnu.org/wiki/Visibility */
#if defined(_WIN32) || defined(CYGWIN) || defined(_WIN32_WCE)
#ifdef BGDWIN32
#ifdef NONDLL
#define BGD_EXPORT_DATA_PROT
#else
#ifdef __GNUC__
#define BGD_EXPORT_DATA_PROT __attribute__((__dllexport__))
#else
#define BGD_EXPORT_DATA_PROT __declspec(dllexport)
#endif
#endif
#else
#ifdef __GNUC__
#define BGD_EXPORT_DATA_PROT __attribute__((__dllimport__))
#else
#define BGD_EXPORT_DATA_PROT __declspec(dllimport)
#endif
#endif
#define BGD_STDCALL __stdcall
#define BGD_EXPORT_DATA_IMPL
#define BGD_MALLOC
#else
#if defined(__GNUC__) || defined(__clang__)
#define BGD_EXPORT_DATA_PROT __attribute__((__visibility__("default")))
#define BGD_EXPORT_DATA_IMPL __attribute__((__visibility__("hidden")))
#else
#define BGD_EXPORT_DATA_PROT
#define BGD_EXPORT_DATA_IMPL
#endif
#define BGD_STDCALL
#define BGD_MALLOC __attribute__((__malloc__))
#endif

#define BGD_DECLARE(rt) BGD_EXPORT_DATA_PROT rt BGD_STDCALL

/* VS2012+ disable keyword macroizing unless _ALLOW_KEYWORD_MACROS is set
   We define inline, and strcasecmp if they're missing
*/
#ifdef _MSC_VER
#define _ALLOW_KEYWORD_MACROS
#ifndef inline
#define inline __inline
#endif
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#endif

#undef ARG_NOT_USED
#define ARG_NOT_USED(arg) (void)arg

/* gd.h: declarations file for the graphic-draw module.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  This software is provided "AS IS." Thomas Boutell and
 * Boutell.Com, Inc. disclaim all warranties, either express or implied,
 * including but not limited to implied warranties of merchantability and
 * fitness for a particular purpose, with respect to this code and accompanying
 * documentation. */

/* stdio is needed for file I/O. */
#include "gd_io.h"
#include <stdarg.h>
#include <stdio.h>

/* The maximum number of palette entries in palette-based images.
   In the wonderful new world of gd 2.0, you can of course have
   many more colors when using truecolor mode. */

#define gdMaxColors 256

/* Image type. See functions below; you will not need to change
   the elements directly. Use the provided macros to
   access sx, sy, the color table, and colorsTotal for
   read-only purposes. */

/* If 'truecolor' is set true, the image is truecolor;
   pixels are represented by integers, which
   must be 32 bits wide or more.

   True colors are represented as follows:

   ARGB

   Where 'A' (alpha channel) occupies only the
   LOWER 7 BITS of the MSB. This very small
   loss of alpha channel resolution allows gd 2.x
   to keep backwards compatibility by allowing
   signed integers to be used to represent colors,
   and negative numbers to represent special cases,
   just as in gd 1.x. */

#define gdAlphaMax 127
#define gdAlphaOpaque 0
#define gdAlphaTransparent 127
#define gdRedMax 255
#define gdGreenMax 255
#define gdBlueMax 255

/**
 * Group: Color Decomposition
 */

/**
 * Macro: gdTrueColorGetAlpha
 *
 * Gets the alpha channel value
 *
 * Parameters:
 *   c - The color
 *
 * See also:
 *   - <gdTrueColorAlpha>
 */
#define gdTrueColorGetAlpha(c) (((c) & 0x7F000000) >> 24)

/**
 * Macro: gdTrueColorGetRed
 *
 * Gets the red channel value
 *
 * Parameters:
 *   c - The color
 *
 * See also:
 *   - <gdTrueColorAlpha>
 */
#define gdTrueColorGetRed(c) (((c) & 0xFF0000) >> 16)

/**
 * Macro: gdTrueColorGetGreen
 *
 * Gets the green channel value
 *
 * Parameters:
 *   c - The color
 *
 * See also:
 *   - <gdTrueColorAlpha>
 */
#define gdTrueColorGetGreen(c) (((c) & 0x00FF00) >> 8)

/**
 * Macro: gdTrueColorGetBlue
 *
 * Gets the blue channel value
 *
 * Parameters:
 *   c - The color
 *
 * See also:
 *   - <gdTrueColorAlpha>
 */
#define gdTrueColorGetBlue(c) ((c) & 0x0000FF)

/**
 * Group: Effects
 *
 * The layering effect
 *
 * When pixels are drawn the new colors are "mixed" with the background
 * depending on the effect.
 *
 * Note that the effect does not apply to palette images, where pixels
 * are always replaced.
 *
 * Modes:
 *   gdEffectReplace    - replace pixels
 *   gdEffectAlphaBlend - blend pixels, see <gdAlphaBlend>
 *   gdEffectNormal     - default mode; same as gdEffectAlphaBlend
 *   gdEffectOverlay    - overlay pixels, see <gdLayerOverlay>
 *   gdEffectMultiply   - overlay pixels with multiply effect, see
 *                        <gdLayerMultiply>
 *
 * See also:
 *   - <gdImageAlphaBlending>
 */
#define gdEffectReplace 0
#define gdEffectAlphaBlend 1
#define gdEffectNormal 2
#define gdEffectOverlay 3
#define gdEffectMultiply 4

#define GD_TRUE 1
#define GD_FALSE 0

#define GD_EPSILON 1e-6
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* This function accepts truecolor pixel values only. The
   source color is composited with the destination color
   based on the alpha channel value of the source color.
   The resulting color is opaque. */

/**
 * @brief Blend two colors
 *
 * This function accepts truecolor pixel values only. The
 * source color is composited with the destination color
 * based on the alpha channel value of the source color.
 * The resulting color is opaque.
 * @param dst The color to blend onto.
 * @param src The color to blend.
 *
 * @see gdImageAlphaBlending gdLayerOverlay gdLayerMultiply
 */
BGD_DECLARE(int) gdAlphaBlend(int dest, int src);

/**
 * @brief Overlay two colors
 *
 * @param dst The color to overlay onto.
 * @param src The color to overlay.
 *
 * @return The resulting color.
 * 
 * @see gdImageAlphaBlending gdAlphaBlend gdLayerMultiply
 */
BGD_DECLARE(int) gdLayerOverlay(int dest, int src);

/**
 * @brief Overlay two colors with multiply effect
 *
 * @param dst The color to overlay onto.
 * @param src The color to overlay.
 *
 * @return The resulting color.
 * 
 * @see gdImageAlphaBlending  gdAlphaBlend gdLayerOverlay
 */
BGD_DECLARE(int) gdLayerMultiply(int dest, int src);

/**
 * @addtogroup TransformScaleRotate Transform, scale and rotate
 * @{
 */
/**
* @brief Let @ref gdImageScaleWithOptions choose the interpolation method from the scale
 * direction.
 *
 * Automatic selection uses @ref GD_LANCZOS3 for downscales or mixed-axis scales,
 * and @ref GD_CATMULLROM for pure upscales.
 */
#define GD_SCALE_INTERPOLATION_AUTO -1

/**
 * @brief gdInterpolationMethod
 *
 * Interpolation kernels used by image scaling, rotation and affine
 * transformation functions. Newly-created images use @ref GD_BILINEAR_FIXED
 * by default. Call @ref gdImageSetInterpolationMethod on the source image before
 * using APIs that read the image's current interpolation method.
 *
 * @ref gdImageScaleWithOptions can either use one of these values explicitly or
 * use @ref GD_SCALE_INTERPOLATION_AUTO to choose a method from the requested
 * scale direction.
 * @note
 *   @ref GD_WEIGHTED4 is not supported by @ref gdImageScale. For downscales or
 *   mixed-axis scales, @ref gdImageScale maps the fixed compatibility methods
 *   (@ref GD_DEFAULT, @ref GD_BILINEAR_FIXED, @ref GD_LINEAR, @ref GD_BICUBIC_FIXED and
 *   @ref GD_BICUBIC) to @ref GD_TRIANGLE to avoid the blur and aliasing of the old
 *   fixed scalers.
 *
 * @see gdImageSetInterpolationMethod gdImageScale gdImageScaleWithOptions gdImageRotateInterpolated gdTransformAffineCopy
 */
typedef enum {
    GD_DEFAULT = 0, /**< Compatibility default. Setting this resolves to @ref GD_LINEAR */
    GD_BELL,        /**< Bell filter. */
    GD_BESSEL,      /**< Bessel filter. */
    GD_BILINEAR_FIXED, /**< Compatibility bilinear scaler. */
    GD_BICUBIC,     /**< Bicubic interpolation. */
    GD_BICUBIC_FIXED, /**< Compatibility bicubic scaler. */
    GD_BLACKMAN,    /**< Blackman filter. */
    GD_BOX,         /**< Box filter. */
    GD_BSPLINE,     /**< B-spline filter. */
    GD_CATMULLROM,  /**< Catmull-Rom filter. */
    GD_GAUSSIAN,    /**< Gaussian filter. */
    GD_GENERALIZED_CUBIC, /**< Generalized cubic filter. */
    GD_HERMITE,     /**< Hermite filter. */
    GD_HAMMING,     /**< Hamming filter. */
    GD_HANNING,     /**< Hanning filter. */
    GD_MITCHELL,    /**< Mitchell filter. */
    GD_NEAREST_NEIGHBOUR, /**< Nearest-neighbour interpolation. */
    GD_POWER,     /**< Power filter. */
    GD_QUADRATIC, /**< Quadratic filter. */
    GD_SINC,      /**< Sinc filter. */
    GD_TRIANGLE,  /**< Triangle filter. */
    GD_WEIGHTED4, /**< Four-pixel weighted interpolation for rotation and affine sampling. */
    GD_LINEAR,    /**< Bilinear interpolation. */
    GD_LANCZOS3,  /**< Lanczos filter with radius 3. */
    GD_LANCZOS8,  /**< Lanczos filter with radius 8. */
    GD_BLACKMAN_BESSEL, /**< Blackman-windowed Bessel filter. */
    GD_BLACKMAN_SINC,   /**< Blackman-windowed sinc filter. */
    GD_QUADRATIC_BSPLINE, /**< Quadratic B-spline filter. */
    GD_CUBIC_SPLINE,      /**< Cubic spline filter. */
    GD_COSINE,         /**< Cosine filter. */
    GD_WELSH,       /**< Welsh filter. */
    GD_METHOD_COUNT = 30
} gdInterpolationMethod;

/* Interpolation function ptr */
typedef double (*interpolation_method)(double, double);
/** @} */

/*
   Group: Types

   typedef: gdImage

   typedef: gdImagePtr

   The data structure in which gd stores images. <gdImageCreate>,
   <gdImageCreateTrueColor> and the various image file-loading functions
   return a pointer to this type, and the other functions expect to
   receive a pointer to this type as their first argument.

   *gdImagePtr* is a pointer to *gdImage*.

   See also:
         <Accessor Macros>

   (Previous versions of this library encouraged directly manipulating
   the contents of the struct.
   We are attempting to move away from this practice so the fields
   will be considered private in 2.5 and later. )
*/
typedef struct gdImageStruct {
    /* Palette-based image pixels */
    unsigned char **pixels;
    int sx;
    int sy;
    /* These are valid in palette images only. See also
       'alpha', which appears later in the structure to
       preserve binary backwards compatibility */
    int colorsTotal;
    int red[gdMaxColors];
    int green[gdMaxColors];
    int blue[gdMaxColors];
    int open[gdMaxColors];
    /* For backwards compatibility, this is set to the
       first palette entry with 100% transparency,
       and is also set and reset by the
       gdImageColorTransparent function. Newer
       applications can allocate palette entries
       with any desired level of transparency; however,
       bear in mind that many viewers, notably
       many web browsers, fail to implement
       full alpha channel for PNG and provide
       support for full opacity or transparency only. */
    int transparent;
    int *polyInts;
    int polyAllocated;
    struct gdImageStruct *brush;
    struct gdImageStruct *tile;
    int brushColorMap[gdMaxColors];
    int tileColorMap[gdMaxColors];
    int styleLength;
    int stylePos;
    int *style;
    int interlace;
    /* New in 2.0: thickness of line. Initialized to 1. */
    int thick;
    /* New in 2.0: alpha channel for palettes. Note that only
       Macintosh Internet Explorer and (possibly) Netscape 6
       really support multiple levels of transparency in
       palettes, to my knowledge, as of 2/15/01. Most
       common browsers will display 100% opaque and
       100% transparent correctly, and do something
       unpredictable and/or undesirable for levels
       in between. TBB */
    int alpha[gdMaxColors];
    /* Truecolor flag and pixels. New 2.0 fields appear here at the
       end to minimize breakage of existing object code. */
    int trueColor;
    int **tpixels;
    /* Should alpha channel be copied, or applied, each time a
       pixel is drawn? This applies to truecolor images only.
       No attempt is made to alpha-blend in palette images,
       even if semitransparent palette entries exist.
       To do that, build your image as a truecolor image,
       then quantize down to 8 bits. */
    int alphaBlendingFlag;
    /* Should the alpha channel of the image be saved? This affects
       PNG at the moment; other future formats may also
       have that capability. JPEG doesn't. */
    int saveAlphaFlag;

    /* There should NEVER BE ACCESSOR MACROS FOR ITEMS BELOW HERE, so this
       part of the structure can be safely changed in new releases. */

    /* 2.0.12: anti-aliased globals. 2.0.26: just a few vestiges after
      switching to the fast, memory-cheap implementation from PHP-gd. */
    int AA;
    int AA_color;
    int AA_dont_blend;

    /* 2.0.12: simple clipping rectangle. These values
      must be checked for safety when set; please use
      gdImageSetClip */
    int cx1;
    int cy1;
    int cx2;
    int cy2;

    /* 2.1.0: allows to specify resolution in dpi */
    unsigned int res_x;
    unsigned int res_y;

    /* Selects quantization method, see gdImageTrueColorToPaletteSetMethod() and
     * gdPaletteQuantizationMethod enum. */
    int paletteQuantizationMethod;
    /* speed/quality trade-off. 1 = best quality, 10 = best speed. 0 =
       method-specific default. Applicable to GD_QUANT_LIQ and
       GD_QUANT_NEUQUANT. */
    int paletteQuantizationSpeed;
    /* Image will remain true-color if conversion to palette cannot achieve
       given quality. Value from 1 to 100, 1 = ugly, 100 = perfect. Applicable
       to GD_QUANT_LIQ.*/
    int paletteQuantizationMinQuality;
    /* Image will use minimum number of palette colors needed to achieve given
       quality. Must be higher than paletteQuantizationMinQuality Value from 1
       to 100, 1 = ugly, 100 = perfect. Applicable to GD_QUANT_LIQ.*/
    int paletteQuantizationMaxQuality;
    gdInterpolationMethod interpolation_id;
    interpolation_method interpolation;
} gdImage;

typedef gdImage *gdImagePtr;

typedef struct gdImageMetadata gdImageMetadata;

#define GD_META_OK 0
#define GD_META_ERR_FORMAT -1
#define GD_META_ERR_PARSE -2
#define GD_META_ERR_NOMEM -3
#define GD_META_ERR_LIMIT -4
#define GD_META_ERR_UNSUPPORTED -5
#define GD_META_ERR_INVALID -6

#define GD_METADATA_DEFAULT_MAX_PROFILE_SIZE ((size_t)64 * 1024 * 1024)
#define GD_METADATA_DEFAULT_MAX_TOTAL_SIZE ((size_t)256 * 1024 * 1024)

BGD_DECLARE(gdImageMetadata *) gdImageMetadataCreate(void);
BGD_DECLARE(void) gdImageMetadataFree(gdImageMetadata *metadata);
BGD_DECLARE(void) gdImageMetadataReset(gdImageMetadata *metadata);
BGD_DECLARE(int)
gdImageMetadataSetLimits(gdImageMetadata *metadata, size_t max_profile_size, size_t max_total_size);
BGD_DECLARE(void)
gdImageMetadataGetLimits(const gdImageMetadata *metadata, size_t *max_profile_size,
                         size_t *max_total_size);
BGD_DECLARE(int)
gdImageMetadataSetProfile(gdImageMetadata *metadata, const char *key, const unsigned char *data,
                          size_t size);
BGD_DECLARE(const unsigned char *)
gdImageMetadataGetProfile(const gdImageMetadata *metadata, const char *key, size_t *size);
BGD_DECLARE(int)
gdImageMetadataRemoveProfile(gdImageMetadata *metadata, const char *key);
BGD_DECLARE(size_t)
gdImageMetadataGetProfileCount(const gdImageMetadata *metadata);
BGD_DECLARE(int)
gdImageMetadataGetProfileAt(const gdImageMetadata *metadata, size_t index, const char **key,
                            const unsigned char **data, size_t *size);

/* Point type for use in polygon drawing. */

/**
 * Group: Types
 *
 * typedef: gdPointF
 *  Defines a point in a 2D coordinate system using floating point
 *  values.
 * x - Floating point position (increase from left to right)
 * y - Floating point Row position (increase from top to bottom)
 *
 * typedef: gdPointFPtr
 *  Pointer to a <gdPointF>
 *
 * See also:
 *  <gdImageCreate>, <gdImageCreateTrueColor>,
 **/
typedef struct {
    double x, y;
} gdPointF, *gdPointFPtr;

/*
  Group: Types

  typedef: gdFont

  typedef: gdFontPtr

  A font structure, containing the bitmaps of all characters in a
  font.  Used to declare the characteristics of a font. Text-output
  functions expect these as their second argument, following the
  <gdImagePtr> argument.  <gdFontGetSmall> and <gdFontGetLarge> both
  return one.

  You can provide your own font data by providing such a structure and
  the associated pixel array. You can determine the width and height
  of a single character in a font by examining the w and h members of
  the structure. If you will not be creating your own fonts, you will
  not need to concern yourself with the rest of the components of this
  structure.

  Please see the files gdfontl.c and gdfontl.h for an example of
  the proper declaration of this structure.

  > typedef struct {
  >   // # of characters in font
  >   int nchars;
  >   // First character is numbered... (usually 32 = space)
  >   int offset;
  >   // Character width and height
  >   int w;
  >   int h;
  >   // Font data; array of characters, one row after another.
  >   // Easily included in code, also easily loaded from
  >   // data files.
  >   char *data;
  > } gdFont;

  gdFontPtr is a pointer to gdFont.

*/
typedef struct {
    /* # of characters in font */
    int nchars;
    /* First character is numbered... (usually 32 = space) */
    int offset;
    /* Character width and height */
    int w;
    int h;
    /* Font data; array of characters, one row after another.
       Easily included in code, also easily loaded from
       data files. */
    char *data;
} gdFont;

/* Text functions take these. */
typedef gdFont *gdFontPtr;

typedef void (*gdErrorMethod)(int, const char *, va_list);

BGD_DECLARE(void) gdSetErrorMethod(gdErrorMethod);
BGD_DECLARE(void) gdClearErrorMethod(void);

/**
 * Group: Colors
 *
 * Colors are always of type int which is supposed to be at least 32 bit large.
 *
 * Kinds of colors:
 *   true colors     - ARGB values where the alpha channel is stored as most
 *                     significant, and the blue channel as least significant
 *                     byte. Note that the alpha channel only uses the 7 least
 *                     significant bits.
 *                     Don't rely on the internal representation, though, and
 *                     use <gdTrueColorAlpha> to compose a truecolor value, and
 *                     <gdTrueColorGetAlpha>, <gdTrueColorGetRed>,
 *                     <gdTrueColorGetGreen> and <gdTrueColorGetBlue> to access
 *                     the respective channels.
 *   palette indexes - The index of a color palette entry (0-255).
 *   special colors  - As listed in the following section.
 *
 * Constants: Special Colors
 *   gdStyled        - use the current style, see <gdImageSetStyle>
 *   gdBrushed       - use the current brush, see <gdImageSetBrush>
 *   gdStyledBrushed - use the current style and brush
 *   gdTiled         - use the current tile, see <gdImageSetTile>
 *   gdTransparent   - indicate transparency, what is not the same as the
 *                     transparent color index; used for lines only
 *   gdAntiAliased   - draw anti aliased
 */

/* For backwards compatibility only. Use gdImageSetStyle()
   for MUCH more flexible line drawing. Also see
   gdImageSetBrush(). */
#define gdDashSize 4
#define gdStyled (-2)
#define gdBrushed (-3)
#define gdStyledBrushed (-4)
#define gdTiled (-5)

/* NOT the same as the transparent color index.
        This is used in line styles only. */
#define gdTransparent (-6)

#define gdAntiAliased (-7)

/* Functions to manipulate images. */

/* Creates a palette-based image (up to 256 colors). */
BGD_DECLARE(gdImagePtr) gdImageCreate(int sx, int sy);

/* An alternate name for the above (2.0). */
#define gdImageCreatePalette gdImageCreate

/* Creates a truecolor image (millions of colors). */
BGD_DECLARE(gdImagePtr) gdImageCreateTrueColor(int sx, int sy);

/* Creates an image from various file types. These functions
   return a palette or truecolor image based on the
   nature of the file being loaded. Truecolor PNG
   stays truecolor; palette PNG stays palette-based;
   JPEG is always truecolor. */
/* PNG */
BGD_DECLARE(gdImagePtr) gdImageCreateFromPng(FILE *fd);
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngCtx(gdIOCtxPtr in);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromPngCtxWithMetadata(gdIOCtxPtr in, gdImageMetadata *metadata);
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngPtr(int size, void *data);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromPngPtrWithMetadata(int size, void *data, gdImageMetadata *metadata);
BGD_DECLARE(void) gdImagePng(gdImagePtr im, FILE *out);
BGD_DECLARE(void) gdImagePngCtx(gdImagePtr im, gdIOCtxPtr out);
/* 2.0.12: Compression level: 0-9 or -1, where 0 is NO COMPRESSION at all,
   1 is FASTEST but produces larger files, 9 provides the best
   compression (smallest files) but takes a long time to compress, and
   -1 selects the default compiled into the zlib library. */
BGD_DECLARE(void) gdImagePngEx(gdImagePtr im, FILE *out, int level);
BGD_DECLARE(void) gdImagePngCtxEx(gdImagePtr im, gdIOCtxPtr out, int level);
BGD_DECLARE(void)
gdImagePngCtxWithMetadata(gdImagePtr im, gdIOCtxPtr out, const gdImageMetadata *metadata);
BGD_DECLARE(void)
gdImagePngCtxExWithMetadata(gdImagePtr im, gdIOCtxPtr out, int level,
                            const gdImageMetadata *metadata);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImagePngPtr(gdImagePtr im, int *size);
BGD_DECLARE(void *) gdImagePngPtrEx(gdImagePtr im, int *size, int level);
BGD_DECLARE(void *)
gdImagePngPtrWithMetadata(gdImagePtr im, int *size, const gdImageMetadata *metadata);
BGD_DECLARE(void *)
gdImagePngPtrExWithMetadata(gdImagePtr im, int *size, int level, const gdImageMetadata *metadata);
BGD_DECLARE(int)
gdImageMetadataInjectPng(void **data, int *size, const gdImageMetadata *metadata);

#define GD_PNG_FILTER_AUTO 0U
#define GD_PNG_FILTER_NONE (1U << 0)
#define GD_PNG_FILTER_SUB (1U << 1)
#define GD_PNG_FILTER_UP (1U << 2)
#define GD_PNG_FILTER_AVERAGE (1U << 3)
#define GD_PNG_FILTER_PAETH (1U << 4)
#define GD_PNG_FILTER_ALL                                                                          \
    (GD_PNG_FILTER_NONE | GD_PNG_FILTER_SUB | GD_PNG_FILTER_UP | GD_PNG_FILTER_AVERAGE |           \
     GD_PNG_FILTER_PAETH)

enum {
    GD_PNG_COMPRESSION_STRATEGY_DEFAULT = 0,
    GD_PNG_COMPRESSION_STRATEGY_FILTERED,
    GD_PNG_COMPRESSION_STRATEGY_HUFFMAN_ONLY,
    GD_PNG_COMPRESSION_STRATEGY_RLE,
    GD_PNG_COMPRESSION_STRATEGY_FIXED
};

typedef struct {
    size_t struct_size;
    int compression_level;
    unsigned int filters;
    int compression_strategy;
    const gdImageMetadata *metadata;
} gdPngWriteOptions;

typedef struct {
    size_t struct_size;
    int width;
    int height;
    int bit_depth;
    int color_type;
    int has_alpha;
    int has_transparency;
    int palette_entries;
    int interlace_method;
    int x_pixels_per_unit;
    int y_pixels_per_unit;
    int physical_unit;
    gdImageMetadata *metadata;
    int decoded_truecolor;
} gdPngInfo;

BGD_DECLARE(void) gdPngWriteOptionsInit(gdPngWriteOptions *options);
BGD_DECLARE(void) gdPngInfoInit(gdPngInfo *info);
BGD_DECLARE(int) gdImagePngWithOptions(gdImagePtr im, FILE *out, const gdPngWriteOptions *options);
BGD_DECLARE(int)
gdImagePngCtxWithOptions(gdImagePtr im, gdIOCtxPtr out, const gdPngWriteOptions *options);
BGD_DECLARE(void *)
gdImagePngPtrWithOptions(gdImagePtr im, int *size, const gdPngWriteOptions *options);
BGD_DECLARE(int) gdPngGetInfo(FILE *in, gdPngInfo *info);
BGD_DECLARE(int) gdPngGetInfoCtx(gdIOCtxPtr in, gdPngInfo *info);
BGD_DECLARE(int) gdPngGetInfoPtr(int size, const void *data, gdPngInfo *info);
BGD_DECLARE(const char *) gdPngGetVersionString(void);

/* QOI */
BGD_DECLARE(gdImagePtr) gdImageCreateFromQoi(FILE *fd);
BGD_DECLARE(gdImagePtr) gdImageCreateFromQoiCtx(gdIOCtxPtr in);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromQoiCtxWithMetadata(gdIOCtxPtr in, gdImageMetadata *metadata);
BGD_DECLARE(gdImagePtr) gdImageCreateFromQoiPtr(int size, void *data);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromQoiPtrWithMetadata(int size, void *data, gdImageMetadata *metadata);
BGD_DECLARE(void *) gdImageQoiPtr(gdImagePtr im, int *size);
BGD_DECLARE(void *) gdImageQoiPtrEx(gdImagePtr im, int *size, int colorspace);
BGD_DECLARE(void *)
gdImageQoiPtrWithMetadata(gdImagePtr im, int *size, const gdImageMetadata *metadata);
BGD_DECLARE(void *)
gdImageQoiPtrExWithMetadata(gdImagePtr im, int *size, int colorspace,
                            const gdImageMetadata *metadata);
BGD_DECLARE(int) gdImageMetadataInjectQoi(void **data, int *size, const gdImageMetadata *metadata);

BGD_DECLARE(void) gdImageQoi(gdImagePtr im, FILE *out);
BGD_DECLARE(void) gdImageQoiCtx(gdImagePtr im, gdIOCtxPtr out);
BGD_DECLARE(void)
gdImageQoiCtxWithMetadata(gdImagePtr im, gdIOCtxPtr out, const gdImageMetadata *metadata);
enum { GD_QOI_SRGB = 0, GD_QOI_LINEAR = 1 };

BGD_DECLARE(void) gdImageQoi(gdImagePtr im, FILE *out);
BGD_DECLARE(void) gdImageQoiCtx(gdImagePtr im, gdIOCtxPtr out);
BGD_DECLARE(void)
gdImageQoiCtxWithMetadata(gdImagePtr im, gdIOCtxPtr out, const gdImageMetadata *metadata);
BGD_DECLARE(void) gdImageQoiEx(gdImagePtr im, FILE *out, int colorspace);
BGD_DECLARE(void)
gdImageQoiCtxEx(gdImagePtr im, gdIOCtxPtr out, int colorspace);
BGD_DECLARE(void)
gdImageQoiCtxExWithMetadata(gdImagePtr im, gdIOCtxPtr out, int colorspace,
                            const gdImageMetadata *metadata);
/* GIF */
/* These read the first frame only */
BGD_DECLARE(gdImagePtr) gdImageCreateFromGif(FILE *fd);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGifCtx(gdIOCtxPtr in);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGifPtr(int size, void *data);
BGD_DECLARE(void) gdImageGifCtx(gdImagePtr im, gdIOCtxPtr out);
BGD_DECLARE(void) gdImageGif(gdImagePtr im, FILE *out);
/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImageGifPtr(gdImagePtr im, int *size);

/* Anims, frames or compositions */
typedef struct gdGifReadStruct *gdGifReadPtr;

typedef struct {
    int width;
    int height;
    int backgroundIndex;
    int globalColorTable;
    int loopCount;
} gdGifInfo;

typedef struct {
    int frameIndex;
    int x;
    int y;
    int width;
    int height;
    int delay;
    int disposal;
    int transparentIndex;
    int localColorTable;
    int interlace;
} gdGifFrameInfo;

BGD_DECLARE(int) gdGifIsAnimated(FILE *fd);
BGD_DECLARE(int) gdGifIsAnimatedCtx(gdIOCtxPtr in);
BGD_DECLARE(int) gdGifIsAnimatedPtr(int size, void *data);
BGD_DECLARE(gdGifReadPtr) gdGifReadOpen(FILE *fd);
BGD_DECLARE(gdGifReadPtr) gdGifReadOpenCtx(gdIOCtxPtr in);
BGD_DECLARE(gdGifReadPtr) gdGifReadOpenPtr(int size, void *data);
BGD_DECLARE(void) gdGifReadClose(gdGifReadPtr gif);
BGD_DECLARE(int) gdGifReadGetInfo(gdGifReadPtr gif, gdGifInfo *info);
BGD_DECLARE(int)
gdGifReadNextFrame(gdGifReadPtr gif, gdGifFrameInfo *info, gdImagePtr *frame);
BGD_DECLARE(int)
gdGifReadNextImage(gdGifReadPtr gif, gdGifFrameInfo *info, gdImagePtr *image);
BGD_DECLARE(gdImagePtr) gdGifReadCloneImage(gdGifReadPtr gif);

/**
 * Group: GifAnim
 *
 *   Legal values for Disposal. gdDisposalNone is always used by
 *   the built-in optimizer if previm is passed.
 *
 * Constants: gdImageGifAnim
 *
 *   gdDisposalUnknown              - Not recommended
 *   gdDisposalNone                 - Preserve previous frame
 *   gdDisposalRestoreBackground    - First allocated color of palette
 *   gdDisposalRestorePrevious      - Restore to before start of frame
 *
 * See also:
 *   - <gdImageGifAnimAdd>
 */
enum { gdDisposalUnknown, gdDisposalNone, gdDisposalRestoreBackground, gdDisposalRestorePrevious };

#define GD_GIF_DISPOSAL_UNKNOWN gdDisposalUnknown
#define GD_GIF_DISPOSAL_NONE gdDisposalNone
#define GD_GIF_DISPOSAL_RESTORE_BACKGROUND gdDisposalRestoreBackground
#define GD_GIF_DISPOSAL_RESTORE_PREVIOUS gdDisposalRestorePrevious

BGD_DECLARE(void)
gdImageGifAnimBegin(gdImagePtr im, FILE *outFile, int GlobalCM, int Loops);
BGD_DECLARE(void)
gdImageGifAnimAdd(gdImagePtr im, FILE *outFile, int LocalCM, int LeftOfs, int TopOfs, int Delay,
                  int Disposal, gdImagePtr previm);
BGD_DECLARE(void) gdImageGifAnimEnd(FILE *outFile);
BGD_DECLARE(void)
gdImageGifAnimBeginCtx(gdImagePtr im, gdIOCtxPtr out, int GlobalCM, int Loops);
BGD_DECLARE(void)
gdImageGifAnimAddCtx(gdImagePtr im, gdIOCtxPtr out, int LocalCM, int LeftOfs, int TopOfs, int Delay,
                     int Disposal, gdImagePtr previm);
BGD_DECLARE(void) gdImageGifAnimEndCtx(gdIOCtxPtr out);
BGD_DECLARE(void *)
gdImageGifAnimBeginPtr(gdImagePtr im, int *size, int GlobalCM, int Loops);
BGD_DECLARE(void *)
gdImageGifAnimAddPtr(gdImagePtr im, int *size, int LocalCM, int LeftOfs, int TopOfs, int Delay,
                     int Disposal, gdImagePtr previm);
BGD_DECLARE(void *) gdImageGifAnimEndPtr(int *size);

/* WBMP */
BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMP(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMPCtx(gdIOCtxPtr infile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromWBMPPtr(int size, void *data);

/* JPEG */
enum {
    GD_JPEG_COLOR_SPACE_UNKNOWN = 0,
    GD_JPEG_COLOR_SPACE_GRAYSCALE = 1,
    GD_JPEG_COLOR_SPACE_RGB = 2,
    GD_JPEG_COLOR_SPACE_YCBCR = 3,
    GD_JPEG_COLOR_SPACE_CMYK = 4,
    GD_JPEG_COLOR_SPACE_YCCK = 5
};

enum {
    GD_JPEG_DENSITY_UNIT_NONE = 0,
    GD_JPEG_DENSITY_UNIT_DPI = 1,
    GD_JPEG_DENSITY_UNIT_DPCM = 2
};

enum {
    GD_JPEG_DCT_DEFAULT = 0,
    GD_JPEG_DCT_SLOW = 1,
    GD_JPEG_DCT_FAST = 2,
    GD_JPEG_DCT_FLOAT = 3
};

typedef struct {
    size_t struct_size;
    int width;
    int height;
    int bits_per_sample;
    int components;
    int color_space;
    int progressive;
    int density_unit;
    int x_density;
    int y_density;
    int has_exif;
    int has_xmp;
    int has_icc;
    int has_iptc;
} gdJpegInfo;

typedef struct {
    size_t struct_size;
    int ignore_warning;
    unsigned int scale_num;
    unsigned int scale_denom;
    int dct_method;
} gdJpegReadOptions;

typedef struct {
    size_t struct_size;
    int quality;
    int progressive;
    int force_no_subsampling;
    const gdImageMetadata *metadata;
} gdJpegWriteOptions;

BGD_DECLARE(void) gdJpegInfoInit(gdJpegInfo *info);
BGD_DECLARE(void) gdJpegReadOptionsInit(gdJpegReadOptions *options);
BGD_DECLARE(void) gdJpegWriteOptionsInit(gdJpegWriteOptions *options);
BGD_DECLARE(int) gdJpegGetInfo(FILE *infile, gdJpegInfo *info);
BGD_DECLARE(int) gdJpegGetInfoCtx(gdIOCtxPtr infile, gdJpegInfo *info);
BGD_DECLARE(int) gdJpegGetInfoPtr(int size, const void *data, gdJpegInfo *info);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpeg(FILE *infile);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegEx(FILE *infile, int ignore_warning);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegCtx(gdIOCtxPtr infile);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegCtxEx(gdIOCtxPtr infile, int ignore_warning);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegCtxWithMetadata(gdIOCtxPtr infile, gdImageMetadata *metadata);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegCtxExWithMetadata(gdIOCtxPtr infile, int ignore_warning,
                                       gdImageMetadata *metadata);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegCtxWithOptions(gdIOCtxPtr infile, const gdJpegReadOptions *options);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJpegPtr(int size, void *data);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegPtrEx(int size, void *data, int ignore_warning);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegPtrWithOptions(int size, void *data, const gdJpegReadOptions *options);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegPtrWithMetadata(int size, void *data, gdImageMetadata *metadata);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromJpegPtrExWithMetadata(int size, void *data, int ignore_warning,
                                       gdImageMetadata *metadata);
BGD_DECLARE(const char *) gdJpegGetVersionString();

/* WEBP */
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebp(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpPtr(int size, void *data);
BGD_DECLARE(gdImagePtr) gdImageCreateFromWebpCtx(gdIOCtxPtr infile);

typedef struct gdWebpReadStruct *gdWebpReadPtr;
typedef struct gdWebpWriteStruct *gdWebpWritePtr;

typedef struct {
    int width;
    int height;
    int frameCount;
    int loopCount;
    int backgroundColor;
    int formatFlags;
} gdWebpInfo;

typedef struct {
    int frameIndex;
    int x;
    int y;
    int width;
    int height;
    int duration;
    int timestamp;
    int dispose;
    int blend;
    int hasAlpha;
    int complete;
} gdWebpFrameInfo;

typedef struct {
    int canvasWidth;
    int canvasHeight;
    int loopCount;
    int backgroundColor;
    int quality;
    int lossless;
    int method;
    int minimizeSize;
    int kmin;
    int kmax;
    int allowMixed;
} gdWebpWriteOptions;

enum { gdWebpDisposeNone, gdWebpDisposeBackground };
enum { gdWebpBlendAlpha, gdWebpBlendNone };

#define GD_WEBP_DISPOSE_NONE gdWebpDisposeNone
#define GD_WEBP_DISPOSE_BACKGROUND gdWebpDisposeBackground
#define GD_WEBP_BLEND_ALPHA gdWebpBlendAlpha
#define GD_WEBP_BLEND_NONE gdWebpBlendNone

BGD_DECLARE(int) gdWebpIsAnimated(FILE *fd);
BGD_DECLARE(int) gdWebpIsAnimatedCtx(gdIOCtxPtr in);
BGD_DECLARE(int) gdWebpIsAnimatedPtr(int size, void *data);
BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpen(FILE *fd);
BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpenCtx(gdIOCtxPtr in);
BGD_DECLARE(gdWebpReadPtr) gdWebpReadOpenPtr(int size, void *data);
BGD_DECLARE(void) gdWebpReadClose(gdWebpReadPtr webp);
BGD_DECLARE(int) gdWebpReadGetInfo(gdWebpReadPtr webp, gdWebpInfo *info);
BGD_DECLARE(int)
gdWebpReadNextFrame(gdWebpReadPtr webp, gdWebpFrameInfo *info, gdImagePtr *frame);
BGD_DECLARE(int)
gdWebpReadNextImage(gdWebpReadPtr webp, gdWebpFrameInfo *info, gdImagePtr *image);
BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpen(FILE *outFile, const gdWebpWriteOptions *options);
BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpenCtx(gdIOCtxPtr out, const gdWebpWriteOptions *options);
BGD_DECLARE(gdWebpWritePtr)
gdWebpWriteOpenPtr(const gdWebpWriteOptions *options);
BGD_DECLARE(int)
gdWebpWriteAddImage(gdWebpWritePtr webp, gdImagePtr image, int durationMs);
BGD_DECLARE(void) gdWebpWriteClose(gdWebpWritePtr webp);
BGD_DECLARE(void *) gdWebpWritePtrFinish(gdWebpWritePtr webp, int *size);

/* JXL */
BGD_DECLARE(gdImagePtr) gdImageCreateFromJxl(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlPtr(int size, void *data);
BGD_DECLARE(gdImagePtr) gdImageCreateFromJxlCtx(gdIOCtxPtr infile);

BGD_DECLARE(void) gdImageJxl(gdImagePtr im, FILE *outFile);
BGD_DECLARE(void)
gdImageJxlEx(gdImagePtr im, FILE *outFile, int lossless, float distance, int effort);
BGD_DECLARE(void *) gdImageJxlPtr(gdImagePtr im, int *size);
BGD_DECLARE(void *)
gdImageJxlPtrEx(gdImagePtr im, int *size, int lossless, float distance, int effort);
BGD_DECLARE(void) gdImageJxlCtx(gdImagePtr im, gdIOCtxPtr outfile);
BGD_DECLARE(void)
gdImageJxlCtxEx(gdImagePtr im, gdIOCtxPtr outfile, int lossless, float distance, int effort);

/* Animation API */
typedef struct gdJxlAnimReader *gdJxlAnimReaderPtr;
typedef struct gdJxlAnim *gdJxlAnimPtr;

typedef struct {
    int width;
    int height;
    int animated;
    int loop_count;
} gdJxlInfo;

typedef struct {
    int delay_ms;
    int x_offset;
    int y_offset;
    int width;
    int height;
    int blend_mode;
    int is_last;
} gdJxlFrameInfo;

#define GD_JXL_BLEND_REPLACE 0
#define GD_JXL_BLEND_ADD 1
#define GD_JXL_BLEND_BLEND 2
#define GD_JXL_BLEND_MULADD 3
#define GD_JXL_BLEND_MUL 4

BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreate(FILE *inFile);
BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreatePtr(int size, void *data);
BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreateCtx(gdIOCtxPtr inCtx);

BGD_DECLARE(int) gdImageJxlAnimReaderGetInfo(gdJxlAnimReaderPtr reader, gdJxlInfo *info);
BGD_DECLARE(gdImagePtr) gdJxlReadNextImage(gdJxlAnimReaderPtr reader, int *delay_ms);
BGD_DECLARE(int)
gdJxlReadNextImageEx(gdJxlAnimReaderPtr reader, int *delay_ms, gdImagePtr *image);

BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreateRaw(FILE *inFile);
BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreateRawPtr(int size, void *data);
BGD_DECLARE(gdJxlAnimReaderPtr) gdImageJxlAnimReaderCreateRawCtx(gdIOCtxPtr inCtx);

BGD_DECLARE(gdImagePtr) gdJxlReadNextFrame(gdJxlAnimReaderPtr reader, gdJxlFrameInfo *info);

BGD_DECLARE(void) gdImageJxlAnimReaderDestroy(gdJxlAnimReaderPtr reader);

BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBegin(FILE *outFile, int width, int height, int lossless, float distance, int effort);
BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBeginEx(FILE *outFile, int width, int height, int lossless, float distance,
                      int effort, int loop_count);

BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBeginCtx(gdIOCtxPtr outCtx, int width, int height, int lossless, float distance,
                       int effort);
BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBeginCtxEx(gdIOCtxPtr outCtx, int width, int height, int lossless, float distance,
                         int effort, int loop_count);

BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBeginPtr(int width, int height, int lossless, float distance, int effort);
BGD_DECLARE(gdJxlAnimPtr)
gdImageJxlAnimBeginPtrEx(int width, int height, int lossless, float distance, int effort,
                         int loop_count);

BGD_DECLARE(int) gdImageJxlAnimAddFrame(gdJxlAnimPtr anim, gdImagePtr im, int delay_ms);

BGD_DECLARE(int) gdImageJxlAnimEnd(gdJxlAnimPtr anim);
BGD_DECLARE(void *) gdImageJxlAnimEndPtr(gdJxlAnimPtr anim, int *size);

/* HEIF */

/**
 * Group: HEIF Coding Format
 *
 * Values that select the HEIF coding format.
 *
 * Constants: gdHeifCodec
 *
 *  GD_HEIF_CODEC_UNKNOWN
 *  GD_HEIF_CODEC_HEVC
 *  GD_HEIF_CODEC_AV1
 *
 * See also:
 *  - <gdImageHeif>
 */
typedef enum {
    GD_HEIF_CODEC_UNKNOWN = 0,
    GD_HEIF_CODEC_HEVC,
    GD_HEIF_CODEC_AV1 = 4,
} gdHeifCodec;

/**
 * Group: HEIF Chroma Subsampling
 *
 * Values that select the HEIF chroma subsampling.
 *
 * Constants: gdHeifCompression
 *
 *  GD_HEIF_CHROMA_420
 *  GD_HEIF_CHROMA_422
 *  GD_HEIF_CHROMA_444
 *
 * See also:
 *  - <gdImageHeif>
 */
typedef const char *gdHeifChroma;

#define GD_HEIF_CHROMA_420 "420"
#define GD_HEIF_CHROMA_422 "422"
#define GD_HEIF_CHROMA_444 "444"

typedef struct {
    size_t struct_size;
    int ignore_transformations;
} gdHeifReadOptions;

typedef struct {
    size_t struct_size;
    int quality;
    int lossless;
    gdHeifCodec codec;
    gdHeifChroma chroma;
} gdHeifWriteOptions;

BGD_DECLARE(void) gdHeifReadOptionsInit(gdHeifReadOptions *options);
BGD_DECLARE(void) gdHeifWriteOptionsInit(gdHeifWriteOptions *options);

BGD_DECLARE(gdImagePtr) gdImageCreateFromHeif(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifPtr(int size, void *data);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromHeifPtrWithOptions(int size, void *data, const gdHeifReadOptions *options);
BGD_DECLARE(gdImagePtr) gdImageCreateFromHeifCtx(gdIOCtxPtr infile);

BGD_DECLARE(void)
gdImageHeifEx(gdImagePtr im, FILE *outFile, int quality, gdHeifCodec codec, gdHeifChroma chroma);
BGD_DECLARE(void) gdImageHeif(gdImagePtr im, FILE *outFile);
BGD_DECLARE(void *) gdImageHeifPtr(gdImagePtr im, int *size);
BGD_DECLARE(void *)
gdImageHeifPtrEx(gdImagePtr im, int *size, int quality, gdHeifCodec codec, gdHeifChroma chroma);
BGD_DECLARE(void *)
gdImageHeifPtrWithOptions(gdImagePtr im, int *size, const gdHeifWriteOptions *options);
BGD_DECLARE(void)
gdImageHeifCtx(gdImagePtr im, gdIOCtxPtr outfile, int quality, gdHeifCodec codec,
               gdHeifChroma chroma);

/* AVIF */
BGD_DECLARE(gdImagePtr) gdImageCreateFromAvif(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromAvifPtr(int size, void *data);
BGD_DECLARE(gdImagePtr) gdImageCreateFromAvifCtx(gdIOCtxPtr infile);

enum {
    GD_AVIF_CHROMA_SUBSAMPLING_AUTO = 0,
    GD_AVIF_CHROMA_SUBSAMPLING_YUV420 = 1,
    GD_AVIF_CHROMA_SUBSAMPLING_YUV444 = 2
};

typedef struct {
    size_t struct_size;
    int quality;
    int speed;
    int lossless;
    int chroma_subsampling;
} gdAvifWriteOptions;

BGD_DECLARE(void) gdAvifWriteOptionsInit(gdAvifWriteOptions *options);
BGD_DECLARE(void) gdImageAvif(gdImagePtr im, FILE *outFile);
BGD_DECLARE(void)
gdImageAvifEx(gdImagePtr im, FILE *outFile, int quality, int speed);
BGD_DECLARE(void *) gdImageAvifPtr(gdImagePtr im, int *size);
BGD_DECLARE(void *)
gdImageAvifPtrEx(gdImagePtr im, int *size, int quality, int speed);
BGD_DECLARE(void *)
gdImageAvifPtrWithOptions(gdImagePtr im, int *size, const gdAvifWriteOptions *options);
BGD_DECLARE(void)
gdImageAvifCtx(gdImagePtr im, gdIOCtxPtr outfile, int quality, int speed);

/* TIFF */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiff(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffCtx(gdIOCtxPtr infile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromTiffPtr(int size, void *data);

typedef struct gdTiffReadStruct *gdTiffReadPtr;

typedef struct {
    int width;
    int height;
    int pageCount;
    int bitsPerSample;
    int samplesPerPixel;
    int compression;
    int photometric;
    float xResolution;
    float yResolution;
    int resolutionUnit;
} gdTiffInfo;

typedef struct {
    int pageIndex;
    int width;
    int height;
    int bitsPerSample;
    int samplesPerPixel;
    int compression;
    int photometric;
    int planar;
    int hasAlpha;
    int isTiled;
    float xResolution;
    float yResolution;
    int resolutionUnit;
} gdTiffPageInfo;

BGD_DECLARE(int) gdTiffIsMultiPage(FILE *fd);
BGD_DECLARE(int) gdTiffIsMultiPageCtx(gdIOCtxPtr in);
BGD_DECLARE(int) gdTiffIsMultiPagePtr(int size, void *data);
BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpen(FILE *fd);
BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpenCtx(gdIOCtxPtr in);
BGD_DECLARE(gdTiffReadPtr) gdTiffReadOpenPtr(int size, void *data);
BGD_DECLARE(void) gdTiffReadClose(gdTiffReadPtr tiff);
BGD_DECLARE(int) gdTiffReadGetInfo(gdTiffReadPtr tiff, gdTiffInfo *info);
/* On success, ownership of *image is transferred to the caller. */
BGD_DECLARE(int)
gdTiffReadNextImage(gdTiffReadPtr tiff, gdTiffPageInfo *info, gdImagePtr *image);

/* TIFF Write API */
#define GD_TIFF_RGB 1
#define GD_TIFF_RGBA 2
#define GD_TIFF_GRAY 3
#define GD_TIFF_BILEVEL 4

#define GD_TIFF_COMPRESSION_NONE 1
#define GD_TIFF_COMPRESSION_CCITT_RLE 2
#define GD_TIFF_COMPRESSION_CCITT_FAX3 3
#define GD_TIFF_COMPRESSION_CCITT_FAX4 4
#define GD_TIFF_COMPRESSION_LZW 5
#define GD_TIFF_COMPRESSION_JPEG 7
#define GD_TIFF_COMPRESSION_ADOBE_DEFLATE 8
#define GD_TIFF_COMPRESSION_DEFLATE 32946
#define GD_TIFF_COMPRESSION_PACKBITS 32773

#define GD_TIFF_PHOTOMETRIC_MINISWHITE 0
#define GD_TIFF_PHOTOMETRIC_MINISBLACK 1
#define GD_TIFF_PHOTOMETRIC_RGB 2
#define GD_TIFF_PHOTOMETRIC_PALETTE 3
#define GD_TIFF_PHOTOMETRIC_TRANSPARENCY_MASK 4
#define GD_TIFF_PHOTOMETRIC_SEPARATED 5
#define GD_TIFF_PHOTOMETRIC_YCBCR 6
#define GD_TIFF_PHOTOMETRIC_CIELAB 8

#define GD_TIFF_PLANARCONFIG_CONTIG 1
#define GD_TIFF_PLANARCONFIG_SEPARATE 2

#define GD_TIFF_RESUNIT_NONE 1
#define GD_TIFF_RESUNIT_INCH 2
#define GD_TIFF_RESUNIT_CENTIMETER 3

#define GD_TIFF_ALPHA_UNASSOCIATED 1
#define GD_TIFF_ALPHA_ASSOCIATED 2

typedef struct {
    int bitDepth;
    int colorspace;
    int compression;
    int jpegQuality;
    int minIsWhite;
    int resolutionUnit;
    float xResolution;
    float yResolution;
    int alphaType;
} gdTiffWriteOptions;

typedef struct gdTiffWriteStruct *gdTiffWritePtr;

BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpen(FILE *outFile, const gdTiffWriteOptions *options);
BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpenCtx(gdIOCtxPtr out, const gdTiffWriteOptions *options);
BGD_DECLARE(gdTiffWritePtr)
gdTiffWriteOpenPtr(const gdTiffWriteOptions *options);
BGD_DECLARE(int) gdTiffWriteAddImage(gdTiffWritePtr write, gdImagePtr image);
BGD_DECLARE(void) gdTiffWriteClose(gdTiffWritePtr write);
BGD_DECLARE(void *) gdTiffWritePtrFinish(gdTiffWritePtr write, int *size);

BGD_DECLARE(void) gdImageTiff(gdImagePtr im, FILE *outFile);
BGD_DECLARE(void *) gdImageTiffPtr(gdImagePtr im, int *size);
BGD_DECLARE(void) gdImageTiffCtx(gdImagePtr image, gdIOCtxPtr out);

/* TGA */
BGD_DECLARE(gdImagePtr) gdImageCreateFromTga(FILE *fp);
BGD_DECLARE(gdImagePtr) gdImageCreateFromTgaCtx(gdIOCtxPtr ctx);
BGD_DECLARE(gdImagePtr) gdImageCreateFromTgaPtr(int size, void *data);

/* BMP */
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmp(FILE *inFile);
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpPtr(int size, void *data);
BGD_DECLARE(gdImagePtr) gdImageCreateFromBmpCtx(gdIOCtxPtr infile);

/* UltraHDR  */

/**
 * Group: UltraHDR
 *
 * UltraHDR (gain map) APIs are separate from <gdImage>. The UltraHDR handle
 * type is opaque and cannot be passed to existing <gdImage*> functions.
 */

/**
 * @name Constants: gdUhdrStatus
 * @brief Return status values used by UltraHDR APIs.
 * @{
 */
#define GD_UHDR_SUCCESS 0        /**< Operation succeeded. */
#define GD_UHDR_NOT_AVAILABLE -1 /**< libgd was built without UltraHDR support. */
#define GD_UHDR_E_INVALID -2     /**< Invalid argument or state. */
#define GD_UHDR_E_UNSUPPORTED -3 /**< Unsupported format or operation. */
#define GD_UHDR_E_ENCODE -4      /**< Encode failure. */
#define GD_UHDR_E_DECODE -5      /**< Decode failure. */
/** @} */


/**
 * Constants: gdUhdrMirrorAxis
 *
 * Mirror axis values used by <gdUhdrImageMirror>.
 *
 *  GD_UHDR_MIRROR_HORIZONTAL
 *  GD_UHDR_MIRROR_VERTICAL
 */
#define GD_UHDR_MIRROR_HORIZONTAL 0
#define GD_UHDR_MIRROR_VERTICAL 1

/**
 * Enum: gdUhdrFormat
 *
 * UltraHDR container format selector.
 *
 *  GD_UHDR_FORMAT_JPEG - UltraHDR JPEG (currently supported)
 *  GD_UHDR_FORMAT_WEBP - reserved for future support
 *  GD_UHDR_FORMAT_HEIF - reserved for future support
 */
typedef enum {
    GD_UHDR_FORMAT_JPEG = 0,
    GD_UHDR_FORMAT_WEBP = 1,
    GD_UHDR_FORMAT_HEIF = 2
} gdUhdrFormat;

/**
 * Typedef: gdUhdrImage
 *
 * Opaque UltraHDR image handle.
 */
typedef struct gdUhdrImageStruct gdUhdrImage;

/**
 * Typedef: gdUhdrImagePtr
 *
 * Pointer to <gdUhdrImage>.
 */
typedef gdUhdrImage *gdUhdrImagePtr;

/**
 * Typedef: gdUhdrError
 *
 * Structured error details for UltraHDR APIs.
 *
 * Fields:
 *  code          - libgd UltraHDR status code (GD_UHDR_*)
 *  provider_code - underlying provider error code, if any
 *  message       - optional human-readable detail string
 */
typedef struct {
    int code;
    int provider_code;
    char message[128];
} gdUhdrError;

/**
 * Typedef: gdUhdrErrorPtr
 *
 * Pointer to <gdUhdrError>.
 */
typedef gdUhdrError *gdUhdrErrorPtr;

BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromFile(const char *filename, int format, gdUhdrErrorPtr err);
BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromCtx(gdIOCtxPtr ctx, int format, gdUhdrErrorPtr err);
BGD_DECLARE(gdUhdrImagePtr)
gdUhdrImageCreateFromPtr(int size, void *data, int format, gdUhdrErrorPtr err);
BGD_DECLARE(void) gdUhdrImageDestroy(gdUhdrImagePtr im);

BGD_DECLARE(gdImagePtr) gdImageCreateFromFile(const char *filename);
BGD_DECLARE(gdImagePtr) gdImageReadFile(const char *filename);
BGD_DECLARE(gdImagePtr) gdImageReadCtx(gdIOCtxPtr ctx);
typedef enum {
	gdImageReadStatusOk = 0,
	gdImageReadStatusUnrecognized,
	gdImageReadStatusUnsupportedFormat,
	gdImageReadStatusCodecUnavailable,
	gdImageReadStatusDecodeFailed
} gdImageReadStatus;
#define GD_IMAGE_READ_RESTRICT_CODEC_API 1
BGD_DECLARE(gdImagePtr) gdImageReadCtxEx(gdIOCtxPtr ctx, int flags, gdImageReadStatus *status, const char **format_name);

/*
  Group: Types

  typedef: gdSource

  typedef: gdSourcePtr

        *Note:* This interface is *obsolete* and kept only for
        *compatibility.  Use <gdIOCtx> instead.

        Represents a source from which a PNG can be read. Programmers who
        do not wish to read PNGs from a file can provide their own
        alternate input mechanism, using the <gdImageCreateFromPngSource>
        function. See the documentation of that function for an example of
        the proper use of this type.

        > typedef struct {
        >         int (*source) (void *context, char *buffer, int len);
        >         void *context;
        > } gdSource, *gdSourcePtr;

        The source function must return -1 on error, otherwise the number
        of bytes fetched. 0 is EOF, not an error!

   'context' will be passed to your source function.

*/
typedef struct {
    int (*source)(void *context, char *buffer, int len);
    void *context;
} gdSource, *gdSourcePtr;

/* Deprecated in favor of gdImageCreateFromPngCtx */
BGD_DECLARE(gdImagePtr) gdImageCreateFromPngSource(gdSourcePtr in);
/* for completeness with Sink 2.x APIs, will be removed in 3.0 with all Sink APIs */
BGD_DECLARE(gdImagePtr) gdImageCreateFromQoiSource(gdSourcePtr in);

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd(FILE *in);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGdCtx(gdIOCtxPtr in);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGdPtr(int size, void *data);
/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImageGdPtr(gdImagePtr im, int *size);
BGD_DECLARE(void) gdImageGd(gdImagePtr im, FILE *out);

BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2(FILE *in);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Ctx(gdIOCtxPtr in);
BGD_DECLARE(gdImagePtr) gdImageCreateFromGd2Ptr(int size, void *data);

BGD_DECLARE(gdImagePtr)
gdImageCreateFromGd2Part(FILE *in, int srcx, int srcy, int w, int h);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromGd2PartCtx(gdIOCtxPtr in, int srcx, int srcy, int w, int h);
BGD_DECLARE(gdImagePtr)
gdImageCreateFromGd2PartPtr(int size, void *data, int srcx, int srcy, int w, int h);

BGD_DECLARE(gdImagePtr) gdImageCreateFromXbm(FILE *in);


/**
 * Writes an image to an IO context in X11 bitmap format.
 *
 * @param image     The <gdImagePtr> to write.
 * @param file_name The prefix of the XBM's identifiers. Illegal characters are
 *                  automatically stripped.
 * @param fg        Which color to use as forground color. All pixels with another
 *                  color are unset.
 * @param out       The <gdIOCtx> to write the image file to.
 *
 */
BGD_DECLARE(void)
gdImageXbmCtx(gdImagePtr image, char *file_name, int fg, gdIOCtxPtr out);

/* NOTE: filename, not FILE */
BGD_DECLARE(gdImagePtr) gdImageCreateFromXpm(char *filename);

BGD_DECLARE(void *) gdImageBmpPtr(gdImagePtr im, int *size, int compression);
BGD_DECLARE(void) gdImageBmp(gdImagePtr im, FILE *outFile, int compression);
BGD_DECLARE(void) gdImageBmpCtx(gdImagePtr im, gdIOCtxPtr out, int compression);

#define GD_BMP_COMPRESS_NONE 0
#define GD_BMP_COMPRESS_RLE8 1
#define GD_BMP_COMPRESS_RLE4 2

#define GD_BMP_FLAG_NONE 0
#define GD_BMP_FLAG_FORCE_V4HDR (1 << 0)
#define GD_BMP_FLAG_QUANTIZE (1 << 1)
#define GD_BMP_FLAG_RGB555 (1 << 2)

BGD_DECLARE(void *)
gdImageBmpPtrEx(gdImagePtr im, int *size, int bpp, int compression, int flags);
BGD_DECLARE(void)
gdImageBmpEx(gdImagePtr im, FILE *outFile, int bpp, int compression, int flags);
BGD_DECLARE(void)
gdImageBmpCtxEx(gdImagePtr im, gdIOCtxPtr out, int bpp, int compression, int flags);

BGD_DECLARE(void) gdImageWBMP(gdImagePtr image, int fg, FILE *out);
BGD_DECLARE(void) gdImageWBMPCtx(gdImagePtr image, int fg, gdIOCtxPtr out);

BGD_DECLARE(int) gdUhdrIsAvailable(void);
BGD_DECLARE(int) gdUhdrImageWidth(gdUhdrImagePtr im);
BGD_DECLARE(int) gdUhdrImageHeight(gdUhdrImagePtr im);
BGD_DECLARE(int) gdUhdrImageHasGainMap(gdUhdrImagePtr im);
BGD_DECLARE(int)
gdUhdrImageResize(gdUhdrImagePtr im, int width, int height, gdUhdrErrorPtr err);
BGD_DECLARE(int)
gdUhdrImageCrop(gdUhdrImagePtr im, int left, int top, int width, int height, gdUhdrErrorPtr err);
BGD_DECLARE(int)
gdUhdrImageRotate(gdUhdrImagePtr im, int degrees, gdUhdrErrorPtr err);
BGD_DECLARE(int)
gdUhdrImageMirror(gdUhdrImagePtr im, int axis, gdUhdrErrorPtr err);
BGD_DECLARE(int)
gdUhdrImageFile(gdUhdrImagePtr im, const char *filename, int format, int quality,
                gdUhdrErrorPtr err);
BGD_DECLARE(int)
gdUhdrImageCtx(gdUhdrImagePtr im, gdIOCtxPtr ctx, int format, int quality, gdUhdrErrorPtr err);
BGD_DECLARE(void *)
gdUhdrImageWritePtr(gdUhdrImagePtr im, int *size, int format, int quality, gdUhdrErrorPtr err);
BGD_DECLARE(gdImagePtr)
gdUhdrImageGetSdr(gdUhdrImagePtr im, gdUhdrErrorPtr err);

BGD_DECLARE(int) gdImageFile(gdImagePtr im, const char *filename);
BGD_DECLARE(int) gdSupportsFileType(const char *filename, int writing);

/* Guaranteed to correctly free memory returned by the gdImage*Ptr
   functions */
BGD_DECLARE(void) gdFree(void *m);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImageWBMPPtr(gdImagePtr im, int *size, int fg);

/* 100 is highest quality (there is always a little loss with JPEG).
   0 is lowest. 10 is about the lowest useful setting. */
BGD_DECLARE(void) gdImageJpeg(gdImagePtr im, FILE *out, int quality);
BGD_DECLARE(void) gdImageJpegCtx(gdImagePtr im, gdIOCtxPtr out, int quality);
BGD_DECLARE(void)
gdImageJpegCtxWithMetadata(gdImagePtr im, gdIOCtxPtr out, int quality,
                           const gdImageMetadata *metadata);
BGD_DECLARE(int)
gdImageJpegWithOptions(gdImagePtr im, FILE *out, const gdJpegWriteOptions *options);
BGD_DECLARE(int)
gdImageJpegCtxWithOptions(gdImagePtr im, gdIOCtxPtr out, const gdJpegWriteOptions *options);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImageJpegPtr(gdImagePtr im, int *size, int quality);
BGD_DECLARE(void *)
gdImageJpegPtrWithMetadata(gdImagePtr im, int *size, int quality, const gdImageMetadata *metadata);
BGD_DECLARE(void *)
gdImageJpegPtrWithOptions(gdImagePtr im, int *size, const gdJpegWriteOptions *options);

/**
 * Group: WebP
 *
 * Constant: gdWebpLossless
 *
 * Lossless quality threshold. When image quality is greater than or equal to
 * <gdWebpLossless>, the image will be written in the lossless WebP format.
 *
 * See also:
 *   - <gdImageWebpEx>
 */
#define gdWebpLossless 101

BGD_DECLARE(void) gdImageWebpEx(gdImagePtr im, FILE *outFile, int quantization);
BGD_DECLARE(void) gdImageWebp(gdImagePtr im, FILE *outFile);
BGD_DECLARE(void *) gdImageWebpPtr(gdImagePtr im, int *size);
BGD_DECLARE(void *)
gdImageWebpPtrEx(gdImagePtr im, int *size, int quantization);
BGD_DECLARE(void)
gdImageWebpCtx(gdImagePtr im, gdIOCtxPtr outfile, int quantization);

/*
  Group: Types

  typedef: gdSink

  typedef: gdSinkPtr

        *Note:* This interface is *obsolete* and kept only for
        *compatibility*.  Use <gdIOCtx> instead.

        Represents a "sink" (destination) to which a PNG can be
        written. Programmers who do not wish to write PNGs to a file can
        provide their own alternate output mechanism, using the
        <gdImagePngToSink> function. See the documentation of that
        function for an example of the proper use of this type.

        > typedef struct {
        >     int (*sink) (void *context, char *buffer, int len);
        >     void *context;
        > } gdSink, *gdSinkPtr;

        The _sink_ function must return -1 on error, otherwise the number of
        bytes written, which must be equal to len.

        _context_ will be passed to your sink function.

*/

typedef struct {
    int (*sink)(void *context, const char *buffer, int len);
    void *context;
} gdSink, *gdSinkPtr;

BGD_DECLARE(void) gdImagePngToSink(gdImagePtr im, gdSinkPtr out);
BGD_DECLARE(void) gdImageQoiToSink(gdImagePtr im, gdSinkPtr out);

BGD_DECLARE(void) gdImageGd2(gdImagePtr im, FILE *out, int cs, int fmt);

/* Best to free this memory with gdFree(), not free() */
BGD_DECLARE(void *) gdImageGd2Ptr(gdImagePtr im, int cs, int fmt, int *size);

BGD_DECLARE(void) gdImageDestroy(gdImagePtr im);

/**
 * @brief Allocates a color
 *
 * This is a simplified variant of <gdImageColorAllocateAlpha> where the alpha
 * channel is always opaque.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 *
 * @return The color value.
 *
 * @see gdImageColorDeallocate
 */
BGD_DECLARE(int) gdImageColorAllocate(gdImagePtr im, int r, int g, int b);

/**
 * @brief Allocates a color
 *
 * This is typically used for palette images, but can be used for truecolor
 * images as well.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 *
 * @return The color value.
 *
 * @see gdImageColorDeallocate
 */
BGD_DECLARE(int)
gdImageColorAllocateAlpha(gdImagePtr im, int r, int g, int b, int a);

/** @brief Gets the closest color of the image
 *
 * This is a simplified variant of <gdImageColorClosestAlpha> where the alpha
 * channel is always opaque.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 * 
 * @return The closest color already available in the palette for palette images;
 *         the color value of the given components for truecolor images.
 *
 * @see gdImageColorExact
 */
BGD_DECLARE(int) gdImageColorClosest(gdImagePtr im, int r, int g, int b);

/**
 * @brief Gets the closest color of the image with alpha channel
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 * @param a  The value of the alpha component.
 *
 * @return The closest color already available in the palette for palette images;
 *         the color value of the given components for truecolor images.
 *
 * @see gdImageColorExactAlpha
 */
BGD_DECLARE(int)
gdImageColorClosestAlpha(gdImagePtr im, int r, int g, int b, int a);

/**
 * @brief Gets the closest color of the image using HWB color space
 * 
 * This function finds the closest color in the image's palette to the specified RGB color using the HWB (Hue, Whiteness, Blackness) color space. It is a more perceptually accurate method for color matching compared to simple RGB distance calculations.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 *
 * @return The closest color already available in the palette for palette images; if
 *         there is no exact color, -1 is returned.
 *         For truecolor images the color value of the given components is returned.
 *
 * @see gdImageColorExact
 */
BGD_DECLARE(int) gdImageColorClosestHWB(gdImagePtr im, int r, int g, int b);

/**
 * @brief Gets the exact color of the image
 *
 * This is a simplified variant of <gdImageColorExactAlpha> where the alpha
 * channel is always opaque.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 *
 * @return The exact color already available in the palette for palette images; if
 *         there is no exact color, -1 is returned.
 *         For truecolor images the color value of the given components is returned.
 *
 * @see gdImageColorClosest
 */
BGD_DECLARE(int) gdImageColorExact(gdImagePtr im, int r, int g, int b);

/**
 * @brief Gets the exact color of the image
 *
 * This is a simplified variant of <gdImageColorExactAlpha> where the alpha
 * channel is always opaque.
 *
 * @param im The image.
 * @param r  The value of the red component.
 * @param g  The value of the green component.
 * @param b  The value of the blue component.
 * @param a  The value of the alpha component.
 *
 * @return The exact color already available in the palette for palette images; if
 *         there is no exact color, -1 is returned.
 *         For truecolor images the color value of the given components is returned.
 *
 * @see gdImageColorClosestAlpha gdTrueColorAlpha
 */
BGD_DECLARE(int)
gdImageColorExactAlpha(gdImagePtr im, int r, int g, int b, int a);

/**
 * @brief Resolves a color in the image
 * @see gdImageColorResolve is an alternative for the code fragment
 * @code
 *  if ((color=gdImageColorExact(im,R,G,B)) < 0)
 *      if ((color=gdImageColorAllocate(im,R,G,B)) < 0)
 *          color=gdImageColorClosest(im,R,G,B);
 * @endcode
 * in a single function.    Its advantage is that it is guaranteed to
 * @return a color index in one search over the color table.
  */
BGD_DECLARE(int) gdImageColorResolve(gdImagePtr im, int r, int g, int b);

/**
 * @brief Same as @ref gdImageColorResovle but with alpha
 * 
 * @param im The image.
 * @param r The red component.
 * @param g The green component.
 * @param b The blue component.
 * @param a The alpha component.
 * 
 * @return The color index of the closest color in the palette or the newly allocated color.
 * 
 * @see gdImageColorExactAlpha gdImageColorClosestAlpha gdTrueColorAlpha
 */
BGD_DECLARE(int)
gdImageColorResolveAlpha(gdImagePtr im, int r, int g, int b, int a);

/* A simpler way to obtain an opaque truecolor value for drawing on a
   truecolor image. Not for use with palette images! */

#define gdTrueColor(r, g, b) (((r) << 16) + ((g) << 8) + (b))

/**
 * Group: Color Composition
 */

/**
 * @brief Compose a truecolor value from its components
 *
 * @param r The red channel (0-255)
 * @param g The green channel (0-255)
 * @param b The blue channel (0-255)
 * @param a The alpha channel (0-127, where 127 is fully transparent, and 0 is
 *          completely opaque).
 *
 * @see gdTrueColorGetAlpha gdTrueColorGetRed gdTrueColorGetGreen gdTrueColorGetBlue gdImageColorExactAlpha
 */
#define gdTrueColorAlpha(r, g, b, a) (((a) << 24) + ((r) << 16) + ((g) << 8) + (b))

/**
 * @brief Removes a palette entry
 *
 * This is a no-op for truecolor images.
 * The function does not alter the image data nor the transparent color or any
 * other places where this color index could have been referenced.
 * The index is marked as open and will be used too for any subsequent
 * @ref gdImageColorAllocate or @ref gdImageColorAllocateAlpha calls. Other lower
 * index may be open as well, the fist open index found will be used.
 *
 * @param im    The image.
 * @param color The palette index.
 *
 * @see gdImageColorAllocate gdImageColorAllocateAlpha
 */
BGD_DECLARE(void) gdImageColorDeallocate(gdImagePtr im, int color);

/* An attempt at getting the results of gdImageTrueColorToPalette to
 * look a bit more like the original (im1 is the original and im2 is
 * the palette version */

BGD_DECLARE(int) gdImageColorMatch(gdImagePtr im1, gdImagePtr im2);

/* Specifies a color index (if a palette image) or an
   RGB color (if a truecolor image) which should be
   considered 100% transparent. FOR TRUECOLOR IMAGES,
   THIS IS IGNORED IF AN ALPHA CHANNEL IS BEING
   SAVED. Use gdImageSaveAlpha(im, 0); to
   turn off the saving of a full alpha channel in
   a truecolor image. Note that gdImageColorTransparent
   is usually compatible with older browsers that
   do not understand full alpha channels well. TBB */

/**
 * @brief Sets the transparent color of the image
 *
 * 
 * Specifies a color index (if a palette image) or an
 * RGB color (if a truecolor image) which should be
 * considered 100% transparent. FOR TRUECOLOR IMAGES,
 * THIS IS IGNORED IF AN ALPHA CHANNEL IS BEING
 * SAVED. Use gdImageSaveAlpha(im, 0); to
 * turn off the saving of a full alpha channel in
 * a truecolor image. Note that gdImageColorTransparent
 * is usually compatible with older browsers that
 * do not understand full alpha channels well. TBB
 * 
 * @param im    The image.
 * @param color The color.
 *
 * @see gdImageGetTransparent
 */
BGD_DECLARE(void) gdImageColorTransparent(gdImagePtr im, int color);

/**
 * @brief Copies the palette from one image to another
 *
 * @param dst The destination image.
 * @param src The source image.
 */
BGD_DECLARE(void) gdImagePaletteCopy(gdImagePtr dst, gdImagePtr src);

typedef int (*gdCallbackImageColor)(gdImagePtr im, int src);

/**
 * @brief Replaces a color in the image with another color
 * 
 * @param im  The image.
 * @param src The source color to be replaced.
 * @param dst The destination color to replace with.
 */
BGD_DECLARE(int) gdImageColorReplace(gdImagePtr im, int src, int dst);

/**
 * @brief Replaces colors in an image with a threshold for perceptual color distance.
 *
 * Note: threshold semantics changed in versions >=2.3.4 — the value now scales
 * linearly with perceptual color distance. Callers using threshold values
 * tuned against the old behavior should apply new_t = sqrt(old_t / 100) * 100
 * to approximate the previous behavior. This is due to a bug fix in the color
 * distance calculation, which previously did not take the square root
 * of the sum of squares, and thus returned a value that was the square
 * of the actual perceptual color distance.
 * The new behavior is more intuitive and consistent with common color distance metrics
 * 
 * @param im The image to operate on.
 * @param src The source color to replace.
 * @param dst The destination color to replace with.
 * @param threshold The threshold for color matching. Colors within this distance from the source color will be replaced with the destination color.
 * @return The number of pixels that were replaced.
 */
BGD_DECLARE(int)
gdImageColorReplaceThreshold(gdImagePtr im, int src, int dst, float threshold);

/**
 * @brief Replaces multiple colors in an image with corresponding destination colors.
 * 
 * @param im The image to operate on.
 * @param len The number of colors to replace.
 * @param src An array of source colors to be replaced.
 * @param dst An array of destination colors to replace with.
 * 
 * @return The number of pixels that were replaced.
 */
BGD_DECLARE(int)
gdImageColorReplaceArray(gdImagePtr im, int len, int *src, int *dst);

/**
 * @brief Replaces colors in an image using a callback function to determine the replacement color.
 * 
 * @param im The image to operate on.
 * @param callback A callback function that takes the image and a source color as parameters and returns the destination color to replace with. @see gdCallbackImageColor
 * 
 * @return The number of pixels that were replaced.
 */
BGD_DECLARE(int)
gdImageColorReplaceCallback(gdImagePtr im, gdCallbackImageColor callback);

/**
 * @defgroup  Per Pixel Operations
 * @{ */

/**
 * @brief Sets the pixel at the specified coordinates to the given color.
 * Replaces or blends with the background depending on the
 * most recent call to @ref gdImageAlphaBlending and the
 * alpha channel value of 'color'; default is to overwrite.
 * Tiling and line styling are also implemented
 * here. All other gd drawing functions pass through this call,
 * allowing for many useful effects.
 * Overlay and multiply effects are used when @ref gdImageAlphaBlending
 * is passed @ref gdEffectOverlay and @ref gdEffectMultiply 
 * 
 * @param im The image.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param color The color to set the pixel to. Color can be a palette index for palette images or a truecolor value for truecolor images.
 * 
 * @see @ref gdImageGetPixel gdImageGetTrueColorPixel gdImageAlphaBlending gdImageCreateTruecolor gdImageCreatePalette
 */
BGD_DECLARE(void) gdImageSetPixel(gdImagePtr im, int x, int y, int color);
/**
 * @brief Gets the color of the pixel at the specified coordinates.
 * 
 * @param im The image.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * 
 * @return The color of the pixel. For palette images, this is the palette index. For truecolor images, this is the truecolor value.
 */
BGD_DECLARE(int) gdImageGetPixel(gdImagePtr im, int x, int y);

/**
 * @brief Gets the truecolor value of the pixel at the specified coordinates.
 * 
 * @param im The image.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * 
 * @return The truecolor value of the pixel. For palette images, this function will return the truecolor value corresponding to the palette index of the pixel.
 */
BGD_DECLARE(int) gdImageGetTrueColorPixel(gdImagePtr im, int x, int y);
/** @} */

/**
 * @brief Sets the resolution of an image.
 *
 * @param im    The image.
 * @param res_x The horizontal resolution in DPI.
 * @param res_y The vertical resolution in DPI.
 *
 * @see gdImageResolutionX gdImageResolutionY
 */
BGD_DECLARE(void)
gdImageSetResolution(gdImagePtr im, const unsigned int res_x, const unsigned int res_y);

/**
 * @defgroup Font Text Rendering, Bitmap Fonts
 *
 * @{ */

/**
 * @brief Gets the built-in giant font.
 */
BGD_DECLARE(gdFontPtr) gdFontGetGiant(void);
/**
 * @brief Gets the built-in large font.
 */
BGD_DECLARE(gdFontPtr) gdFontGetLarge(void);
/**
 * @brief Gets the built-in medium bold font.
 */
BGD_DECLARE(gdFontPtr) gdFontGetMediumBold(void);
/**
 * @brief Gets the built-in small font.
 */
BGD_DECLARE(gdFontPtr) gdFontGetSmall(void);
/**
 * @brief Gets the built-in tiny font.
 */
BGD_DECLARE(gdFontPtr) gdFontGetTiny(void);
/**
 * @brief Draws a single character.
 *
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param c     The character.
 * @param color The color.
 *
 * Variants @ref gdImageCharUp
 *
 * @see gdFontPtr
 */
BGD_DECLARE(void) gdImageChar(gdImagePtr im, gdFontPtr f, int x, int y, int c, int color);

/**
 * @brief Draws a single character rotated 90 degrees counterclockwise.
 * 
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param c     The character.
 * @param color The color.
 */
BGD_DECLARE(void) gdImageCharUp(gdImagePtr im, gdFontPtr f, int x, int y, int c, int color);

/**
 * @brief Draws a character string.
 *
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param s     The character string.
 * @param color The color.
 *
 * Variants:
 *  - @ref gdImageStringUp
 *  - @ref gdImageString16
 *  - @ref gdImageStringUp16
 *
 * @see gdFontPtr gdImageStringTTF gdImageString
 */
BGD_DECLARE(void)
gdImageString(gdImagePtr im, gdFontPtr f, int x, int y, unsigned char *s, int color);

/**
 * @brief Draws a string rotated 90 degrees counterclockwise.
 *
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param s     The string.
 * @param color The color.
 */
BGD_DECLARE(void)
gdImageStringUp(gdImagePtr im, gdFontPtr f, int x, int y, unsigned char *s, int color);

/**
 * @brief Draws a character string with 16-bit characters.
 * 
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param s     The character string (16-bit).
 * @param color The color.
 */
BGD_DECLARE(void)
gdImageString16(gdImagePtr im, gdFontPtr f, int x, int y, unsigned short *s, int color);

/**
 * @brief Draws a string rotated 90 degrees counterclockwise with 16-bit characters.
 * 
 * @param im    The image to draw onto.
 * @param f     The raster font.
 * @param x     The x coordinate of the upper left pixel.
 * @param y     The y coordinate of the upper left pixel.
 * @param s     The string (16-bit).
 * @param color The color.
 */
BGD_DECLARE(void)
gdImageStringUp16(gdImagePtr im, gdFontPtr f, int x, int y, unsigned short *s, int color);
/** @} */

/**
 * @defgroup freetypefont Font Text Rendering, FreeType 2
 * @{
 */

/**
 * @brief Set up the font cache.
 *
 * This is called automatically from the string rendering functions, if it
 * has not already been called. So there's no need to call this function
 * explicitly.
 */
BGD_DECLARE(int) gdFontCacheSetup(void);


/**
 * @brief Shut down the font cache and free the allocated resources.
 *
 * @note This function has to be called whenever FreeType operations have been invoked, to avoid resource leaks. It doesn't harm to call this function multiple times.
 */
BGD_DECLARE(void) gdFontCacheShutdown(void);

/**
 * @brief Alias of @ref gdFontCacheShutdown.
 * @deprecated
 */
BGD_DECLARE(void) gdFreeFontCache(void);


/**
 * @brief Draws a string using FreeType 2 fonts. Alias of @ref gdImageStringFT. Provided for backwards compatibility only. 
 * @deprecated
 */
BGD_DECLARE(char *)
gdImageStringTTF(gdImagePtr im, int *brect, int fg, const char *fontlist, double ptsize,
                 double angle, int x, int y, const char *string);


/**
 * @brief Render an UTF-8 string onto a gd image.
 *
 * @param im       The image to draw onto.
 * @param brect    The bounding rectangle as array of 8 integers where each pair
 *                 represents the x- and y-coordinate of a point. The points
 *                 specify the lower left, lower right, upper right and upper left
 *                 corner.
 * @param fg       The font color.
 * @param fontlist The semicolon delimited list of font filenames to look for.
 * @param ptsize   The height of the font in typographical points (pt).
 * @param angle    The angle in radian to rotate the font counter-clockwise.
 * @param x        The x-coordinate of the basepoint (roughly the lower left corner)
 *                   of the first letter.
 * @param y        The y-coordinate of the basepoint (roughly the lower left corner)
 *                   of the first letter.
 * @param string   The string to render.
 *
 * Variant @ref gdImageStringFTEx
 *
 * @see gdImageString
 */
BGD_DECLARE(char *)
gdImageStringFT(gdImagePtr im, int *brect, int fg, const char *fontlist, double ptsize,
                double angle, int x, int y, const char *string);

/*
  Group: Types

  typedef: gdFTStringExtra

  typedef: gdFTStringExtraPtr

  A structure and associated pointer type used to pass additional
  parameters to the <gdImageStringFTEx> function. See
  <gdImageStringFTEx> for the structure definition.

  Thanks to Wez Furlong.
*/

/* 2.0.5: provides an extensible way to pass additional parameters.
   Thanks to Wez Furlong, sorry for the delay. */
typedef struct {
    int flags;          /* Logical OR of gdFTEX_ values */
    double linespacing; /* fine tune line spacing for '\n' */
    int charmap;        /* TBB: 2.0.12: may be gdFTEX_Unicode,
                           gdFTEX_Shift_JIS, gdFTEX_Big5,
                           or gdFTEX_Adobe_Custom;
                           when not specified, maps are searched
                           for in the above order. */
    int hdpi;           /* if (flags & gdFTEX_RESOLUTION) */
    int vdpi;           /* if (flags & gdFTEX_RESOLUTION) */
    char *xshow;        /* if (flags & gdFTEX_XSHOW)
                   then, on return, xshow is a malloc'ed
                   string containing xshow position data for
                   the last string.
       
                   NB. The caller is responsible for gdFree'ing
                   the xshow string.
                */
    char *fontpath;     /* if (flags & gdFTEX_RETURNFONTPATHNAME)
                   then, on return, fontpath is a malloc'ed
                   string containing the actual font file path name
                   used, which can be interesting when fontconfig
                   is in use.
    
                   The caller is responsible for gdFree'ing the
                   fontpath string.
                */

} gdFTStringExtra, *gdFTStringExtraPtr;

#define gdFTEX_LINESPACE 1
#define gdFTEX_CHARMAP 2
#define gdFTEX_RESOLUTION 4
#define gdFTEX_DISABLE_KERNING 8
#define gdFTEX_XSHOW 16
/* The default unless gdFTUseFontConfig(1); has been called:
   fontlist is a full or partial font file pathname or list thereof
   (i.e. just like before 2.0.29) */
#define gdFTEX_FONTPATHNAME 32
/* Necessary to use fontconfig patterns instead of font pathnames
   as the fontlist argument, unless gdFTUseFontConfig(1); has
   been called. New in 2.0.29 */
#define gdFTEX_FONTCONFIG 64
/* Sometimes interesting when fontconfig is used: the fontpath
   element of the structure above will contain a gdMalloc'd string
   copy of the actual font file pathname used, if this flag is set
   when the call is made */
#define gdFTEX_RETURNFONTPATHNAME 128

/* 

 */

/**
 * @brief Enable or disable fontconfig by default.
 * 
 * If flag is nonzero, the fontlist parameter to gdImageStringFT
 * and gdImageStringFTEx shall be assumed to be a fontconfig font pattern
 * if fontconfig was compiled into gd. This function returns zero
 * if fontconfig is not available, nonzero otherwise.
 * If GD is built without libfontconfig support, this function is a NOP.
 *
 * @param flag Zero to disable, nonzero to enable.
 *
 * @see gdImageStringFTEx
 */
BGD_DECLARE(int) gdFTUseFontConfig(int flag);

/* These are NOT flags; set one in 'charmap' if you set the
   gdFTEX_CHARMAP bit in 'flags'. */
#define gdFTEX_Unicode 0 /**< Unicode character map */
#define gdFTEX_Shift_JIS 1 /**< Shift_JIS character map */
#define gdFTEX_Big5 2 /**< Big5 character map */
#define gdFTEX_Adobe_Custom 3 /**< Adobe Custom character map */
#define gdFTEX_MacRoman gdFTEX_Adobe_Custom /**< Deprecated compatibility name used by bundled PHP's historical libgd. */

/**
 * @brief Draws a string using FreeType 2 fonts with additional parameters.
 * 
 * @param im       The image to draw onto.
 * @param brect    The bounding rectangle as array of 8 integers where each pair
 *                 represents the x- and y-coordinate of a point. The points
 *                 specify the lower left, lower right, upper right and upper left
 *                 corner.
 * @param fg       The font color.
 * @param fontlist The semicolon delimited list of font filenames to look for.
 * @param ptsize   The height of the font in typographical points (pt).
 * @param angle    The angle in radian to rotate the font counter-clockwise.
 * @param x        The x-coordinate of the basepoint (roughly the lower left corner)
 *                   of the first letter.
 * @param y        The y-coordinate of the basepoint (roughly the lower left corner)
 *                   of the first letter.
 * @param string   The string to render.
 * @param strex    A pointer to a <gdFTStringExtra> structure containing additional parameters for rendering the string.
 * 
 * @return A pointer to a string containing an error message if an error occurred, or NULL if the operation was successful.
 */
BGD_DECLARE(char *)
gdImageStringFTEx(gdImagePtr im, int *brect, int fg, const char *fontlist, double ptsize,
                  double angle, int x, int y, const char *string, gdFTStringExtraPtr strex);

/** @} */

/**
 * @defgroup PixelDraw lines, ellipses, polygons and Arc Drawing pixel operations
 * 
 * @note 2.4+ brings a 2D Vector APIs with high quality rendering and options. Similar to Canvas 2D APIs. We recommend it for new usages.
 * 
 * @{
 */

 /**
 * @brief A point in the coordinate space of the image
 */
typedef struct {
    int x, y; /**< The x and y coordinates of the point. */
} gdPoint, *gdPointPtr; /**< A pointer to a <gdPoint>. */

/**
 * @brief A rectangle in the coordinate space of the image
  */
typedef struct {
    int x, y; /**< The x and y coordinates of the upper left corner. */
    int width, height; /**< The width and height of the rectangle. */
} gdRect, *gdRectPtr; /**< A pointer to a @ref gdRect. */


/** 
 * @brief Style flags for drawing arcs and chords
 * Style is a bitwise OR ( | operator ) of these.
 * gdArc and gdChord are mutually exclusive;
 * gdChord just connects the starting and ending
 * angles with a straight line, while gdArc produces
 * a rounded edge. gdPie is a synonym for gdArc.
 * gdNoFill indicates that the arc or chord should be
 * outlined, not filled. gdEdged, used together with
 * gdNoFill, indicates that the beginning and ending
 * angles should be connected to the center; this is
 * a good way to outline (rather than fill) a
 * 'pie slice'.
 */
#define gdArc 0 /**< mutually exclusive with gdChord */
#define gdPie gdArc /**< synonym for gdArc */
#define gdChord 1 /**< mutually exclusive with gdArc */
#define gdNoFill 2 /**< indicates that the arc or chord should be outlined, not filled */
#define gdEdged 4 /**< used together with gdNoFill, indicates that the beginning and ending angles should be connected to the center */

/**
 * @brief Draws a closed polygon
 *
 * @param  im The image.
 * @param  p  The vertices as array of <gdPoint>s.
 * @param  n  The number of vertices.
 * @param  c  The color.
 *
 * @see gdImageOpenPolygon gdImageFilledPolygon
 */
BGD_DECLARE(void) gdImagePolygon(gdImagePtr im, gdPointPtr p, int n, int c);

/**
 * @brief Draws an open polygon
 *
 * @param im The image.
 * @param p  The vertices as array of <gdPoint>s.
 * @param n  The number of vertices.
 * @param c  The color.
 *
 * @see gdImagePolygon
 */
BGD_DECLARE(void) gdImageOpenPolygon(gdImagePtr im, gdPointPtr p, int n, int c);


/**
 * @brief Draws a filled polygon
 *
 * The polygon is filled using the even-odd fillrule what can leave unfilled
 * regions inside of self-intersecting polygons. This behavior might change in
 * a future version.
 *
 * @param  im The image.
 * @param  p  The vertices as array of <gdPoint>s.
 * @param  n  The number of vertices.
 * @param  c  The color.
 *
 * @see gdImagePolygon
 */
BGD_DECLARE(void)
gdImageFilledPolygon(gdImagePtr im, gdPointPtr p, int n, int c);

BGD_DECLARE(void)
gdImageFilledArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color, int style);
BGD_DECLARE(void)
gdImageArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color);

/**
 * @brief Draw an ellipse, stroke only.
 *
 * @note This function does not support @ref gdImageSetThickness. GD 3.0 supports
 * actual 2D vectors operation, you may rely on it if you need better 2D drawing
 * operations.
 *
 * @param  im   The destination image.
 * @param  cx   x-coordinate of the center.
 * @param  cy   y-coordinate of the center.
 * @param  w    The ellipse width.
 * @param  h    The ellipse height.
 * @param  color The color of the ellipse. A color identifier created with one of the
 * image color allocate functions.
 *
 * @see gdImageFilledEllipse
 */
BGD_DECLARE(void)
gdImageEllipse(gdImagePtr im, int cx, int cy, int w, int h, int color);
BGD_DECLARE(void)
gdImageFilledEllipse(gdImagePtr im, int cx, int cy, int w, int h, int color);

BGD_DECLARE(void) gdImageAABlend(gdImagePtr im);

BGD_DECLARE(void) gdImageLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);

/* For backwards compatibility only. Use gdImageSetStyle()
   for much more flexible line drawing. */
BGD_DECLARE(void) gdImageDashedLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
/* Corners specified (not width and height). Upper left first, lower right
   second. */

/**
 * @brief Draws a rectangle.
 *
 * @param  im    The image.
 * @param  x1    The x-coordinate of one of the corners.
 * @param  y1    The y-coordinate of one of the corners.
 * @param  x2    The x-coordinate of another corner.
 * @param  y2    The y-coordinate of another corner.
 * @param  color The color.
 *
 * @see gdImageFilledRectangle
 */
BGD_DECLARE(void) gdImageRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
/* Solid bar. Upper left corner first, lower right corner second. */
BGD_DECLARE(void) gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);

/**
 * @brief Sets the clipping rectangle
 *
 * The clipping rectangle restricts the drawing area for following drawing
 * operations.
 *
 * @param  im - The image.
 * @param  x1 - The x-coordinate of the upper left corner.
 * @param  y1 - The y-coordinate of the upper left corner.
 * @param  x2 - The x-coordinate of the lower right corner.
 * @param  y2 - The y-coordinate of the lower right corner.
 *
 * @see gdImageGetClip
 */
BGD_DECLARE(void) gdImageSetClip(gdImagePtr im, int x1, int y1, int x2, int y2);

/**
 * @brief Gets the current clipping rectangle
 *
 * @param  im   The image.
 * @param  x1P  (out) The x-coordinate of the upper left corner.
 * @param  y1P  (out) The y-coordinate of the upper left corner.
 * @param  x2P  (out) The x-coordinate of the lower right corner.
 * @param  y2P  (out) The y-coordinate of the lower right corner.
 *
 * @see gdImageSetClip
 */
BGD_DECLARE(void) gdImageGetClip(gdImagePtr im, int *x1P, int *y1P, int *x2P, int *y2P);

/**
 * @brief Sets the brush for following drawing operations
 *
 * @param  im    The image.
 * @param  brush The brush image.
 */
BGD_DECLARE(void) gdImageSetBrush(gdImagePtr im, gdImagePtr brush);

/**
 * @brief Sets the tile for following drawing operations
 * 
 * The tile is used for filling areas with a repeating pattern. The tile image is repeated to fill the area being drawn.
 * 
 * @param  im   The image.
 * @param  tile The tile image.
 */
BGD_DECLARE(void) gdImageSetTile(gdImagePtr im, gdImagePtr tile);


/**
 * @brief Set the color for subsequent anti-aliased drawing
 *
 * If @ref gdAntiAliased is passed as color to drawing operations that support
 * anti-aliased drawing (such as @ref gdImageLine and @ref gdImagePolygon), the actual
 * color to be used can be set with this function.
 *
 * Example: draw an anti-aliased blue line:
 * @code
 * gdImageSetAntiAliased(im, gdTrueColorAlpha(0, 0, gdBlueMax, gdAlphaOpaque));
 * gdImageLine(im, 10,10, 20,20, gdAntiAliased);
 * @endcode
 *
 * @param  im - The image.
 * @param  c  - The color.
 *
 * @see gdImageSetAntiAliasedDontBlend
 */
BGD_DECLARE(void) gdImageSetAntiAliased(gdImagePtr im, int c);

/**
 * Set the color and "dont_blend" color for subsequent anti-aliased drawing
 *
 * This extended variant of <gdImageSetAntiAliased> allows to also specify a
 * (background) color that will not be blended in anti-aliased drawing
 * operations.
 *
 * @param im         The image.
 * @param c          The color.
 * @param dont_blend Whether to blend.
 */
BGD_DECLARE(void) gdImageSetAntiAliasedDontBlend(gdImagePtr im, int c, int dont_blend);

/**
 * @brief Sets the style for following drawing operations
 *
 * @param  im        The image.
 * @param  style     An array of color values.
 * @param  noOfPixel The number of color values.
 */
BGD_DECLARE(void) gdImageSetStyle(gdImagePtr im, int *style, int noOfPixels);


/**
 * Sets the thickness for following drawing operations
 *
 * @param  im        The image.
 * @param  thickness The thickness in pixels.
 */
BGD_DECLARE(void) gdImageSetThickness(gdImagePtr im, int thickness);


/** @} */

BGD_DECLARE(void)
gdImageFillToBorder(gdImagePtr im, int x, int y, int border, int color);
BGD_DECLARE(void) gdImageFill(gdImagePtr im, int x, int y, int color);


/** @defgroup cloneandcopy Clone, copy and image properties
 * @{ */
/**
 * @brief Copy an area of an image to another image
 *
 * @param dst  - The destination image.
 * @param src  - The source image.
 * @param dstX - The x-coordinate of the upper left corner to copy to.
 * @param dstY - The y-coordinate of the upper left corner to copy to.
 * @param srcX - The x-coordinate of the upper left corner to copy from.
 * @param srcY - The y-coordinate of the upper left corner to copy from.
 * @param w    - The width of the area to copy.
 * @param h    - The height of the area to copy.
 *
 * @see gdImageCopyMerge gdImageCopyMergeGray gdImageCopyResized gdImageCopyResampled gdImageCopyRotated gdImageScale gdImageScaleWithOptions
 */
BGD_DECLARE(void)
gdImageCopy(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h);

/**
 * @brief Copy an area of an image to another image ignoring alpha
 *
 * The source area will be copied to the destination are by merging the pixels.
 *
 * @note This function is a substitute for real alpha channel operations, so it doesn't pay attention to the alpha channel.
 *
 * @param dst  The destination image.
 * @param src  The source image.
 * @param dstX The x-coordinate of the upper left corner to copy to.
 * @param dstY The y-coordinate of the upper left corner to copy to.
 * @param srcX The x-coordinate of the upper left corner to copy from.
 * @param srcY The y-coordinate of the upper left corner to copy from.
 * @param w    The width of the area to copy.
 * @param h    The height of the area to copy.
 * @param pct  The percentage in range 0..100.
 *
 * @see gdImageCopy  gdImageCopyMergeGray
 */
BGD_DECLARE(void)
gdImageCopyMerge(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w,
                 int h, int pct);


/**
 * @brief Copy an area of an image to another image ignoring alpha
 *
 * The source area will be copied to the grayscaled destination area by merging
 * the pixels.
 *
 * @note This function is a substitute for real alpha channel operations, so it doesn't pay attention to the alpha channel.
 *
 * @param dst  - The destination image.
 * @param src  - The source image.
 * @param dstX - The x-coordinate of the upper left corner to copy to.
 * @param dstY - The y-coordinate of the upper left corner to copy to.
 * @param srcX - The x-coordinate of the upper left corner to copy from.
 * @param srcY - The y-coordinate of the upper left corner to copy from.
 * @param w    - The width of the area to copy.
 * @param h    - The height of the area to copy.
 * @param pct  - The percentage of the source color intensity in range 0..100.
 *
 * @see gdImageCopy gdImageCopyMerge
 */
BGD_DECLARE(void)
gdImageCopyMergeGray(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w,
                     int h, int pct);


/**
 * @brief Copy a resized area from an image to another image
 *
 * If the source and destination area differ in size, the area will be resized
 * using nearest-neighbor interpolation.
 *
 * @param dst  The destination image.
 * @param src  The source image.
 * @param dstX The x-coordinate of the upper left corner to copy to.
 * @param dstY The y-coordinate of the upper left corner to copy to.
 * @param srcX The x-coordinate of the upper left corner to copy from.
 * @param srcY The y-coordinate of the upper left corner to copy from.
 * @param dstW The width of the area to copy to.
 * @param dstH The height of the area to copy to.
 * @param srcW The width of the area to copy from.
 * @param srcH The height of the area to copy from.
 *
 * @see gdImageCopyResampled gdImageScale
 */
BGD_DECLARE(void)
gdImageCopyResized(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW,
                   int dstH, int srcW, int srcH);

/**
 * @brief Copy a resampled area from an image to another image
 *
 * If the source and destination area differ in size, the area will be resized
 * using bilinear interpolation for truecolor images, and nearest-neighbor
 * interpolation for palette images.
 *
 * @param dst  The destination image.
 * @param src  The source image.
 * @param dstX The x-coordinate of the upper left corner to copy to.
 * @param dstY The y-coordinate of the upper left corner to copy to.
 * @param srcX The x-coordinate of the upper left corner to copy from.
 * @param srcY The y-coordinate of the upper left corner to copy from.
 * @param dstW The width of the area to copy to.
 * @param dstH The height of the area to copy to.
 * @param srcW The width of the area to copy from.
 * @param srcH The height of the area to copy from.
 *
 * @see gdImageCopyResized gdImageScale
 */
BGD_DECLARE(void)
gdImageCopyResampled(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY,
                     int dstW, int dstH, int srcW, int srcH);

/**
 * @brief Copy a rotated area from an image to another image
 *
 * The area is counter-clockwise rotated using nearest-neighbor interpolation.
 *
 * @param dst The destination image.
 * @param src The source image.
 * @param  dstX  The x-coordinate of the center of the area to copy to.
 * @param  dstY  The y-coordinate of the center of the area to copy to.
 * @param  srcX  The x-coordinate of the upper left corner to copy from.
 * @param  srcY  The y-coordinate of the upper left corner to copy from.
 * @param  srcW  The width of the area to copy from.
 * @param  srcH  The height of the area to copy from.
 * @param  angle The angle in degrees.
 *
 * @see gdImageRotateInterpolated
 */
BGD_DECLARE(void)
gdImageCopyRotated(gdImagePtr dst, gdImagePtr src, double dstX, double dstY, int srcX, int srcY,
                   int srcWidth, int srcHeight, int angle);

/**
 * @brief Clones an image
 *
 * Creates an exact duplicate of the given image.
 *
 * @param src The source image.
 *
 * @returns The cloned image on success, NULL on failure.
 */
BGD_DECLARE(gdImagePtr) gdImageClone(gdImagePtr src);

/**
 * @brief Sets whether an image is interlaced
 *
 * This is relevant only when saving the image in a format that supports
 * interlacing.
 *
 * @param im            The image.
 * @param interlaceArg  Whether the image is interlaced.
 *
 * @see gdImageGetInterlaced
 */
BGD_DECLARE(void) gdImageInterlace(gdImagePtr im, int interlaceArg);

/** @} */

/**
 * @brief Sets the effect for subsequent drawing operations
 *
 * @note The effect is used for truecolor images only.
 *
 * @note in gd 2.4+, a configure flag is available to use the accurate and correct blending algorithm for truecolor images. This is the default behavior.
 *       The old, faster, but less accurate algorithm can be used by configuring gd with --disable-accurate-blending.
 * @param im The image.
 * @param alphaBlendingArg The effect.
 *
 * Effects: @ref gdEffectOverlay, @ref gdEffectMultiply, @ref gdEffectNormal
 * 
 * 
 */
BGD_DECLARE(void) gdImageAlphaBlending(gdImagePtr im, int alphaBlendingArg);

/**
 * @brief Sets the save alpha flag
 *
 * The save alpha flag specifies whether the alpha channel of the pixels should
 * be saved. This is supported only for image formats that support full alpha
 * transparency, e.g. PNG.
 * 
 * @param im The image.
 * @param saveAlphaArg The save alpha flag (1 to save alpha, 0 to not save alpha).
 */
BGD_DECLARE(void) gdImageSaveAlpha(gdImagePtr im, int saveAlphaArg);

/**
 * @defgroup  Color Quantization
 * 
 * @{ */

 /**
 * Note that @ref GD_QUANT_JQUANT does not retain the alpha channel, and
 * @ref GD_QUANT_NEUQUANT does not support dithering.
 *
 * @see gdImageTrueColorToPaletteSetMethod
 */
enum gdPaletteQuantizationMethod {
    GD_QUANT_DEFAULT = 0, /**< Default quantization method */
    GD_QUANT_JQUANT = 1, /**< libjpeg's old median cut */
    GD_QUANT_NEUQUANT = 2, /**< NeuQuant - approximation using Kohonen neural network */
    GD_QUANT_LIQ = 3 /**< libimagequant combination aiming for highest quality */
};


/**
 * @brief Creates a new palette image from a truecolor image
 *
 * This is the same as calling @ref gdImageCreatePaletteFromTrueColor with the
 * quantization method @ref GD_QUANT_NEUQUANT.
 *
 * @param im            - The image.
 * @param max_color     - The number of desired palette entries.
 * @param sample_factor - The quantization precision between 1 (highest quality) and
 *                        10 (fastest).
 *
 * @returns A newly create palette image; NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageNeuQuant(gdImagePtr im, const int max_color, int sample_factor);


/**  @brief Selects quantization method used for subsequent @ref gdImageTrueColorToPalette calls.
 *
 *   @details See @ref gdPaletteQuantizationMethod enum (e.g. @ref GD_QUANT_NEUQUANT,
 *   @ref GD_QUANT_LIQ). Speed is from 1 (highest quality) to 10 (fastest). Speed 0
 *   selects method-specific default (recommended).
 * 
 *   @param im The image to set the quantization method for.
 *   @param method The quantization method to use.
 *   @param speed The speed/quality tradeoff for the selected method.
 * 
 *   @returns FALSE if the given method is invalid or not available.
 */
BGD_DECLARE(int)
gdImageTrueColorToPaletteSetMethod(gdImagePtr im, int method, int speed);

/**
 * @brief Sets the quality range for subsequent @ref gdImageTrueColorToPalette calls.
 * @details  Chooses quality range that subsequent call to @ref gdImageTrueColorToPalette will
 * aim for. Min and max quality is in range 1-100 (1 = ugly, 100 = perfect). Max
 * must be higher than min. If palette cannot represent image with at least
 * min_quality, then image will remain true-color. If palette can represent image
 * with quality better than max_quality, then lower number of colors will be
 * used. This function has effect only when @ref GD_QUANT_LIQ method has been selected
 * and the source image is true-color.
 *
 *   @param im           The image.
 *   @param min_quality  The minimum quality in range 1-100 (1 = ugly, 100 = perfect).
 *                       If the palette cannot represent the image with at least
 *                       min_quality, then no conversion is done.
 *   @param max_quality  The maximum quality in range 1-100 (1 = ugly, 100 = perfect),
 *                       which must be higher than the min_quality. If the palette can
 *                       represent the image with a quality better than max_quality,
 *                       then fewer colors than requested will be used.
 */
BGD_DECLARE(void)
gdImageTrueColorToPaletteSetQuality(gdImagePtr im, int min_quality, int max_quality);


/** 
 * @brief Converts a truecolor image to a palette-based image.
 * @details This function converts a truecolor image to a palette-based image
 * using a high-quality two-pass quantization routine
 * which attempts to preserve alpha channel information
 * as well as R/G/B color information when creating
 * a palette. If ditherFlag is set, the image will be
 * dithered to approximate colors better, at the expense
 * of some obvious "speckling." colorsWanted can be
 * anything up to 256. If the original source image
 * includes photographic information or anything that
 * came out of a JPEG, 256 is strongly recommended.
 * 
 * Better yet, don't use these function -- write real
 * truecolor PNGs and JPEGs. The disk space gain of
 * conversion to palette is not great (for small images
 * it can be negative) and the quality loss is ugly.
 *
 * DIFFERENCES: @ref gdImageCreatePaletteFromTrueColor creates and
 * returns a new image. @ref gdImageTrueColorToPalette modifies
 * an existing image, and the truecolor pixels are discarded.
 *
 * @param im           The image.
 * @param dither       Whether dithering should be applied.
 * @param colorsWanted The number of desired palette entries.
 * 
 * @returns a newly created palette image on success, NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageCreatePaletteFromTrueColor(gdImagePtr im, int ditherFlag, int colorsWanted);


/**
 * @brief Converts a truecolor image to a palette image
 *
 * @param im           The image.
 * @param dither       Whether dithering should be applied.
 * @param colorsWanted The number of desired palette entries.
 *
 * @return Non-zero if the conversion succeeded, zero otherwise.
 *
 * @see gdImageCreatePaletteFromTrueColor gdImageTrueColorToPaletteSetMethod  gdImagePaletteToTrueColor
 */
BGD_DECLARE(int)
gdImageTrueColorToPalette(gdImagePtr im, int ditherFlag, int colorsWanted);

/** @brief Converts a palette-based image to a truecolor image 
 * 
 * @details This function converts a palette-based image to a truecolor image. The
 * palette is discarded, and the image is converted to truecolor. The alpha channel
 * information is preserved.
 * @param src The source image.
 * @return Non-zero if the conversion succeeded, zero otherwise.
 *
 */
BGD_DECLARE(int) gdImagePaletteToTrueColor(gdImagePtr src);

/** @} */

/**
 * @defgroup  ImageFilters Image Filters and convolutions
 * @{ */
/*
 * @brief @ref gdImagePixelate options
 *
 * Negate the imag src, white becomes black,
 * The red, green, and blue intensities of an image are negated.
 * White becomes black, yellow becomes blue, etc.
 */

enum gdPixelateMode { 
    GD_PIXELATE_UPPERLEFT, /**< Use the upper-left pixel of each block */
    GD_PIXELATE_AVERAGE /**< Use the average color of each block */
};
/**
 * @brief Pixelates an image
 * 
 * Pixelates an image by dividing it into blocks of the specified size and replacing each block with a single color.
 * The color can be determined by either the upper-left pixel of the block or the average color of all pixels in the block, depending on the mode specified.
 * 
 * @param im The image to pixelate.
 * @param block_size The size of the blocks to use for pixelation. Must be greater than 0.
 * @param mode The mode to use for determining the color of each block. @ref gdPixelateMode
 * 
 * @return Non-zero on success, zero on failure. Failure: Returns zero if im is NULL or block_size is less than or equal to 0.
 */
BGD_DECLARE(int)
gdImagePixelate(gdImagePtr im, int block_size, const unsigned int mode);

typedef struct {
    int sub;
    int plus;
    unsigned int num_colors;
    int *colors;
    unsigned int seed;
} gdScatter, *gdScatterPtr;

BGD_DECLARE(int) gdImageScatter(gdImagePtr im, int sub, int plus);
BGD_DECLARE(int)
gdImageScatterColor(gdImagePtr im, int sub, int plus, int colors[], unsigned int num_colors);
BGD_DECLARE(int) gdImageScatterEx(gdImagePtr im, gdScatterPtr s);

/**
 * @brief Smooth an image
 *
 * (see smooth.jpg)
 *
 * @param im     The image.
 * @param weight The strength of the smoothing.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageConvolution
 */

/**
 * @brief Smooth an image
 *
 * Smooth an image
 *
 * (see smooth.jpg)
 * 
 * @param src        The source image.
 * @param weight     The strength of the smoothing.
 *
 * @return Non-zero on success, zero on failure. Failure: Returns zero if im is NULL or weight is invalid.
 *
 * @see gdImageConvolution @ref gdImageGaussianBlur gdImageEmboss gdImageMeanRemoval
 */
 BGD_DECLARE(int) gdImageSmooth(gdImagePtr im, float weight);

/**
 * @brief Mean removal of an image
 *
 * (see mean_removal.jpg)
 *
 * @param im The image.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageEdgeDetectQuick gdImageConvolution
 */
 BGD_DECLARE(int) gdImageMeanRemoval(gdImagePtr im);

/**
 * @brief Emboss an image
 *
 * (see emboss.jpg)
 *
 * @param im The image.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageConvolution
 */
BGD_DECLARE(int) gdImageEmboss(gdImagePtr im);

/**
 * @brief Gaussian blur of an image
* Performs a Gaussian blur of radius 1 on the
* image.  The image is modified in place.
*
* *NOTE:* You will almost certain want to use
* @ref gdImageCopyGaussianBlurred instead, as it allows you to change
* your kernel size and sigma value.  Future versions of this
* function may fall back to calling it instead of
* @ref gdImageConvolution, causing subtle changes so be warned.
*
* @param im The image to blur.
*
* @returns GD_TRUE (1) on success, GD_FALSE (0) on failure.
*
* @see @gdImageConvolution for more information on how to use convolution matrices to achieve different effects.
*/
BGD_DECLARE(int) gdImageGaussianBlur(gdImagePtr im);

/**
 * @brief Edge detection of an image
 *
 * (see edge_detect_quick.jpg)
 *
 * @param src The image.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageMeanRemoval gdImageConvolution
 */
BGD_DECLARE(int) gdImageEdgeDetectQuick(gdImagePtr src);

/**
 * @brief Selective blur of an image
 * 
 * @param src The image.
 *
 * @return Non-zero on success, zero on failure.
 */
BGD_DECLARE(int) gdImageSelectiveBlur(gdImagePtr src);

/**
 * @brief Apply a convolution matrix to an image.
 *
 * Depending on the matrix, a wide range of effects can be accomplished, e.g.
 * blurring, sharpening, embossing, and edge detection.
 *
 * @param src        The image.
 * @param filter     The 3x3 convolution matrix.
 * @param filter_div The value to divide the convoluted channel values by.
 * @param offset     The value to add to the convoluted channel values.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageEdgeDetectQuick
 * @see gdImageGaussianBlur
 * @see gdImageEmboss
 * @see gdImageMeanRemoval
 * @see gdImageSmooth
 */
BGD_DECLARE(int)
gdImageConvolution(gdImagePtr src, float filter[3][3], float filter_div, float offset);

/**
 * @brief Change channel values of an image
 *
 * @param src   The image.
 * @param red   The value to add to the red channel of all pixels.
 * @param green The value to add to the green channel of all pixels.
 * @param blue  The value to add to the blue channel of all pixels.
 * @param alpha The value to add to the alpha channel of all pixels.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageBrightness
 */
BGD_DECLARE(int)
gdImageColor(gdImagePtr src, const int red, const int green, const int blue, const int alpha);

/**
 * @brief Change the contrast of an image
 *
 * @param src      The image.
 * @param contrast The contrast adjustment value. Negative values increase, postive
 *                 values decrease the contrast. The larger the absolute value, the
 *                 stronger the effect.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageBrightness
 */
BGD_DECLARE(int) gdImageContrast(gdImagePtr src, double contrast);

/**
 * @brief Change the brightness of an image
 *
 * @param src        The image.
 * @param brightness The value to add to the color channels of all pixels.
 *
 * @return Non-zero on success, zero on failure.
 *
 * @see gdImageContrast gdImageColor
 */
BGD_DECLARE(int) gdImageBrightness(gdImagePtr src, int brightness);


/**
 * @brief Convert an image to grayscale
 *
 * The red, green and blue components of each pixel are replaced by their
 * weighted sum using the same coefficients as the REC.601 luma (Y')
 * calculation. The alpha components are retained.
 *
 * For palette images the result may differ due to palette limitations.
 *
 * @param src The image.
 *
 * @return Non-zero on success, zero on failure.
 */
BGD_DECLARE(int) gdImageGrayScale(gdImagePtr src);

/**
 * @brief Invert an image
 *
 * @param src The image.
 *
 * @return Non-zero on success, zero on failure.
 */
BGD_DECLARE(int) gdImageNegate(gdImagePtr src);

/**
 * @brief Return a copy of the source image _src_ blurred according to the parameters using the Gaussian Blur algorithm.
 * Return a copy of the source image _src_ blurred according to the
 * parameters using the Gaussian Blur algorithm.
 * 
 * _radius_ is a radius, not a diameter so a radius of 2 (for
 * example) will blur across a region 5 pixels across (2 to the
 * center, 1 for the center itself and another 2 to the other edge).
 * 
 * _sigma_ represents the "fatness of the curve (lower == fatter).
 * If _sigma_ is less than or equal to 0,
 * <gdImageCopyGaussianBlurred> ignores it and instead computes an
 * "optimal" value.  Be warned that future versions of this function
 * may compute sigma differently.
 * 
 * The resulting image is always truecolor.
 * 
 * More Details:
 * 
 * A Gaussian Blur is generated by replacing each pixel's color
 * values with the average of the surrounding pixels' colors.  This
 * region is a circle whose radius is given by argument _radius_.
 * Thus, a larger radius will yield a blurrier image.
 * 
 * This average is not a simple mean of the values.  Instead, values
 * are weighted using the Gaussian function (roughly a bell curve
 * centered around the destination pixel) giving it much more
 * influence on the result than its neighbours.  Thus, a fatter curve
 * will give the center pixel more weight and make the image less
 * blurry; lower _sigma_ values will yield flatter curves.
 * 
 * Currently, <gdImageCopyGaussianBlurred> computes the default sigma
 * as
 * 
 * (2/3)*radius
 * 
 * Note, however that we reserve the right to change this if we find
 * a better ratio.  If you absolutely need the current sigma value,
 * you should set it yourself.
 *
 * @param  src  the source image
 * @param  radius  the blur radius (*not* diameter--range is 2*radius + 1)
 * @param  sigma   the sigma value or a value <= 0.0 to use the computed default
 *
 * @return The new image or NULL if an error occurred.  The result is always truecolor.
 *
 * Example:
 * @code
 * 
 * FILE *in;
 * gdImagePtr result, src;
 * 
 * in = fopen("foo.png", "rb");
 * src = gdImageCreateFromPng(in);
 * 
 * result = gdImageCopyGaussianBlurred(im, src->sx / 10, -1.0);
 * 
 * @endcode
 */
BGD_DECLARE(gdImagePtr)
gdImageCopyGaussianBlurred(gdImagePtr src, int radius, double sigma);
/** @} */

/**
 * Group: Accessor Macros
 */

/**
 * Macro: gdImageTrueColor
 *
 * Whether an image is a truecolor image.
 *
 * Parameters:
 *   im - The image.
 *
 * Returns:
 *   Non-zero if the image is a truecolor image, zero for palette images.
 */
#define gdImageTrueColor(im) ((im)->trueColor)

/**
 * Macro: gdImageSX
 *
 * Gets the width (in pixels) of an image.
 *
 * Parameters:
 *   im - The image.
 */
#define gdImageSX(im) ((im)->sx)

/**
 * Macro: gdImageSY
 *
 * Gets the height (in pixels) of an image.
 *
 * Parameters:
 *   im - The image.
 */
#define gdImageSY(im) ((im)->sy)

/**
 * Macro: gdImageColorsTotal
 *
 * Gets the number of colors in the palette.
 *
 * This macro is only valid for palette images.
 *
 * Parameters:
 *   im - The image
 */
#define gdImageColorsTotal(im) ((im)->colorsTotal)

/**
 * Macro: gdImageRed
 *
 * Gets the red component value of a given color.
 *
 * Parameters:
 *   im - The image.
 *   c  - The color.
 */
#define gdImageRed(im, c) ((im)->trueColor ? gdTrueColorGetRed(c) : (im)->red[(c)])

/**
 * Macro: gdImageGreen
 *
 * Gets the green component value of a given color.
 *
 * Parameters:
 *   im - The image.
 *   c  - The color.
 */
#define gdImageGreen(im, c) ((im)->trueColor ? gdTrueColorGetGreen(c) : (im)->green[(c)])

/**
 * Macro: gdImageBlue
 *
 * Gets the blue component value of a given color.
 *
 * Parameters:
 *   im - The image.
 *   c  - The color.
 */
#define gdImageBlue(im, c) ((im)->trueColor ? gdTrueColorGetBlue(c) : (im)->blue[(c)])

/**
 * Macro: gdImageAlpha
 *
 * Gets the alpha component value of a given color.
 *
 * Parameters:
 *   im - The image.
 *   c  - The color.
 */
#define gdImageAlpha(im, c) ((im)->trueColor ? gdTrueColorGetAlpha(c) : (im)->alpha[(c)])

/**
 * Macro: gdImageGetTransparent
 *
 * Gets the transparent color of the image.
 *
 * Parameters:
 *   im - The image.
 *
 * See also:
 *   - <gdImageColorTransparent>
 */
#define gdImageGetTransparent(im) ((im)->transparent)

/**
 * Macro: gdImageGetInterlaced
 *
 * Whether an image is interlaced.
 *
 * Parameters:
 *   im - The image.
 *
 * Returns:
 *   Non-zero for interlaced images, zero otherwise.
 *
 * See also:
 *   - <gdImageInterlace>
 */
#define gdImageGetInterlaced(im) ((im)->interlace)

/**
 * Macro: gdImagePalettePixel
 *
 * Gets the color of a pixel.
 *
 * Calling this macro is only valid for palette images.
 * No bounds checking is done for the coordinates.
 *
 * Parameters:
 *   im - The image.
 *   x  - The x-coordinate.
 *   y  - The y-coordinate.
 *
 * See also:
 *   - <gdImageTrueColorPixel>
 *   - <gdImageGetPixel>
 */
#define gdImagePalettePixel(im, x, y) (im)->pixels[(y)][(x)]

/**
 * Macro: gdImageTrueColorPixel
 *
 * Gets the color of a pixel.
 *
 * Calling this macro is only valid for truecolor images.
 * No bounds checking is done for the coordinates.
 *
 * Parameters:
 *   im - The image.
 *   x  - The x-coordinate.
 *   y  - The y-coordinate.
 *
 * See also:
 *   - <gdImagePalettePixel>
 *   - <gdImageGetTrueColorPixel>
 */
#define gdImageTrueColorPixel(im, x, y) (im)->tpixels[(y)][(x)]

/**
 * Macro: gdImageResolutionX
 *
 * Gets the horizontal resolution in DPI.
 *
 * Parameters:
 *   im - The image.
 *
 * See also:
 *   - <gdImageResolutionY>
 *   - <gdImageSetResolution>
 */
#define gdImageResolutionX(im) (im)->res_x

/**
 * Macro: gdImageResolutionY
 *
 * Gets the vertical resolution in DPI.
 *
 * Parameters:
 *   im - The image.
 *
 * See also:
 *   - <gdImageResolutionX>
 *   - <gdImageSetResolution>
 */
#define gdImageResolutionY(im) (im)->res_y

/* I/O Support routines. */

BGD_DECLARE(gdIOCtxPtr) gdNewFileCtx(FILE *);
/* If data is null, size is ignored and an initial data buffer is
   allocated automatically. NOTE: this function assumes gd has the right
   to free or reallocate "data" at will! Also note that gd will free
   "data" when the IO context is freed. If data is not null, it must point
   to memory allocated with gdMalloc, or by a call to gdImage[something]Ptr.
   If not, see gdNewDynamicCtxEx for an alternative. */
BGD_DECLARE(gdIOCtxPtr) gdNewDynamicCtx(int size, void *data);
/* 2.0.21: if freeFlag is nonzero, gd will free and/or reallocate "data" as
   needed as described above. If freeFlag is zero, gd will never free
   or reallocate "data", which means that the context should only be used
   for *reading* an image from a memory buffer, or writing an image to a
   memory buffer which is already large enough. If the memory buffer is
   not large enough and an image write is attempted, the write operation
   will fail. Those wishing to write an image to a buffer in memory have
   a much simpler alternative in the gdImage[something]Ptr functions. */
BGD_DECLARE(gdIOCtxPtr) gdNewDynamicCtxEx(int size, void *data, int freeFlag);
BGD_DECLARE(gdIOCtxPtr) gdNewSSCtx(gdSourcePtr in, gdSinkPtr out);
BGD_DECLARE(void *) gdDPExtractData(gdIOCtxPtr ctx, int *size);

#define GD2_CHUNKSIZE 128
#define GD2_CHUNKSIZE_MIN 64
#define GD2_CHUNKSIZE_MAX 4096

#define GD2_VERS 2
#define GD2_ID "gd2"
#define GD2_FMT_RAW 1
#define GD2_FMT_COMPRESSED 2


/**
 * @defgroup imagecomparison Image Comparison
 * @{ */
#define GD_CMP_IMAGE 1 /**< Actual image IS different */
#define GD_CMP_NUM_COLORS 2 /**< Number of colors in pallette differ */
#define GD_CMP_COLOR 4 /**< Image colors differ */
#define GD_CMP_SIZE_X 8 /**< Image width differs */
#define GD_CMP_SIZE_Y 16 /**< Image heights differ */
#define GD_CMP_TRANSPARENT 32 /**< Transparent color differs */
#define GD_CMP_BACKGROUND 64 /**< Background color differs */
#define GD_CMP_INTERLACE 128 /**< Interlaced setting differs */
#define GD_CMP_TRUECOLOR 256 /**< Truecolor vs palette differs */

/**
 * @brief Compare two images
 *
 * compare two images and some of its attributes. The images must be of the same size, otherwise the function will return -1.
 * For accurate image content comparison, use @ref gdImagePerceptualDiff instead.
 * 
 * @param im1 An image.
 * @param im2 Another image.
 *
 * @return A bitmask of @ref <Image Comparison> flags where each set flag signals which attributes of the images are different.
 */
BGD_DECLARE(int) gdImageCompare(gdImagePtr im1, gdImagePtr im2);

typedef enum {
    GD_IMAGE_DIFF_NONE,
    GD_IMAGE_DIFF_OVERLAY,
    GD_IMAGE_DIFF_MASK
} gdImageDiffMode;

typedef struct {
    gdImageDiffMode mode;
    int highlight_color;
} gdImagePerceptualDiffOptions;

typedef struct {
    unsigned int pixels_changed;
    /* Largest normalized perceptual distance, in the range 0.0 to 1.0. */
    double maximum_delta;
} gdImagePerceptualDiffResult;

/*
 * Compare two equally sized images using a perceptual YIQ distance.
 *
 * A NULL options pointer selects an overlay with opaque red highlights. A
 * non-NULL diff_image receives a newly allocated truecolor image for overlay
 * and mask modes; the caller owns it and must call gdImageDestroy(). Passing
 * NULL for diff_image computes statistics only. The result is always reset,
 * including on failure.
 *
 * Returns 1 on success, or 0 for invalid arguments or allocation failure.
 */
BGD_DECLARE(int)
gdImagePerceptualDiff(gdImagePtr image1, gdImagePtr image2, double threshold,
                      const gdImagePerceptualDiffOptions *options,
                      gdImagePtr *diff_image,
                      gdImagePerceptualDiffResult *result);
/** @} */


/**
 * @defgroup TransformScaleRotate Transform, scale and rotate
 *
 * Image transformation APIs for interpolation, scaling, rotation and affine
 * mapping.
 *
 * Affine matrices use a six-element double array:
 *
 * matrix[0] == xx
 * matrix[1] == yx
 * matrix[2] == xy
 * matrix[3] == yy
 * matrix[4] == x0
 * matrix[5] == y0
 *
 * A point (x, y) is transformed as:
 *
 * x_new = xx * x + xy * y + x0
 * y_new = yx * x + yy * y + y0
 *
 * @{
 */

/**
 * @brief Flip an image vertically
 *
 * The image is mirrored upside-down.
 *
 * @param im The image.
 *
 * @see gdImageFlipHorizontal, gdImageFlipBoth
 */
BGD_DECLARE(void) gdImageFlipHorizontal(gdImagePtr im);

/**
 * @brief Flip an image horizontally
 *
 * The image is mirrored left-right.
 *
 * @param im The image.
 * @see gdImageFlipVertical, gdImageFlipBoth
 */
BGD_DECLARE(void) gdImageFlipVertical(gdImagePtr im);

/**
 * @brief Flip an image vertically and horizontally
 *
 * The image is mirrored upside-down and left-right.
 *
 * @param im The image.
 * @see gdImageFlipVertical, gdImageFlipHorizontal
 */
BGD_DECLARE(void) gdImageFlipBoth(gdImagePtr im);

/**
 * Group: Crop
 *
 * @see gdImageCropAuto gdImageCropThreshold gdCrop
 **/
enum gdCropMode {
    GD_CROP_DEFAULT = 0, /*< Same as GD_CROP_TRANSPARENT */
    GD_CROP_TRANSPARENT, /*< Crop using the transparent color */
    GD_CROP_BLACK, /*< Crop black borders */
    GD_CROP_WHITE, /*< Crop white borders */
    GD_CROP_SIDES, /*< Crop using colors of the 4 corners */
    GD_CROP_THRESHOLD /*< Crop using a threshold */
};

/**
 * @brief Crop an image to a given rectangle
 *
 * @param src The image.
 * @param crop The cropping rectangle, @ref gdRect.
 *
 * @returns The newly created cropped image, or NULL on failure.
 *
  * @see gdImageCrop gdImageCropThreshold gdImageAutoCropWithOptions
 */
BGD_DECLARE(gdImagePtr) gdImageCrop(gdImagePtr src, const gdRect *crop);


/**
 * @brief Crop an image automatically
 *
 * This function detects the cropping area according to the given _mode_.
 *
 * @param im The image.
 * @param mode The cropping mode, @ref gdCropMode.
 *
 * @returns The newly created cropped image, or NULL on failure.
 *
 * @see gdImageCrop gdImageCropThreshold gdImageAutoCropWithOptions
 */
BGD_DECLARE(gdImagePtr) gdImageCropAuto(gdImagePtr im, const unsigned int mode);


/**
 * @brief Crop an image using a given color
 *
 * The _threshold_ defines the tolerance to be used while comparing the image
 * color and the color to crop. The method used to calculate the color
 * difference is based on the color distance in the RGB(A) cube.
 *
 * @param im The image.
 * @param color The crop color.
 * @param threshold The crop threshold.
 *
 * @returns The newly created cropped image, or NULL on failure.
 *
 * @see gdImageCrop gdImageCropThreshold gdImageAutoCropWithOptions
 */
BGD_DECLARE(gdImagePtr)
gdImageCropThreshold(gdImagePtr im, const unsigned int color, const float threshold);

/**
 * @brief Options for automatic cropping
 * 
 * This structure defines the options for automatic cropping.
 */
typedef struct {
    enum gdCropMode mode; /**< The cropping mode, @see gdCropMode. */
    float threshold; /**< The crop threshold. */
    int color; /**< The crop color. */
} gdAutoCropOptions;

/**
 * @brief Crop an image automatically with options
 * 
 * This function detects the cropping area according to the given options.
 * 
 * @param src The image.
 * @param options The cropping options, @ref gdAutoCropOptions.
 * 
 * @returns The newly created cropped image, or NULL on failure.
 */
BGD_DECLARE(gdImagePtr)
gdImageAutoCropWithOptions(gdImagePtr src, const gdAutoCropOptions *options);

/**
 * @brief Set the interpolation method stored on an image.
 *
 * Scaling, rotation and affine transformation functions use this value when
 * they sample pixels from the image. Newly-created images default to
 * @ref GD_BILINEAR_FIXED. Passing @ref GD_DEFAULT is accepted and stores
 * @ref GD_LINEAR.
 *
 * Some transform APIs have optimized paths for specific methods. In
 * particular, @ref gdImageScale uses @ref GD_TRIANGLE when downscaling or doing a
 * mixed-axis scale with the fixed compatibility methods.
 *
 * Parameters:
 *   im - The image.
 *   id - The interpolation method.
 *
 * Returns:
 *   Non-zero on success, zero on failure.
 *
 * See also:
 *   - @see gdInterpolationMethod
 *   - @see gdImageGetInterpolationMethod
 */
BGD_DECLARE(int)
gdImageSetInterpolationMethod(gdImagePtr im, gdInterpolationMethod id);

/**
 * @brief Return the interpolation method currently stored on an image.
 *
 * Parameters:
 *   im - The image.
 *
 * Returns:
 *   The current interpolation method.
 *
 * See also:
 *   - @see gdInterpolationMethod
 *   - @see gdImageSetInterpolationMethod
 */
BGD_DECLARE(gdInterpolationMethod) gdImageGetInterpolationMethod(gdImagePtr im);

/**
 * @brief Scale an image to an exact width and height using the source image's current
 * @ref gdInterpolationMethod.
 *
 * The returned image is newly allocated and must be destroyed with
 * @ref gdImageDestroy. If the requested dimensions match the source dimensions,
 * this function returns a clone of the source image. Width and height must be
 * greater than zero.
 *
 * Notes:
 *   @ref GD_WEIGHTED4 is not supported by this function. For downscales and
 *   mixed-axis scales, the fixed compatibility methods are sampled with
 *   @ref GD_TRIANGLE for better filtering.
 *
 * Parameters:
 *   src        - The source image.
 *   new_width  - The requested output width.
 *   new_height - The requested output height.
 *
 * Returns:
 *   The scaled image on success, or NULL on failure.
 *
 * See also:
 *   - @see gdImageSetInterpolationMethod
 *   - @see gdImageScaleWithOptions
 *   - @see gdImageCopyResampled
 *   - @see gdImageCopyResized
 */
BGD_DECLARE(gdImagePtr)
gdImageScale(const gdImagePtr src, const unsigned int new_width, const unsigned int new_height);

/**
 * Constants: gdScaleFit
 *
 * Controls how @ref gdImageScaleWithOptions maps the source aspect ratio into the
 * requested output size.
 *
  * Defaults:
 *   When @ref gdImageScaleWithOptions receives NULL options, the fit defaults to
 *   @ref GD_SCALE_FIT_COVER.
 */
typedef enum {
    GD_SCALE_FIT_COVER,   /**< Preserve aspect ratio, fill requested size, crop overflow. */
    GD_SCALE_FIT_CONTAIN, /**< Preserve aspect ratio, fit inside requested size, pad the rest. */
    GD_SCALE_FIT_FILL,    /**< Stretch to requested size without preserving aspect ratio. */
    GD_SCALE_FIT_INSIDE,  /**< Preserve aspect ratio; output is no larger than requested size. */
    GD_SCALE_FIT_OUTSIDE  /**< Preserve aspect ratio; output is no smaller than requested size. */
} gdScaleFit;

/**
 * Chooses the anchor used when @ref gdImageScaleWithOptions pads or crops an
 * image. North and south refer to the top and bottom of the output; west and
 * east refer to the left and right.
 *
 * Defaults:
 *   When @ref gdImageScaleWithOptions receives NULL options, gravity defaults to
 *   @ref GD_SCALE_GRAVITY_CENTER.
 */
typedef enum {

    GD_SCALE_GRAVITY_NORTHWEST, /**< Anchor to the top-left corner. */
    GD_SCALE_GRAVITY_NORTH,     /**< Anchor to the top edge. */
    GD_SCALE_GRAVITY_NORTHEAST, /**< Anchor to the top-right corner. */
    GD_SCALE_GRAVITY_WEST,      /**< Anchor to the left edge. */
    GD_SCALE_GRAVITY_CENTER,    /**< Anchor to the center. */
    GD_SCALE_GRAVITY_EAST,      /**< Anchor to the right edge. */
    GD_SCALE_GRAVITY_SOUTHWEST, /**< Anchor to the bottom-left corner. */
    GD_SCALE_GRAVITY_SOUTH,     /**< Anchor to the bottom edge. */
    GD_SCALE_GRAVITY_SOUTHEAST  /**< Anchor to the bottom-right corner. */
} gdScaleGravity;

/**
 * Constants: gdScaleStrategy
 *
 * Optional crop strategy for @ref GD_SCALE_FIT_COVER in
 * @ref gdImageScaleWithOptions.
 *
 *
 * Notes:
 *   Entropy and attention strategies are valid only with
 *   @ref GD_SCALE_FIT_COVER. If a strategy cannot find an interesting crop,
 *   @ref gdImageScaleWithOptions falls back to the normal gravity-based cover
 *   crop.
 *
 * Defaults:
 *   When @ref gdImageScaleWithOptions receives NULL options, strategy defaults to
 *   @ref GD_SCALE_STRATEGY_NONE.
 */
typedef enum {
    GD_SCALE_STRATEGY_NONE,      /**< Crop using gravity only. */
    GD_SCALE_STRATEGY_ENTROPY,   /**< Prefer a high-entropy crop region. */
    GD_SCALE_STRATEGY_ATTENTION  /**< Prefer a likely visual-attention crop region. */
} gdScaleStrategy;

/**
 * Struct: gdScaleOptions
 *
 * Options for @ref gdImageScaleWithOptions.
 *
 * Members:
 *   fit              - Aspect-ratio behavior. See @ref gdScaleFit.
 *   gravity          - Crop or padding anchor. See @ref gdScaleGravity.
 *   strategy         - Optional cover-crop strategy. See @ref gdScaleStrategy.
 *   background_color - Truecolor ARGB background used for padding and for transparent palette pixels.
 *   interpolation    - A gdInterpolationMethod value (see @ref gdImageSetInterpolationMethod),
 *                      or @ref GD_SCALE_INTERPOLATION_AUTO.
 *
 * Defaults:
 *   If the options pointer passed to @ref gdImageScaleWithOptions is NULL, gd uses
 *   @ref GD_SCALE_FIT_COVER, @ref GD_SCALE_GRAVITY_CENTER,
 *   @ref GD_SCALE_STRATEGY_NONE, background color 0x7f000000 and
 *   @ref GD_SCALE_INTERPOLATION_AUTO.
 *
 * Notes:
 *   For palette sources, gd prepares a truecolor working copy. Transparent
 *   pixels are replaced with background_color before scaling; if
 *   background_color is a valid palette index, that palette entry is converted
 *   to truecolor.
 */
typedef struct {
    gdScaleFit fit;            /**< Aspect-ratio behavior. */
    gdScaleGravity gravity;    /**< Crop or padding anchor. */
    gdScaleStrategy strategy;  /**< Optional cover-crop strategy. */
    int background_color;      /**< Truecolor ARGB background used for padding and palette transparency. */
    int interpolation;         /**< Interpolation method, or @ref GD_SCALE_INTERPOLATION_AUTO. */
} gdScaleOptions;

/**
 * @brief Scale an image using aspect-ratio, gravity, crop-strategy and interpolation options.
 *
 * Scale an image using aspect-ratio, gravity, crop-strategy and interpolation
 * options.
 *
 * This is the higher-level scaling API. It can stretch, contain, cover, pad or
 * return an inside/outside size while preserving the source aspect ratio. The
 * returned image is newly allocated and must be destroyed with  @ref gdImageDestroy.
 * 
 * @param src        - The source image.
 * @param new_width  - The requested width.
 * @param new_height - The requested height.
 * @param options    - Scaling options, or NULL for the default options.
 *
 * @returns The scaled image on success, or NULL on failure. Caller owns the returned image and must call @ref gdImageDestroy when done.
 *          Returns NULL if src is NULL, either dimension is zero, interpolation is
 *          invalid, or an entropy/attention strategy is used with a fit other than @ref GD_SCALE_FIT_COVER.
 *
 * @see gdScaleOptions gdImageScale gdImageInterestingCropRegion
 */
BGD_DECLARE(gdImagePtr)
gdImageScaleWithOptions(const gdImagePtr src, const unsigned int new_width,
                        const unsigned int new_height, const gdScaleOptions *options);

/**
 * @brief  Methods used to find an interesting crop region.
 *  @see gdImageInterestingCropRegion gdImageScaleWithOptions
 */
typedef enum {
    GD_INTERESTING_ENTROPY,    /**< Prefer regions with higher image entropy. */
    GD_INTERESTING_ATTENTION   /**< Prefer regions with likely visual attention. */
} gdInterestingMethod;

/**
 * @brief Find a source crop region with the requested aspect ratio using an
 * interesting-region method.
 *
 * @param src           - The source image.
 * @param target_width  - The target width used to compute the crop aspect ratio.
 * @param target_height - The target height used to compute the crop aspect ratio.
 * @param method        - The interesting-region method.
 * @param crop          - Receives the crop rectangle on success.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 *
 * @see gdInterestingMethod gdImageEntropyCropRegion gdImageScaleWithOptions
 */
BGD_DECLARE(int)
gdImageInterestingCropRegion(const gdImagePtr src, unsigned int target_width,
                             unsigned int target_height, gdInterestingMethod method,
                             gdRectPtr crop);

/**
 * @brief Find a high-entropy source crop region with the requested aspect ratio.
 *
 * @param src           - The source image.
 * @param target_width  - The target width used to compute the crop aspect ratio.
 * @param target_height - The target height used to compute the crop aspect ratio.
 * @param crop          - Receives the crop rectangle on success.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 *
 * @see gdImageInterestingCropRegion
 */
BGD_DECLARE(int)
gdImageEntropyCropRegion(const gdImagePtr src, unsigned int target_width,
                         unsigned int target_height, gdRectPtr crop);

/**
 * @brief Rotate an image by an arbitrary angle using the source image's current @ref gdInterpolationMethod.
 *
 * The returned image is newly allocated and must be destroyed with
 * @ref gdImageDestroy. Palette sources are converted to truecolor internally for
 * arbitrary-angle rotation. Angles that are multiples of 90 degrees use the
 * optimized rotate paths.
 *
 * @param src     - The source image.
 * @param angle   - Rotation angle in degrees.
 * @param bgcolor - Background color for uncovered pixels. For palette sources, this may be a palette index.
 *
 * @returns The rotated image on success, or NULL on failure.
 *
 * @note bgcolor must be non-negative. The source interpolation method must be a valid concrete method; @ref GD_DEFAULT is not accepted by this function.
 *
 * @see gdImageSetInterpolationMethod gdImageRotate90 gdImageRotate180 gdImageRotate270
  */
BGD_DECLARE(gdImagePtr)
gdImageRotateInterpolated(const gdImagePtr src, const float angle, int bgcolor);

/**
 * @brief Standard affine matrix operations.
 */
typedef enum {
    GD_AFFINE_TRANSLATE = 0,   /**< Translation matrix. */
    GD_AFFINE_SCALE,           /**< Scale matrix. */
    GD_AFFINE_ROTATE,          /**< Rotation matrix. */
    GD_AFFINE_SHEAR_HORIZONTAL,/**< Horizontal shear matrix. */
    GD_AFFINE_SHEAR_VERTICAL   /**< Vertical shear matrix. */
} gdAffineStandardMatrix;

/**
 * @brief Apply an affine matrix to a floating-point point.
 *
 * @param dst    - Receives the transformed point.
 * @param src    - Source point.
 * @param affine - Matrix in gd's six-value affine form.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdAffineApplyToPointF(gdPointFPtr dst, const gdPointFPtr src, const double affine[6]);

/**
 * @brief Invert an affine matrix.
 *
 * @param dst - Receives the inverse matrix.
 * @param src - Source matrix.
 *
 * @returns GD_TRUE on success, or GD_FALSE if the matrix cannot be inverted.
 */
BGD_DECLARE(int) gdAffineInvert(double dst[6], const double src[6]);

/**
 * @brief Build a horizontal and/or vertical flip from an affine matrix.
 *
 * @param dst_affine - Receives the flipped matrix.
 * @param src_affine - Source matrix.
 * @param flip_h     - Non-zero to flip horizontally.
 * @param flip_v     - Non-zero to flip vertically.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdAffineFlip(double dst_affine[6], const double src_affine[6], const int flip_h, const int flip_v);

/**
 * @brief Concatenate two affine matrices.
 *
 * The result is equivalent to applying m1 and then m2. The destination may be
 * the same array as either input.
 *
 * @param dst - Receives the concatenated matrix.
 * @param m1  - First matrix.
 * @param m2  - Second matrix.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdAffineConcat(double dst[6], const double m1[6], const double m2[6]);

/**
 * @brief Store an identity affine matrix.
 *
 * @param dst - Receives the identity matrix.
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int) gdAffineIdentity(double dst[6]);

/**
 * @brief Store a scale affine matrix.
 *
 * @param dst     - Receives the scale matrix.
 * @param scale_x - Horizontal scale factor.
 * @param scale_y - Vertical scale factor.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdAffineScale(double dst[6], const double scale_x, const double scale_y);

/**
 * @brief Store a rotation affine matrix.
 *
 * In gd's image coordinate system, increasing y moves downward; positive
 * angles rotate counterclockwise in that system.
 *
 * @param dst   - Receives the rotation matrix.
 * @param angle - Rotation angle in degrees.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int) gdAffineRotate(double dst[6], const double angle);

/**
 * @brief Store a horizontal shear affine matrix.
 *
 * @param dst   - Receives the shear matrix.
 * @param angle - Shear angle in degrees.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int) gdAffineShearHorizontal(double dst[6], const double angle);

/**
 * @brief Store a vertical shear affine matrix.
 *
 * @param dst   - Receives the shear matrix.
 * @param angle - Shear angle in degrees.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int) gdAffineShearVertical(double dst[6], const double angle);

/**
 * @brief Store a translation affine matrix.
 *
 * @param dst      - Receives the translation matrix.
 * @param offset_x - Horizontal offset.
 * @param offset_y - Vertical offset.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdAffineTranslate(double dst[6], const double offset_x, const double offset_y);

/**
 * @brief Return the linear expansion factor of an affine matrix.
 *
 * This is the square root of the factor by which the matrix changes area.
 *
 * @param src - Source matrix.
 *
 * @returns The expansion factor.
 */
BGD_DECLARE(double) gdAffineExpansion(const double src[6]);

/**
 * @brief Test whether an affine matrix preserves axis-aligned rectangles.
 *
 * @param src - Source matrix.
 *
 * @returns GD_TRUE if the matrix is rectilinear, otherwise GD_FALSE.
 */
BGD_DECLARE(int) gdAffineRectilinear(const double src[6]);

/**
 * @brief Compare two affine matrices.
 *
 * @param matrix1 - First matrix.
 * @param matrix2 - Second matrix.
 *
 * @returns GD_TRUE if the matrices are equal within gd's affine tolerance, otherwise
 *          GD_FALSE.
 */
BGD_DECLARE(int)
gdAffineEqual(const double matrix1[6], const double matrix2[6]);

/**
 * @brief Apply an affine transform to a source region and create an image containing
 * the complete transformed result.
 *
 * The new image is truecolor with alpha saving enabled. Areas not covered by
 * the transformed source are transparent. The source image's current
 * @ref gdInterpolationMethod controls sampling. Palette sources may be converted
 * to truecolor internally.
 *
 * @param dst      - Receives the newly-created destination image.
 * @param src      - Source image.
 * @param src_area - Source rectangle, or NULL to transform the full image.
 * @param affine   - Matrix in gd's six-value affine form.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure. On failure, *dst is NULL.
 *
 * @see gdTransformAffineCopy gdTransformAffineBoundingBox gdImageSetInterpolationMethod
 */
BGD_DECLARE(int)
gdTransformAffineGetImage(gdImagePtr *dst, const gdImagePtr src, gdRectPtr src_area,
                          const double affine[6]);

/**
 * @brief Apply an affine transform to a source region and copy the transformed pixels
 * into an existing destination image.
 *
 * The source image's current @ref gdInterpolationMethod controls sampling.
 * Transparent samples are skipped. Destination bounds and alpha-blending
 * settings are honored.
 *
 * @param dst        - Destination image.
 * @param dst_x      - Destination x offset.
 * @param dst_y      - Destination y offset.
 * @param src        - Source image.
 * @param src_region - Source rectangle to transform.
 * @param affine     - Matrix in gd's six-value affine form.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 *
 * @see gdTransformAffineGetImage  gdTransformAffineBoundingBox gdImageSetInterpolationMethod
 */
BGD_DECLARE(int)
gdTransformAffineCopy(gdImagePtr dst, int dst_x, int dst_y, const gdImagePtr src,
                      gdRectPtr src_region, const double affine[6]);
/*
gdTransformAffineCopy(gdImagePtr dst, int x0, int y0, int x1, int y1,
                          const gdImagePtr src, int src_width, int src_height,
                          const double affine[6]);
*/
/**
 * @brief Compute the bounding box of a source rectangle after applying an affine
 * transform.
 *
 * @param src    - Source rectangle.
 * @param affine - Matrix in gd's six-value affine form.
 * @param bbox   - Receives the transformed bounding box.
 *
 * @returns GD_TRUE on success, or GD_FALSE on failure.
 */
BGD_DECLARE(int)
gdTransformAffineBoundingBox(gdRectPtr src, const double affine[6], gdRectPtr bbox);

/** @} */

/* resolution affects ttf font rendering, particularly hinting */
#define GD_RESOLUTION 96 /* pixels per inch */

/* Version information functions */
BGD_DECLARE(int) gdMajorVersion(void);
BGD_DECLARE(int) gdMinorVersion(void);
BGD_DECLARE(int) gdReleaseVersion(void);
BGD_DECLARE(const char *) gdExtraVersion(void);
BGD_DECLARE(const char *) gdVersionString(void);

/* newfangled special effects */
#include "gdfx.h"

#ifdef __cplusplus
}
#endif

#endif /* GD_H */
