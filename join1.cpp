#include "file_manager.h"
#include "errors.h"
#include <iostream>
#include <limits.h>
#include <fstream>
#include <string>

ifstream infile;
FileManager fm;
FileHandler fh1, fh2, fh3;
PageHandler ph1, ph2, ph3;
int k = PAGE_CONTENT_SIZE/sizeof(int);

void output_close() {
    while(k < PAGE_CONTENT_SIZE/sizeof(int)) {
        ((int *)ph2.GetData())[k++] = INT_MIN;
    }
    fh2.UnpinPage(ph2.GetPageNum());
    fh2.FlushPage(ph2.GetPageNum());
}

void output_write(int value) {
    if(k == PAGE_CONTENT_SIZE/sizeof(int)) {
        fh2.UnpinPage(ph2.GetPageNum());
        fh2.FlushPage(ph2.GetPageNum());
        ph2 = fh2.NewPage();
        k = 0;
    }
    ((int *)ph2.GetData())[k++] = value;
}

int main(int argc, const char* argv[]) {
    fh1 = fm.OpenFile(argv[1]);
    fh2 = fm.OpenFile(argv[2]);
    fh3 = fm.CreateFile(argv[3]);
    int i = 0;
    while(true) {
        try {
            ph1 = fh1.PageAt(i);
        } catch(...) {
            break;
        }
        int j = 0;
        while(true) {
            try {
                ph2 = fh2.PageAt(j);
            } catch(...) {
                break;
            }
            for(int a=0; a<PAGE_CONTENT_SIZE/sizeof(int); a++) {
                for(int b=0; b<PAGE_CONTENT_SIZE/sizeof(int); b++) {
                    if(((int *)ph1.GetData())[a] == ((int *)ph2.GetData())[b]) {
                        output_write(((int *)ph1.GetData())[a]);
                    }
                }
            }
            fh2.UnpinPage(j);
            fh2.FlushPage(j);
            j++;
        }
        fh1.UnpinPage(i);
        fh1.FlushPage(i);
        i++;
    }
    output_close();
    fm.CloseFile(fh1);
    fm.CloseFile(fh2);
    fm.CloseFile(fh3);
    return 0;
}