#include "Forward_list.h"
#include <forward_list>
#include <list>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory_resource>
#include <functional>
#include <algorithm>

using namespace std;

#include <cassert>

template <class Container>
void print(const Container& cont) {
	for (const auto& x : cont)
		cout << x << ' ';
	
	cout << '\n';
}

void print(const Forward_list<int>& list)
{
    std::cout << "list: {";
    for (char comma[3] = {'\0', ' ', '\0'}; int i : list)
    {
        std::cout << comma << i;
        comma[0] = ',';
    }
    std::cout << "}\n";
}

void test_insert_after() {
	Forward_list<int> ints{1, 2, 3, 4, 5};
    print(ints);
 
    // insert_after (2)
    auto beginIt = ints.begin();
    ints.insert_after(beginIt, -6);
    print(ints);
 
    // insert_after (3)
    auto anotherIt = beginIt;
    ++anotherIt;
    anotherIt = ints.insert_after(anotherIt, 2, -7);
    print(ints);
 
    // insert_after (4)
    const std::vector<int> v = {-8, -9, -10};
    anotherIt = ints.insert_after(anotherIt, v.cbegin(), v.cend());
    print(ints);
 
    // insert_after (5)
    ints.insert_after(anotherIt, {-11, -12, -13, -14});
    print(ints);
}
 
void test_splice_after() {
	using F = Forward_list<int>;
    
	// Demonstrate the meaning of open range (first, last)
    // in overload (5): the first element of l1 is not moved.
    F l1 = {1, 2, 3, 4, 5};
    F l2 = {10, 11, 12};
 
    l2.splice_after(l2.cbegin(), l1, l1.cbegin(), l1.cend());
    // Not equivalent to l2.splice_after(l2.cbegin(), l1);
    // which is equivalent to
    // l2.splice_after(l2.cbegin(), l1, l1.cbefore_begin(), l1.end());
 
    assert((l1 == F{1}));
    assert((l2 == F{10, 2, 3, 4, 5, 11, 12}));
 
    // Overload (1)
    F x = {1, 2, 3, 4, 5};
    F y = {10, 11, 12};
    x.splice_after(x.cbegin(), y);
    assert((x == F{1, 10, 11, 12, 2, 3, 4, 5}));
    assert((y == F{}));
 
    // Overload (3)
    x = {1, 2, 3, 4, 5};
    y = {10, 11, 12};
    x.splice_after(x.cbegin(), y, y.cbegin());
    assert((x == F{1, 11, 2, 3, 4, 5}));
    assert((y == F{10, 12}));
 
    // Overload (5)
    x = {1, 2, 3, 4, 5};
    y = {10, 11, 12};
    x.splice_after(x.cbegin(), y, y.cbegin(), y.cend());
    assert((x == F{1, 11, 12, 2, 3, 4, 5}));
    assert((y == F{10}));
}

int main()
{
	
}


