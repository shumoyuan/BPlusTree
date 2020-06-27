#include "BPlusTree.h"
#include <math.h>

BPlusTree::BPlusTree(int newOrder)
{//init BPlusTree
	this->order = newOrder;
	this->root = new LeafNode();
	this->splitIndex = minIndex -1; //When split the rear element should start from here
	this->minIndex = newOrder%2 == 0 ? newOrder / 2 : newOrder / 2 + 1; //each node has at least ceil(m/2) children
}

BPlusTree::~BPlusTree()
{//deconstruct BPlusTree

	if (this->root != NULL) 
	{
		delete this->root;
	}
}

float BPlusTree::Search(int key)
{//Search for one key
	//locate the leaf node has key
	Node* t = this->root;
    //when not a leaf, recursively get the child has this key
	while (!t->isLeaf) 
	{
		bool getLeaf = false;
		for (int i = 0; i < t->keys.size(); ++i) 
		{
			if (t->keys[i] > key) 
			{
				t = ((IndexNode*)t)->Children[i];
				getLeaf = true;
				break;
			}
		}
		if (!getLeaf)
		{
			t = ((IndexNode*)t)->Children[t->keys.size()];
		}
	}
	//get the value of key from leaf node
	for (int i = 0; i < t->keys.size(); ++i) 
	{
		if (t->keys[i] == key)
		{
			return((LeafNode*)t)->values[i];
		}
	}

	return NULL;
}

vector<float> BPlusTree::Search(int key1, int key2)
{//Search for range of two keys
	//if key1 > key2, switch two keys
	if (key1 > key2) 
	{
		int tmpKey = key1;
	    	key1 = key2;
		key2 = tmpKey;
		cout << "Key1 is bigger than Key2. Do switch." << endl;
	}
	//init result list
	vector<float> resultList;
	
	//locate the leaf node has key1
	Node* t = this->root;
	while (!t->isLeaf) 
	{
		bool getLeaf = false;
		for (int i = 0; i < t->keys.size(); ++i)
		{
			if (t->keys[i] > key1)
			{
				t = ((IndexNode*)t)->Children[i];
				getLeaf = true;
				break;
			}
		}
		if (!getLeaf)
		{
			t = ((IndexNode*)t)->Children[t->keys.size()];
		}
	}
	
	//save all values < key2 in this node into resultlist
	for (int i = 0; i < t->keys.size(); ++i) 
	{
		if (t->keys[i] >= key1 && t->keys[i] <= key2)
		{
			resultList.push_back(((LeafNode*)t)->values[i]);
		}
	}
	
	//Recursively get next node contain keys < key2, and save the values into resultlist
	while (t->keys[t->keys.size() - 1] <= key2)
	{
		t = ((LeafNode*)t)->rightSibling;
		for (int i = 0; i < t->keys.size(); ++i)
		{
			if (t->keys[i] <= key2)
			{
				resultList.push_back(((LeafNode*)t)->values[i]);
			}
		}
	}

	return resultList;

}

void BPlusTree::Insert(int newKey, float value)
{//Insert newKey and value into leaf node and update the tree
	//locate the leaf node could insert the newKey
	Node* t = this->root;
	while (!t->isLeaf) {
		bool getLeaf = false;
		for (int i = 0; i < t->keys.size(); ++i)
		{
			if (t->keys[i] > newKey)
			{
				t = ((IndexNode*)t)->Children[i];
				getLeaf = true;
				break;
			}
		}
		if (!getLeaf)
		{
			t = ((IndexNode*)t)->Children[t->keys.size()];
		}
	}
	//if node is empty or newKey bigger than all other keys in node, save the key in the end of node
	if (t->keys.size() == 0 || newKey > t->keys.back()) 
	{
		t->keys.push_back(newKey);
		((LeafNode*)t)->values.push_back(value);
	}
	//if newKey not the biggest, get the index in the node and save it into node
	else 
	{
		for (int i = 0; i < t->keys.size();++i)
		{
            //if newKey is duplicated, return
			if (t->keys[i] == newKey)
			{
				cout << "Newkey" << newKey << " is dupicated." << endl;
				return;
			}
            //get the first key bigger than newKey and insert key and value of newKey pair before it
			else if (t->keys[i] > newKey)
			{
				t->keys.insert(t->keys.begin() + i, newKey);
				((LeafNode*)t)->values.insert(((LeafNode*)t)->values.begin() + i, value);
				break;
			}
		}
	}
	//if size of node is overfull, split it
	if (t->keys.size() > this->order - 1)
	{
		//creat a new node as right sibling of this node, and save the item between ceil(m/2) and end into the new node
		//and erase these items from this node
		Node* newT = new LeafNode();
		newT->Parent = t->Parent;
        //for two situations, order is odd or even, the index will be a little different
		if(order%2 == 0)
		{
            //for order is even
			newT->keys.insert(newT->keys.begin(), t->keys.begin() + this->minIndex, t->keys.end());
			t->keys.erase(t->keys.begin() + this->minIndex , t->keys.end());
			((LeafNode*)newT)->values.insert(((LeafNode*)newT)->values.begin(), ((LeafNode*)t)->values.begin() + this->minIndex, ((LeafNode*)t)->values.end());
			((LeafNode*)t)->values.erase(((LeafNode*)t)->values.begin() + this->minIndex, ((LeafNode*)t)->values.end());
		}
		else
		{
            //for order is odd
			newT->keys.insert(newT->keys.begin(), t->keys.begin() + this->minIndex-1, t->keys.end());
			t->keys.erase(t->keys.begin() + this->minIndex-1 , t->keys.end());
			((LeafNode*)newT)->values.insert(((LeafNode*)newT)->values.begin(), ((LeafNode*)t)->values.begin() + this->minIndex-1, ((LeafNode*)t)->values.end());
			((LeafNode*)t)->values.erase(((LeafNode*)t)->values.begin() + this->minIndex-1, ((LeafNode*)t)->values.end());
		}

		//set the relation of new Node
		((LeafNode*)newT)->rightSibling = ((LeafNode*)t)->rightSibling;
		((LeafNode*)newT)->leftSibling = t;
		((LeafNode*)t)->rightSibling = newT;
		//set newKey that needs to insert into parent as the first key of the new node		
		newKey = newT->keys[0];

		//Recursively update the tree until root
		while (t->Parent != NULL) 
		{	
			//point t to its parent			
			t = t->Parent;

			//if newKey bigger than all other keys in node, save the key in the end of node
                	if (newKey > t->keys.back())
                	{
                    		t->keys.push_back(newKey);
                    		((IndexNode*)t)->Children.push_back(newT);
                	}
			//if newKey not the biggest, get the index in the node and save it into node
			else
			{
				for (int i = 0; i < t->keys.size(); ++i)
				{
				 	if(t->keys[i] > newKey)
					{
						t->keys.insert(t->keys.begin() + i, newKey);
						((IndexNode*)t)->Children.insert(((IndexNode*)t)->Children.begin() + i + 1, newT);
						break;
					}	
				}
			}
			//if index node is overfull, split it
			if (t->keys.size() > this->order - 1) 
            		{
				//creat a new node as right sibling of this node, and save the item between ceil(m/2)+1 and end into the new node
				//and erase these items from this node
				Node* newRight = new IndexNode();
				newRight->Parent = t->Parent;
                
                //set the newKey to insert as the middle of keys
				newKey = t->keys[minIndex-1];
				newRight->keys.insert(newRight->keys.begin(), t->keys.begin() + this->minIndex, t->keys.end());
				((IndexNode*)newRight)->Children.insert(((IndexNode*)newRight)->Children.begin(), ((IndexNode*)t)->Children.begin() + this->minIndex, ((IndexNode*)t)->Children.end());
                //set parents for pairs inserted to the new node
				for (int i = minIndex; i < ((IndexNode*)t)->Children.size(); ++i)
				{
					((IndexNode*)t)->Children[i]->Parent = newRight;
				}
				//erase these pairs from former node
				t->keys.erase(t->keys.begin() + this->minIndex -1, t->keys.end());
				((IndexNode*)t)->Children.erase(((IndexNode*)t)->Children.begin() + this->minIndex, ((IndexNode*)t)->Children.end());
                //for recursive use
				newT = newRight;
			}
			else
            //not overfull, set parent for newT and return
			{
				newT->Parent = t;
				return;
			}
		}
		//if parent is empty, create a new one
		if (t->Parent == NULL)
		{
			t->Parent = new IndexNode();
			//insert two node into the children list
			((IndexNode*)t->Parent)->Children.insert(((IndexNode*)t->Parent)->Children.begin(), t);
			((IndexNode*)t->Parent)->Children.insert(((IndexNode*)t->Parent)->Children.begin() + 1, newT);
			//insert the first key of latter node into parent
			if (!t->isLeaf)
			{
				(t->Parent)->keys.insert((t->Parent)->keys.begin(), ((IndexNode*)newT)->Children.front()->keys.front());
			}
			else
			{
				(t->Parent)->keys.insert((t->Parent)->keys.begin(), newT->keys.front());
			}
			//set relations of new parent
			newT->Parent = t->Parent;
			this->root = t->Parent;
		}
		//if parent is not empty, just done it
		else
		{
			return;
		}
	}
}

void BPlusTree::Delete(int key) 
{//delete item with key from tree
	//locate the leaf node has key
	Node* t = this->root;
	while (!t->isLeaf) 
    	{
		bool getLeaf = false;
		for (int i = 0; i < t->keys.size(); ++i) 
        	{
			if (t->keys[i] > key) {
				t = ((IndexNode*)t)->Children[i];
				getLeaf = true;
				break;
			}
		}
		if (!getLeaf)
		{
			t = ((IndexNode*)t)->Children[t->keys.size()];
		}
	}
	//delete item with key from node, otherwise show it as error to the user
	bool finishDelete = false;
	for (int i = 0; i < t->keys.size(); ++i)
	{
		if (t->keys[i] == key)
		{
			int childID = 0;
            //get the index of this key
			for(int i = 0;i < t->Parent->keys.size() +1;i++)
			{
				if(((IndexNode*)t->Parent)->Children[i]->keys[0] >= key)
				{
					childID = i;
					break;
				}
			}
            //erase this key from node
			t->keys.erase(t->keys.begin() + i);
			((LeafNode*)t)->values.erase(((LeafNode*)t)->values.begin() + i);
            //after erase, node is empty, just delete this node
			if (t->keys.size() == 0) {
                //set the relation of rightSibling and leftSibling before delete
				if(((LeafNode*)t)->rightSibling != NULL && ((LeafNode*)t)->leftSibling != NULL)
				{
					((LeafNode*)((LeafNode*)t)->leftSibling)->rightSibling = ((LeafNode*)t)->rightSibling;
				}
				if(((LeafNode*)t)->leftSibling != NULL && ((LeafNode*)t)->rightSibling != NULL)
				{
					((LeafNode*)((LeafNode*)t)->rightSibling)->leftSibling = ((LeafNode*)t)->leftSibling;
				}
                
                //delete this node and erase it from parent's children list
				((IndexNode*)t->Parent)->Children.erase(((IndexNode*)t->Parent)->Children.begin() + childID);
                //erase the key in parent for this node
                //if the child is 1st or 2nd one, delete the first key in parent
				if(childID==0||childID==1)
				{
					t->Parent->keys.erase(t->Parent->keys.begin());
				}
                //else, delete the index one from parent
				else
				{
					t->Parent->keys.erase(t->Parent->keys.begin() + childID - 1);
				}
			}
			finishDelete = true;
			break;
		}
	}
	if (!finishDelete)
	{
		cout << "Error: No such key " << key << " in the tree. Delete stop." << endl;
		return;
	}
	//after delete, parent of this node become decificient, rebalance the tree
	if (((IndexNode*)t->Parent)->Children.size() < minIndex && t->Parent != this->root)
	{		
		//get num of pairs in the parent
        int keyNums = 0;        
		for(int i = 0; i < ((IndexNode*)t->Parent)->Children.size(); i++)
		{
			keyNums = keyNums + ((IndexNode*)t->Parent)->Children[i]->keys.size();	
		}
		//Fast routine: if deficient subtree has more than ceil(m/2) keys, just do rotation		
		if(keyNums >= minIndex)
		{			
			//get the Node has more than 1 key to rotate			
			int rotationID = 0;
			for(int i = 0; i < ((IndexNode*)t->Parent)->Children.size(); i++)
			{
				if(((IndexNode*)t->Parent)->Children[i]->keys.size()>1)
				{
					rotationID = i;
					break;
				}	
			}

			//set the node to be splited
			t = ((IndexNode*)t->Parent)->Children[rotationID];
            //get the first pair from this node and erase it from node
			int insertKey = t->keys.front();		
			float insertValue = ((LeafNode*)t)->values.front();
			t->keys.erase(t->keys.begin());
			((LeafNode*)t)->values.erase(((LeafNode*)t)->values.begin());
            //newKey to update is the new first key in this node
			int rotationKey = t->keys[0];

			//insert this key to parent's keys list
            if(t->Parent->keys.size()!=0)
			{
				t->Parent->keys.insert(t->Parent->keys.begin()+rotationID+1, rotationKey);
			}
			else
			{
				t->Parent->keys.push_back(rotationKey);
			}
            //reinsert the pair deleted before to the tree
			Insert(insertKey,insertValue);
			return;			
		}		
		while (t->Parent != NULL) 
		{
			//point t to its parent
			t = t->Parent;
			Node* rightSibling;
			Node* leftSibling;
			leftSibling = NULL;
			rightSibling = NULL;

			int parentKey = 0;

			//get the index of node among children
			int index = -1;
			for (int i = 0; i < ((IndexNode*)t->Parent)->Children.size(); ++i)
			{
				if (((IndexNode*)t->Parent)->Children[i] == t)
				{
					index = i;
					break;
				}
			}
			//node is not the first one, it has left sibling
			if (index - 1 >= 0)
			{
				leftSibling = ((IndexNode*)t->Parent)->Children[index - 1];
			}
			//node is not the last one, it has right sibling
			if (index + 1 < ((IndexNode*)t->Parent)->Children.size())
			{
				rightSibling = ((IndexNode*)t->Parent)->Children[index + 1];
			}

			//node has leftsibling and it is not deficient if borrow one item, borrow last item from it and update the key in the parent node
			if (leftSibling != NULL && ((IndexNode*)leftSibling)->Children.size() - 1 >= this->minIndex)
			{
				//build a new node to insert, which has the last item of last child from leftSibling, and delete it from that node
				Node* insertNode = new Node();
				int insertKey = ((IndexNode*)leftSibling)->Children.back()->keys.back();
				float insertValue = ((LeafNode*)((IndexNode*)leftSibling)->Children.back())->values.back();
                //erase this pair from leftSibling
				((IndexNode*)leftSibling)->Children.back()->keys.pop_back();
				((LeafNode*)((IndexNode*)leftSibling)->Children.back())->values.pop_back();

                //get the index for the new pair
				int parentIndex = 0;
				for (int i = 0; i < (t->Parent)->keys.size(); i++)
				{
					if (t->Parent->keys[i] > insertKey)
					{
						parentIndex = i;
						break;
					}
				}
				//if parentIndex bigger than 0, parentNode is not the smallest, replace it 
				if (parentIndex > 0)
				{
					(t->Parent)->keys.erase((t->Parent)->keys.begin() + parentIndex - 1);
					(t->Parent)->keys.insert((t->Parent)->keys.begin() + parentIndex - 1, insertKey);
				}
				// else parentNode is 0, insertKey is smallest, repalce the first one
				else
				{
					(t->Parent)->keys.erase((t->Parent)->keys.begin());
					(t->Parent)->keys.insert((t->Parent)->keys.begin(), insertKey);
				}
				Insert(insertKey,insertValue);
				return;
			}
			//node has rightsibling and it is not deficient if borrow one item, borrow last item from it and update the key in the parent node
			else if (rightSibling != NULL && ((IndexNode*)rightSibling)->Children.size() - 1 >= this->minIndex)
			{
				//build a new node to insert, which has the first item of first child from rightSibling, and delete it from that node
				Node* insertNode = new Node();
				int insertKey = ((IndexNode*)rightSibling)->Children.front()->keys.front();
				float insertValue = ((LeafNode*)((IndexNode*)rightSibling)->Children.front())->values.front();
                //erase this pair from rightSibling
				((IndexNode*)rightSibling)->Children.front()->keys.erase(((IndexNode*)rightSibling)->Children.front()->keys.begin());
				((LeafNode*)((IndexNode*)rightSibling)->Children.front())->values.erase(((LeafNode*)((IndexNode*)rightSibling)->Children.front())->values.begin());

				//get index of first key bigger than insertKey and replace the former one with the former second key from rightSibling 
				int parentIndex = 0;
				for (int i = 0; i < (t->Parent)->keys.size(); i++)
				{
					if (t->Parent->keys[i] > insertKey)
					{
						parentIndex = i;
						break;
					}
				}
				//if parentIndex bigger than 0, there are more than one index key in parent, replace the former one 
				if (parentIndex > 0)
				{
					(t->Parent)->keys.erase((t->Parent)->keys.begin() + parentIndex - 1);
					(t->Parent)->keys.insert((t->Parent)->keys.begin() + parentIndex - 1, ((IndexNode*)rightSibling)->Children.front()->keys.front());
				}
				// else there is no other key bigger than index key, replace the last one with the former second key(now the first one)from rightSibling
				else
				{
					(t->Parent)->keys.erase((t->Parent)->keys.end());
					(t->Parent)->keys.push_back(rightSibling->keys[0]);
				}
				Insert(insertKey,insertValue);
				return;
			}
			//can not borrow any item from siblings, then merge with one of sibling
			//node has leftsibling, insert all items of leftsibling and keys of parent into front of this node and delete leftSibling
			else if (leftSibling != NULL && ((IndexNode*)leftSibling)->Children.size() - 1 < this->minIndex)
			{
				((IndexNode*)t)->Children.insert(((IndexNode*)t)->Children.begin(), ((IndexNode*)leftSibling)->Children.begin(), ((IndexNode*)leftSibling)->Children.end());
				t->keys.insert(t->keys.begin(), t->Parent->keys[index - 1]);
				parentKey = t->Parent->keys[index - 1];
				t->keys.insert(t->keys.begin(), leftSibling->keys.begin(), leftSibling->keys.end());
				for (int i = 0; i < ((IndexNode*)leftSibling)->Children.size(); ++i)
				{
					((IndexNode*)leftSibling)->Children[i]->Parent = t;
				}
				t->Parent->keys.erase(t->Parent->keys.begin() + index - 1);
				((IndexNode*)t->Parent)->Children.erase(((IndexNode*)t->Parent)->Children.begin() + index);
				leftSibling = t;
			}
			//node has rightsibling, insert all items of rightsibling and keys of parent into end of this node and delete rightsibling
			else if (rightSibling != NULL && ((IndexNode*)rightSibling)->Children.size() - 1 < this->minIndex)
			{
				((IndexNode*)t)->Children.insert(((IndexNode*)t)->Children.end(), ((IndexNode*)rightSibling)->Children.begin(), ((IndexNode*)rightSibling)->Children.end());
				t->keys.insert(t->keys.end(), t->Parent->keys[index]);
				parentKey = t->Parent->keys[index];
				t->keys.insert(t->keys.end(), rightSibling->keys.begin(), rightSibling->keys.end());
				for (int i = 0; i < ((IndexNode*)rightSibling)->Children.size(); ++i)
				{
					((IndexNode*)rightSibling)->Children[i]->Parent = t;
				}
				t->Parent->keys.erase(t->Parent->keys.begin() + index);
				((IndexNode*)t->Parent)->Children.erase(((IndexNode*)t->Parent)->Children.begin() + index + 1);
				rightSibling = t;
			}

			//After delete, if parent become empty, delete parent and point this node to the parent of former parent
			if (t->Parent->keys.size() == 0) 
			{
				//if root is empty,set this node as root
				if (t->Parent == this->root && this->root->keys.size() == 0)
				{
					this->root = t;
					t->Parent = NULL;
					return;
				}
                //else, delete parent and point this node to the parent of former parent
				else
				{
					int deleteIndex = 0;
					for (int i = 0; i < (t->Parent->Parent->keys.size()); i++)
					{
						if (t->Parent->Parent->keys[i] > parentKey) {
							deleteIndex = i;
							break;
						}
					}
					t->Parent = t->Parent->Parent;
					((IndexNode*)t->Parent->Parent)->Children[deleteIndex] = t;
				}
			}
		}
		//if root is empty,set this node as root
		if (t->Parent == this->root && this->root->keys.size() == 0)
		{
			this->root = t;
			t->Parent = NULL;
			return;
		}
	}
		
	else if(!t->isLeaf)
	{ 
		//for the update of index node, just like a B tree
        //if node is deficient, borrow a item from sibling
		if (((LeafNode*)t)->values.size() < minIndex && t->Parent != NULL) 
   		{
	
			//init siblings in use
			Node* rightSibling;
			Node* leftSibling;
			leftSibling = NULL;
			rightSibling = NULL;

			//get the index of node among children
			int index = -1;
			for (int i = 0; i < ((IndexNode*)t->Parent)->Children.size(); ++i)
			{
				if (((IndexNode*)t->Parent)->Children[i] == t)
				{
					index = i;
					break;
				}
			}
			//node is not the first one, it has left sibling
			if (index - 1 >= 0)
			{
				leftSibling = ((IndexNode*)t->Parent)->Children[index - 1];
			}
			//node is not the last one, it has right sibling
			if (index + 1 < ((IndexNode*)t->Parent)->Children.size())
			{
				rightSibling = ((IndexNode*)t->Parent)->Children[index + 1];
			}
			//node has leftsibling and it is not deficient if borrow one item, borrow last item from it and update the key in the parent node
			if (leftSibling != NULL && ((LeafNode*)leftSibling)->values.size() - 1 >= minIndex)
			{
				t->keys.insert(t->keys.begin(), leftSibling->keys.back());
				((LeafNode*)t)->values.insert(((LeafNode*)t)->values.begin(), ((LeafNode*)leftSibling)->values.back());
				leftSibling->keys.erase(leftSibling->keys.end());
				((LeafNode*)leftSibling)->values.erase(((LeafNode*)leftSibling)->values.end());
				t->Parent->keys[index - 1] = t->keys.front();
				return;
			}
			//node has rightsibling and it is not deficient if borrow one item, borrow first item from it and update the key in the parent node
			else if (rightSibling != NULL && ((LeafNode*)rightSibling)->values.size() - 1 >= minIndex)
			{
				t->keys.push_back(rightSibling->keys.front());
				((LeafNode*)t)->values.push_back(((LeafNode*)rightSibling)->values.front());
				rightSibling->keys.erase(rightSibling->keys.begin());
				((LeafNode*)rightSibling)->values.erase(((LeafNode*)rightSibling)->values.begin());
				t->Parent->keys[index] = rightSibling->keys.front();
				return;
			}
			//can not borrow any item from siblings, then merge with one of sibling
			//node has leftsibling, insert all items into end of leftsibling and delete this node
			else if (leftSibling != NULL && ((LeafNode*)leftSibling)->values.size() - 1 < minIndex)
			{
				leftSibling->keys.insert(leftSibling->keys.end(), t->keys.begin(), t->keys.end());
				((LeafNode*)leftSibling)->values.insert(((LeafNode*)leftSibling)->values.end(), ((LeafNode*)t)->values.begin(), ((LeafNode*)t)->values.end());
				((LeafNode*)leftSibling)->rightSibling = ((LeafNode*)t)->rightSibling;

				t->Parent->keys.erase(t->Parent->keys.begin() + index - 1);
				((IndexNode*)t->Parent)->Children.erase(((IndexNode*)t->Parent)->Children.begin() + index);
				t = leftSibling;
			}
			//node has rightsibling, insert all items into end of this node and delete rightsibling
			else if (rightSibling != NULL && ((LeafNode*)rightSibling)->values.size() - 1 < minIndex)
			{
				t->keys.insert(t->keys.end(), rightSibling->keys.begin(), rightSibling->keys.end());
				((LeafNode*)t)->values.insert(((LeafNode*)t)->values.begin(), ((LeafNode*)rightSibling)->values.begin(), ((LeafNode*)rightSibling)->values.end());
				((LeafNode*)t)->rightSibling = ((LeafNode*)rightSibling)->rightSibling;

				t->Parent->keys.erase(t->Parent->keys.begin() + index);
				((IndexNode*)t->Parent)->Children.erase(((IndexNode*)t->Parent)->Children.begin() + index + 1);
			}
			//Recursively update the tree until root
			while (t->Parent != this->root)
			{
				//init siblings in use
				leftSibling = NULL;
				rightSibling = NULL;
				//point t to its parent
				t = t->Parent;
				//if parent is not deficient, just stop update
				if (((IndexNode*)t)->Children.size() >= splitIndex)
				{
					return;
				}
				//get the index of node among children
				int index = -1;
				for (int i = 0; i < ((IndexNode*)t->Parent)->Children.size(); ++i)
				{
					if (((IndexNode*)t->Parent)->Children[i] == t) {
						index = i;
						break;
					}
				}
				//node is not the first one, it has left sibling
				if (index - 1 >= 0)
				{
					leftSibling = ((IndexNode*)t->Parent)->Children[index - 1];
				}
				//node is not the last one, it has right sibling
				if (index + 1 < ((IndexNode*)t->Parent)->Children.size())
				{
					rightSibling = ((IndexNode*)t->Parent)->Children[index + 1];
				}
				//node has leftsibling and it is not deficient if borrow one item, borrow last item from it and update the key in the parent node
				if (leftSibling != NULL && ((IndexNode*)leftSibling)->Children.size() - 1 >= this->minIndex)
				{
					((IndexNode*)t)->Children.insert(((IndexNode*)t)->Children.begin(), ((IndexNode*)leftSibling)->Children.back());
					t->keys.insert(t->keys.begin(), t->Parent->keys[index - 1]);
					t->Parent->keys[index] = t->keys.front();
					((IndexNode*)leftSibling)->Children.erase(((IndexNode*)leftSibling)->Children.end());
					leftSibling->keys.erase(leftSibling->keys.end());
					((IndexNode*)t)->Children.front()->Parent = t;
					return;
				}
				//node has rightsibling and it is not deficient if borrow one item, borrow last item from it and update the key in the parent node
				else if (rightSibling != NULL && ((IndexNode*)rightSibling)->Children.size() - 1 >= this->minIndex)
				{
					((IndexNode*)t)->Children.push_back(((IndexNode*)rightSibling)->Children.front());
					t->keys.push_back(t->Parent->keys[index]);
					t->Parent->keys[index] = rightSibling->keys.front();
					((IndexNode*)rightSibling)->Children.erase(((IndexNode*)rightSibling)->Children.begin());
					rightSibling->keys.erase(rightSibling->keys.begin());
					((IndexNode*)t)->Children.back()->Parent = t;
					return;
				}
				//can not borrow any item from siblings, then merge with one of sibling
				//node has leftsibling, insert all items into end of leftsibling and delete this node	
				else if (leftSibling != NULL && ((IndexNode*)leftSibling)->Children.size() - 1 < this->minIndex)
				{
					((IndexNode*)leftSibling)->Children.insert(((IndexNode*)leftSibling)->Children.end(), ((IndexNode*)t)->Children.begin(), ((IndexNode*)t)->Children.end());
					//leftSibling->keys.insert(leftSibling->keys.end(), t->Parent->keys[index - 1]);
					leftSibling->keys.insert(leftSibling->keys.end(), t->keys.begin(), t->keys.end());
					for (int i = 0; i < ((IndexNode*)t)->Children.size(); ++i)
					{
						((IndexNode*)t)->Children[i]->Parent = leftSibling;
					}
					t->Parent->keys.erase(t->Parent->keys.begin() + index - 1);
					((IndexNode*)t->Parent)->Children.erase(((IndexNode*)t->Parent)->Children.begin() + index);
					t = leftSibling;
				}
				//node has rightsibling, insert all items into front of rightsibling and delete this node
				else if (rightSibling != NULL && ((IndexNode*)rightSibling)->Children.size() - 1 < this->minIndex)
				{
					((IndexNode*)rightSibling)->Children.insert(((IndexNode*)rightSibling)->Children.begin(), ((IndexNode*)t)->Children.begin(), ((IndexNode*)t)->Children.end());
					//rightSibling->keys.insert(rightSibling->keys.begin(), t->Parent->keys[index]);
					rightSibling->keys.insert(rightSibling->keys.begin(), t->keys.begin(), t->keys.end());
					for (int i = 0; i < ((IndexNode*)t)->Children.size(); ++i)
					{
						((IndexNode*)t)->Children[i]->Parent = rightSibling;
					}
					t->Parent->keys.erase(t->Parent->keys.begin() + index);
					((IndexNode*)t->Parent)->Children.erase(((IndexNode*)t->Parent)->Children.begin() + index);
					t = rightSibling;
				}
			}

			//if root is empty,set this node as root
			if (t->Parent == this->root && this->root->keys.size() == 0) 
			 {
				this->root = t;
				t->Parent = NULL;
				return;
			}
		}
	}

}

void BPlusTree::printTree()
{
    //for debug, print the keys of each node in every level in the tree
    //for empty tree
	if (this->root->keys.size() == 0)
	{
		cout << "[]" << endl;
	}
	//init the list for node
    vector<Node*> node;
	node.push_back(this->root);
	while (node.size())
	{
		long size = node.size();
		for (int i = 0; i < size; ++i)
		{
			//add node into the list
            if (!node[i]->isLeaf)
			{
				for (int j = 0; j < ((IndexNode*)node[i])->Children.size(); ++j)
				{
					node.push_back(((IndexNode*)node[i])->Children[j]);
				}
			}
            
            //print the keys in the nodes
			cout << "[";
            		int k = 0;
			for (k = 0; k < node[i]->keys.size()-1; k++)
			{
				cout << node[i]->keys[k] << ",";
			}
			cout << node[i]->keys[k]<< "]";
		}
		node.erase(node.begin(), node.begin() + size);
		cout << endl;
	}
}

void BPlusTree::printValues()
{
    //for debug, Print the linked list for values
	Node*t = this->root;
    //Recursively get the leftmost leaf node
	while(!t->isLeaf)
	{
		t=((IndexNode*)t)->Children[0];
	}
    //Recursively print every value in each node, until there is no rightSibling
	while(t!=NULL){
		for(int i=0;i<t->keys.size();++i)
		{
			cout<<((LeafNode*)t)->values[i]<<",";
		}
		t=((LeafNode*)t)->rightSibling;
	}
	cout <<endl;
}

