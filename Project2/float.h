#ifndef FLOAT_H
#define FLOAT_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <iostream>


typedef uint8_t		float8;
typedef uint16_t	float16;
typedef uint32_t	float32;

/*===CONVERSION METHOD: IEE754===================================================================
/*
/*																								*/

//Convert float with given iee754 varaibles
float		ConvertIEE754(bool sign, uint32_t exponent, float mantissa)
{
	//Get the e
	int e = exponent - 127;

	float ret = pow(-1, sign) * (1 + mantissa) * pow(2, e);

	return ret;
}

/*===FLOAT: Minifloat============================================================================*/
// Size: 4 bytes

float8		float_to_float8(float val) 
{

	return 0;
}

float		float8_to_float(float8 uf)
{
	uf = 0b11111111;

	bool bSigned = (uf & 0x80) != 0;

	uint32_t cExponent = 0;
	cExponent += (((uf & 0x40) != 0) * 1);
	cExponent += (((uf & 0x20) != 0) * 2);
	cExponent += (((uf & 0x10) != 0) * 4);
	cExponent += (((uf & 0x8) != 0) * 8);

	float cMantissa = 0;
	cMantissa += (((uf & 0x4) != 0) * pow(2, -1));
	cMantissa += (((uf & 0x2) != 0) * pow(2, -2));
	cMantissa += (((uf & 0x1) != 0) * pow(2, -3));

	if (cExponent == (char)0b1111 && cMantissa == (char)0) return INFINITY * (bSigned*(-1));

	return ConvertIEE754(bSigned, cExponent, cMantissa);

}

#endif