#include <decomp/convex_decomposition.hpp>
#include <catch2/catch.hpp>

using namespace decomp;

TEST_CASE("can build a correct half-edge graph")
{
    std::vector<Point> pointList = { { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } };
    std::vector<std::uint16_t> triangleList = { 0, 1, 2, 0, 2, 3 };

    auto graph = buildHalfEdgeGraph(triangleList, {});

    REQUIRE(graph.size() == 6);

    for (auto&& edge : graph)
    {
        REQUIRE(edge->next != nullptr);
        REQUIRE(edge->fixed == false);

        if (edge->vertex == 2 && edge->next->vertex == 0)
        {
            REQUIRE(edge->partner);
            REQUIRE(edge->partner->partner == edge.get());
            REQUIRE(edge->partner->vertex == 0);
            REQUIRE(edge->partner->next->vertex == 2);
        }
        else if (edge->vertex == 0 && edge->next->vertex == 2)
        {
            REQUIRE(edge->partner);
            REQUIRE(edge->partner->partner == edge.get());
            REQUIRE(edge->partner->vertex == 2);
            REQUIRE(edge->partner->next->vertex == 0);
        }
        else
        {
            REQUIRE(edge->partner == nullptr);
        }
    }
}

TEST_CASE("can mark edges as fixed")
{
    std::vector<Point> pointList = { { -3.0, -3.0 }, { 3.0, -3.0 }, { 3.0, 3.0 }, { -3.0, 3.0 } };
    std::vector<std::uint16_t> triangleList = { 0, 1, 2, 0, 2, 3 };
    std::vector<EdgeID> fixedEdges = { {0, 2} };

    auto graph = buildHalfEdgeGraph(triangleList, fixedEdges);

    REQUIRE(graph.size() == 6);

    for (auto&& edge : graph)
    {
        REQUIRE(edge->next != nullptr);

        if (edge->vertex == 2 && edge->next->vertex == 0)
        {
            REQUIRE(edge->partner);
            REQUIRE(edge->partner->partner == edge.get());
            REQUIRE(edge->partner->vertex == 0);
            REQUIRE(edge->partner->next->vertex == 2);
            REQUIRE(edge->fixed == true);
        }
        else if (edge->vertex == 0 && edge->next->vertex == 2)
        {
            REQUIRE(edge->partner);
            REQUIRE(edge->partner->partner == edge.get());
            REQUIRE(edge->partner->vertex == 2);
            REQUIRE(edge->partner->next->vertex == 0);
            REQUIRE(edge->fixed == true);
        }
        else
        {
            REQUIRE(edge->partner == nullptr);
            REQUIRE(edge->fixed == false);
        }
    }
}
