// This program illustrates the use of decomp library.
// It creates the convex decomposition of a polygon with
// two holes. Input and output is exported to standard output
// as JSON.
// This JSON can be used to display the input polygon and
// the convex polygons, with Python script plot_decomp.py
//
// ./demo | python plot_decomp.py

#include <decomp/convex_decomposition.hpp>
#include <decomp/output.hpp>

#include <iostream>
#include <vector>

using namespace decomp;

int main()
{
  const PointList pointList={
    // points on the outer polygon
    {-4, 0}, {-3, -2}, {3, -2}, {4, 0}, {3, 2}, {-3, 2},
    // points on the left hole
    {-3, 0}, {-2, -1}, {-1, 0}, {-2, 1},
    // points on the right hole
    {1, 0}, {2, -1}, {3, 0}, {2, 1}
  };

  const IndexList outerPolygon = {
    0, 1, 2, 3, 4, 5
  };

  const std::vector<IndexList> holeList = {
    {13, 12, 11, 10},
    {9, 8, 7, 6}
  };

  const std::vector<IndexList> convexPolygonList = decompose(
    pointList, outerPolygon, holeList);

  json::dump(
    std::cout, pointList, outerPolygon, holeList, convexPolygonList);

  return 0;
}
