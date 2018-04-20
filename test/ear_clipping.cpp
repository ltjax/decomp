#include <decomp/triangulation.hpp>

using namespace decomp;

int main()
{
    std::vector<Point> pointList = {
        { 0, 2 }, { -2, 0 }, { 0, -2 }, { 2, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 0 }
    };

    std::vector<std::uint16_t> indexList = { 0, 1, 2, 3, 0, 4, 7, 6, 5, 4 };

    auto triangleList = earClipping(pointList, indexList);

    if (triangleList.size() != 8 * 3)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
