
#include "../triangulation.hpp"

using namespace decomp;

int main()
{
    std::vector<Point> pointList = { { -2, -2 }, { 2, -2 }, { 2, 2 }, { -2, 2 } };

    std::vector<std::uint16_t> ccwPolygon = { 0, 1, 2, 3 };
    std::vector<std::uint16_t> cwPolygon = { 3, 2, 1, 0 };

    if (computeWinding(pointList, ccwPolygon) != Winding::CounterClockwise)
        return EXIT_FAILURE;

    if (computeWinding(pointList, cwPolygon) != Winding::Clockwise)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}