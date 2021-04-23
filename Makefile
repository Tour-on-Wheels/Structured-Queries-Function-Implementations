linearsearchobjects = buffer_manager.o file_manager.o linearsearch.o

binarysearchobjects = buffer_manager.o file_manager.o binarysearch.o

deletionobjects = buffer_manager.o file_manager.o deletion.o

join1objects = buffer_manager.o file_manager.o join1.o

join2objects = buffer_manager.o file_manager.o join2.o

linearsearch : $(linearsearchobjects)
	g++ -std=c++11 -o linearsearch $(linearsearchobjects)
  
binarysearch : $(binarysearchobjects)
	g++ -std=c++11 -o binarysearch $(binarysearchobjects)

deletion : $(deletionobjects)
	g++ -std=c++11 -o deletion $(deletionobjects)

join1 : $(join1objects)
	g++ -std=c++11 -o join1 $(join1objects)

join2 : $(join2objects)
	g++ -std=c++11 -o join2 $(join2objects)

linearsearch.o : linearsearch.cpp
	g++ -std=c++11 -c linearsearch.cpp

binarysearch.o : binarysearch.cpp
	g++ -std=c++11 -c binarysearch.cpp

deletion.o : deletion.cpp
	g++ -std=c++11 -c deletion.cpp
  
join1.o :
	g++ -std=c++11 -c join1.cpp

join2.o :
	g++ -std=c++11 -c join2.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp

clean :
	rm -f *.o
	rm -f linearsearch
	rm -f binarysearch
	rm -f deletion
	rm -f join1
	rm -f join2