#include <iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
#include <fstream>
#include <limits.h>

int main(int argc, const char* argv[]){
    if(argc <4)
    {
        cout<<"Few arguments"<<endl;
        return 0;
    }
    FileManager fm;
    FileHandler in = fm.OpenFile(argv[1]);
    FileHandler out = fm.CreateFile(argv[3]);

    int totalPage = in.LastPage().GetPageNum()+1;
    int intPerPage = PAGE_CONTENT_SIZE/sizeof(int);
    
    char* data = in.LastPage().GetData();
    int intLastPage = 0;
    int curr;
    int i=-4;
    while(intLastPage < intPerPage)
    {
        memcpy(&curr, &data[i+sizeof(int)], sizeof(int));
        if(curr == INT_MIN)
            break;
        intLastPage++;
        i = i+4;
    }
    int totalnumbers = (totalPage-1)*intPerPage+intLastPage;

    ifstream query;
    query.open(argv[2]);
    string temp;
    while(query >> temp)
    {
        int a;
        query>>a;
        int start = 0;
        int end = totalnumbers-1;

        while(start <= end)
        {
            int mid = start + (end-start)/2;
            int pageIndex = mid/intPerPage;
            int offset = mid%intPerPage;
            data = in.PageAt(pageIndex).GetData();

            memcpy(&curr, &data[offset*sizeof(int)], sizeof(int));
            int prev = INT_MIN;
            if(mid > 0)
            {
                if(offset != 0)
                    memcpy(&prev, &data[(offset-1)*sizeof(int)], sizeof(int));
                else {
                    in.UnpinPage(pageIndex);
                    in.FlushPage(pageIndex);
                    data = in.PageAt(pageIndex-1).GetData();
                    memcpy(&prev, &data[intPerPage*sizeof(int)], sizeof(int));
                }
            }
        }
        
    }
}