#include "ConcurrentQueue.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>

void producer(ConcurrentQueue<int>& q, int num_ints) {
    for (int i = 0; i < num_ints; i++) {
        q.enqueue(i);
    }
}

void consumer(ConcurrentQueue<int>& q, int num_ints) {
    int x;
    int count = 0;

    while (count < num_ints) {
        if (q.dequeue(&x)) {
            count++;
        }
    }
}

bool testQueue(int num_producers, int num_consumers, int num_ints) {
    ConcurrentQueue<int> q;
    std::vector<std::thread> threads;

    threads.reserve(num_producers + num_consumers);

    for (int i = 0; i < num_producers; i++) {
        threads.push_back(std::thread(producer, std::ref(q), num_ints));
    }

    for (int i = 0; i < num_consumers; i++) {
        threads.push_back(std::thread(consumer, std::ref(q), num_ints));
    }

    for (int i = 0; i < (int)threads.size(); i++) {
        threads[i].join();
    }

    int expected = (num_producers - num_consumers) * num_ints;
    return q.size() == expected;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "Usage: ./ConcurrentQueueTest producers consumers ints\n";
        return 1;
    }

    int num_producers = std::atoi(argv[1]);
    int num_consumers = std::atoi(argv[2]);
    int num_ints = std::atoi(argv[3]);

    if (testQueue(num_producers, num_consumers, num_ints)) {
        std::cout << "Test passed\n";
    } else {
        std::cout << "Test failed\n";
    }

    return 0;
}