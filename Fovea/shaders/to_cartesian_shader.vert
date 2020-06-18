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

layout (std140) uniform Matrices {
	mat4 pvm;
};

// INPUT:

in vec4 position; 

// OUTPUT:

out vec4 pc; // out pc to send point coordinate data to fragment shader7

// LOGIC:

// UTILITY :
float _log(float base, float x) {
    return log(x) / log(base); // Basiswechselsatz
}

// transform cartesian coordinates to polar coordinates
vec4 fovea_to_log_polar(vec4 v) {
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
	
	//pc = vec4(v_pol, v.w);
	
	return vec4( v_pol, v.w );
}

// transform polar coordinates to cartesian coordinates
vec4 fovea_to_cartesian(vec4 in_) {
	float vp = in_.x; // p is x-direction in output
  	float va = in_.y; // alpha is y-direction in output
  
	float vd = pow(base, vp);
	float x = vd * cos(va) + fx; // adding fovea
  	float y = vd * sin(va) + fy;
	
  	return vec4(x, y, in_.z, in_.w);
}

void main() {
	pc = position;
	
	//gl_Position = pvm * position;
	//gl_Position = pvm * fovea_to_log_polar(position);
	
	gl_Position = pvm * fovea_to_cartesian(fovea_to_log_polar(position)); // transform to log polar and back to test if the two transformations are inverse
} 
