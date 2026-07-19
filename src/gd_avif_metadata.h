#ifndef GD_AVIF_METADATA_H
#define GD_AVIF_METADATA_H

#include "gd.h"

#ifdef HAVE_LIBAVIF
#include <avif/avif.h>

int gdAvifGetInfoPtr(int size, const void *data, gdAvifInfo *info);
int gdAvifGetInfoCtx(gdIOCtxPtr in, gdAvifInfo *info);
int gdAvifGetInfo(FILE *inFile, gdAvifInfo *info);
int gdAvifApplyMetadata(avifImage *image, const gdImageMetadata *metadata);
#endif

#endif
