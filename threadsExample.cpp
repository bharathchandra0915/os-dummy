#include <iostream>
#include <thread>
#include <future>
#include <chrono>

using namespace std::chrono;
using namespace std;
typedef unsigned long long ull;

ull  find_even_sum(ull start, ull end)
{
    ull evensum =0;

    
    for (ull i = start ; i <= end; i++)
    {
        
        if((i&1) == 0) 
        evensum+=i;
    }
    return evensum;
    
}

ull find_odd_sum(ull start, ull end)
{
    
    ull oddsum=0;
    for (ull i = start ; i <= end; i++)
    {
        if((i&1) == 1)
         oddsum+=i;
    }
    return oddsum;
}

int main()
{
        
    

    ull start = 0;
    ull  end = 1900000000;
    auto starttime = high_resolution_clock::now();

    packaged_task <ull()> t1(find_even_sum(start, end));
    future<ull> evensum = t1.get_future();
    thread othread(move(t1));
    cout << evensum.get() << endl;
    othread.join();

    // thread t1(find_even_sum, start, end);
    // thread t2(find_odd_sum, start, end);

    // t1.join();
    // t2.join();


    // find_even_sum(start,end);
    // find_odd_sum(start, end);


    auto stoptime = high_resolution_clock::now();
    auto diff = duration_cast<milliseconds>( stoptime - starttime );
    // cout<< evensum <<endl;
    // cout << oddsum << endl;
    cout << "time taken " << diff.count() <<" microseconds"<< endl;
    return 0;
}
