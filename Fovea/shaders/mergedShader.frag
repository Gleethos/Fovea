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

in vec4 pc; // pixel coordinate of current output which comes from vertex shader
out vec4 outputFragment;

vec4 fovea_to_cartesian(vec4 in_) {
	float vp = in_.x; // p is x-direction in output
  	float va = in_.y; // alpha is y-direction in output
  
	float vd = pow(base, vp);
	float x = vd * cos(va) + fx; // adding fovea
  	float y = vd * sin(va) + fy;
	
  	return vec4(x, y, in_.z, in_.w);
}

void main()
{
	//outputFragment = fovea_to_cartesian(vec4(gl_FragCoord.xyz, 1));
	outputFragment = pc;
	//outputFragment = vec4(fx, fy, base, r); // only to check if uniform block parameters are working
	//outputFragment = vec4(1.0, 0.0, 1.0, 1.0);
} 


