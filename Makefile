linearsearchobjects = buffer_manager.o file_manager.o linearsearch.o
binarysearchobjects = buffer_manager.o file_manager.o binarysearch.o
deletionobjects = buffer_manager.o file_manager.o deletion.o

linearsearch : $(linearsearchobjects)
	g++ -std=c++11 -o linearsearch $(linearsearchobjects)

linearsearch.o : linearsearch.cpp
	g++ -std=c++11 -c linearsearch.cpp

binarysearch : $(binarysearchobjects)
	g++ -std=c++11 -o binarysearch $(binarysearchobjects)

binarysearch.o : binarysearch.cpp
	g++ -std=c++11 -c binarysearch.cpp

binaryTest :
	make clean
	make binarysearch
	rm -f TestCases/TC_search/outputbinary
	./binarysearch TestCases/TC_search/sorted_input TestCases/TC_search/query_search.txt TestCases/TC_search/outputbinary

deletion : $(deletionobjects)
	g++ -std=c++11 -o deletion $(deletionobjects)

deletion.o : deletion.cpp
	g++ -std=c++11 -c deletion.cpp

deletionTest :
	make clean
	make deletion
	cp TestCases/TC_delete/sorted_input TestCases/TC_delete/outputdeletion
	./deletion TestCases/TC_delete/outputdeletion TestCases/TC_delete/query_delete.txt

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp

clean :
	rm -f *.o
	rm -f linearsearch
	rm -f binarysearch
	rm -f outputbinary