#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
#include <fstream>
#include <limits.h>
using namespace std;
struct MyException : public exception {
   const char * what () const throw () {
      return "C++ Exception";
   }
};
int main(int argc,char* argv[]) {
    FileManager fm;
    
    FileHandler output= fm.OpenFile(argv[1]);
	int nums_in_page = PAGE_CONTENT_SIZE/sizeof(int);
	PageHandler out;
	int* outdata;
    int i=0;
    try{
	while(true){
		out = output.PageAt(i);
		int* outdata = (int*) out.GetData();
		for(int j=0;j<6;j++){
			cout<<outdata[j]<<" "; 
		}
		cout<<"\n";
        output.UnpinPage(i);
        output.FlushPages();
        i++;
	}
    }
    catch(...){}
	fm.CloseFile(output);
	return 0;
}