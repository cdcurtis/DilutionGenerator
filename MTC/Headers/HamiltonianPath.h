#ifndef __HAMILTONIANNODE_H__
#define __HAMILTONIANNODE_H__

#include <iostream>
struct HamiltonianNode {
    int name;
    int edgeType;
    HamiltonianNode* next;
    
    HamiltonianNode(int n): name(n), edgeType(-1), next(NULL) { }
    HamiltonianNode(int n, int t): name(n), edgeType(t), next(NULL) { }
};

struct HamiltonianPath { 
    HamiltonianNode* head;
    HamiltonianNode* tail;

	HamiltonianPath(): head(NULL), tail(NULL) {}
    HamiltonianPath(HamiltonianNode* n): head(n),tail(n) { }
    HamiltonianPath(int n) {
        head = new HamiltonianNode (n);
        tail = head;
    }
    HamiltonianPath(int n, int type) {
        head = new HamiltonianNode (n,type);
        tail = head;
    }
    
	HamiltonianPath(const HamiltonianPath & hp){
		HamiltonianNode* n = new HamiltonianNode(hp.head->name, hp.head->edgeType);
		head=n;
		tail=head;
		for(HamiltonianNode* temp = hp.head->next; temp != NULL; temp = temp->next){
			this->AddToPath(temp->name, temp->edgeType);
		}
	}
	HamiltonianPath& operator=(HamiltonianPath rhs);
	
    void AddToPath(HamiltonianNode* n);
  
    void AddToPath(int n, int type);
     
    void print (std :: string = "");
};

#endif /*__HAMILTONIANNODE_H__*/