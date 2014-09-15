/*
 * MinMix.h
 *
 *  Created on: Sep 12, 2014
 *      Author: chris
 */

#ifndef MINMIX_H_
#define MINMIX_H_

#include "../../DagGen/Headers/DagGen.h"

class MinMix
{
public:
	static DagGen RunMinMix(int, char *[]);

private:
	MinMix() { }
	std :: vector<int> convertRatio(std :: string);
	std :: vector<std :: string> convertFluidList( std :: string fluidNames);
	bool is_number(const std::string& s);

};


#endif /* MINMIX_H_ */
