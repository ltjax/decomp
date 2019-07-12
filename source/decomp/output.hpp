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
void writePolygon(std::ostream& out, PointList const& points, IndexList const& indices);

/** Render the given triangles as SVG, mostly for debugging
 */
void writeTriangles(std::ostream& out, PointList const& points, IndexList const& indices);

} // namespace svg

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
