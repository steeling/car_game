#version 120
attribute vec4 vertexPosition;
attribute vec3 vertNormal;
attribute vec2 vertTexCoords;

uniform vec2 texCoordsTime;
uniform mat4 P;
uniform mat4 MV;
uniform mat3 T;
uniform vec3 lightPosCam;

varying vec2 fragTexCoords0;
varying vec4 positionEye; // Pass to fragment shader
varying vec3 normal;


void main()
{
	normal = vec3(MV * vec4(vertNormal, 0.0));

	positionEye = MV * vertexPosition;

	gl_Position = P * MV * vertexPosition;
	fragTexCoords0 = vertTexCoords;
}
