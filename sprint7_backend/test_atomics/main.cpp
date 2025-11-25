#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <atomic>
#include <iostream>
#include <memory>
#include <thread>
using namespace std;
using namespace std::string_literals;

#include <boost/lockfree/spsc_queue.hpp>
#include <iostream>

int main() {


    std::jthread([](){

    });

    float arr[][2] = {{1,2},{3,4},{5,6}};
    int rows = sizeof(arr) / sizeof(arr[0]);      // 4 rows
    int cols = sizeof(arr[0]) / sizeof(arr[0][0]); // 3 columns
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++)
            std::cout << arr[i][j] << ' ';
        std::cout << std::endl;
    }




    std::atomic<float> at1;

    struct MyStruct{
        float a;
        float b;
        float c;
    };

    std::atomic<MyStruct> at2;


    std::cout << std::atomic<float>::is_always_lock_free << std::endl; 
    std::cout << std::atomic<MyStruct>::is_always_lock_free << std::endl; 
    std::cout << std::atomic<std::shared_ptr<MyStruct>>::is_always_lock_free << std::endl;

    boost::lockfree::spsc_queue<MyStruct, boost::lockfree::capacity<1024>> queue;

    if (queue.is_lock_free()) {
        std::cout << "The SPSC queue is lock-free!\n";
    } else {
        std::cout << "The SPSC queue may block.\n";
    }
}
