#include <iostream>
#include <fstream>
#include "../Headers/HamiltonianPath.h"

HamiltonianPath& HamiltonianPath :: operator=(HamiltonianPath rhs){
	this->head = rhs.head;
	this->tail = rhs.tail;
	return *this;
}

void HamiltonianPath :: AddToPath(HamiltonianNode* n)
{
    if(n){
        if(!head){
			head = n;
			tail=head;
		}
		else{
			tail-> next = n;
    		tail = tail->next;
    	}
    }
}
void HamiltonianPath :: AddToPath(int n, int type)
{
	HamiltonianNode *temp =  new HamiltonianNode (n, type);
	if(!head){
		head = temp;
		tail=head;
	}
	else{
		tail-> next = temp;
    	tail = tail->next;
    }
}
    
void HamiltonianPath :: print (std :: string fileName)
{
	std:: ostream& out = (fileName != "") ? *(new std::ofstream(fileName.c_str())) : std::cout; 
	
    for(HamiltonianNode* temp = head; temp !=  NULL; temp = temp -> next){
        if(temp->edgeType !=-1)
            out <<" --" << temp->edgeType << "--> " << temp-> name;
        else
            out << temp->name;
    }
    out << std:: endl;
    if (&out!=&std::cout)  
   		delete(&out);
}
