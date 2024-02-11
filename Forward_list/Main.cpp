#include "Forward_list.h"
#include <forward_list>
#include <vector>
#include <iostream>
#include <unordered_map>

using namespace std;

#define Test

#ifndef Test

int main() {
	forward_list<pair<int, int>> l;

	l.emplace_front(1, 2);
	l.emplace_front(3, 2);

	auto it = l.begin();

	cout << it->first;


	forward_list<int> r;

	r.push_front(1);
	r.push_front(2);

	auto i = r.begin();

}

#endif




#ifdef Test

template <typename T>
class vec {
public:
	size_t sz;
	T arr[5];

public:
	template <bool IsConst>
	struct common_iterator {
	private:
		conditional_t<IsConst, const T*, T*> ptr;

	public:
		common_iterator(const T* ptr) :ptr(ptr) {}

		conditional_t<IsConst, const T&, T&> operator*() {
			return *ptr;
		}

		conditional_t<IsConst, const T*, T*> operator->() {
			return ptr;
		}

		common_iterator& operator++() {
			++ptr;
			return *this;
		}

		bool operator!=(common_iterator rhs) {
			return ptr != rhs.ptr;
		}
	};

	using const_iterator = common_iterator<true>;
	using iterator = common_iterator<false>;

	iterator begin() {
		return iterator(arr);
	}

	iterator end() {
		return iterator(arr + 5);
	}

	const_iterator begin() const {
		return const_iterator(arr);
	}

	const_iterator end() const {
		return const_iterator(arr + 5);
	}

	const_iterator cbegin() const {
		return const_iterator(arr);
	}

	const_iterator cend() const {
		return const_iterator(arr + 5);
	}

};

template <class Container>
void print(const Container& cont) {
	for (const auto& x : cont)
		cout << x << ' ';
	
	cout << '\n';
}


int main() {

	

	//Forward_list <vector< string >> l;
	//
	//
	//l.push_front({ {"123"}, {"g;kdlfhn"}});
	//l.emplace_front("12468");


	Forward_list<int> l{ 1,2,3,4,5,6,7,8,9,10 };

	for (auto x : l)
		cout << x << ' ';

	//print(l);



	//cout << ll.max_size();
}








#endif