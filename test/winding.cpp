#include <decomp/triangulation.hpp>
#include <catch2/catch.hpp>

using namespace decomp;

TEST_CASE("can compute winding")
{
    std::vector<Point> pointList = { { -2, -2 }, { 2, -2 }, { 2, 2 }, { -2, 2 } };


    std::vector<std::uint16_t> cwPolygon = { 3, 2, 1, 0 };

    SECTION("of counter-clockwise polygon")
    {
      std::vector<std::uint16_t> ccwPolygon = { 0, 1, 2, 3 };
      REQUIRE(computeWinding(pointList, ccwPolygon) == Winding::CounterClockwise);
    }

    SECTION("of clockwise polygon")
    {
      REQUIRE(computeWinding(pointList, cwPolygon) == Winding::Clockwise);
    }
}
