#include <iostream>
#include <thread>
#include <vector>
void say_hello(int id) {
    std::cout << "Hello " << id << "\n";
}

int main() {
    int N = 2; // number of threads
    std::vector<std::thread> threads;
    for (int i = 0; i < N; i++) {
        threads.push_back(std::thread(say_hello, i));
    }
    for (auto& t : threads) {
        t.join();
    }
}
