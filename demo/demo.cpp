#include <vector>
#include <iostream>

#include <decomp/convex_decomposition.hpp>
#include <decomp/triangulation.hpp>
#include <decomp/operations.hpp>
#include <decomp/output.hpp>

template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
    const size_t n = v.size();
    if (n == 0) {
        out << "{}";
    } else {
        out << '{' << v[0];
        for (size_t i = 1; i < n; ++i) {
            out << ", " << v[i];
        }
        out << '}';
    }
    return out;
}

int main()
{
std::vector<decomp::Point> pointList={
  // points on the outer polygon
  {-4, 0}, {-3, -2}, {3, -2}, {4, 0}, {3, 2}, {-3, 2},
  // points on the left hole
  {-3, 0}, {-2, -1}, {-1, 0}, {-2, 1},
  // points on the right hole
  {1, 0}, {2, -1}, {3, 0}, {2, 1}
};

std::vector<std::uint16_t> outerPolygon={
  0, 1, 2, 3, 4, 5
};

std::vector<std::vector<std::uint16_t>> holeList={
  {13, 12, 11, 10},
  {9, 8, 7, 6}
};

std::vector<decomp::IndexList> convexPolygonList=decomp::decompose(pointList, outerPolygon, holeList);

std::cout << "import matplotlib" << std::endl;
std::cout << "import matplotlib.pyplot as plt" << std::endl;
std::cout << "import numpy as np" << std::endl;
std::cout << "plt.subplot(2, 1, 1)" << std::endl;
std::cout << "outerPolygon = np.array([";
for (auto& e:outerPolygon)
    std::cout << "[" << pointList[e][0] << "," << pointList[e][1] << "]," << std::endl;
std::cout << "[" << pointList[outerPolygon[0]][0] << "," << pointList[outerPolygon[0]][1] << "]," << std::endl;
std::cout << "])" << std::endl;
std::cout << "plt.plot(outerPolygon[:,0], outerPolygon[:,1])" << std::endl;


for (auto& hole:holeList)
{
    std::cout << "outerPolygon = np.array([";
    for (auto& e:hole)
        std::cout << "[" << pointList[e][0] << "," << pointList[e][1] << "]," << std::endl;
    std::cout << "[" << pointList[hole[0]][0] << "," << pointList[hole[0]][1] << "]," << std::endl;
    std::cout << "])" << std::endl;
    std::cout << "plt.plot(outerPolygon[:,0], outerPolygon[:,1])" << std::endl;
}
std::cout << "plt.grid()" << std::endl;

std::cout << "plt.subplot(2, 1, 2)" << std::endl;
for (auto& hole:convexPolygonList)
{
    std::cout << "outerPolygon = np.array([";
    for (auto& e:hole)
        std::cout << "[" << pointList[e][0] << "," << pointList[e][1] << "]," << std::endl;
    std::cout << "[" << pointList[hole[0]][0] << "," << pointList[hole[0]][1] << "]," << std::endl;
    std::cout << "])" << std::endl;
    std::cout << "plt.plot(outerPolygon[:,0], outerPolygon[:,1])" << std::endl;
}

std::cout << "plt.grid()" << std::endl;

// std::cout << "fig.savefig(\"test.png\")" << std::endl;
std::cout << "plt.show()" << std::endl;

// for (auto ind: convexPolygonList)
// decomp::svg::writePolygon(std::cout, pointList, ind, std::vector<decomp::IndexList>{});

return 0;
}
