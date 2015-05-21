#version 120

varying vec2 fragTexCoords0;
varying vec4 positionEye; // passed from the vertex shader
varying vec3 normal;

uniform float s;
uniform sampler2D texture0;
uniform sampler2D textureNormal;
uniform int useNormal;

uniform vec3 lightPosCam;
uniform vec3 ks;
uniform vec3 ka;

void main()
{
	vec3 kd = texture2D(texture0, fragTexCoords0).rgb;

	vec3 n = normalize(normal);
	vec3 e = -1.0 * normalize(vec3(positionEye));
	vec3 l = normalize(lightPosCam + vec3(positionEye));
	vec3 h = normalize(e + l);

	vec3 cd = ka * max(dot(l, n), 0.4);
	vec3 cs = ks * pow(max(dot(h, n), 0.0), s);

	vec3 color =  cd + cs + kd;
	gl_FragColor = vec4( color , 1.0);
}
