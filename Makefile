CC = g++ -O2 -Wno-deprecated

tag = -i

ifdef linux
tag = -n
endif

bplustree: main.cc BPlusTree.cc
	$(CC) main.cc BPlusTree.cc -o bplustree

