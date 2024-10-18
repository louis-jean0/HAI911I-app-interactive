#version 140

// --------------------------------------------------
// shader definition
// --------------------------------------------------

uniform sampler3D mask; // déclaration de la map mask

uniform float xCutPosition;
uniform float yCutPosition;
uniform float zCutPosition;

uniform int xCutDirection; 
uniform int yCutDirection;
uniform int zCutDirection;

uniform float xMax;
uniform float yMax;
uniform float zMax;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

varying vec3 position;
varying vec3 textCoord;

bool ComputeVisibility(vec3 point){
	vec3 xDir = vec3(1.*xCutDirection,0.,0.);
	vec3 yDir = vec3(0.,1.*yCutDirection,0.);
	vec3 zDir = vec3(0.,0.,1.*zCutDirection);

	vec3 xCut = vec3(xCutPosition, 0.,0.);
	vec3 yCut = vec3(0., yCutPosition,0.);
	vec3 zCut = vec3(0., 0.,zCutPosition);

	//TODO compute visibility

	vec3 xVec = point - xCut;
	vec3 yVec = point - yCut;
	vec3 zVec = point - zCut;

	float xVisibility = dot(xVec,xDir);
	float yVisibility = dot(yVec,yDir);
	float zVisibility = dot(zVec,zDir);

	if(xVisibility < 0.0 || yVisibility < 0.0 || zVisibility < 0.0) {
		return false;
	}

	return true;
}

vec3 rayTrace(vec3 inpos){
	vec3 camPos = (inverse(mv_matrix) * vec4(0, 0, 0, 1)).xyz;
	vec3 dir = normalize(inpos - camPos);

	//TODO raytrace	


	return vec3(0);
}

// --------------------------------------------------
// Fragment Shader:
// --------------------------------------------------
void main() {

	if(!ComputeVisibility(position)){
		discard;
	}

	gl_FragColor = texture(mask, textCoord);

}
