
#include "../triangulation.hpp"

using namespace decomp;

int main()
{
    std::vector<Point> pointList = { { -2, -2 }, { 2, -2 }, { 2, 2 }, { -2, 2 },
                                     { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } };

    std::vector<std::uint16_t> outerPolygon = { 0, 1, 2, 3 };
    std::vector<std::uint16_t> innerPolygon = { 7, 6, 5, 4 };

    auto mergedPolygon = removeHoles(pointList, outerPolygon, { innerPolygon });

    if (mergedPolygon.size() != 10)
        return -1;

    std::vector<std::uint16_t> correctPolygon = { 2, 3, 0, 1, 2, 6, 5, 4, 7, 6 };
    if (!std::equal(correctPolygon.begin(), correctPolygon.end(), mergedPolygon.begin()))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}