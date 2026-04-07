#include "SerialQueue.hpp"
#include <iostream>

int main() {
    SerialQueue<int> q;
    int x;

    // empty test
    if (!q.dequeue(&x)) {
        std::cout << "Empty test passed\n";
    }

    // simple test
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);

    q.dequeue(&x);
    std::cout << x << std::endl;

    q.dequeue(&x);
    std::cout << x << std::endl;

    q.dequeue(&x);
    std::cout << x << std::endl;

    // 100 test
    for (int i = 1; i <= 100; i++) {
        q.enqueue(i);
    }

    bool ok = true;
    for (int i = 1; i <= 100; i++) {
        if (!q.dequeue(&x) || x != i) {
            ok = false;
        }
    }

    if (ok) std::cout << "100 test passed\n";

    return 0;
}