#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <string>

class Record;

using namespace std;

class Node {
public:
	vector<int> keys;
	Node* Parent;
	bool isLeaf;
};

class LeafNode : public Node
{
public:
	vector<float> values;
	Node* leftSibling;
	Node* rightSibling;
	
	LeafNode() {
		isLeaf = true;
		Parent = NULL;
		leftSibling = NULL;
		rightSibling = NULL;
	}
};

class IndexNode : public Node
{
public:
	vector<Node*> Children;

	IndexNode() {
		isLeaf = false;
		Parent = NULL;
	}
};

#endif
