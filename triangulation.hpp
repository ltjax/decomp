#ifndef TRIANGULATION_HEADER
#define TRIANGULATION_HEADER

#include <cstdint>
#include <vector>

namespace triangulation {

class Point
{
public:
	Point(double v=0.0)
	: data{v, v}
	{
	}
	
	Point(double x, double y)
	: data{x, y}
	{
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

std::vector<std::uint16_t> removeHoles(std::vector<Point> const& pointList,
	std::vector<std::uint16_t> indexList, std::vector<std::vector<std::uint16_t>> holeList);
	
std::vector<std::uint16_t> earClipping(std::vector<Point> const& pointList,
	std::vector<std::uint16_t> const& polygon);

}

#endif