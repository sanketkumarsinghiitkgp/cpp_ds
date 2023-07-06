#include <iostream>
#include <forward_list>
#include <string>
#include <vector>
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
		max_load_factor=1;
		num_elements = 0;
		last_element=storage.begin();
	}
	unordered_set(const unordered_set<T>& other) {
		//might not be the most efficient way should 'reserve' some space
		//TODO implement 'reserve()' function.
		for(auto it=other.begin();it!=other.end();it++)
		{
			insert(*it);
		}
	}
	unordered_set(const std::vector<T>& init_list)
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
	void print() {
		for(typename unordered_set<T>::iterator it = begin(); it!=end(); it++) {
			std::cout<<*it<<" ";
		}
		std::cout<<std::endl;
	}
	bool insert(const T& value) {
		if (find(value)!=end())
			return false;
		if(num_elements+1>1ll*max_load_factor*cap) {
			rehash();
		}
		int hv = hasher(value);
		int h = hv % cap;
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
		cap*=2;
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
			int h = element.first%cap;
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
		storage = std::move(new_storage); //std::move is mandatory for retaining iterators.
		load_factor = (num_elements*1.0/cap);
		last_element = new_last_element;
		return true; //not handling error scenarios;
	}
	
	bool erase(int value) {
		int h = hasher(value)%cap;		
		auto it = buffer[h];
		if (it == storage.end())
			return false;
		auto prev_it = it;
		it++;
		while(it!=storage.end() and it->first%cap==h) {
			if(it->second == value) {
				if (it == last_element) {
					last_element = prev_it;
				}
				storage.erase_after(prev_it);
				num_elements--;
				load_factor = (num_elements*1.0/cap);
				return true;
			}
			prev_it=it;
		}
		return false; //not handling error scenarios;
	}

	//  typenames that dependent on template parameter need the 'typename' keyword before them.
	iterator begin()
	{
		return iterator(storage.begin());
	}
	iterator end()
	{
		return iterator(storage.end());
	}
	iterator find(const T& value) {
		int h = hasher(value)%cap;
		auto it = buffer[h];
		if(buffer[h]==storage.end())
			return end();
		it++;
		while(it!=storage.end() and it->first%cap==h) {
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
	int num_elements;
	Hash hasher;
};

int main()
{
	unordered_set<int> mp;
	mp.insert(1);
	mp.print();
	mp.insert(2);
	mp.print();
	mp.insert(3);
	mp.print();
	mp.erase(2);
	mp.print();
	mp.insert(3);
	mp.print();
	mp.insert(10);
	mp.print();
	std::cout<<(mp.find(10)!=mp.end()?"found":"not found")<<std::endl;
}
