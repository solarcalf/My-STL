#ifndef _Queue
#define _Queue

#define ND [[nodiscard]]

#include <deque>


template <typename T, class Container = std::deque<T>>
class Queue {
public:
	using container_type	= typename Container;
	using value_type		= typename Container::value_type;
	using size_type			= typename Container::size_type;
	using reference			= typename Container::reference;
	using const_reference	= typename Container::const_reference;


	Queue() = default;

	Queue(const Queue& q) : cont(q.cont) {}

	Queue(Queue&& q) noexcept : cont(std::move(q.cont)) {}

	explicit Queue(const Container& cont) : cont(cont) {}

	explicit Queue(Container&& cont) noexcept(std::is_nothrow_move_constructible_v<Container>) : cont(std::move(cont)) {}

	Queue& operator=(const Queue& q) { 
		cont = q.cont; 
		return *this;
	}
	
	Queue& operator=(Queue&& q) noexcept(std::is_nothrow_move_assignable_v<Container>) {
		cont = std::move(q.cont);
		return *this;
	}


	// Element access
	reference front() noexcept(noexcept(cont.front())) { 
		return cont.front(); 
	}

	reference back() noexcept(noexcept(cont.back())) { 
		return cont.back(); 
	}

	const_reference front() const noexcept(noexcept(cont.front())) {
		return cont.front(); 
	}

	const_reference back() const noexcept(noexcept(cont.back())) { 
		return cont.back(); 
	}


	// Capacity
	ND size_type size() const noexcept(noexcept(cont.size())) { 
		return cont.size(); 
	}

	ND bool empty() const noexcept(noexcept(cont.empty())) { 
		return cont.empty();
	}


	// Modifiers
	void push(const value_type& val) { 
		cont.push_back(val); 
	}

	void push(value_type&& val) { 
		cont.push_back(std::move(val)); 
	}

	template<typename ... Args>
	void emplace(Args&&... args) { 
		cont.emplace_back(std::forward<Args>(args)...);
	}

	void pop() noexcept(noexcept(cont.pop_front())) { 
		cont.pop_front(); 
	}

	void swap(Queue& rhs) noexcept(std::is_nothrow_swappable_v<Container>) { 
		cont.swap(rhs);
	}

	ND const Container& Get_container() const noexcept {
		return cont;
	}

protected:
	Container cont;
};


#endif // !_Queue
