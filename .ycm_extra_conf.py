def Settings(**kwargs):
	return {
		'flags': [
			'-std=c++17',
			'-I./extern/libme/include', '-I./extern/glfw/include',
			'--include=lme/type.hpp',
			'-DWAYLAND'
		]
	}
