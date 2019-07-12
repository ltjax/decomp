
#include <decomp/triangulation.hpp>
#include <catch2/catch.hpp>

using namespace decomp;

TEST_CASE("remove holes from polygon")
{
    std::vector<Point> pointList = { { -2, -2 }, { 2, -2 }, { 2, 2 }, { -2, 2 },
                                     { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } };

    std::vector<std::uint16_t> outerPolygon = { 0, 1, 2, 3 };
    std::vector<std::uint16_t> innerPolygon = { 7, 6, 5, 4 };

    auto mergedPolygon = removeHoles(pointList, outerPolygon, { innerPolygon });

    // 2 x 4 vertices + 2 for the connection-edge
    REQUIRE(mergedPolygon.size() == 10);

    std::vector<std::uint16_t> correctPolygon = { 2, 3, 0, 1, 2, 6, 5, 4, 7, 6 };
    REQUIRE(mergedPolygon == correctPolygon);
}
