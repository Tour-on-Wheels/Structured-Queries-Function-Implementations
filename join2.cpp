#include "file_manager.h"
#include "errors.h"
#include <iostream>
#include <limits.h>

FileManager fm;
FileHandler fh1, fh2, fh3;
PageHandler ph1, ph2, ph3;
int k = PAGE_CONTENT_SIZE/sizeof(int);

void output_close() {
    while(k < PAGE_CONTENT_SIZE/sizeof(int)) {
        ((int *)ph3.GetData())[k++] = INT_MIN;
    } fh3.FlushPage(ph3.GetPageNum());
}

void output_write(int value) {
    if(k == PAGE_CONTENT_SIZE/sizeof(int)) {
        fh3.FlushPage(ph3.GetPageNum());
        ph3 = fh3.NewPage();
        k = 0;
    } ((int *)ph3.GetData())[k++] = value;
}

int main(int argc, const char* argv[]) {
    fh1 = fm.OpenFile(argv[1]);
    int size1 = fh1.LastPage().GetPageNum()+1;
    fh1.FlushPages();
    fh2 = fm.OpenFile(argv[2]);
    int size2 = fh2.LastPage().GetPageNum()+1;
    fh2.FlushPages();
    fh3 = fm.CreateFile(argv[3]);
    for(int i = 0; i < size1; i++) {
        ph1 = fh1.PageAt(i);
        for(int a=0; a<PAGE_CONTENT_SIZE/sizeof(int); a++) {
            if(((int *)ph1.GetData())[a] == INT_MIN) {
                break;
            } bool found = false; 
            for(int j = 0; j < size2; j++) {
                ph2 = fh2.PageAt(j);
                for(int b=0; b<PAGE_CONTENT_SIZE/sizeof(int); b++) {
                    if(((int *)ph2.GetData())[b] == INT_MIN) {
                        break;
                    } else if(((int *)ph1.GetData())[a] == ((int *)ph2.GetData())[b]) {
                        found = true;
                        output_write(((int *)ph1.GetData())[a]);
                    } else if(found) {
                        break;
                    }
                } fh2.FlushPage(j);
            }
        } fh1.FlushPage(i);
    } output_close();
    fm.CloseFile(fh1);
    fm.CloseFile(fh2);
    fm.CloseFile(fh3);
    return 0;
}