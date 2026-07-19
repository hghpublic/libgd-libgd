#ifndef GD_HEIF_METADATA_H
#define GD_HEIF_METADATA_H

#include "gd.h"

#ifdef HAVE_LIBHEIF
#include <libheif/heif.h>

int gdHeifGetInfoPtr(int size, const void *data, gdHeifInfo *info);
int gdHeifGetInfoCtx(gdIOCtxPtr in, gdHeifInfo *info);
int gdHeifGetInfo(FILE *inFile, gdHeifInfo *info);
int gdHeifApplyMetadata(struct heif_context *context, struct heif_image *image,
                        struct heif_image_handle *handle, const gdImageMetadata *metadata);
#endif

#endif
