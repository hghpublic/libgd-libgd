#include "gd_vector2d.h"
#include "gdtest.h"

#include <math.h>

int main(void)
{
	gdImagePtr image = gdImageCreateTrueColor(80, 80);
	gdContextPtr context = NULL;
	gdPathPtr path = gdPathCreate();
	gdPathPtr copy;
	gdPathMatrix matrix;

	gdTestAssert(image != NULL && path != NULL);
	if (!image || !path) {
		gdPathDestroy(path);
		gdImageDestroy(image);
		return gdNumFailures();
	}

	gdPathRectangle(path, 5, 5, 20, 15);
	gdPathMoveTo(path, 10, 40);
	gdPathArcTo(path, 30, 40, 30, 60, 8);
	gdPathArc(path, 50, 20, 10, 0, 3.14159265358979323846);
	gdPathNegativeArc(path, 50, 50, 10, 0, -3.14159265358979323846);
	copy = gdPathDuplicate(path);
	gdTestAssert(copy != NULL);

	context = gdContextCreateForImage(image);
	gdTestAssert(context != NULL);
	if (context && copy) {
		gdPathMatrixInitTranslate(&matrix, 3, 2);
		gdPathTransform(copy, &matrix);
		gdContextAppendPath(context, copy);
		gdContextSetSourceRgb(context, 0.2, 0.5, 0.8);
		gdContextSetLineWidth(context, 2);
		gdContextStroke(context);
		gdContextFlushImage(context);
	}

	gdContextDestroy(context);
	gdPathDestroy(copy);
	gdPathDestroy(path);
	gdImageDestroy(image);
	return gdNumFailures();
}
