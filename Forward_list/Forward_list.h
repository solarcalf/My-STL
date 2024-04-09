#ifndef _Forward_List
#define _Forward_List

#define ND [[nodiscard]]

#include <memory>
#include <iterator>
#include <functional>


template <typename T, typename Allocator = std::allocator<T>>
class Forward_list {
public:
	using value_type		= T;
	using size_type			= size_t;
	using reference			= value_type&;
	using const_reference	= const value_type&;
	using allocator_type	= Allocator;
	using difference_type	= std::ptrdiff_t;
	using pointer			= typename std::allocator_traits<Allocator>::pointer;
	using const_pointer		= typename std::allocator_traits<Allocator>::const_pointer;


private:
	template <typename U>
	struct Node {
		U val;
		mutable Node<U>* next;

		Node(Node<U>* next = nullptr, const U& val = U()) :val(val), next(next) {}
		Node(Node<U>* next = nullptr, U&& val = U()) :val(std::move(val)), next(next) {}

		template <class... Args>
		Node(Node<U>* next, Args&&... args) : val(std::forward<Args>(args)...), next(next) {}
	};


	template <bool IsConst>
	struct common_iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;  
		using reference = T&;  

		friend class Forward_list;

	private:
		std::conditional_t<IsConst, const Node<T>*, Node<T>*> ptr = nullptr;

	public:
		common_iterator(Node<T>* ptr) : ptr(ptr) {}

		template <bool IsOtherConst>
		common_iterator(common_iterator<IsOtherConst> other) : ptr(other.ptr) {}

		std::conditional_t<IsConst, const T&, T&> operator*() {
			return ptr->val;
		}

		std::conditional_t<IsConst, const T*, T*> operator->() {
			return &(ptr->val);
		}

		common_iterator& operator++() {
			ptr = ptr->next;
			return *this;
		}

		common_iterator operator++(int) {
			common_iterator copy_iter(this->ptr);
			++(*this);
			return copy_iter;
		}

		template <bool IsOtherConst>
		bool operator!=(common_iterator<IsOtherConst> other) {
			return ptr != other.ptr;
		}

		template <bool IsOtherConst>
		bool operator==(common_iterator<IsOtherConst> other) {
			return ptr == other.ptr;
		}

	};

public:
	using iterator			=	common_iterator<false>;
	using const_iterator	=	common_iterator<true>;

	ND iterator begin() noexcept {
		return iterator(head);
	}

	ND iterator end() noexcept {
		return iterator(tail);
	}

	ND const_iterator begin() const noexcept {
		return const_iterator(head);
	}

	ND const_iterator end() const noexcept {
		return const_iterator(tail);
	}

	ND const_iterator cbegin() const noexcept {
		return const_iterator(head);
	}

	ND const_iterator cend() const noexcept {
		return const_iterator(tail);
	}


	Forward_list() {};

	explicit Forward_list(const Allocator& alloc) : alloc(alloc) {}

	Forward_list(size_type count, const T& value, const Allocator& alloc = Allocator()): alloc(alloc) {		
		for (size_t i = 0; i < count; ++i) 
			push_front(value);
	}

	template <typename U = T, std::enable_if_t<std::is_default_constructible_v<U>, int> = 0>
	explicit Forward_list(size_type count, const Allocator& alloc = Allocator()) : Forward_list(count, T(), alloc) {}

	template<class Iterator, typename std::enable_if_t<
    std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category> &&
    !std::is_integral_v<Iterator>, Iterator>* = nullptr>
	Forward_list(Iterator first, Iterator last, const Allocator& alloc = Allocator()) : alloc(alloc) {
		if constexpr (std::is_same_v<typename std::iterator_traits<Iterator>::iterator_category, std::bidirectional_iterator_tag> ||	
					  std::is_same_v<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>) {	
			if (last == first)
				return;

			--last;
			while (last > first) 
				push_front(*(last--));

			push_front(*first);
		}
		else {
			while (first != last)
				push_front(*(first++));

			reverse();
		}
	}

	Forward_list(const Forward_list& other, const Allocator& alloc) : alloc(alloc) {
		if (!other.sz)
			return;
		
		auto iter = other.head;
		push_front(iter->val);
		iter = iter->next;
		Node<T>* pre_tail = head;

		while (iter != other.tail) {
			tail = std::allocator_traits<NodeAlloc>::allocate(this->alloc, 1);
			std::allocator_traits<NodeAlloc>::construct(this->alloc, tail, nullptr, iter->val);

			pre_tail->next = tail;
			iter = iter->next;
			pre_tail = tail;
			tail = nullptr;
		}
	} 

	Forward_list(const Forward_list& other) : Forward_list(other, std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.get_allocator())) {}

	Forward_list(Forward_list&& other, const Allocator& alloc) noexcept {
		if (this->alloc == other.alloc) {
			std::swap(head, other.head);
			std::swap(tail, other.tail);
			std::swap(sz, other.sz);
		}
		else {
			Forward_list copied(other, alloc);
			this->swap(copied);
		}
	}

	Forward_list(Forward_list&& other) noexcept : Forward_list(std::move(other), std::move(other.get_allocator())) {}										

	Forward_list(std::initializer_list<T> init, const Allocator& alloc = Allocator()) : alloc(alloc) {
		auto first = init.begin();
		auto last = init.end();
		
		if (last == first)
			return;

		--last;

		while (last > first) {
			push_front(std::move(*last));
			last--;
		}

		push_front(std::move(*first));
	}

	~Forward_list() { clear(); }

	Forward_list& operator=(const Forward_list& other) {
		if (this == &other)
			return *this;
		
		if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_copy_assignment::value) {
			Forward_list<T> copied(other, other.get_allocator());
			this->swap(copied);
			alloc = other.alloc;
			copied.alloc = NodeAlloc();
		}
		else {
			Forward_list<T> copied(other, get_allocator());
			this->swap(copied);
		}
		
		return *this;
	}

	Forward_list& operator=(Forward_list&& other) noexcept(std::allocator_traits<Allocator>::is_always_equal::value) {
		if (std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value || alloc == other.alloc) {
			Forward_list<T> copied(std::move(other), other.get_allocator());
			this->swap(copied);
			alloc = other.alloc;
			copied.alloc = NodeAlloc();
		}
		else {
			Forward_list<T> copied(std::move(other));
			this->swap(copied);

			alloc = other.alloc;
		}
		return *this;
	}

	Forward_list& operator=(std::initializer_list<T> ilist) {
		Forward_list copied(ilist, alloc);
		this->swap(copied);

		return *this;
	}

	constexpr allocator_type get_allocator() const { return Allocator(); }


	// Element access

	ND reference front() noexcept { return head->val; }

	ND const_reference front() const noexcept { return head->val; }


	// Capacity

	ND bool empty() const noexcept { return sz == 0; }

	ND size_type size() const noexcept { return sz; }


	// Modifiers

	void clear() {
		while (sz) pop_front();
	}

	void push_front(const T& val) {
		emplace_front(val);
	}

	void push_front(T&& val) {
		emplace_front(std::move(val));
	}

	void pop_front() {
		auto new_head = head->next;

		std::allocator_traits<NodeAlloc>::destroy(alloc, head);
		std::allocator_traits<NodeAlloc>::deallocate(alloc, head, 1);

		head = new_head;
		--sz;
	}

	template <class... Args>
	reference emplace_front(Args&&... args) { 
		auto p = std::allocator_traits<NodeAlloc>::allocate(alloc, 1);
		std::allocator_traits<NodeAlloc>::construct(alloc, p, head, std::forward<Args>(args)...);
		
		++sz;
		head = p;
		return head->val;
	}

	void swap(Forward_list& other) noexcept(std::allocator_traits<Allocator>::is_always_equal::value) {
		if (std::allocator_traits<allocator_type>::propagate_on_container_swap::value)
			std::swap(alloc, other.alloc);
			
		std::swap(sz, other.sz);
		std::swap(head, other.head);
		std::swap(tail, other.tail);
	}

	iterator insert_after(const_iterator pos, const T& value) {
		return emplace_after(pos, value);
	}

	iterator insert_after(const_iterator pos, const T&& value) {
		return emplace_after(pos, std::move(value));
	}

	iterator insert_after( const_iterator pos, size_type count, const T& value ) {
		if (count == 0) return iterator(const_cast<Node<T>*>(pos.ptr));
		Node<T>* pointer_to_pos = const_cast<Node<T>*>(pos.ptr);
		Node<T>* next_to_pos = pointer_to_pos->next;

		Node<T>* new_nodes_first = std::allocator_traits<NodeAlloc>::allocate(alloc, 1);
		std::allocator_traits<NodeAlloc>::construct(alloc, new_nodes_first, nullptr, value);
		Node<T>* new_nodes_last = new_nodes_first;

		for (size_t i = 1; i < count; ++i) {
			Node<T>* new_node = std::allocator_traits<NodeAlloc>::allocate(alloc, 1);
			std::allocator_traits<NodeAlloc>::construct(alloc, new_node, nullptr, value);
			new_nodes_last->next = new_node;
			new_nodes_last = new_node;
		}

		pointer_to_pos->next = new_nodes_first;
		new_nodes_last->next = next_to_pos;
		sz += count;
		return iterator(new_nodes_last);
	}

	template<class InputIt, typename std::enable_if<
    std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>::value &&
    !std::is_integral<InputIt>::value, InputIt>::type* = nullptr>
	iterator insert_after(const_iterator pos, InputIt first, InputIt last) {
		Node<T>* pointer_to_pos = const_cast<Node<T>*>(pos.ptr);
		if (first == last) return iterator(pointer_to_pos);
		Node<T>* next_to_pos = pointer_to_pos->next;

		Node<T>* new_nodes_first = std::allocator_traits<NodeAlloc>::allocate(alloc, 1);
		std::allocator_traits<NodeAlloc>::construct(alloc, new_nodes_first, nullptr, *(first++));
		Node<T>* new_nodes_last = new_nodes_first;
		++sz;

		while (first != last) {
			Node<T>* new_node = std::allocator_traits<NodeAlloc>::allocate(alloc, 1);
			std::allocator_traits<NodeAlloc>::construct(alloc, new_node, nullptr, *(first++));
			new_nodes_last->next = new_node;
			new_nodes_last = new_node;
			++sz;
		}

		pointer_to_pos->next = new_nodes_first;
		new_nodes_last->next = next_to_pos;
		return iterator(new_nodes_last);
	}

	iterator insert_after(const_iterator pos, std::initializer_list<T> ilist) {
		return insert_after(pos, ilist.begin(), ilist.end());
	}

	template< class... Args >
	iterator emplace_after(const_iterator pos, Args&&... args) {
		Node<T>* pointer_to_pos = const_cast<Node<T>*>(pos.ptr);
		Node<T>* next_to_pos = pointer_to_pos->next;

		Node<T>* new_node = std::allocator_traits<NodeAlloc>::allocate(alloc, 1);
		std::allocator_traits<NodeAlloc>::construct(alloc, new_node, next_to_pos, std::forward<Args>(args)...);
		pointer_to_pos->next = new_node;
		++sz;

		return iterator(new_node);
	}

	iterator erase_after(const_iterator pos) {
		Node<T>* pointer_to_pos = const_cast<Node<T>*>(pos.ptr);
		if (pointer_to_pos->next == nullptr) return iterator(nullptr);
		Node<T>* next_to_pos = pointer_to_pos->next;
		pointer_to_pos->next = next_to_pos->next;

		std::allocator_traits<NodeAlloc>::destroy(alloc, next_to_pos);
		std::allocator_traits<NodeAlloc>::deallocate(alloc, next_to_pos, 1);
		--sz;
		return iterator(next_to_pos);
	}

	iterator erase_after(const_iterator first, const_iterator last) {
		if (first == last) return iterator(const_cast<Node<T>*>(last.ptr));
		Node<T>* pointer_to_last = const_cast<Node<T>*>(last.ptr);
		Node<T>* pointer_to_first = const_cast<Node<T>*>(first.ptr);
		Node<T>* to_erase = pointer_to_first->next;

		while (to_erase != pointer_to_last) {
			Node<T>* next_to_erase = to_erase->next;
			std::allocator_traits<NodeAlloc>::destroy(alloc, to_erase);
			std::allocator_traits<NodeAlloc>::deallocate(alloc, to_erase, 1);
			to_erase = next_to_erase;
			--sz;
		}

		pointer_to_first->next = pointer_to_last;
		return iterator(pointer_to_last);
	}

	template <typename U = T, std::enable_if_t<std::is_default_constructible<U>::value, int> = 0>
	void resize(size_type count) {
		resize(count, T());
	}

	void resize( size_type count, const T& value ) {
		if (count == sz) return;
		while (sz > count)
			pop_front();
		while (sz < count)
			push_front(value);
	}

	// Operations
	size_type remove(const T& val) {
		size_type count = remove_if([val](const T& x) { return x == val; });
		return count;
	}

	template <typename UnaryPredicate>
	size_type remove_if(UnaryPredicate p) {
		size_t removed = 0;
		while (head && p(head->val)) {
			pop_front();
			++removed;
		}

		if (!sz) return 0;
		Node<T>* left = head;
		Node<T>* right = head->next;

		while (right) {
			if (p(right->val)) {
				left->next = right->next;

				std::allocator_traits<NodeAlloc>::destroy(alloc, right);
				std::allocator_traits<NodeAlloc>::deallocate(alloc, right, 1);
				--sz;

				++removed;
				right = left->next;
			}
			else {
				left = right;
				right = right->next;
			}
		}

		return removed;
	}

	void reverse() {
		if (!sz) return;

		Node<T>* left = nullptr;
		Node<T>* right = head;
		tail = left;

		while (right) {
			auto next_to_right = right->next;
			right->next = left;
			left = right;
			right = next_to_right;
		}

		head = left;
	}

	template< class BinaryPredicate = std::equal_to<T>>
	size_type unique(BinaryPredicate equal = BinaryPredicate()) {
		if (sz <= 1)
			return 0;

		size_type count = 0;
		Node<T>* cur = head, * next = head->next;

		while (next != nullptr) {
			if (equal(cur->val, next->val)) {
				next = next->next;
				std::allocator_traits<NodeAlloc>::destroy(alloc, cur->next);
				std::allocator_traits<NodeAlloc>::deallocate(alloc, cur->next, 1);

				cur->next = next;
				++count;
				--sz;
			}
			else {
				cur = next;
				next = next->next;
			}
		}

		return count;
	}

	void splice_after(const_iterator pos, Forward_list& other) {
		if (other.empty()) return;

		Node<T>* before_other_end = other.begin().ptr;
		while (before_other_end->next != nullptr)
			before_other_end = before_other_end->next;
		
		Node<T>* pointer_to_pos = const_cast<Node<T>*>(pos.ptr);
		Node<T>* next_to_pos = pointer_to_pos->next;

		pointer_to_pos->next = other.begin().ptr;
		before_other_end->next = next_to_pos;

		other.head = nullptr;
		other.sz = 0;
		sz += other.sz;
	}

	void splice_after(const_iterator pos, Forward_list& other, const_iterator it) {
		Node<T>* pointer_to_it = const_cast<Node<T>*>(it.ptr);
		Node<T>* pointer_to_pos = const_cast<Node<T>*>(pos.ptr);
		Node<T>* next_to_it = pointer_to_it->next;
		Node<T>* next_to_pos = pointer_to_pos->next;
		
		pointer_to_it->next = next_to_it->next;
		pointer_to_pos->next = next_to_it;
		next_to_it->next = next_to_pos;
		++sz;
		--other.sz;
	}

	void splice_after(const_iterator pos, Forward_list& other, const_iterator first, const_iterator last) {
		if (first == last) return;

		Node<T>* pointer_to_other_first = const_cast<Node<T>*>(first.ptr);
		Node<T>* pointer_to_other_last = const_cast<Node<T>*>(last.ptr);
		Node<T>* before_other_last = pointer_to_other_first;

		size_t count = 0;
		while (before_other_last->next != pointer_to_other_last) {
			before_other_last = before_other_last->next;
			++count;
		}

		if (count == 0) return;
		sz += count;
    	other.sz -= count;

		Node<T>* pointer_to_pos = const_cast<Node<T>*>(pos.ptr);
		Node<T>* next_to_pos = pointer_to_pos->next;

		pointer_to_pos->next = pointer_to_other_first->next;
		before_other_last->next = next_to_pos;
		pointer_to_other_first->next = pointer_to_other_last;
	}


	template <typename Compare = std::less<T>>
	void merge(Forward_list<T, Allocator>& other) {
		if (this == &other || other.head == nullptr) return;

		Node<T>* left = this->head;
		Node<T>* right = other.head;
		this->sz += other.sz;
		other.sz = 0;

		if (head == nullptr) {
			head = right;
			right = nullptr;
			return;
		}

		Compare comp;
		if (comp(right->val, head->val)) {
			Node<T>* great = right;
			while (great->next && comp(great->next->val, head->val))
				great = great->next;

			Node<T>* next_to_great = great->next;
			great->next = head;
			head = right;
			right = next_to_great;
		}

		while (left && right && left->next) {
			if (comp(left->next->val, right->val)) { 
				left = left->next;
			}
			else {
				Node<T>* next_to_right = right->next;
				Node<T>* next_to_left = left->next;

				left->next = right;
				right->next = next_to_left;
				right = next_to_right;
			}
		}

		left->next = right;
	}
	
private:

	template <typename _Iter, typename Compare>
	void merge_sort(_Iter begin, _Iter end, const Compare& comp) {
		size_t dist = std::distance(begin, end);
		if (dist <= 1) return;

		_Iter mid = std::next(begin, dist / 2); 
	
		merge_sort(begin, mid, comp); // [b, m)
		merge_sort(mid, end, comp); // [m, e)

		T* buffer = new T[dist];
		size_t ind = 0;
		_Iter left = begin;
		_Iter right = mid;

		while (left != mid && right != end) {
			if (comp(*left, *right)) buffer[ind] = std::move(*(left++));
			else buffer[ind] = std::move(*(right++));
			ind++;
		}

		while (left != mid)
			buffer[ind++] = std::move(*(left++));
		while (right != end)
			buffer[ind++] = std::move(*(right++));

		for (size_t i = 0; i < dist; ++i) 
			*(begin++) = std::move(buffer[i]);

		delete[] buffer;
	}

public:
	template <typename Compare = std::less<T>>
	void sort(Compare comp = Compare()) {
		merge_sort(begin(), end(), comp);
	}	

	using NodeAlloc = typename Allocator::template rebind<Node<T>>::other;
	
	NodeAlloc alloc;
	Node<T>* head = nullptr;
	Node<T>* tail = nullptr;
	size_t sz = 0;
};


enum class ListCompareResult {
		Less,
		Equal,
		Greater 
	};

template<typename T, typename Allocator>
ListCompareResult compare_lists(const Forward_list<T, Allocator>& lhs, const Forward_list<T, Allocator>& rhs) {
    auto lit = lhs.begin(), rit = rhs.begin();
    while (lit != lhs.end() && rit != rhs.end()) {
        if (*lit < *rit) return ListCompareResult::Less;
        if (*rit < *lit) return ListCompareResult::Greater;
        ++lit;
        ++rit;
    }
    if (lit == lhs.end() && rit == rhs.end()) return ListCompareResult::Equal;
    if (lit == lhs.end()) return ListCompareResult::Less;
    return ListCompareResult::Greater;
}

template<typename T, typename Allocator>
bool operator==(const Forward_list<T, Allocator>& lhs, const Forward_list<T, Allocator>& rhs) {
    return compare_lists(lhs, rhs) == ListCompareResult::Equal;
}

template<typename T, typename Allocator>
bool operator!=(const Forward_list<T, Allocator>& lhs, const Forward_list<T, Allocator>& rhs) {
    return compare_lists(lhs, rhs) != ListCompareResult::Equal;
}

template<typename T, typename Allocator>
bool operator<=(const Forward_list<T, Allocator>& lhs, const Forward_list<T, Allocator>& rhs) {
    return compare_lists(lhs, rhs) != ListCompareResult::Greater;
}

template<typename T, typename Allocator>
bool operator>(const Forward_list<T, Allocator>& lhs, const Forward_list<T, Allocator>& rhs) {
    return compare_lists(lhs, rhs) == ListCompareResult::Greater;
}

template<typename T, typename Allocator>
bool operator>=(const Forward_list<T, Allocator>& lhs, const Forward_list<T, Allocator>& rhs) {
    return compare_lists(lhs, rhs) != ListCompareResult::Less;
}

namespace std
{
	template <typename T, typename Allocator>
	void swap(Forward_list<T, Allocator>& l, Forward_list<T, Allocator>& r) {
		l.swap(l, r);
	}
}

#endif
