#version 330

// CONTEXT VARIABLES: 
//~~~~~~~~~~~~~~~~~~~
//(uniform! -> https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/uniform.php )

layout (std140) uniform Params 
{
	// | Fovea (center):
	// \----------------

	float fx, fy;

	// | Base:
	// \-------

	float base; // = 2;

	// | Radius: (... of view cone)
	// \-------------------------------
	float r;// = ...;
	//float rp;// = _log(base, r); // log scaled r...
}; //________________________________________________________

//INPUTS :

in vec4 pc; // pixel coordinate of current output which comes from vertex shader
uniform sampler2D polBufferTexture; //---> FROM POLAR BUFFER : has been set by this shader in "render_to_polar_buffer == 1" mode!

uniform int is_rendering_to_polar_texture;

// OUTPUTS :

layout(location = 0) out vec4 outputFragment;

// LOGIC :

// wrap input vector in colorized_polar function for future modifications of vector
vec4 colorized_polar(vec4 in_)
{
	return in_;
}

vec2 fovea_to_cartesian(vec2 in_) {
	float vp = in_.x; // p is x-direction in output
  	float va = in_.y; // alpha is y-direction in output
  
	float vd = pow(base, vp);
	float x = vd * cos(va) + fx; // adding fovea
  	float y = vd * sin(va) + fy;
	
  	return vec2(x, y);
}

void main()
{
	//outputFragment = vec4(1.0, 0.0, 0.0, 1.0);
	//outputFragment = texture2D(polBufferTexture, vec2(1.0, 1.0));
	//outputFragment = pc;
	
	//outputFragment = texture2D(polBufferTexture, fovea_to_cartesian(gl_TexCoord[0]));
	if (is_rendering_to_polar_texture == 1) 
	{
		outputFragment = pc;
	}
	else
	{
		outputFragment = texture2D(polBufferTexture, fovea_to_cartesian(vec2(1.0, 1.0)));
	}
	
	//outputFragment = texture2D(polBufferTexture, gl_TexCoord[0]);
	//outputFragment = texture2D(polBufferTexture, vec2(1.0, 1.0));
	
	//outputFragment = vec4(0.0, 0.0, 1.0, 1.0);

}