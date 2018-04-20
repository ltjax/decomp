#include <decomp/convex_decomposition.hpp>

using namespace decomp;

#define TEST_ASSERT(C)                                                                                                 \
    if (!(C))                                                                                                          \
    return EXIT_FAILURE

int main()
{
    std::vector<Point> pointList = { { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } };

    std::vector<std::uint16_t> triangleList = { 0, 1, 2, 0, 2, 3 };

    auto graph = buildHalfEdgeGraph(triangleList);

    for (auto&& edge : graph)
    {
        TEST_ASSERT(edge->next != nullptr);

        if (edge->vertex == 2 && edge->next->vertex == 0)
        {
            TEST_ASSERT(edge->partner && edge->partner->partner == edge.get() && edge->partner->vertex == 0 &&
                        edge->partner->next->vertex == 2);
        }
        else if (edge->vertex == 0 && edge->next->vertex == 2)
        {
            TEST_ASSERT(edge->partner && edge->partner->partner == edge.get() && edge->partner->vertex == 2 &&
                        edge->partner->next->vertex == 0);
        }
        else
        {
            TEST_ASSERT(edge->partner == nullptr);
        }
    }

    return EXIT_SUCCESS;
}
