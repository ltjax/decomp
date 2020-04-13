#ifndef LIB_DECOMP_OUTPUT
#define LIB_DECOMP_OUTPUT

#include "triangulation.hpp"
#include <ostream>

namespace decomp
{

namespace svg
{
/** Render the given polygon as SVG, mostly for debugging
 */
void writePolygon(std::ostream& out, PointList const& points, IndexList const& indices,
        std::vector<IndexList> const& holes);

/** Render the given triangles as SVG, mostly for debugging
 */
void writeTriangles(std::ostream& out, PointList const& points, IndexList const& indices);

} // namespace svg

namespace json
{
/** Convert the given inputs and output to a JSON text representation that is suitable for export and plotting.
 */
std::ostream & dump(
    std::ostream &out,
    PointList const& pointList,
    IndexList const& outerPolygon,
    std::vector<IndexList> const& holeList,
    std::vector<IndexList> const& convexPolygonList);
} // namespace json

/** Convert the given point list to a text representation that is suitable for initializer list usage.
 */
void writePoints(std::ostream& out, PointList const& points);

/** Convert the given index list to a text representation that is suitable for initializer list usage.
 */
void writeIndices(std::ostream& out, IndexList const& indices);

/** Convert the given list of indexs list to a text representation that is suitable for initializer list usage.
 */
void writeIndices(std::ostream& out, std::vector<IndexList> const& indices);

} // namespace decomp
#endif
