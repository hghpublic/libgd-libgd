#include "vector2d_example.h"

#include <stdio.h>

int main(void)
{
	gdImagePtr image = vector2d_create_image(320, 200,
		gdTrueColorAlpha(255, 255, 255, 0));
	gdContextPtr context;
	gdPathPtr path;
	gdPathPtr copy;
	gdPathMatrix matrix;

	if (!image) {
		fprintf(stderr, "Cannot create image\n");
		return 1;
	}

	context = gdContextCreateForImage(image);
	path = gdPathCreate();
	if (!context || !path) {
		gdContextDestroy(context);
		gdPathDestroy(path);
		gdImageDestroy(image);
		return 1;
	}

	gdPathRectangle(path, 20, 20, 100, 70);
	gdPathMoveTo(path, 30, 140);
	gdPathArcTo(path, 80, 140, 80, 100, 20);
	gdPathArc(path, 80, 100, 20, 0, 3.14159265358979323846);
	gdPathNegativeArc(path, 80, 100, 10, 0, -3.14159265358979323846);

	copy = gdPathDuplicate(path);
	if (!copy) {
		gdPathDestroy(path);
		gdContextDestroy(context);
		gdImageDestroy(image);
		return 1;
	}
	gdPathMatrixInitTranslate(&matrix, 160, 0);
	gdPathTransform(copy, &matrix);

	gdContextAppendPath(context, path);
	gdContextAppendPath(context, copy);
	gdContextSetSourceRgba(context, 0.15, 0.45, 0.85, 0.75);
	gdContextSetLineWidth(context, 5);
	gdContextStroke(context);
	gdContextFlushImage(context);

	vector2d_save_png(image, "path_api.png");
	gdPathDestroy(copy);
	gdPathDestroy(path);
	gdContextDestroy(context);
	gdImageDestroy(image);
	return 0;
}
