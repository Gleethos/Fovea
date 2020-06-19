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
}; 
//=========================================================================================//
// TAKES MODEL VERTECIES AND TRANSFORMS THEM TO LOG POLAR! -=> NO MORE PROCESSING IN FRAG..//
//=========================================================================================//

layout (std140) uniform Matrices {
	mat4 pvm;
};

// INPUT:

in vec4 position; 

// OUTPUT:

out vec4 pc; // out pc to send point coordinate data to fragment shader

// LOGIC:

// UTILITY :
float _log(float base, float x) 
{
    return log(x) / log(base); // Basiswechselsatz
}

// transform cartesian coordinates to polar coordinates
vec4 fovea_to_log_polar(vec4 v) 
{
	vec2 fv_vec = vec2( v.x-fx, v.y-fy );

	float vp = _log( // logarithmic distance!
		base,
		pow(pow(fv_vec.x, 2) + pow(fv_vec.y, 2), 0.5) // distance
	);
			
	float va = atan( fv_vec.y, fv_vec.x  ); // polar angle
	vec3 v_pol; // vertices  radial

	v_pol.x = vp;
	v_pol.y = va;
	v_pol.z = v.z;

	return vec4( v_pol, v.w );
}

// TESTING ONLY :  -> Should not be used here in final implementation!
//===========================================================
vec4 fovea_to_cartesian(vec4 in_) // transform polar coordinates to cartesian coordinates
{
	float vp = in_.x; // p is x-direction in output
  	float va = in_.y; // alpha is y-direction in output
  
	float vd = pow(base, vp);
	float x = vd * cos(va) + fx; // adding fovea
  	float y = vd * sin(va) + fy;
	
  	return vec4(x, y, in_.z, in_.w);
}
//===========================================================
// ^ TESTING ONLY ^

void main() 
{
	pc = position;
	
	gl_Position = fovea_to_log_polar(pvm * position); // <--- goal!
	
	//gl_Position = fovea_to_cartesian(fovea_to_log_polar(pvm * position)); // transform to log polar and back to test if the two transformations are inverse
	//gl_Position = pvm * position; // <- ^ These produce same result: Proves that one is inverse to th other...
} 







