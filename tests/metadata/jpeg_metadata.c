#include <string.h>

#include "gd.h"
#include "gdtest.h"

static void fill_icc(unsigned char *icc, size_t size) {
	size_t i;

	for (i = 0; i < size; i++) {
		icc[i] = (unsigned char)(i & 0xff);
	}
}

static void assert_profile_equals(gdImageMetadata *metadata, const char *key,
								  const unsigned char *expected,
								  size_t expected_size) {
	const unsigned char *actual;
	size_t actual_size;

	actual = gdImageMetadataGetProfile(metadata, key, &actual_size);
	gdTestAssert(actual != NULL);
	gdTestAssert(actual_size == expected_size);
	if (actual != NULL && actual_size == expected_size) {
		gdTestAssert(memcmp(actual, expected, expected_size) == 0);
	}
}

int main(void) {
	static const unsigned char exif[] = {'M', 'M', 0, 42, 0, 0, 0, 8};
	static const unsigned char xmp[] =
		"http://ns.adobe.com/xap/1.0/\0<x:xmpmeta>gd</x:xmpmeta>";
	static const unsigned char iptc[] = "Photoshop 3.0\0gd-iptc";
	unsigned char icc[70000];
	gdImagePtr im;
	gdImagePtr decoded;
	gdImageMetadata *metadata;
	gdImageMetadata *decoded_metadata;
	void *jpeg;
	int size = 0;
	int white;
	gdJpegWriteOptions options;

	fill_icc(icc, sizeof(icc));

	metadata = gdImageMetadataCreate();
	decoded_metadata = gdImageMetadataCreate();
	gdTestAssert(metadata != NULL);
	gdTestAssert(decoded_metadata != NULL);

	gdTestAssert(gdImageMetadataSetProfile(metadata, "exif", exif,
										   sizeof(exif)) == GD_META_OK);
	gdTestAssert(gdImageMetadataSetProfile(metadata, "xmp", xmp, sizeof(xmp)) ==
				 GD_META_OK);
	gdTestAssert(gdImageMetadataSetProfile(metadata, "iptc", iptc,
										   sizeof(iptc)) == GD_META_OK);
	gdTestAssert(gdImageMetadataSetProfile(metadata, "icc", icc, sizeof(icc)) ==
				 GD_META_OK);

	im = gdImageCreateTrueColor(8, 8);
	gdTestAssert(im != NULL);
	white = gdTrueColor(255, 255, 255);
	gdImageFilledRectangle(im, 0, 0, 7, 7, white);

	gdJpegWriteOptionsInit(&options);
	options.quality = 90;
	options.metadata = metadata;
	jpeg = gdImageJpegPtrWithOptions(im, &size, &options);
	gdTestAssert(jpeg != NULL);
	gdTestAssert(size > 0);

	gdTestAssert(gdJpegGetMetadataPtr(size, jpeg, decoded_metadata) == 0);
	decoded = gdImageCreateFromJpegPtr(size, jpeg);
	gdTestAssert(decoded != NULL);

	assert_profile_equals(decoded_metadata, "exif", exif, sizeof(exif));
	assert_profile_equals(decoded_metadata, "xmp", xmp, sizeof(xmp));
	assert_profile_equals(decoded_metadata, "iptc", iptc, sizeof(iptc));
	gdTestAssert(gdImageMetadataGetProfile(decoded_metadata, "icc", NULL) == NULL);

	gdFree(jpeg);
	gdImageDestroy(decoded);
	gdImageDestroy(im);
	gdImageMetadataFree(decoded_metadata);
	gdImageMetadataFree(metadata);

	return gdNumFailures();
}
