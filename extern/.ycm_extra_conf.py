def Settings(**kwargs):
	return {
		'flags': [
			'-std=c++20',
			'-I./include/lme',
			'--include=type.hpp',
		]
	}
