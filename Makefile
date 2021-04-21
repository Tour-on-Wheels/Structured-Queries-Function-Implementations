linearsearchobjects = buffer_manager.o file_manager.o linearsearch.o
binarysearchobjects = buffer_manager.o file_manager.o binarysearch.o

linearsearch : $(linearsearchobjects)
	g++ -std=c++11 -o linearsearch $(linearsearchobjects)

linearsearch.o : linearsearch.cpp
	g++ -std=c++11 -c linearsearch.cpp

binarysearch : $(binarysearchobjects)
	g++ -std=c++11 -o binarysearch $(binarysearchobjects)

binarysearch.o : binarysearch.cpp
	g++ -std=c++11 -c binarysearch.cpp

brun :
	rm output.txt
	./binarysearch TestCases/TC_search/sorted_input TestCases/TC_search/query_search.txt output.txt

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp

clean :
	rm -f *.o
	rm -f linearsearch
	rm -f binarysearch