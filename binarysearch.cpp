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
    
    int firstpage = fh1.FirstPage().GetPageNum();
    int lastpage = fh1.LastPage().GetPageNum();
    int total_nos = lastpage*(PAGE_CONTENT_SIZE/4) + no_of_elements_in_lastpage(fh1);

    fh1.UnpinPage(firstpage);
    fh1.FlushPage(firstpage);
    fh1.UnpinPage(lastpage);
    fh1.FlushPage(lastpage);

    if(debug_mode) printfile(fh1,"INPUT FILE");
	while(inputFile >> search_tag){
        inputFile>>num;
		if(debug_mode) cout << "Query Search Number: " << num << endl;
        int start = 0;
        int end = total_nos-1;
        int first_occurence=-1;
        int last_occurence=-1;
        
        while(start<=end){
            int val, val_;
            int mid = start + (end-start)/2;
            int pg = mid/(PAGE_CONTENT_SIZE/4);
            int offset = mid%(PAGE_CONTENT_SIZE/4);             
            char *data1 = fh1.PageAt(pg).GetData();

            fh1.UnpinPage(pg);
            fh1.FlushPage(pg);

            memcpy(&val, &data1[offset*4], sizeof(int));
            int pg_ =-1, offset_=-1; 
            if(mid>0){
                pg_ = (mid-1)/(PAGE_CONTENT_SIZE/4);
                offset_ = (mid-1)%(PAGE_CONTENT_SIZE/4);
                char *data1_ = fh1.PageAt(pg_).GetData();
                memcpy(&val_, &data1_[offset_*4], sizeof(int));
                fh1.UnpinPage(pg_);
                fh1.FlushPage(pg_);
            }
            if((mid==0 || num>val_) && val==num){
                first_occurence = mid;
                break;
            }
            else if(num>val) start = mid+1;
            else end = mid - 1;
        }

        start = 0;
        end = total_nos-1;

        while(start<=end){
            int val, val_;
            int mid = start + (end-start)/2;
            int pg = mid/(PAGE_CONTENT_SIZE/4);
            int offset = mid%(PAGE_CONTENT_SIZE/4);             
            char *data1 = fh1.PageAt(pg).GetData();
            fh1.UnpinPage(pg);
            fh1.FlushPage(pg);
            memcpy(&val, &data1[offset*4], sizeof(int));
            int pg_ =-1, offset_=-1; 
            if(mid<total_nos-1){
                pg_ = (mid+1)/(PAGE_CONTENT_SIZE/4);
                offset_ = (mid+1)%(PAGE_CONTENT_SIZE/4);
                char *data1_ = fh1.PageAt(pg_).GetData();
                fh1.UnpinPage(pg_);
                fh1.FlushPage(pg_);
                memcpy(&val_, &data1_[offset_*4], sizeof(int));
            }
            if((mid==total_nos-1 || num<val_) && val==num){
                last_occurence = mid;
                break;
            }
            else if(num<val) end = mid - 1;
            else start = mid + 1;
        }

        // cout << first_occurence << ":" << last_occurence << endl;
        // fm.PrintBuffer();
        for(int i=first_occurence; i<=last_occurence; i++){
            if(i==-1) break;
            entries++;
            int x = i / (PAGE_CONTENT_SIZE/4);
            int y = i % (PAGE_CONTENT_SIZE/4);
            if(debug_mode) cout << "FOUND " << num << " AT (" << x << "," << y << ")" << endl;
            PageHandler ph2;
            if(entries/(PAGE_CONTENT_SIZE/4)>page_count){
                page_count++;
                ph2 = fh2.NewPage();
            }
            else{
                ph2 = fh2.LastPage();
            }
            int offset = (entries-1)%(PAGE_CONTENT_SIZE/4);
            char* data2 = ph2.GetData();
            memcpy(&data2[offset*4], &x, sizeof(int));
            entries++;
            offset = (entries-1)%(PAGE_CONTENT_SIZE/4);
            memcpy(&data2[offset*4], &y, sizeof(int));   
            page_number_flush = ph2.GetPageNum();
            fh2.MarkDirty(page_number_flush);
            fh2.UnpinPage(page_number_flush);
            fh2.FlushPage(page_number_flush);
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
        char* data2 = ph2.GetData();        
        memcpy(&data2[offset*4], &MINUS_ONE, sizeof(int));
        entries++;
        offset = (entries-1)%(PAGE_CONTENT_SIZE/4);
        memcpy(&data2[offset*4], &MINUS_ONE, sizeof(int)); 
        page_number_flush = ph2.GetPageNum();
        fh2.MarkDirty(page_number_flush);
        fh2.UnpinPage(page_number_flush);
        fh2.FlushPage(page_number_flush);
    }
    int empty_spaces = (page_count+1)*(PAGE_CONTENT_SIZE/4)-entries;
    
    for(int i=0; i<empty_spaces; i++){
        entries++;
        PageHandler ph2 = fh2.LastPage();
        char* data2 = ph2.GetData();
        int offset = (entries-1)%(PAGE_CONTENT_SIZE/4);
        memcpy(&data2[offset*4], &MINUS_INF, sizeof(int));

        page_number_flush = ph2.GetPageNum();
        fh2.MarkDirty(page_number_flush);
        fh2.UnpinPage(page_number_flush);
        fh2.FlushPage(page_number_flush);
    }
    if(debug_mode) printfile(fh2,"OUTPUT FILE");
    fm.CloseFile(fh1);
    fm.CloseFile(fh2);
}