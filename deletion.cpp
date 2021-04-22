#include <iostream>
#include "file_manager.h"
#include "errors.h"
#include <cstring>
#include <fstream>
#include <limits.h>

int intPerPage;
int totalnumbers;

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




void BinarySearch(int a, int start, int end, bool &isFound, int &iterstart, int &iterend, FileHandler &in)
{
    if(end < start)
        return;
    if(start <= end)
    {
        int mid = start + (end-start)/2;
        if(a==3)
        {
            cout<<mid<<endl;
        }
        int pageIndex = mid/intPerPage;
        int offset = mid%intPerPage;
        PageHandler pg = in.PageAt(pageIndex);
        char* data = pg.GetData();
        int curr;
        memcpy(&curr, &data[1*sizeof(int)], sizeof(int));
        cout<<"before"<<curr<<endl;
        in.FlushPages();
        memcpy(&curr, &data[1*sizeof(int)], sizeof(int));
        cout<<"after"<<curr<<endl;
        memcpy(&curr, &data[offset*sizeof(int)], sizeof(int));
        
        if(mid==6)
            {
                cout<<"mid: "<<mid<<"intPerPage: "<<intPerPage<<endl;
                cout<<"pageIndex : "<<pageIndex<<"offset: "<<offset<<endl;
                cout<<"curr: "<<curr<<endl;
            }
        int prev = INT_MIN;
        if(mid > 0 && curr == a) {
            if(offset != 0)
                memcpy(&prev, &data[(offset-1)*sizeof(int)], sizeof(int));
            else {
                if(mid==6)
                {
                    cout<<"SOMETHING"<<endl;
                }
                PageHandler p = in.PageAt(pageIndex-1);
                char* data1 = p.GetData();
                memcpy(&prev, &data1[(intPerPage-1)*sizeof(int)], sizeof(int));
            }
        }
        int next = INT_MAX;
        if(mid < totalnumbers-1 && curr == a) {
            if(offset != intPerPage-1)
            {
                memcpy(&next, &data[(offset+1)*sizeof(int)], sizeof(int));
                if(mid==6)
                {
                    cout<<"HELL YEAH"<<next<<endl;
                }
            }
            else {
                char* data1 = in.PageAt(pageIndex+1).GetData();
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
            {
                cout<<"yeah!"<<end<<endl;
                BinarySearch(a, mid+1, end, isFound, iterstart, iterend, in);
            }
            return;
        }
        if(mid==6)
            cout<<"next: "<<next<<endl;
        if(curr == a && next > curr) {
            isFound = true;
            iterend = mid;
            if(iterstart == -1)
            {
                cout<<"nah!"<<mid<<endl;
                BinarySearch(a, start, mid-1, isFound, iterstart, iterend, in);
            }
            return;
        }
        if(curr >= a && (iterstart == -1 || iterend == -1))
            BinarySearch(a, start, mid-1, isFound, iterstart, iterend, in);
        if(curr <= a && (iterstart == -1 || iterend == -1))
            BinarySearch(a, mid+1, end, isFound, iterstart, iterend, in);
    }
}

void deleteRange(int start, int diff, FileHandler &in)
{
    cout<<"start "<<start<<" diff "<<diff<<" totalNumbers "<<totalnumbers<<endl;
    if(start+diff >= totalnumbers)
    {
        for(int i=start; i<totalnumbers; i++)
        {
            int pageIndex = i/intPerPage;
            int offset = i%intPerPage;
            PageHandler pg = in.PageAt(pageIndex);
            char* data = pg.GetData();
            int t=INT_MIN;
            memcpy(&data[offset*sizeof(int)], &t, sizeof(int));
            in.MarkDirty(pg.GetPageNum());
            in.FlushPage(pg.GetPageNum());
        }
    }
    else {
        int src = start+diff;
        int dest = start;
        int pageIndexsrc = src/intPerPage;
        int offsetsrc = src%intPerPage;
        int pageIndexdest = dest/intPerPage;
        int offsetdest = dest%intPerPage;
        PageHandler pgsrc = in.PageAt(pageIndexsrc);
        PageHandler pgdest = in.PageAt(pageIndexdest);
        char* datasrc = pgsrc.GetData();
        char* datadest = pgdest.GetData();

        memcpy(&datadest[offsetdest*sizeof(int)], &datasrc[offsetsrc*sizeof(int)], sizeof(int));
        in.MarkDirty(pgdest.GetPageNum());
        in.FlushPages();
        deleteRange(start+1, diff, in);
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
    PageHandler lp = in.LastPage();
    printfile(in, "INPUT FILE");
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
    int totalnumbers1 = (totalPage-1)*intPerPage+intLastPage;
    totalnumbers = totalnumbers1;
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
            cout<<"Deleting from "<<iterstart<<" to "<<iterend<<" value: "<<a<<endl;
            int diff = iterend-iterstart+1;
            deleteRange(iterstart, iterend-iterstart+1, in);
            totalnumbers = totalnumbers-diff;
            printfile(in, "INPUT FILE");
            // break;
        }
    }
}