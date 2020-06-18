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
// TAKES POLAR TEXTURE AND HANDS IT TO "to_cartesian_shader.frag"! -=> TO CARTESIAN THERE! //
//=========================================================================================//

layout (std140) uniform Matrices 
{
	mat4 pvm;
};

// INPUT:

in vec4 position; 
in vec2 texCoord;

// OUTPUT:

out vec2 texCoordAsVertex;

// LOGIC:
 
void main() 
{
	texCoordAsVertex = texCoord;
	gl_Position = pvm * position;
} 
