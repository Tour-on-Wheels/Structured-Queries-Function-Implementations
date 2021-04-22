linearsearchobjects = buffer_manager.o file_manager.o linearsearch.o

join1objects = buffer_manager.o file_manager.o join1.o

linearsearch : $(linearsearchobjects)
	g++ -std=c++11 -o linearsearch $(linearsearchobjects)

join1  : $(join1objects)
	g++ -std=c++11 -o join1 $(join1objects)

linearsearch.o : linearsearch.cpp
	g++ -std=c++11 -c linearsearch.cpp

join1.o :
	g++ -std=c++11 -c join1.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp

clean :
	rm -f *.o
	rm -f linearsearch
	rm -f join1