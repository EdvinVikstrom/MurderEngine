def Settings(**kwargs):
	return {
		'flags': [
			'-g', '-std=c++20',
			'-I./extern/libme/include', '-I./extern/glfw/include', '-I./extern/portaudio/include', '-I./extern/rapidxml',
			'--include=lme/type.hpp',
			'-DME_USE_VULKAN'
		]
	}
