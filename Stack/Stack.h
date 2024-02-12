#ifndef _Stack
#define _Stack

#define ND [[nodiscard]]

#include <deque>

template<typename T, class Container = std::deque<T>> 
class Stack {

public:
	using container_type	= Container;
	using value_type	= typename Container::value_type;
	using size_type		= typename Container::size_type;
	using reference		= typename Container::reference;
	using const_reference	= typename Container::const_reference;


	Stack() = default;

	Stack(const Stack<T>& s) noexcept(std::is_nothrow_copy_constructible_v<Container>) 
		: cont(s.cont) {}

	Stack(Stack<T>&& s) noexcept(std::is_nothrow_move_constructible_v<Container>) 
		: cont(std::move(s.cont)) {}

	explicit Stack(const Container& cont) noexcept(std::is_nothrow_copy_constructible_v<Container>) 
		: cont(cont) {}

	explicit Stack(Container&& cont) noexcept(std::is_nothrow_move_constructible_v<Container>) 
		: cont(std::move(cont)) {}

	Stack& operator=(const Stack& rhs) & noexcept(std::is_nothrow_assignable_v<Container>) {
		cont = rhs.cont;
		return *this;
	}

	Stack& operator=(Stack&& rhs) & noexcept(std::is_nothrow_move_assignable_v<Container>) {
		cont = std::move(rhs);
		return *this;
	}

	// Element access
	reference top() noexcept(noexcept(this->cont.back())) { 
		return cont.back();
	}

	const_reference top() const noexcept(noexcept(this->cont.back())) { 
		return cont.back();
	}

	// Capacity
	ND bool empty() const noexcept(noexcept(this->cont.empty())) { 
		return cont.empty();
	}

	ND size_type size() const noexcept(noexcept(this->cont.size())) { 
		return cont.size();
	}

	// Modifiers
	void push(const value_type& val) { 
		cont.push_back(val);
	}

	void push(value_type&& val) { 
		cont.push_back(std::move(val));
	}

	template< class... Args >
	void emplace(Args&&... args) { 
		cont.emplace_back(std::forward<Args>(args)...); 
	}

	void pop() noexcept(noexcept(this->cont.pop_back())) {
		cont.pop_back();
	}

	void swap(Stack& rhs) noexcept(std::is_nothrow_swappable_v<Container>) { 
		std::swap(cont, rhs.cont); 
	}

	ND const Container& _Get_container() const noexcept { 
		return cont; 
	}

protected:
	Container cont;	
};

template <typename T, class Container>
void swap(Stack<T, Container>& lhs, Stack<T, Container>& rhs) noexcept(std::is_nothrow_swappable_v<Container>) {
	lhs.swap(rhs);
}


#endif // _Stack
