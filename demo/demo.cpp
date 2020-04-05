// This program illustrates the use of decomp library
// It creates the convex decomposition of a polygon with
// two holes and displays to standard output a python script
// that displays the input polygon and patches of its convex
// polygon.

#include <vector>
#include <fstream>
#include <iostream>

#include <decomp/convex_decomposition.hpp>
#include <decomp/triangulation.hpp>
#include <decomp/operations.hpp>
#include <decomp/output.hpp>

std::ostream & dump_python_plot_script(
    std::ostream &out,
    const std::vector<decomp::Point>& pointList,
    const std::vector<std::uint16_t>& outerPolygon,
    const std::vector<std::vector<std::uint16_t>>& holeList,
    const std::vector<decomp::IndexList>& convexPolygonList)
{
    out << "import matplotlib" << std::endl;
    out << "import matplotlib.pyplot as plt" << std::endl;
    out << "import matplotlib.patches as patches" << std::endl;
    out << "import numpy as np" << std::endl;
    out << "import random" << std::endl;
    out << "random.seed(666)" << std::endl;
    out << "plt.subplot(2, 1, 1)" << std::endl;
    out << "outerPolygon = np.array([";
    for (auto& e:outerPolygon)
        out << "[" << pointList[e][0] << "," << pointList[e][1] << "]," << std::endl;
    out << "[" << pointList[outerPolygon[0]][0] << "," << pointList[outerPolygon[0]][1] << "]," << std::endl;
    out << "])" << std::endl;
    out << "plt.plot(outerPolygon[:,0], outerPolygon[:,1])" << std::endl;

    for (auto& hole:holeList)
    {
        out << "hole = np.array([";
        for (auto& e:hole)
            out << "[" << pointList[e][0] << "," << pointList[e][1] << "]," << std::endl;
        out << "[" << pointList[hole[0]][0] << "," << pointList[hole[0]][1] << "]," << std::endl;
        out << "])" << std::endl;
        out << "plt.plot(hole[:,0], hole[:,1])" << std::endl;
    }
    out << "plt.grid()" << std::endl;

    out << "sp = plt.subplot(2, 1, 2)" << std::endl;
    for (auto& convexPolygon:convexPolygonList)
    {
        out << "r = random.random()" << std::endl;
        out << "b = random.random()" << std::endl;
        out << "g = random.random()" << std::endl;
        out << "color = (r, g, b)" << std::endl;
        out << "convexPolygon = np.array([";
        for (auto& e:convexPolygon)
            out << "[" << pointList[e][0] << "," << pointList[e][1] << "]," << std::endl;
        out << "[" << pointList[convexPolygon[0]][0] << "," << pointList[convexPolygon[0]][1] << "]," << std::endl;
        out << "])" << std::endl;
        out << "plt.plot(convexPolygon[:,0], convexPolygon[:,1], color=color)" << std::endl;
        out << "poly = patches.Polygon(convexPolygon, fc=color, alpha=0.8)" << std::endl;
        out << "sp.add_patch(poly)" << std::endl;
    }

    out << "plt.grid()" << std::endl;
    out << "plt.show()" << std::endl;
    return out;
}

int main()
{
    const std::vector<decomp::Point> pointList={
        // points on the outer polygon
        {-4, 0}, {-3, -2}, {3, -2}, {4, 0}, {3, 2}, {-3, 2},
        // points on the left hole
        {-3, 0}, {-2, -1}, {-1, 0}, {-2, 1},
        // points on the right hole
        {1, 0}, {2, -1}, {3, 0}, {2, 1}
    };

    const std::vector<std::uint16_t> outerPolygon = {
        0, 1, 2, 3, 4, 5
    };

    const std::vector<std::vector<std::uint16_t>> holeList = {
        {13, 12, 11, 10},
        {9, 8, 7, 6}
    };

    const std::vector<decomp::IndexList> convexPolygonList = decomp::decompose(
        pointList, outerPolygon, holeList);

    dump_python_plot_script(
        std::cout, pointList, outerPolygon, holeList, convexPolygonList);

    // Create SVG images of input and output
    // A scaling factor is used to have nice SVG image sizes.
    const double scaling_factor = 100.0;
    std::vector<decomp::Point> pointListOut;
    for (auto& p: pointList)
    {
        pointListOut.push_back(decomp::Point(scaling_factor * p.x(), scaling_factor * p.y()));
    }

    std::ofstream svg("input.svg");
    decomp::svg::writePolygon(svg, pointListOut, outerPolygon, holeList);

    std::ofstream svg_output("output.svg");
    decomp::svg::writePolygon(svg_output, pointListOut, {}, convexPolygonList);
    return 0;
}
