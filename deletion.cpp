#include "file_manager.h"
#include "errors.h"
#include <iostream>
#include <limits.h>
#include <fstream>
#include <string>

ifstream infile;
FileManager fm;
FileHandler fh1;
PageHandler ph1, ph2;
int k = PAGE_CONTENT_SIZE/sizeof(int);

void output_close(int size) {
    while(k < PAGE_CONTENT_SIZE/sizeof(int)) {
        ((int *)ph2.GetData())[k++] = INT_MIN;
    } fh1.MarkDirty(ph2.GetPageNum());
    fh1.FlushPage(ph2.GetPageNum());
    for(int i = ph2.GetPageNum()+1; i < size; i++) {
        fh1.DisposePage(i);
    }
}

void output_write(int value) {
    if(k == PAGE_CONTENT_SIZE/sizeof(int)) {
        fh1.MarkDirty(ph2.GetPageNum());
        fh1.FlushPage(ph2.GetPageNum());
        ph2 = fh1.PageAt(ph2.GetPageNum()+1);  
        k = 0;
    } ((int *)ph2.GetData())[k++] = value;
}

int main(int argc, const char* argv[]) {
    fh1 = fm.OpenFile(argv[1]);
    infile.open(argv[2]);
    string str;
    int num;
    while(infile >> str) {
        infile >> num;
        bool found = false;
        int size = fh1.LastPage().GetPageNum()+1;
        fh1.FlushPages();
        for(int i = 0; i < size; i++) {
            ph1 = fh1.PageAt(i);
            for(int j = 0; j < PAGE_CONTENT_SIZE/sizeof(int); j++) {
                if(((int *)ph1.GetData())[j] == INT_MIN) {
                    break;
                } else if((!found) && (((int *)ph1.GetData())[j] == num)) {
                    found = true;
                    ph2 = fh1.PageAt(i);
                    k = j;
                } else if((found) && (((int *)ph1.GetData())[j] != num)) {
                    output_write(((int *)ph1.GetData())[j]);
                }
            } if(ph2.GetPageNum() != i) {
                fh1.FlushPage(i);
            }
        } output_close(size);
    } fm.CloseFile(fh1);
    infile.close();
    return 0;
}