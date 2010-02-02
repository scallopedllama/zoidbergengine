#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include <nds.h>

template <class T> struct vector2D {
	vector2D(T _x, T _y)
	{
		x = _x;
		y = _y;
	}
	vector2D() {
		x = y = 0;
	}
/*
	T& operator [] (int i)
	{
		if(i == 0)
			return &x;
		else
			return &y;
	}
	T& x()
	{
		return &x;
	}
	T& y()
	{
		return &y;
	}*/

    T x;
    T y;
};

#endif // VECTOR_H_INCLUDED
