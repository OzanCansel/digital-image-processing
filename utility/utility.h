#ifndef _UTILITY_H
#define _UTILITY_H

#include <string>
#include <opencv2/opencv.hpp>
#include "NamedType.h"

namespace dip
{
template <class Container>
void split(const std::string& str, Container& cont, char delim = ' ')
{
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

//Href : 
struct UpperBoundaryParameter;

template <typename T>
NamedType<T, struct UpperBoundaryParameter> Upper(T val)
{
	return NamedType<T, struct UpperBoundaryParameter>(val);
}

struct LowerBoundaryParameter;
template <typename T>
NamedType<T, struct LowerBoundaryParameter> Lower(T val)
{
	return NamedType<T, struct LowerBoundaryParameter>(val);
}

template<typename T ,typename U ,typename V>
T stayInBoundaries(T val, NamedType<U , UpperBoundaryParameter> upper, NamedType<V , LowerBoundaryParameter> lower)
{
	//Check upper boundary
	if (val > upper.get())
		return upper.get();
	
	//Check lower boundary
	if (val < lower.get())
		return lower.get();

	return val;
}

cv::Mat drawHistogram(double* values,  int range);

}

#endif