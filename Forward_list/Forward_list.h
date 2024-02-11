#ifndef _Forward_List
#define _Forward_List

#define ND [[nodiscard]]

#include <xmemory>
#include <iostream>  /////// REMOVE

///	This is implementation for C++14
/// 
/// Questions and problems:
/// 1) If we have Forward_list instance and call operator= have we change allocator?												# In process
/// 2) Don't forget to fix allocator as a class field and in every method which uses alloc											# Fixed
/// 3) Ambiguous call	template <class Iterator>					 with  
/// 					Forward_list(Iterator first, Iterator last)		   Forward_list(size_type, const T&, const Allocator&)
/// 
/// 
/// There are what have to be edited for C++17:
/// 1) Mention std::contiguous_iterator_tag in  template <class Iterator>
/// 											Forward_list(Iterator first, Iterator last) 
/// 2) std::allocator<T>::construct is depricated -> use std::allocator_traits
/// 
/// 
/// Changes for C++20:
/// 1) iterator_tags are depricated, use concepts instead
/// 
/// 



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
		Node<U>* next;

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

	private:
		std::conditional_t<IsConst, const Node<T>*, Node<T>*> ptr = nullptr;
	
	public:
		common_iterator(Node<T>* ptr) : ptr(ptr) {}

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

	explicit Forward_list(size_type count, const Allocator& alloc = Allocator()) : Forward_list(count, T(), alloc) {}

	template <class Iterator>																												
	Forward_list(Iterator first, Iterator last, const Allocator& alloc = Allocator()) : alloc(alloc) {
		if constexpr (std::is_same_v<std::iterator_traits<Iterator>::iterator_category, std::bidirectional_iterator_tag> ||	
			std::is_same_v<std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>) {	
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

	Forward_list(const Forward_list& other) : Forward_list(other, Allocator()) {}																// Использовать стандартный аллокатор или other.alloc?

	Forward_list(Forward_list&& other, const Allocator& alloc) noexcept {																		// CHECK надо ли аллоцировать память по новому?
		if (this->alloc == other.alloc) {
			std::swap(head, other.head);
			std::swap(tail, other.tail);
			std::swap(sz, other.sz);
		}
		else { // Заново аллоцировать память с новым аллокатором и перемещать каждый объект в новую память
			
		}
	}

	Forward_list(Forward_list&& other) noexcept : Forward_list(std::move(other), std::move(other.alloc)) {}										

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

	Forward_list& operator=(const Forward_list& other) {						// CHECK
		if (this == &other)
			return *this;
		
		Forward_list<T> copied(other);
		this->swap(copied);

		return *this;
	}

	Forward_list& operator=(Forward_list&& other) noexcept {		// CHECK! Why noexcept
		if (std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value || alloc == other.alloc) {
			Forward_list<T> copied(std::move(other), other.alloc());
			// ?
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


	// Element access

	ND reference front() noexcept { return head->val; }

	ND const_reference front() const noexcept { return head->val; }


	// Capacity

	ND bool empty() noexcept { return sz == 0; }

	ND size_type size() noexcept { return sz; }


	// Modifiers

	void clear() {
		while (sz)
			pop_front();
	}


	void push_front(const T& val) {
		auto p = std::allocator_traits<NodeAlloc>::allocate(alloc, 1);
		std::allocator_traits<NodeAlloc>::construct(alloc, p, head, val);

		head = p;
		++sz;
	}

	void push_front(T&& val) {
		auto p = std::allocator_traits<NodeAlloc>::allocate(alloc, 1);
		std::allocator_traits<NodeAlloc>::construct(alloc, p, head, std::move(val));

		head = p;
		++sz;
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

	void swap(Forward_list& other) noexcept(std::allocator_traits<NodeAlloc>::is_always_equal::value) {
		if (std::allocator_traits<allocator_type>::propagate_on_container_swap::value)
			std::swap(alloc, other.alloc);
			
		std::swap(sz, other.sz);
		std::swap(head, other.head);
		std::swap(tail, other.tail);
	}


	// Operations

	size_type remove(const T& val) {
		size_type count = remove_if([](const T& x) {
			return x = val;
			});

		return count;
	}

	template <typename UnaryPredicate>
	size_type remove_if(UnaryPredicate p) {

	}

	void reverse() {
		if (!sz)
			return;

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

	size_type unique() {
		size_type count = unique([](int x, int y) {
			return x == y;
			});

		return count;
	}

	template< class BinaryPredicate >
	size_type unique(BinaryPredicate equal) {
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

	// merge sort
	void sort() {
		
	}

	template <typename Compare>
	void sort(Compare comp) { // comp = less

	}

public:
	using NodeAlloc = typename Allocator::template rebind<Node<T>>::other;
	
	NodeAlloc alloc;
	Node<T>* head = nullptr;
	Node<T>* tail = nullptr;
	size_t sz = 0;
};

#endif