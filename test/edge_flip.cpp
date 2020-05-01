#include <decomp/convex_decomposition.hpp>
#include <catch2/catch.hpp>

using namespace decomp;

TEST_CASE("flip edges for better triangles")
{
    std::vector<Point> pointList = { { -3.0, 0.0 }, { 0.0, -1.0 }, { 3.0, 0.0 }, { 0.0, 1.0 } };
    std::vector<std::uint16_t> triangleList = { 0, 1, 2, 0, 2, 3 };

    auto graph = buildHalfEdgeGraph(triangleList, {});
    edgeFlip(pointList, graph);

    auto one = std::find_if(graph.begin(), graph.end(), [](auto const& edge) { return edge->vertex == 0; });
    REQUIRE(one != graph.end());

    auto edgeOne = one->get();
    REQUIRE(edgeOne->vertex == 0);
    REQUIRE(edgeOne->next->vertex == 1);
    REQUIRE(edgeOne->next->next->vertex == 3);

    auto two = std::find_if(graph.begin(), graph.end(), [](auto const& edge) { return edge->vertex == 2; });
    REQUIRE(two != graph.end());

    auto edgeTwo = two->get();
    REQUIRE(edgeTwo->vertex == 2);
    REQUIRE(edgeTwo->next->vertex == 3);
    REQUIRE(edgeTwo->next->next->vertex == 1);
}

TEST_CASE("do not flip edges that are fixed")
{
    std::vector<Point> pointList = { { -3.0, 0.0 }, { 0.0, -1.0 }, { 3.0, 0.0 }, { 0.0, 1.0 } };
    std::vector<std::uint16_t> triangleList = { 0, 1, 2, 0, 2, 3 };
    std::vector<EdgeID> fixedEdges = { {0, 2} };

    auto graph = buildHalfEdgeGraph(triangleList, fixedEdges);
    edgeFlip(pointList, graph);

    auto one = std::find_if(graph.begin(), graph.end(), [](auto const& edge) { return edge->vertex == 1; });
    REQUIRE(one != graph.end());

    auto edgeOne = one->get();
    REQUIRE(edgeOne->vertex == 1);
    REQUIRE(edgeOne->next->vertex == 2);
    REQUIRE(edgeOne->next->next->vertex == 0);

    auto two = std::find_if(graph.begin(), graph.end(), [](auto const& edge) { return edge->vertex == 3; });
    REQUIRE(two != graph.end());

    auto edgeTwo = two->get();
    REQUIRE(edgeTwo->vertex == 3);
    REQUIRE(edgeTwo->next->vertex == 0);
    REQUIRE(edgeTwo->next->next->vertex == 2);
}
