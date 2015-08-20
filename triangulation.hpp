#ifndef LIB_DECOMP_HEADER
#define LIB_DECOMP_HEADER

#include <cstdint>
#include <vector>
#include <cmath>

namespace decomp {

class Point
{
public:
	explicit Point(double v=0.0)
	{
		data[0]=v;
		data[1]=v;
	}
	
	Point(double x, double y)
	{
		data[0]=x;
		data[1]=y;
	}
	
	double& operator[](int i)
	{
		return data[i];
	}
	
	double operator[](int i) const
	{
		return data[i];
	}
	
	Point& operator+=(Point const& rhs)
	{
		for (int i=0; i<2; ++i)
			data[i]+=rhs.data[i];
		
		return *this;
	}
	
	Point& operator-=(Point const& rhs)
	{
		for (int i=0; i<2; ++i)
			data[i]-=rhs.data[i];
		
		return *this;
	}
	
	double x() const {return data[0];}
	double y() const {return data[1];}
private:
	double data[2];
};

inline Point operator+(Point const& lhs, Point const& rhs)
{
	Point result(lhs);
	return result += rhs;
}

inline Point operator-(Point const& lhs, Point const& rhs)
{
	Point result(lhs);
	return result -= rhs;
}

inline double squared(Point const& p)
{
	return p[0]*p[0]+p[1]*p[1];
}

inline Point normalize(Point const& p)
{
	double length=std::sqrt(squared(p));
	return{p.x()/length, p.y()/length};
}

inline double dot(Point const& lhs, Point const& rhs)
{
	return lhs[0]*rhs[0]+lhs[1]*rhs[1];
}

using PointList=std::vector<Point>;
using IndexList=std::vector<std::uint16_t>;

IndexList removeHoles(PointList const& pointList,
					  IndexList indexList, std::vector<IndexList> holeList);
	
IndexList earClipping(PointList const& pointList,
					  IndexList const& polygon);

}

#endif