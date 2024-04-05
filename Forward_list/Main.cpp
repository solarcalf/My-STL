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

	Forward_list<int> l{ 1,2,3,4,5,6,7,8,9,10 };
	Forward_list<int> r{};

	r = l;

	for (auto x : r)
		cout << x << ' ';

	cout << '\n';

}

