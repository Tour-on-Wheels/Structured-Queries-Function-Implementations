linearsearchobjects = buffer_manager.o file_manager.o linearsearch.o

binarysearchobjects = buffer_manager.o file_manager.o binarysearch.o

join1objects = buffer_manager.o file_manager.o join1.o

linearsearch : $(linearsearchobjects)
	g++ -std=c++11 -o linearsearch $(linearsearchobjects)
  
binarysearch : $(binarysearchobjects)
	g++ -std=c++11 -o binarysearch $(binarysearchobjects)

join1  : $(join1objects)
	g++ -std=c++11 -o join1 $(join1objects)

linearsearch.o : linearsearch.cpp
	g++ -std=c++11 -c linearsearch.cpp

binarysearch.o : binarysearch.cpp
	g++ -std=c++11 -c binarysearch.cpp -o binarysearch.o
  
join1.o :
	g++ -std=c++11 -c join1.cpp

binaryTest :
	make clean
	make binarysearch
	./binarysearch TestCases/TC_search/sorted_input TestCases/TC_search/query_search.txt TestCases/TC_search/outputbinary

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp

clean :
	rm -f *.o
	rm -f linearsearch
  rm -f binarysearch
	rm -f join1
	rm -f outputbinary