#include "pipeline/fwd.hpp"

#pragma once

namespace ppl 
{

	template<typename ... Args>
	class Process 
	{
	public:
		virtual void init(Args & ... args) {}
		virtual void update(Args & ... args) {}
		virtual void end(Args & ... args) {}

		void pause() { paused = true; }
		void play() { paused = false; }

		void abort() { aborted = true; }

		friend class Pipeline<Args...>;

	private:
		bool paused = false;
		bool aborted = false;
	};

}