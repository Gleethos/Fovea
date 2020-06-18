#version 330

#define _DEBUG_VERSION

#if defined(_DEBUG_VERSION)
in  vec4 color;
#else
//uniform vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
#endif

out vec4 outputF;

void main()
{
	outputF = color;
} 


