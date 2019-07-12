#ifndef LIB_DECOMP_OPERATIONS
#define LIB_DECOMP_OPERATIONS

#include "triangulation.hpp"
#include <cstdint>
#include <unordered_map>

namespace decomp
{

/** Utility to remove unused vertices from a point-list.
 */
class Remapper
{
public:
    /** Successively use this to remap all your indices.
     */
    IndexList apply(IndexList const& indices);

    /** Once all indices are remapped, get the new mapped point list here.
     */
    PointList mapped(PointList const& points);

private:
    std::unordered_map<std::uint16_t, std::uint16_t> mMapping;
};

} // namespace decomp

#endif
