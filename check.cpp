#include "file_manager.h"
#include "errors.h"
#include <iostream>
#include <limits.h>

int main(int argc, const char* argv[]) {
	FileManager fm;
    FileHandler fh = fm.OpenFile(argv[1]);
	int size = fh.LastPage().GetPageNum()+1;
	fh.FlushPages();
	for(int i = 0; i < size; i++) {
		PageHandler ph = fh.PageAt(i);
		for(int j=0; j<PAGE_CONTENT_SIZE/sizeof(int); j++) {
			cout<<((int *)ph.GetData())[j]<<" ";
		} cout<<endl;
		fh.FlushPage(i);
	} fm.CloseFile(fh);
	return 0;
}