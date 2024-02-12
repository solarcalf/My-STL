#include "Stack.h"
#include <iostream>
#include <deque>

using namespace std;

int main() {
    deque<int> d{1, 2, 3, 4, 5};
    Stack<int> s(d);

    while (s.size()) {
        cout << s.top() << '\n';
        s.pop();
    }
    
    return 0;
}