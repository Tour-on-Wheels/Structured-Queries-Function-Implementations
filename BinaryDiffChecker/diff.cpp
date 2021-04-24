#include <iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
#include <fstream>
#include <limits.h>

int MINUS_ONE = -1;
int MINUS_INF = INT_MIN;
bool debug_mode = true;

void printfile(FileHandler fh, string s){
    cout << "-------------------------- " << s << " --------------------------\n";
    cout << "\t";
    for(int j = 0; j < PAGE_CONTENT_SIZE/4; j++) {
        cout << "Offset"<< j << "\t"; 
    }
    cout << endl;
    int x = fh.FirstPage().GetPageNum();
    int y = fh.LastPage().GetPageNum();
    fh.UnpinPage(x);
    fh.FlushPage(x);
    fh.UnpinPage(y);
    fh.FlushPage(y);
    for(int i=x; i<=y; i++){
        PageHandler ph = fh.PageAt(i);
        char *data = ph.GetData();
        cout << "Page" << i << "\t";
        for(int j = 0; j < PAGE_CONTENT_SIZE/4; j++) {
            int val;
            memcpy(&val, &data[j*4], sizeof(int));     
            if(val==INT_MIN){
                cout << "-Inf\t";
            }
            else{
                cout << val  << "\t";            
            }
        }
        fh.UnpinPage(i);
        fh.FlushPage(i);
        cout << endl;
    }
    cout << "-----------------------------------------------------------------\n";
}

int no_of_elements_in_lastpage(FileHandler fh){
    int l = fh.LastPage().GetPageNum();
    char * data = fh.PageAt(l).GetData();
    fh.UnpinPage(l);
    fh.FlushPage(l);
    int i;
    for(i=0; i<PAGE_CONTENT_SIZE/4; i++){
        int val;
        memcpy(&val, &data[i*4], sizeof(int));
        if(val==INT_MIN){
            break;
        }
    }
    return i;
}


void printchar(char *c, int k){
    if(k==1)
    cout << "CHAR1 = ";
    else
    cout << "CHAR2 = ";
    for(int i=0; i<PAGE_CONTENT_SIZE/4; i++){
        cout << (int)(c[i*4]) << " ";
    }
    cout << endl;
}


int main(int argc, const char* argv[]){

    FileManager fm;
    FileHandler fh1 = fm.OpenFile(argv[1]);
    FileHandler fh2 = fm.OpenFile(argv[2]);
    
    printfile(fh1, "first file");
    printfile(fh2, "second file");
}