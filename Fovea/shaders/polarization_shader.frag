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
// SIMPLY OUTPUTS POLARIZED & RASTERIZED VERTEX FRAGMENTS... IS BEING STORED IN TEXTURE... //
//=========================================================================================//

//INPUTS :

in vec4 pc; // pixel coordinate of current output which comes from vertex shader


// OUTPUTS :

layout(location = 0) out vec4 outputFragment;


// LOGIC :


void main()
{
	// pixel coordinates used here as rgba color for fragment
	outputFragment = pc; // This goes straight into the polar texture!
}