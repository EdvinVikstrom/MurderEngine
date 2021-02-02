def Settings(**kwargs):
	return {
		'flags': [
			'-g', '-Wall', '-std=c++20',
			'-I./extern/libme/include', '-I./extern/vulkan/include', '-I./extern/glfw/include', '-I./extern/portaudio/include', '-I./extern/rapidxml/include',
			'--include=lme/type.hpp',
			'-DME_USE_VULKAN'
		]
	}
