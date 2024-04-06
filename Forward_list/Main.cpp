#include "Forward_list.h"
#include <forward_list>
#include <list>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory_resource>

using namespace std;


template <class Container>
void print(const Container& cont) {
	for (const auto& x : cont)
		cout << x << ' ';
	
	cout << '\n';
}


int main() {

	Forward_list<int> l{ 1, 2, 4 };
	Forward_list<int> r{ -1, -1, 0, 1, 2, 3, 5 };

	l.merge(r);

	print(l);
}

