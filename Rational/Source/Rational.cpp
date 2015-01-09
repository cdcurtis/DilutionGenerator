/*
 * Rational.cc
 *
 *  Created on: May 6, 2013
 *      Author: Kenneth O'Neal
 */

#include "../Headers/Rational.h"

using namespace std;

///Begin Rational class functions
Rational::Rational() :
numer(0), denom(1)
{

}
Rational::Rational(int a)
: numer(a), denom(1)
{

}
Rational::Rational(int a, int b)
: numer(a), denom(b)
{

}
void Rational::display() const
{
	cout<<numer<<"/"<<denom<<endl;
}

const Rational Rational::add(const Rational & second) const
{
	//add: (a/b) + (c/d) = (a*d + b*c) / (b*d)
	int new_numer = (this->numer * second.denom) + (second.numer * this->denom);
	int new_denom = this->denom * second.denom;
	const Rational Result(new_numer, new_denom);
	return Result;
}
const Rational Rational::subtract(const Rational & second) const
{
	//subtract: (a/b) - (c/d) = (a*d - b*c) / (b*d)
	int new_numer = (this->numer * second.denom) - (this->denom*second.numer);
	int new_denom = (this->denom * second.denom);
	const Rational Result(new_numer, new_denom);
	return Result;
}
const Rational Rational::multiply(const Rational & second) const
{
	//multiply: (a/b) * (c/d) = (a*c) / (b*d)
	int new_numer = (this->numer * second.numer);
	int new_denom = (this->denom * second.denom);
	const Rational Result(new_numer, new_denom);
	return Result;

}
const Rational Rational::divide(const Rational & second) const
{
	//divide: (a/b) / (c/d) = (a*d) / (c*b)
	int new_numer = (this->numer * second.denom);
	int new_denom = (this->denom * second.numer);
	const Rational Result(new_numer, new_denom);
	return Result;

}
const Rational Rational::operator+ (const Rational & second) const
{
	Rational temp;
	temp.numer = numer*second.denom + second.numer*denom;
	temp.denom = denom*second.denom;
	return temp;
}

Rational Rational::FloatToRat(double value, int num_ops) {
	double tolerance = 0.000001;
	double check_val = 0.00;
	double check_denom = 0.00;
	int top = 0;
	int bot;
	while ((fabs(check_val - value)) > tolerance) {
		bot = 0;
		check_denom = value + 1;
		top++;
		while (check_denom > value) {
			bot++;
			check_denom = (static_cast<double>(top) / static_cast<double>(bot));
		}

		check_val = check_denom;
	}
	Rational temp = Rational(top, bot);
	return temp;
}
Rational Rational::ConctoRat(double Concentration, int num_ops) {
	int denom = pow(2, num_ops);
	int numer = 0;
	long double value = (static_cast<long double>(numer)
			/ static_cast<long double>(denom));

	while (value < Concentration) {
		numer = numer + 1;
		value = (static_cast<long double>(numer)
				/ static_cast<long double>(denom));
	}
	Rational RatConc = Rational(numer, denom);
	return RatConc;
}


long double Rational::RattoFloat(Rational Concentration) {
	long double Conc = (static_cast<long double>(Concentration.numer)
			/ static_cast<long double>(Concentration.denom));
	return Conc;
}


