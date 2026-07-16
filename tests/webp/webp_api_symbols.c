#include "gd.h"
#include "gdtest.h"

int main(void)
{
	gdWebpReadOptions readOptions;
	gdWebpWriteOptions writeOptions;
	void (*readOptionsInit)(gdWebpReadOptions *) = gdWebpReadOptionsInit;
	void (*writeOptionsInit)(gdWebpWriteOptions *) = gdWebpWriteOptionsInit;
	int (*isAnimated)(FILE *) = gdWebpIsAnimated;
	int (*isAnimatedCtx)(gdIOCtxPtr) = gdWebpIsAnimatedCtx;
	int (*isAnimatedPtr)(int, void *) = gdWebpIsAnimatedPtr;
	gdWebpReadPtr (*readOpen)(FILE *, const gdWebpReadOptions *) =
		gdWebpReadOpen;
	gdWebpReadPtr (*readOpenCtx)(gdIOCtxPtr, const gdWebpReadOptions *) =
		gdWebpReadOpenCtx;
	gdWebpReadPtr (*readOpenPtr)(int, void *, const gdWebpReadOptions *) =
		gdWebpReadOpenPtr;
	int (*readGetInfo)(gdWebpReadPtr, gdWebpInfo *) = gdWebpReadGetInfo;
	int (*readNextFrame)(gdWebpReadPtr, gdWebpFrameInfo *, gdImagePtr *) =
		gdWebpReadNextFrame;
	int (*readNextImage)(gdWebpReadPtr, gdWebpFrameInfo *, gdImagePtr *) =
		gdWebpReadNextImage;
	void (*readClose)(gdWebpReadPtr) = gdWebpReadClose;
	gdWebpWritePtr (*writeOpen)(FILE *, const gdWebpWriteOptions *) =
		gdWebpWriteOpen;
	gdWebpWritePtr (*writeOpenCtx)(gdIOCtxPtr, const gdWebpWriteOptions *) =
		gdWebpWriteOpenCtx;
	gdWebpWritePtr (*writeOpenPtr)(const gdWebpWriteOptions *) =
		gdWebpWriteOpenPtr;
	int (*writeAddImage)(gdWebpWritePtr, gdImagePtr, int) =
		gdWebpWriteAddImage;
	void (*writeClose)(gdWebpWritePtr) = gdWebpWriteClose;
	void *(*writePtrFinish)(gdWebpWritePtr, int *) = gdWebpWritePtrFinish;

	gdWebpReadOptionsInit(&readOptions);
	gdWebpWriteOptionsInit(&writeOptions);

	gdTestAssert(readOptions.struct_size == sizeof(readOptions));
	gdTestAssert(readOptions.coalesced == 1);
	gdTestAssert(writeOptions.struct_size == sizeof(writeOptions));
	gdTestAssert(writeOptions.quality == -1);
	gdTestAssert(writeOptions.method == -1);

	gdTestAssert(readOptionsInit != NULL);
	gdTestAssert(writeOptionsInit != NULL);
	gdTestAssert(isAnimated != NULL);
	gdTestAssert(isAnimatedCtx != NULL);
	gdTestAssert(isAnimatedPtr != NULL);
	gdTestAssert(readOpen != NULL);
	gdTestAssert(readOpenCtx != NULL);
	gdTestAssert(readOpenPtr != NULL);
	gdTestAssert(readGetInfo != NULL);
	gdTestAssert(readNextFrame != NULL);
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
