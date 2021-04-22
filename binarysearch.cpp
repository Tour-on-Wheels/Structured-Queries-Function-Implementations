#include <iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
#include <fstream>
#include <limits.h>

int intPerPage;
int totalnumbers;

void BinarySearch(int a, int start, int end, bool &isFound, int &iterstart, int &iterend, FileHandler &in)
{
    if(end < start)
        return;
    if(start <= end)
    {
        int mid = start + (end-start)/2;
        int pageIndex = mid/intPerPage;
        int offset = mid%intPerPage;
        PageHandler pg = in.PageAt(pageIndex);
        in.FlushPages();
        char* data = pg.GetData();
        int curr;
        memcpy(&curr, &data[offset*sizeof(int)], sizeof(int));
        int prev = INT_MIN;
        if(mid > 0 && curr == a) {
            if(offset != 0)
                memcpy(&prev, &data[(offset-1)*sizeof(int)], sizeof(int));
            else {
                
                char* data1 = in.PageAt(pageIndex-1).GetData();
                in.FlushPages();
                memcpy(&prev, &data1[(intPerPage-1)*sizeof(int)], sizeof(int));
            }
        }
        
        int next = INT_MAX;
        if(mid < totalnumbers-1 && curr == a) {
            if(offset != intPerPage-1)
                memcpy(&next, &data[(offset+1)*sizeof(int)], sizeof(int));
            else {
                char* data1 = in.PageAt(pageIndex+1).GetData();
                in.FlushPages();
                memcpy(&next, &data1[0], sizeof(int));
            }
        }
        if(curr == a && prev < curr && next > curr) {
            isFound = true;
            iterstart = mid;
            iterend = mid;
            return;
        }
        if(curr == a && prev < curr) {
            isFound = true;
            iterstart = mid;
            if(iterend == -1)
                BinarySearch(a, mid+1, end, isFound, iterstart, iterend, in);
            return;
        }
        if(curr == a && next > curr) {
            isFound = true;
            iterend = mid;
            if(iterstart == -1)
                BinarySearch(a, start, mid-1, isFound, iterstart, iterend, in);
            return;
        }
        if(curr >= a && (iterstart == -1 || iterend == -1))
            BinarySearch(a, start, mid-1, isFound, iterstart, iterend, in);
        if(curr <= a && (iterstart == -1 || iterend == -1))
            BinarySearch(a, mid+1, end, isFound, iterstart, iterend, in);
    }
}

int main(int argc, const char* argv[]){
    if(argc <4)
    {
        cout<<"Few arguments"<<endl;
        return 0;
    }
    FileManager fm;
    FileHandler in = fm.OpenFile(argv[1]);
    FileHandler out = fm.CreateFile(argv[3]);
    PageHandler lp = in.LastPage();
    in.FlushPages();
    int totalPage = lp.GetPageNum()+1;
    intPerPage = PAGE_CONTENT_SIZE/sizeof(int);
    char* data = lp.GetData();
    int intLastPage = 0;
    int curr=0;
    int i=-4;
    while(intLastPage < intPerPage) {        
        memcpy(&curr, &data[i+sizeof(int)], sizeof(int));
        if(curr == INT_MIN)
            break;
        intLastPage++;
        i = i+4;
    }
    totalnumbers = (totalPage-1)*intPerPage+intLastPage;

    ifstream query;
    query.open(argv[2]);
    string temp;
    int sizeofout=0;
    while(query >> temp)
    {
        int a;
        query>>a;
        int start = 0;
        int end = totalnumbers-1;
        bool isFound = false;
        int iterstart = -1;
        int iterend = -1;
        BinarySearch(a, start, end, isFound, iterstart, iterend, in);
        if(isFound) {
            for(int i=iterstart; i<=iterend; i++) {
                int pageIndex = i/intPerPage;
                int offset = i%intPerPage;

                PageHandler pg;
                int offsetOut = 0;
                if(sizeofout%intPerPage == 0)
                    pg = out.NewPage();
                else {
                    offsetOut = sizeofout%intPerPage;
                    pg = out.LastPage();
                }
                data = pg.GetData();
                memcpy(&data[offsetOut*sizeof(int)], &pageIndex, sizeof(int));
                sizeofout++;
                if(offsetOut == intPerPage-1) {
                    out.MarkDirty(pg.GetPageNum());
                    out.FlushPages();
                    pg = out.NewPage();
                    offsetOut = -1;
                }
                data = pg.GetData();
                memcpy(&data[(offsetOut+1)*sizeof(int)], &offset, sizeof(int));
                sizeofout++;
                out.MarkDirty(pg.GetPageNum());
                out.FlushPages();
            }
        }
        PageHandler pg;
        int offset = 0;
        if(sizeofout%intPerPage == 0)
            pg = out.NewPage();
        else {
            offset = sizeofout%intPerPage;
            pg = out.LastPage();
        }
        data = pg.GetData();
        int t = -1;
        memcpy(&data[offset*sizeof(int)], &t, sizeof(int));
        sizeofout++;
        if(offset == intPerPage-1) {
            out.MarkDirty(pg.GetPageNum());
            out.FlushPages();
            pg = out.NewPage();
            offset = -1;
        }
        data = pg.GetData();
        memcpy(&data[(offset+1)*sizeof(int)], &t, sizeof(int));
        sizeofout++;
        out.MarkDirty(pg.GetPageNum());
        out.FlushPages();
    }

    if(sizeofout%intPerPage != 0) {
        PageHandler pg = out.LastPage();
        data = pg.GetData();
        int offset = sizeofout%intPerPage;
        for(int i=offset; i<intPerPage; i++) {
            int t = INT_MIN;
            memcpy(&data[i*4], &t, sizeof(int));
        }
        out.MarkDirty(pg.GetPageNum());
        out.FlushPages();
    }
    fm.CloseFile(in);
    fm.CloseFile(out);
}