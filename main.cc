#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "BPlusTree.h"

//#include "BPlusTree.h"

using namespace std;

int main(int argc, char** argv)
{
	ifstream readFile(argv[1]);                //get input filename from terminal and open the file
	ofstream writeFile("output_file.txt");     //open the output file
	string i;                                  //temp string for processing
	int order = 0;                             //init the order

	//Recursively get order from input file
    while (order == 0) {
		if (readFile)
		{
			getline(readFile, i);
			int index = 0;
            //delete blanks from input string
			if (!i.empty())
			{
				while ((index = i.find(' ', index)) != string::npos)
				{
					i.erase(index, 1);
				}
			}
            
            //find init command and filter the order number
			if (i.find("Initialize") != string::npos)
			{
				int leftParen = i.find_first_of("(", 0);
				int rightParen = i.find_first_of(")", 0);
				order = stoi(i.substr(leftParen + 1, rightParen - leftParen - 1));
			}
		}
	}

    //init a new tree
	BPlusTree t(order);

    //get rest of command
	if (readFile)
	{
		while (getline(readFile, i)) {
			int index = 0;
            //delete blanks from input string
			if (!i.empty())
			{
				while ((index = i.find(' ', index)) != string::npos)
				{
					i.erase(index, 1);
				}
			}

			//cout << i << endl;
            
            //find insert command and split key and value, insert the pair to tree
			if (i.find("Insert") != string::npos)
			{
				int leftParen = i.find_first_of("(", 0);
				int rightParen = i.find_first_of(")", 0);
				int comma = i.find_first_of(",", 0);
				int key = stoi(i.substr(leftParen + 1, comma - leftParen - 1));
				float value = stof(i.substr(comma + 1, rightParen - comma - 1));
				t.Insert(key, value);
				//t.printTree();
				//t.printValues();
			}
            //find insert command and split key, delete the pair from tree
			else if (i.find("Delete") != string::npos)
			{
				int leftParen = i.find_first_of("(", 0);
				int rightParen = i.find_first_of(")", 0);
				int key = stoi(i.substr(leftParen + 1, rightParen - leftParen - 1));
				t.Delete(key);
				//t.printTree();
				//t.printValues();
			}
            //find search command and split key, search the pair from tree
			else if (i.find("Search") != string::npos)
			{
				//if there is a comma in the string, true for range search, false for ordinary search
                int m = i.find(",");
				if (m > i.size()) {
					m = 0;
				}
				if (m) {
                    //true, for range search, split two keys and transfer to command for the tree
					int leftParen = i.find_first_of("(", 0);
					int rightParen = i.find_first_of(")", 0);
					int comma = i.find_first_of(",", 0);
					int key1 = stoi(i.substr(leftParen + 1, comma - leftParen - 1));
					int key2 = stoi(i.substr(comma + 1, rightParen - comma - 1));
					vector<float> list = t.Search(key1, key2);
                    //For return if there is no result, return null, or return the result list
					if(list.size()==0)
					{
						writeFile << "Null" << endl;
					}
					else
					{
						int i = 0;
						for (i; i < list.size() - 1; i++)
						{
							writeFile << list[i] << ",";
						}
						writeFile << list[i] << endl;
					}
				}
				else {
                    //fasle, for ordinary search, split key and transfer to command for the tree
					int leftParen = i.find_first_of("(", 0);
					int rightParen = i.find_first_of(")", 0);
					int key = stoi(i.substr(leftParen + 1, rightParen - leftParen - 1));
					float result = t.Search(key);
                    //For return if there is no result, return null, or return the result
					if (result == NULL) 
					{
						writeFile << "Null" << endl;
					}
					else
					{
						writeFile << result << endl;
					}
				}
			}
			else {
				cout << "Invalid command." << endl;
				return 1;
			}
		}
	}
	else {
		cout << "Read error: No such file." << endl;
		return 1;
	}
    
    //finished all command, close two files
    writeFile.close();
    readFile.close();

	return 0;
}
