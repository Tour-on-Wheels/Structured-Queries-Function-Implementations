#include <iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
#include <fstream>
#include <limits.h>

int MINUS_ONE = -1;
int MINUS_INF = INT_MIN;
bool debug_mode = false;

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

int main(int argc, const char* argv[]){
    ifstream inputFile;
	inputFile.open(argv[2]);

	FileManager fm;
	FileHandler fh1 = fm.OpenFile(argv[1]);
    FileHandler fh2 = fm.CreateFile(argv[3]);
	int page_count = -1;
    int entries = 0;
	string search_tag;
    int num;
    int page_number_flush;
    
    if(debug_mode) printfile(fh1, "INPUT FILE");
    
	while(inputFile >> search_tag) {
        inputFile>>num;
		if(debug_mode) cout << "Query Search Number: " << num << endl;
        int x = fh1.FirstPage().GetPageNum();
        int y = fh1.LastPage().GetPageNum();
        fh1.UnpinPage(x);
        fh1.UnpinPage(y);
        fh1.FlushPage(x);
        fh1.FlushPage(y);
        for(int i=x; i<=y; i++){
            PageHandler ph = fh1.PageAt(i);
            char *data = ph.GetData();
            page_number_flush = ph.GetPageNum();
            fh1.UnpinPage(page_number_flush);
            fh1.FlushPage(page_number_flush);
            for(int j = 0; j < PAGE_CONTENT_SIZE/4; j++){
                int val;
                memcpy(&val, &data[j*4], sizeof(int));
                if(val == INT_MIN){
                    break;
                }
                else if(val == num){
                    entries++;
                    PageHandler ph2;
                    if(entries/(PAGE_CONTENT_SIZE/4)>page_count){
                        page_count++;
                        ph2 = fh2.NewPage();
                    }
                    else{
                        ph2 = fh2.LastPage();
                    }
                    int offset = (entries-1)%(PAGE_CONTENT_SIZE/4);
                    char* data1 = ph2.GetData();
                    memcpy(&data1[offset*4], &i, sizeof(int));
                    entries++;
                    offset = (entries-1)%(PAGE_CONTENT_SIZE/4);
                    memcpy(&data1[offset*4], &j, sizeof(int));                    
                    if(debug_mode) cout << "FOUND " << num << " AT (" << i << "," << j << ")" << endl;

                    page_number_flush = ph2.GetPageNum();
                    fh2.MarkDirty(page_number_flush);
                    fh2.UnpinPage(page_number_flush);
                    fh2.FlushPage(page_number_flush);

                    ph = fh1.PageAt(i);
                    page_number_flush = ph.GetPageNum();
                    data = ph.GetData();
                    fh1.UnpinPage(page_number_flush);
                    fh1.FlushPage(page_number_flush);
                }
            }
        }
        entries++;
        PageHandler ph2;
        if(entries/(PAGE_CONTENT_SIZE/4)>page_count){
            page_count++;
            ph2 = fh2.NewPage();
        }
        else{
            ph2 = fh2.LastPage();
        }
        int offset = (entries-1)%(PAGE_CONTENT_SIZE/4);
        char* data1 = ph2.GetData();
        
        memcpy(&data1[offset*4], &MINUS_ONE, sizeof(int));
        entries++;
        offset = (entries-1)%(PAGE_CONTENT_SIZE/4);
        memcpy(&data1[offset*4], &MINUS_ONE, sizeof(int));      

        page_number_flush = ph2.GetPageNum();
        fh2.MarkDirty(page_number_flush);
        fh2.UnpinPage(page_number_flush);
        fh2.FlushPage(page_number_flush);
	}
    int empty_spaces = (page_count+1)*(PAGE_CONTENT_SIZE/4)-entries;
    for(int i=0; i<empty_spaces; i++){
        entries++;
        PageHandler ph2 = fh2.LastPage();
        char* data1 = ph2.GetData();
        int offset = (entries-1)%(PAGE_CONTENT_SIZE/4);
        memcpy(&data1[offset*4], &MINUS_INF, sizeof(int));
    
        page_number_flush = ph2.GetPageNum();
        fh2.MarkDirty(page_number_flush);
        fh2.UnpinPage(page_number_flush);
        fh2.FlushPage(page_number_flush);
    }
    // fh2.UnpinPage(page_count);
    // fh2.FlushPage(page_count);
    if(debug_mode) printfile(fh2,"OUTPUT FILE");
    fm.CloseFile(fh1);
    fm.CloseFile(fh2);
}