#ifdef DPP_CORO
#pragma once
#include <coroutine>
#include <dpp/restresults.h>

namespace dpp {
	
	/**
	 * @brief Shorthand for the coroutine handle's type
	 */
	using handle_type = std::coroutine_handle<struct promise>;

	class cluster;

	/**
	 * @brief Return type for coroutines
	 */
	struct task {
		/**
		 * @brief Required nested promise_type for coroutines
		 */
		using promise_type = dpp::promise;
	};

	/**
	 * @brief Implementation of promise_type for dpp's coroutines
	 */
	struct promise {
		/**
		 * @brief A pointer to the cluster making the requests in the coroutine
		 */
		cluster* bot = nullptr;

		/**
		 * @brief The result of the last co_await-ed function
		 */
		confirmation_callback_t callback;

		/**
		 * @brief Construct a new promise object
		 */
		promise() = default;

		/**
		 * @brief Construct a new promise object
		 * 
		 * @param ev Base type of all events, only used to get the dpp::cluster pointer
		 */
		promise(const dpp::event_dispatch_t& ev) : bot(ev.from->creator) { }

		/**
		 * @brief Get the return  object
		 * 
		 * @return task dpp::task type 
		 */
		task get_return_object() {
			return {};
		}

		/**
		 * @brief Function called when the coroutine is first suspended, never suspends
		 * 
		 * @return std::suspend_never Never suspend this coroutine at the first suspend point
		 */
		std::suspend_never initial_suspend() noexcept {
			return {};
		}

		/**
		 * @brief Function called when the coroutine reaches its last suspension point
		 * 
		 * @return std::suspend_never Never suspend this coroutine at the final suspend point
		 */
		std::suspend_never final_suspend() noexcept {
			return {};
		}

		/**
		 * @brief Function called when the coroutine returns nothing
		 */
		void return_void() noexcept {}

		/**
		 * @brief Function called when coroutine throws a un-catch-ed exception. Does nothing
		 */
		void unhandled_exception() { 
			/* try { std::rethrow_exception(std::current_exception()); } */ 
			/* catch (const std::exception& e) { std::cout << e.what() << '\n'; } */ 
		}
	};

	/**
	 * @brief A co_await-able struct which returns the result of stored api call when co_await-ed. Meant to be opaque to the user
	 * 
	 * @tparam T The type of the function (lambda if auto-generated by the php script) handling the making of api call
	 */
	template <typename T> 
	struct awaitable {
		/**
		 * @brief Pointer to the nested promise object of the coroutine, used for storing and retrieving the result of an api call
		 */
		promise* p;
		
		/**
		 * @brief Pointer to the cluster making the api request
		 */
		cluster* bot;

		/**
		 * @brief The function handling the making of request, using the cluster pointer
		 */
		T api_req;

		/**
		 * @brief Construct a new awaitable object
		 * 
		 * @param cl pointer to the cluster making the api request 
		 * @param api_call a function to invoke with the cluster pointer, handles the making of request
		 */
		awaitable(cluster* cl, T api_call) : bot{cl}, api_req{api_call} {} 

		/**
		 * @brief First function called when this object is co_await-ed, its return type tells if the coroutine should be immediately suspended
		 * 
		 * @return bool false, signifying immediate suspension  
		 */
		bool await_ready() noexcept {
			return false;
		}

		/**
		 * @brief Function called when the coroutine is suspended, makes the api request and queues the resumption of the suspended coroutine, storing the result in promise object
		 * 
		 * @param handle the handle to the suspended coroutine
		 */
		void await_suspend(handle_type handle) { 
			/* p = &handle.promise(); */
			/* if (!p->bot) p->bot = bot; */
			api_req([handle](const confirmation_callback_t& cback) { handle.promise().callback = cback; handle.resume(); });
		}

		/**
		 * @brief Function called when the coroutine is resumed by its handle, handles the retrieval and return of result from promise object
		 * 
		 * @return confirmation_callback_t the result of the api call
		 */
		confirmation_callback_t await_resume() {
			return p->callback;
		}
	};

};

/* template<> */
/* struct std::coroutine_traits<void, const dpp::interaction_create_t&> { */
/* 	using promise_type = dpp::promise; */
/* }; */
#endif
