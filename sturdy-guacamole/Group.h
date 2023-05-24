#pragma once

#include <vector>
#include <tuple>

template<typename T>
struct GroupComponent
{
	std::vector<T> column;
};

template<typename... Ts>
class Group : public GroupComponent<Ts>...
{
public:
	using FirstType = std::tuple_element_t<0, std::tuple<Ts...>>;

	void push_back(Ts&&... args) {
		(GroupComponent<Ts>::column.push_back(std::forward<Ts>(args)), ...);
	}

	template<typename T, typename... Args>
	void emplace_back(Args&&... args) {
		GroupComponent<T>::column.emplace_back(std::forward<Args>(args)...);
	}

	template<typename T>
	std::vector<T>& get() {
		return GroupComponent<T>::column;
	}

	template<typename T>
	T& get(size_t index) {
		return GroupComponent<T>::column[index];
	}

	//std::tuple<Ts&&...> get(size_t index) const {
	//	return std::make_tuple(GroupComponent<Ts>::column[index]...);
	//}

	void resize(size_t size) {
		(GroupComponent<Ts>::column.resize(size), ...);
	}

	void reserve(size_t size) {
		(GroupComponent<Ts>::column.reserve(size), ...);
	}

	size_t size() const {
		return GroupComponent<FirstType>::column.size();
	}
};
