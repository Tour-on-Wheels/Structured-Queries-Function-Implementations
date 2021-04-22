#include "file_manager.h"
#include "errors.h"
#include <iostream>
#include <limits.h>
#include <string>

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
    try {
        int i = 0;
        while(true) {
            ph1 = fh1.PageAt(i);
            for(int a=0; a<PAGE_CONTENT_SIZE/sizeof(int); a++) {
                if(((int *)ph1.GetData())[a] == INT_MIN) {
                    throw InvalidPageException();
                } try {
                    bool exp = false; 
                    int j = 0;
                    while(true) {
                        ph2 = fh2.PageAt(j);
                        for(int b=0; b<PAGE_CONTENT_SIZE/sizeof(int); b++) {
                            if(((int *)ph2.GetData())[b] == INT_MIN) {
                                throw InvalidPageException();
                            } else if(((int *)ph1.GetData())[a] == ((int *)ph2.GetData())[b]) {
                                exp = true;
                                output_write(((int *)ph1.GetData())[a]);
                            } else if(exp) {
                                throw InvalidPageException();
                            }
                        }
                        fh2.UnpinPage(j);
                        fh2.FlushPage(j);
                        j++;
                    }
                } catch(...) {
                    continue;
                }
            }
            fh1.UnpinPage(i);
            fh1.FlushPage(i);
            i++;
        }
    } catch(...) {
        output_close();
    }
    fm.CloseFile(fh1);
    fm.CloseFile(fh2);
    fm.CloseFile(fh3);
    return 0;
}