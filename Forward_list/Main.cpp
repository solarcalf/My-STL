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


template <class Container>
void print(const Container& cont) {
	for (const auto& x : cont)
		cout << x << ' ';
	
	cout << '\n';
}


int main() {

	vector<int> v{1, 2, 3};
	
	Forward_list<int> l(v.begin(), v.end());
	Forward_list<int> r(3, 0);
	

	// Forward_list<int> r{-1, -2, -3, -4};

	print(l);
	print(r);
}


