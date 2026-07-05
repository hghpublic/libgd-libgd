#include "gd_vector2d_private.h"
#include "gd_array.h"
#include "gd_path.h"
#include "gd_path_dash.h"
#include "gd_path_matrix.h"
#include "gdtest.h"

#include <math.h>

static gdPathOps op_at(gdPathPtr path, unsigned int index)
{
    return *(gdPathOpsPtr)gdArrayIndex(&path->elements, index);
}

static gdPointF point_at(gdPathPtr path, unsigned int index)
{
    return *(gdPointFPtr)gdArrayIndex(&path->points, index);
}

static void assert_point(gdPathPtr path, unsigned int index, double x, double y)
{
    gdPointF point = point_at(path, index);
    gdTestAssert(fabs(point.x - x) < 1e-12);
    gdTestAssert(fabs(point.y - y) < 1e-12);
}

static void test_path_representation(void)
{
    gdPathPtr path = gdPathCreate();
    gdPathClose(path);
    gdTestAssert(gdArrayNumElements(&path->elements) == 0);
    gdTestAssert(gdArrayNumElements(&path->points) == 0);

    gdPathMoveTo(path, 1, 2);
    gdPathLineTo(path, 3, 4);
    gdPathQuadTo(path, 5, 6, 7, 8);
    gdPathCurveTo(path, 9, 10, 11, 12, 13, 14);
    gdPathClose(path);
    gdPathClose(path);

    gdTestAssert(gdArrayNumElements(&path->elements) == 5);
    gdTestAssert(gdArrayNumElements(&path->points) == 8);
    gdTestAssert(path->contours == 1);
    gdTestAssert(op_at(path, 0) == gdPathOpsMoveTo);
    gdTestAssert(op_at(path, 1) == gdPathOpsLineTo);
    gdTestAssert(op_at(path, 2) == gdPathOpsQuadTo);
    gdTestAssert(op_at(path, 3) == gdPathOpsCubicTo);
    gdTestAssert(op_at(path, 4) == gdPathOpsClose);
    assert_point(path, 0, 1, 2);
    assert_point(path, 1, 3, 4);
    assert_point(path, 2, 5, 6);
    assert_point(path, 3, 7, 8);
    assert_point(path, 4, 9, 10);
    assert_point(path, 5, 11, 12);
    assert_point(path, 6, 13, 14);
    assert_point(path, 7, 1, 2);

    gdPathMoveTo(path, -2, -3);
    gdPathLineTo(path, -4, -5);
    gdTestAssert(path->contours == 2);
    gdTestAssert(gdArrayNumElements(&path->elements) == 7);
    gdTestAssert(gdArrayNumElements(&path->points) == 10);
    gdPathDestroy(path);
}

static void test_relative_path_operations(void)
{
    gdPathPtr path = gdPathCreate();

    gdPathRelMoveTo(path, 2, 3);
    gdPathRelLineTo(path, 4, 5);
    gdPathRelQuadTo(path, 1, 2, 3, 4);
    gdPathRelCurveTo(path, -1, -2, 5, 6, 7, 8);
    gdPathClose(path);
    gdPathRelLineTo(path, 10, 20);

    gdTestAssert(gdArrayNumElements(&path->elements) == 6);
    gdTestAssert(gdArrayNumElements(&path->points) == 9);
    gdTestAssert(path->contours == 1);
    gdTestAssert(op_at(path, 0) == gdPathOpsMoveTo);
    gdTestAssert(op_at(path, 1) == gdPathOpsLineTo);
    gdTestAssert(op_at(path, 2) == gdPathOpsQuadTo);
    gdTestAssert(op_at(path, 3) == gdPathOpsCubicTo);
    gdTestAssert(op_at(path, 4) == gdPathOpsClose);
    gdTestAssert(op_at(path, 5) == gdPathOpsLineTo);
    assert_point(path, 0, 2, 3);
    assert_point(path, 1, 6, 8);
    assert_point(path, 2, 7, 10);
    assert_point(path, 3, 9, 12);
    assert_point(path, 4, 8, 10);
    assert_point(path, 5, 14, 18);
    assert_point(path, 6, 16, 20);
    assert_point(path, 7, 2, 3);
    assert_point(path, 8, 12, 23);

    gdPathDestroy(path);
}

static void test_public_path_helpers(void)
{
    gdPathPtr empty = gdPathCreate();
    gdPathPtr empty_copy = gdPathDuplicate(empty);
    gdPathPtr path = gdPathCreate();
    gdPathPtr copy;

    gdTestAssert(empty_copy != NULL);
    gdTestAssert(gdArrayNumElements(&empty_copy->elements) == 0);
    gdTestAssert(gdPathDuplicate(NULL) == NULL);

    gdPathRectangle(path, 2, 3, 10, 20);
    gdTestAssert(path->contours == 1);
    gdTestAssert(gdArrayNumElements(&path->elements) == 6);
    gdTestAssert(op_at(path, 5) == gdPathOpsClose);
    assert_point(path, 0, 2, 3);
    assert_point(path, 2, 12, 23);

    copy = gdPathDuplicate(path);
    gdTestAssert(copy != NULL);
    gdPathLineTo(copy, 99, 100);
    gdTestAssert(gdArrayNumElements(&path->elements) == 6);
    gdTestAssert(gdArrayNumElements(&copy->elements) == 7);
    assert_point(path, 0, 2, 3);
    assert_point(copy, 0, 2, 3);

    gdPathDestroy(copy);
    gdPathDestroy(path);
    gdPathDestroy(empty_copy);
    gdPathDestroy(empty);
}

static void test_public_arcs(void)
{
    gdPathPtr positive = gdPathCreate();
    gdPathPtr negative = gdPathCreate();
    gdPathPtr tangent = gdPathCreate();
    unsigned int before;

    gdPathArc(positive, 10, 10, 5, 0, M_PI / 2);
    gdPathNegativeArc(negative, 10, 10, 5, 0, -M_PI / 2);
    gdTestAssert(gdArrayNumElements(&positive->elements) > 1);
    gdTestAssert(gdArrayNumElements(&negative->elements) > 1);
    gdTestAssert(op_at(positive, 0) == gdPathOpsMoveTo);
    gdTestAssert(op_at(negative, 0) == gdPathOpsMoveTo);
    assert_point(positive, 0, 15, 10);
    assert_point(negative, 0, 15, 10);
    assert_point(positive, gdArrayNumElements(&positive->points) - 1, 10, 15);
    assert_point(negative, gdArrayNumElements(&negative->points) - 1, 10, 5);

    gdPathMoveTo(tangent, 0, 0);
    gdPathArcTo(tangent, 10, 0, 10, 10, 2);
    gdTestAssert(op_at(tangent, 1) == gdPathOpsLineTo);
    assert_point(tangent, 1, 8, 0);
    before = gdArrayNumElements(&tangent->elements);
    gdPathArcTo(tangent, 20, 7, 20, 7, 2);
    gdTestAssert(gdArrayNumElements(&tangent->elements) == before + 1);
    assert_point(tangent, gdArrayNumElements(&tangent->points) - 1, 20, 7);
    gdPathArcTo(tangent, 30, 11, 40, 15, 0);
    assert_point(tangent, gdArrayNumElements(&tangent->points) - 1, 30, 11);

    gdPathDestroy(tangent);
    gdPathDestroy(negative);
    gdPathDestroy(positive);
}

static void test_dash_structure(void)
{
    const double values[] = {10, 5};
    gdPathDashPtr dash = gdPathDashCreate(values, 2, 0);
    gdPathPtr path = gdPathCreate();
    gdPathMoveTo(path, 0, 0);
    gdPathLineTo(path, 30, 0);

    gdPathPtr dashed = gdPathApplyDash(dash, path);
    gdTestAssert(dashed != NULL);
    gdTestAssert(gdArrayNumElements(&dashed->elements) == 4);
    gdTestAssert(op_at(dashed, 0) == gdPathOpsMoveTo);
    gdTestAssert(op_at(dashed, 1) == gdPathOpsLineTo);
    gdTestAssert(op_at(dashed, 2) == gdPathOpsMoveTo);
    gdTestAssert(op_at(dashed, 3) == gdPathOpsLineTo);
    assert_point(dashed, 0, 0, 0);
    assert_point(dashed, 1, 10, 0);
    assert_point(dashed, 2, 15, 0);
    assert_point(dashed, 3, 25, 0);

    gdPathDestroy(dashed);
    gdPathDestroy(path);
    gdPathDashDestroy(dash);
    gdTestAssert(gdPathDashCreate(NULL, 0, 0) == NULL);
}

static void path_bounds(gdPathPtr path, double *min_x, double *min_y,
                        double *max_x, double *max_y)
{
    unsigned int count = gdArrayNumElements(&path->points);
    gdTestAssert(count > 0);
    gdPointF point = point_at(path, 0);
    *min_x = *max_x = point.x;
    *min_y = *max_y = point.y;
    for (unsigned int i = 1; i < count; i++) {
        point = point_at(path, i);
        gdTestAssert(isfinite(point.x) && isfinite(point.y));
        if (point.x < *min_x) *min_x = point.x;
        if (point.x > *max_x) *max_x = point.x;
        if (point.y < *min_y) *min_y = point.y;
        if (point.y > *max_y) *max_y = point.y;
    }
}

static void test_stroke_caps_and_matrix(void)
{
    gdPathPtr path = gdPathCreate();
    gdPathMoveTo(path, 20, 20);
    gdPathLineTo(path, 20, 40);

    gdStroke stroke = {30, 4, gdLineCapRound, gdLineJoinRound, NULL};
    gdPathMatrix matrix;
    gdPathMatrixInitIdentity(&matrix);
    gdPathPtr round = gdPathStrokeToPath(path, &stroke, &matrix);
    gdTestAssert(round != NULL);

    double min_x, min_y, max_x, max_y;
    path_bounds(round, &min_x, &min_y, &max_x, &max_y);
    gdTestAssert(fabs(min_x - 5.0) <= 1.0 / 64.0);
    gdTestAssert(fabs(max_x - 35.0) <= 1.0 / 64.0);
    gdTestAssert(fabs(min_y - 5.0) <= 1.0 / 64.0);
    gdTestAssert(fabs(max_y - 55.0) <= 1.0 / 64.0);
    gdTestAssert(fabs((min_x + max_x) / 2.0 - 20.0) <= 1.0 / 64.0);

    gdPathDestroy(round);
    for (int cap = gdLineCapButt; cap <= gdLineCapSquare; cap++) {
        for (int join = gdLineJoinMiter; join <= gdLineJoinBevel; join++) {
            stroke.cap = (gdLineCap)cap;
            stroke.join = (gdLineJoin)join;
            gdPathMatrixInit(&matrix, 2, 0, 0, 1.5, 7, -3);
            gdPathPtr stroked = gdPathStrokeToPath(path, &stroke, &matrix);
            gdTestAssert(stroked != NULL);
            gdTestAssert(gdArrayNumElements(&stroked->elements) > 0);
            path_bounds(stroked, &min_x, &min_y, &max_x, &max_y);
            gdPathDestroy(stroked);
        }
    }
    gdPathDestroy(path);
}

int main(void)
{
    test_path_representation();
    test_relative_path_operations();
    test_public_path_helpers();
    test_public_arcs();
    test_dash_structure();
    test_stroke_caps_and_matrix();
    return gdNumFailures();
}
