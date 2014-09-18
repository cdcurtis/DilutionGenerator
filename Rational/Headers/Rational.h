/*
 * Rational.h
 *
 *  Created on: May 6, 2013
 *      Author: Kenneth O'Neal
 */

#ifndef RATIONAL_H_
#define RATIONAL_H_

#include <vector>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;


class Rational
{
	public:
	double numer;
	double denom;

	Rational();
	Rational(int);
	Rational(int, int);

	//manipulators
	const Rational add(const Rational &) const;
	const Rational subtract(const Rational &) const;
	const Rational multiply(const Rational &) const;
	const Rational divide(const Rational &) const;
	const Rational operator + (const Rational &) const;

	//I/O
	void display() const;

	//converters
	static Rational FloatToRat(double value, int num_ops);
	static Rational ConctoRat(double Concentration, int num_ops);
	static long double RattoFloat(Rational Concentration); 
};


#endif /* RATIONAL_H_ */
