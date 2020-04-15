#ifndef MACRO_H
#define MACRO_H	

#include <iostream>
#include <math.h>

#define SWITCHANDRUN(bname, func)	\
if(!bname)							\
{									\
	func							\
	bname = true;					\
}							

#define IF16BIT		if(sizeof(void*)==2)
#define IF32BIT		if(sizeof(void*)==4)
#define IF64BIT		if(sizeof(void*)==8)

#define DISTBETWEENPOINT(x1,y1,x2,y2) std::sqrtf(std::powf(x2-x1, 2)+std::powf(y2-y1, 2))

#endif