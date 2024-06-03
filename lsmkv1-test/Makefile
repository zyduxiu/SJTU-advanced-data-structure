
LINK.o = $(LINK.cc)
CXXFLAGS = -std=c++14 -Wall

all: correctness persistence

correctness: kvstore.o correctness.o memtable.o SSTable.o vlog.o skiplist.o Bloomfilter.o

persistence: kvstore.o persistence.o memtable.o SSTable.o vlog.o skiplist.o Bloomfilter.o 

clean:
	-rm -f correctness persistence *.o
