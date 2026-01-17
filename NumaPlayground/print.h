#pragma once
#ifdef __cpp_lib_print
#include <print>
#else
#include <iostream>
#include <format>
#endif

namespace compat {
	template<class... Args>
	void Print(std::format_string<Args...> fmt, Args&&... args)
	{
#ifdef __cpp_lib_print
		std::print(fmt, std::forward<Args>(args)...);
#else
		std::cout << std::format(fmt, std::forward<Args>(args)...);
#endif
	}
}