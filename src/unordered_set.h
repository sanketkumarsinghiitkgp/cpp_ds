#pragma once
#include <iostream>
#include <forward_list>
#include <string>
#include <vector>
#include <cstddef>
template<typename T, typename Hash=std::hash<T>>
class unordered_set {
	public:
	//using iterator = typename std::forward_list<std::pair<std::size_t, T>>::iterator; //typename is required since its a dependent name.
	unordered_set()
	{
		cap = 1;
		storage.push_front({hasher(T()),T()}); //dummy node
		buffer = new typename std::forward_list<std::pair<std::size_t, T>>::iterator[cap];
		buffer[0] = storage.end();
		load_factor=0;
		min_load_factor=0.25;
		max_load_factor=1;
		num_elements = 0;
		last_element=storage.begin();
	}
	unordered_set(const unordered_set<T>& other): unordered_set()  {
		//might not be the most efficient way should 'reserve' some space
		//TODO implement 'reserve()' function.
		for(auto &x: other)
		{
			insert(x);
		}
	}
	unordered_set(const std::vector<T>& init_list): unordered_set()
	{
		for (auto &x:init_list)
		{
			insert(x);
		}
	}
	~unordered_set() 
	{
		delete[] buffer;
	}
	struct iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using reference = value_type&;
		iterator(const typename std::forward_list<std::pair<std::size_t,T>>::iterator& _list_iterator): list_iterator(_list_iterator) {}
		reference operator*() const
		{
			return list_iterator->second;
		}
		iterator& operator++()//prefix increment
		{
			//exceptions?
			list_iterator++;
			return *this;
		}
		//postfix iterator
		iterator operator++(int)
		{
			iterator tmp = *this;
			list_iterator++;
			return tmp;
		}
		friend bool operator==(const iterator& a, const iterator& b)
		{
			return a.getListIterator()==b.getListIterator();
		}
		friend bool operator!=(const iterator& a, const iterator& b)
		{
			return a.getListIterator()!=b.getListIterator();
		}
		const typename std::forward_list<std::pair<std::size_t,T>>::iterator& getListIterator() const
		{
			return list_iterator;
		}
	private:
		typename std::forward_list<std::pair<std::size_t,T>>::iterator list_iterator;
	};
	struct const_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using reference = const value_type&;
		const_iterator(const typename std::forward_list<std::pair<std::size_t,T>>::const_iterator& _list_iterator): list_iterator(_list_iterator) {}
		reference operator*() const
		{
			return list_iterator->second;
		}
		const_iterator& operator++()//prefix increment
		{
			//exceptions?
			list_iterator++;
			return *this;
		}
		//postfix iterator
		const_iterator operator++(int)
		{
			const_iterator tmp = *this;
			list_iterator++;
			return tmp;
		}
		friend bool operator==(const const_iterator& a, const const_iterator& b)
		{
			return a.getListIterator()==b.getListIterator();
		}
		friend bool operator!=(const const_iterator& a, const const_iterator& b)
		{
			return a.getListIterator()!=b.getListIterator();
		}
		const typename std::forward_list<std::pair<std::size_t,T>>::const_iterator& getListIterator() const
		{
			return list_iterator;
		}
	private:
		typename std::forward_list<std::pair<std::size_t,T>>::const_iterator list_iterator;
	};
	void print() {
		for(auto &x:*this) {
			std::cout<<x<<" ";
		}
		std::cout<<std::endl;
	}
	bool insert(const T& value) {
		if (find(value)!=end())
			return false;
		if(num_elements+1>1ll*max_load_factor*cap) {
			cap*=2;
			rehash();
		}
		int hv = hasher(value);
		int h = std::abs((int)hv) % cap;
		if (buffer[h]!=storage.end()) {
			storage.insert_after(buffer[h], {hv, value});
		}
		else {
			auto actual_last_element = storage.insert_after(last_element,{hv, value});
			buffer[h] = last_element;
			last_element = actual_last_element;
		}
		num_elements++;
		load_factor = (num_elements*1.0/cap);
		return true; //not handling error scenarios;
	}
	bool rehash() {
		typename std::forward_list<std::pair<std::size_t, T>>::iterator* new_buffer = new typename std::forward_list<std::pair<std::size_t, T>>::iterator[cap];	
		std::forward_list<std::pair<std::size_t, T>> new_storage; 
		new_storage.push_front({hasher(T()),T()});
		auto new_last_element = new_storage.begin();
		std::fill_n(new_buffer, cap, new_storage.end());
		auto it = storage.begin();
		it++;
		//we need to make sure that "dummy" node value doesn't get copied in the rehashing process. Cost me hours :(.
		for(;it!=storage.end();it++) {
			auto element = *it;
			int h = std::abs((int)element.first)%cap;
			if(new_buffer[h]!=new_storage.end())
				new_storage.insert_after(new_buffer[h],{element.first, element.second});
			else {
				auto actual_new_last_element = new_storage.insert_after(new_last_element,{element.first, element.second});
				new_buffer[h]=new_last_element;
				new_last_element=actual_new_last_element;
			}
		}
		delete[] buffer;
		buffer = new_buffer;
		//storage is RAII so no need to explicitly "clear()" it.
		storage = std::move(new_storage); //std::move is mandatory for retaining iterators.
		load_factor = (num_elements*1.0/cap);
		last_element = new_last_element;
		return true; //not handling error scenarios;
	}
	
	bool erase(T value) {
		//I should probably decrease the cap of the hash table if the num_elements < some threshold, look into this
		int h = std::abs((int)hasher(value))%cap;		
		auto it = buffer[h];
		if (it == storage.end())
			return false;
		auto prev_it = it;
		it++;
		while(it!=storage.end() and std::abs((int)it->first)%cap==h) {
			if(it->second == value) {
				if (it == last_element) {
					last_element = prev_it;
				}
				storage.erase_after(prev_it);
				num_elements--;
				load_factor = (num_elements*1.0/cap);
				if (load_factor < min_load_factor) {
					cap/=2;
					rehash();
				}
				return true;
			}
			prev_it=it;
		}
		return false; //not handling error scenarios;
	}

	//  typenames that dependent on template parameter need the 'typename' keyword before them.
	iterator begin() 
	{
		auto it = storage.begin();
		it++; //skipping the dummy node
		return iterator(it);
	}
	iterator end() 
	{
		return iterator(storage.end());
	}
	const_iterator cbegin() const // made it const so that iterating over a const reference of unordered_set doesn't throw an error.
	{
		auto it=storage.cbegin();
		it++;
		return const_iterator(it);
	}
	const_iterator cend() const // made it const so that iterating over a const reference of unordered_set doesn't throw an error.
	{
		return const_iterator(storage.cend());
	}
	const_iterator begin() const // made it const so that iterating over a const reference of unordered_set doesn't throw an error.
	{
		return cbegin();
	}
	const_iterator end() const // made it const so that iterating over a const reference of unordered_set doesn't throw an error.
	{
		return cend();
	}
	iterator find(const T& value) {
		int h = std::abs((int)hasher(value))%cap;
		auto it = buffer[h];
		if(buffer[h]==storage.end())
			return end();
		it++;
		while(it!=storage.end() and std::abs((int)it->first)%cap==h) {
			if(it->second == value) {
				return it;
			}
			it++;
		}
		return iterator(storage.end());
	}
	private:
	typename std::forward_list<std::pair<std::size_t, T>>::iterator* buffer; 	
	typename std::forward_list<std::pair<std::size_t, T>>::iterator last_element;
	std::forward_list<std::pair<std::size_t,T>> storage;
	int cap;
	double load_factor;
	double max_load_factor;
	double min_load_factor;
	int num_elements;
	Hash hasher;
};
