module;

#include <cassert>

export module context_handle;

import std;
using namespace std;

export extern "C++" namespace t {
	template<typename Context>
	class context_handle_t {
		unique_ptr<Context> context_;
	public:
		template<typename... Args>
		context_handle_t(Args&&... args);
		~context_handle_t();

		operator Context&();
		operator const Context&() const;
	};

	template<typename Context>
	context_handle_t<Context>::operator Context&() {
		assert(context_ != nullptr);
		return *context_;
	}

	template<typename Context>
	context_handle_t<Context>::operator const Context&() const {
		return const_cast<const Context&>(const_cast<context_handle_t*>(this)->operator Context&());
	}
}
