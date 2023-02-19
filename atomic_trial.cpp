
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

using namespace std;
using namespace std::chrono;

std::atomic <long> ans(0);
// long ans = 0;

void sumnum( const vector<int> &arr, int strt, int end)
{
    for (int i = strt; i <=end; i++)
    {
        ans += arr[i];
    }
    
}


int main()
{
    // long ans =0;
    long actualans =0;
    vector <int> toBeSummed;
    for (int i = 0; i < 1900000; i++)
    {
        
        toBeSummed.push_back(i);
    }
    for (int i = 0; i < 1900000; i++)
    {
        actualans += toBeSummed[i];
    }
    cout << "The actual ans is " << actualans << endl;
    auto starttime = high_resolution_clock::now();

    // sumnum(toBeSummed, 0,  1900000-1, ans); // range of 4000 to 6000 microseconds and ans == 1074716082
    thread p1(sumnum, toBeSummed , 0, 950000);
    thread p2(sumnum, toBeSummed, 950001, 1900000-1);

    p1.join();
    p2.join();

    auto stoptime = high_resolution_clock::now();
    auto diff = duration_cast<microseconds>( stoptime - starttime );
    
    cout << "time taken " << diff.count() <<" microseconds"<< endl;
    cout << "multithread ans is " << ans<< endl;
   

}
