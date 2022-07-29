#include "pipeline/fwd.hpp"
#include "pipeline/process.hpp"

#include <map>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm> 
#include <functional>
#include <type_traits>

#include <iostream>

#include <cstdint>

#pragma once

namespace ppl 
{
	template<typename ... Data>
	class Pipeline 
	{
	public:
		using process_type = Process<Data ... >;
		using id_type = std::uint32_t;
		using process_ptr = std::unique_ptr<process_type>;

		Pipeline();

		template <typename Type, typename... Args>
		void pushbk(id_type id, Args && ... args);

		template <typename Type, typename... Args>
		void insert(id_type befor, id_type id, Args && ... args);

		void detach(id_type id);

		void update(Data & ... data);

		void for_each(std::function<void(id_type id, process_type& process)> f);


	private:
		std::map<id_type, process_ptr> m_ProcessMap;
		std::vector<id_type>           m_ProcessOrder;
	};

	template <typename ... Data>
	Pipeline<Data ... >::Pipeline() : m_ProcessMap(), m_ProcessOrder() {}

	// template <typename ... Data>
	// using id_type = typename Pipeline<Data...>::id_type;

	template <typename ... Data>
	template <typename Type, typename... Args>
	void Pipeline<Data ... >::pushbk(id_type id, Args && ... args) 
	{
		//TODO static_assert(std::is_base_of<Process<Data>, Type>, "T should inherit from B");
		process_ptr ptr = std::make_unique<Type>(std::forward<Args>(args)...);

		m_ProcessMap[id] = std::move(ptr);
		m_ProcessOrder.push_back(id);
	}

	template <typename ... Data>
	template <typename Type, typename... Args>
	void Pipeline<Data ... >::insert(id_type befor, id_type id, Args && ... args) 
	{
		//TODO static_assert(std::is_base_of<Process<Data>, Type>, "T should inherit from B");
		process_ptr ptr = std::make_unique<Type>(std::forward<Args>(args)...);

		m_ProcessMap[id] = std::move(ptr);
		m_ProcessOrder.insert(std::find(m_ProcessOrder.begin(), m_ProcessOrder.end(), befor), id);
	}

	template <typename ... Data>
	void Pipeline<Data...>::detach(id_type id) 
	{
		m_ProcessMap.erase(m_ProcessMap.find(id));
		m_ProcessOrder.erase(std::find(m_ProcessOrder.begin(), m_ProcessOrder.end(), id));
	}

	template <typename ... Data>
	void Pipeline<Data ... >::update(Data & ... data) 
	{

		std::vector<std::pair<id_type, std::vector<id_type>::iterator>> abort = std::vector<std::pair<id_type, std::vector<id_type>::iterator>>();

		for(std::vector<id_type>::iterator it = m_ProcessOrder.begin(); it != m_ProcessOrder.end(); ++it) 
		{
			id_type id = *it;
			process_type& process = *m_ProcessMap[id];
		   	if (!process.paused && !process.aborted) { process.update(data...); }
			if (process.aborted) { abort.push_back(std::make_pair(id, it)); } 
		}

		for (std::vector<std::pair<id_type, std::vector<id_type>::iterator>>::reverse_iterator i = abort.rbegin(); i != abort.rend(); ++i ) 
		{
			id_type id = i->first;
			std::vector<id_type>::iterator it = i->second;

			m_ProcessMap.erase(m_ProcessMap.find(id));
			m_ProcessOrder.erase(it);
		} 


	}

	template <typename ... Data>
	void Pipeline<Data...>::for_each(std::function<void(id_type id, process_type& process)> f) 
	{
		for (id_type id : m_ProcessOrder) 
		{
			process_type& process = *m_ProcessMap[id];
			f(id, process);
		}
	}

}