/*
 * SharedData.h
 *
 *  Created on: Sep 15, 2014
 *      Author: chris
 */

#ifndef SHAREDDATA_H_
#define SHAREDDATA_H_


struct CV //concentration value structure
{
	int num;
	int deno;
	long double val;
	CV(int n, int d, long double v): num(n), deno(d), val(v) {}
};

enum usability {reusable, non_reusable, reused, both_output}; // - reusable are the nodes with a waste droplet while nodes with no waste are reused. Buffers and reagents are non_reusable.
enum answer {yes, no};

#endif /* SHAREDDATA_H_ */
