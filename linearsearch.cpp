#include "file_manager.h"
#include "errors.h"
#include <iostream>
#include <limits.h>
#include <fstream>
#include <string>

ifstream infile;
FileManager fm;
FileHandler fh1, fh2;
PageHandler ph1, ph2;
int k = PAGE_CONTENT_SIZE/sizeof(int);

void output_close() {
    while(k < PAGE_CONTENT_SIZE/sizeof(int)) {
        ((int *)ph2.GetData())[k++] = INT_MIN;
    } fh2.FlushPage(ph2.GetPageNum());
}

void output_write(int value) {
    if(k == PAGE_CONTENT_SIZE/sizeof(int)) {
        fh2.FlushPage(ph2.GetPageNum());
        ph2 = fh2.NewPage();
        k = 0;
    } ((int *)ph2.GetData())[k++] = value;
}

int main(int argc, const char* argv[]) {
    fh1 = fm.OpenFile(argv[1]);
    int size = fh1.LastPage().GetPageNum()+1;
    fh1.FlushPages();
    infile.open(argv[2]);
    fh2 = fm.CreateFile(argv[3]);
    string str;
    int num;
    while(infile >> str) {
        infile >> num;
        for(int i = 0; i < size; i++) {
            ph1 = fh1.PageAt(i);
            for(int j=0; j<PAGE_CONTENT_SIZE/sizeof(int); j++) {
                if(((int *)ph1.GetData())[j] == INT_MIN) {
                    break;
                } else if(((int *)ph1.GetData())[j] == num) {
                    output_write(i);
                    output_write(j);
                }
            } fh1.FlushPage(i);
        } output_write(-1);
        output_write(-1);
    } output_close();
    fm.CloseFile(fh1);
    infile.close();
    fm.CloseFile(fh2);
    return 0;
}