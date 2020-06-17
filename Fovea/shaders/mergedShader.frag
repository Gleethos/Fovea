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
uniform sampler2D polBuffer; //---> FROM POLAR BUFFER : has been set by this shader in "to_polar_buffer == 1" mode!

// | Mode: ( log polar to -> frame buffer ...  OR ... polpixel to -> color buffer!! )
	// \-------------------------------

uniform int to_polar_buffer; 
//~-=>  0 : performs carteesian transformation on "polpixel" !
//~-=>  1 : simply return the pc from the vertex shader which will be stored in the polarBuffer... (becomes polpixel)


// OUTPUTS :

layout(location = 0) out vec4 outputFragment;

// LOGIC :

vec4 colorized_polar(vec4 in_)
{
	return in_;
}

vec4 fovea_to_cartesian(vec2 in_) {
	float vp = in_.x; // p is x-direction in output
  	float va = in_.y; // alpha is y-direction in output
  
	float vd = pow(base, vp);
	float x = vd * cos(va) + fx; // adding fovea
  	float y = vd * sin(va) + fy;
	
  	return vec2(x, y);
}

void main()
{
	//outputFragment = fovea_to_cartesian(vec4(gl_FragCoord.xyz, 1));
	//outputFragment = pc;
	//outputFragment = vec4(fx, fy, base, r); // only to check if uniform block parameters are working
	//outputFragment = vec4(1.0, 0.0, 1.0, 1.0);
	
	if(to_polar_buffer == 1)
	{
		//outputFragment = colorized_polar(pc);
		outputFragment = vec4(1.0, 0.0, 0.0, 1.0);
	}
	else
	{
		//outputFragment = texture2D(polBuffer, fovea_to_caresian(gl_TexCoord[0]));
		//outputFragment = texture2D(polBuffer, gl_TexCoord[0]);
		
		outputFragment = vec4(0.0, 0.0, 1.0, 1.0);
	}
}