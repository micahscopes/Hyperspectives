#ifdef GL_ES
precision highp float;
#endif

// same name and type as VS
varying vec3 vNormal;

void main() {

	// calc the dot product and clamp
	// 0 -> 1 rather than -1 -> 1
	vec3 light = vec3(0.5,0.9,1.0);
	
	// ensure it's normalized
	light = normalize(light);
	
	float r = max(0.05, dot(normalize(vNormal), normalize(vec3(0.3,0.3,0))));
	float g = max(0.05, dot(normalize(vNormal), normalize(vec3(0,0.2,0.4))));
	float b = max(0.05, dot(normalize(vNormal), normalize(vec3(0,0,1.4))));
	
	// feed into our frag colour
	gl_FragColor = vec4(r,g,b,1.0);
	
}
