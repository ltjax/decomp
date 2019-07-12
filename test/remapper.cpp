#include <decomp/operations.hpp>
#include <catch2/catch.hpp>

using namespace decomp;

TEST_CASE("Can remap indices and points")
{
    Remapper remapper;

    auto newIndices = remapper.apply({1,4,5});
    REQUIRE(newIndices == IndexList{0, 1, 2});

    auto points = PointList{{4.f, 4.f}, {8.f, 8.f}, {15.f, 15.f}, {16.f, 16.f}, {23.f, 23.f}, {42.f, 42.f}};
    auto newPoints = remapper.mapped(points);
    REQUIRE(newPoints == PointList{{8.f, 8.f}, {23.f, 23.f}, {42.f, 42.f}});
}
