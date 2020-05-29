#version 330

#define _DEBUG_VERSION

layout (std140) uniform Matrices {
	mat4 pvm;
} ;

in vec4 position;

#if defined(_DEBUG_VERSION)
out vec4 color;
#endif

void main()
{
#if defined(_DEBUG_VERSION)
	 color = position;
#endif

	gl_Position = pvm * position ;
} 
