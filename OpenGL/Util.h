#pragma once

#include"pch.h"

namespace util {
	int loop = 0;

	/*
	* slows down the current thread.
	* @param[in] milliseconds: time for sleeping the current calling thread.
	* @param[in] logIteration: true or false, it prints the loop iteration.
	*/
	void handleloopCount(size_t milliseconds, bool logIteration) {
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
		if (logIteration) {
			loop++;
			std::cout << loop << std::endl;
		}
	}
}

