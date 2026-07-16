#include "gd.h"
#include "gdtest.h"

int main(void)
{
	gdTiffReadOptions readOptions;
	gdTiffWriteOptions writeOptions;
	void (BGD_STDCALL *readOptionsInit)(gdTiffReadOptions *) = gdTiffReadOptionsInit;
	void (BGD_STDCALL *writeOptionsInit)(gdTiffWriteOptions *) = gdTiffWriteOptionsInit;
	int (BGD_STDCALL *isMultiPage)(FILE *) = gdTiffIsMultiPage;
	int (BGD_STDCALL *isMultiPageCtx)(gdIOCtxPtr) = gdTiffIsMultiPageCtx;
	int (BGD_STDCALL *isMultiPagePtr)(int, void *) = gdTiffIsMultiPagePtr;
	gdTiffReadPtr (BGD_STDCALL *readOpen)(FILE *, const gdTiffReadOptions *) =
		gdTiffReadOpen;
	gdTiffReadPtr (BGD_STDCALL *readOpenCtx)(gdIOCtxPtr, const gdTiffReadOptions *) =
		gdTiffReadOpenCtx;
	gdTiffReadPtr (BGD_STDCALL *readOpenPtr)(int, void *, const gdTiffReadOptions *) =
		gdTiffReadOpenPtr;
	int (BGD_STDCALL *readGetInfo)(gdTiffReadPtr, gdTiffInfo *) = gdTiffReadGetInfo;
	int (BGD_STDCALL *readNextImage)(gdTiffReadPtr, gdTiffPageInfo *, gdImagePtr *) =
		gdTiffReadNextImage;
	void (BGD_STDCALL *readClose)(gdTiffReadPtr) = gdTiffReadClose;
	gdTiffWritePtr (BGD_STDCALL *writeOpen)(FILE *, const gdTiffWriteOptions *) =
		gdTiffWriteOpen;
	gdTiffWritePtr (BGD_STDCALL *writeOpenCtx)(gdIOCtxPtr, const gdTiffWriteOptions *) =
		gdTiffWriteOpenCtx;
	gdTiffWritePtr (BGD_STDCALL *writeOpenPtr)(const gdTiffWriteOptions *) =
		gdTiffWriteOpenPtr;
	int (BGD_STDCALL *writeAddImage)(gdTiffWritePtr, gdImagePtr) = gdTiffWriteAddImage;
	void (BGD_STDCALL *writeClose)(gdTiffWritePtr) = gdTiffWriteClose;
	void *(BGD_STDCALL *writePtrFinish)(gdTiffWritePtr, int *) = gdTiffWritePtrFinish;

	gdTiffReadOptionsInit(&readOptions);
	gdTiffWriteOptionsInit(&writeOptions);

	gdTestAssert(readOptions.notused == 0);
	gdTestAssert(writeOptions.bitDepth == 8);
	gdTestAssert(writeOptions.colorspace == GD_TIFF_RGBA);
	gdTestAssert(writeOptions.compression == GD_TIFF_COMPRESSION_ADOBE_DEFLATE);
	gdTestAssert(writeOptions.resolutionUnit == GD_TIFF_RESUNIT_INCH);
	gdTestAssert(writeOptions.xResolution == 72.0f);
	gdTestAssert(writeOptions.yResolution == 72.0f);
	gdTestAssert(writeOptions.alphaType == GD_TIFF_ALPHA_UNASSOCIATED);

	gdTestAssert(readOptionsInit != NULL);
	gdTestAssert(writeOptionsInit != NULL);
	gdTestAssert(isMultiPage != NULL);
	gdTestAssert(isMultiPageCtx != NULL);
	gdTestAssert(isMultiPagePtr != NULL);
	gdTestAssert(readOpen != NULL);
	gdTestAssert(readOpenCtx != NULL);
	gdTestAssert(readOpenPtr != NULL);
	gdTestAssert(readGetInfo != NULL);
	gdTestAssert(readNextImage != NULL);
	gdTestAssert(readClose != NULL);
	gdTestAssert(writeOpen != NULL);
	gdTestAssert(writeOpenCtx != NULL);
	gdTestAssert(writeOpenPtr != NULL);
	gdTestAssert(writeAddImage != NULL);
	gdTestAssert(writeClose != NULL);
	gdTestAssert(writePtrFinish != NULL);

	return gdNumFailures();
}
