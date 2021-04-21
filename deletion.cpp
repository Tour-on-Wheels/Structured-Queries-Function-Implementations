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
        if(mid > 0 && curr == a)
        {
            if(offset != 0)
                memcpy(&prev, &data[(offset-1)*sizeof(int)], sizeof(int));
            else {
                
                char* data1 = in.PageAt(pageIndex-1).GetData();
                in.FlushPages();
                memcpy(&prev, &data1[(intPerPage-1)*sizeof(int)], sizeof(int));
            }
        }
        
        int next = INT_MAX;
        if(mid < totalnumbers-1 && curr == a)
        {
            if(offset != intPerPage-1)
                memcpy(&next, &data[(offset+1)*sizeof(int)], sizeof(int));
            else {
                char* data1 = in.PageAt(pageIndex+1).GetData();
                in.FlushPages();
                memcpy(&next, &data1[0], sizeof(int));
            }
        }
        if(curr == a && prev < curr && next > curr)
        {
            isFound = true;
            iterstart = mid;
            iterend = mid;
            return;
        }
        if(curr == a && prev < curr)
        {
            isFound = true;
            iterstart = mid;
            if(iterend == -1)
                BinarySearch(a, mid+1, end, isFound, iterstart, iterend, in);
            return;
        }
        if(curr == a && next > curr)
        {
            isFound = true;
            iterend = mid;
            if(iterstart == -1)
                BinarySearch(a, start, mid-1, isFound, iterstart, iterend, in);
            return;
        }
        if(curr >= a && (iterstart == -1 || iterend == -1))
        {
            BinarySearch(a, start, mid-1, isFound, iterstart, iterend, in);
        }
        if(curr <= a && (iterstart == -1 || iterend == -1))
        {
            BinarySearch(a, mid+1, end, isFound, iterstart, iterend, in);
        }
    }
}

int main(int argc, const char* argv[]) {
    if(argc < 3)
    {
        cout<<"Few arguments"<<endl;
        return 0;
    }
    FileManager fm;
    FileHandler in = fm.OpenFile(argv[1]);
    
}