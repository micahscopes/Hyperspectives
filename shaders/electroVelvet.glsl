#ifdef GL_ES
precision highp float;
#endif

// same name and type as VS
varying vec3 vNormal;
uniform float time;

float noiz(vec2 co){
    return fract(sin(dot(co.xy ,vec2(15.2314,264.1))) * 6543.4321 * sin(time));
}

void main() {

	// calc the dot product and clamp
	// 0 -> 1 rather than -1 -> 1
	vec3 light = vec3(0.5,0.9,1.0);
	
	// ensure it's normalized
	light = normalize(light);
	
	float r = max(0.05, dot(normalize(vNormal), (0.1*noiz(vNormal.xy)+0.9)*vec3(1,0.2,0)));
	float g = max(0.03, dot(normalize(vNormal), (0.2*noiz(vNormal.xy)+1.0)*vec3(0.2,0.7,0.5)));
	float b = max(0.05, dot(normalize(vNormal), (0.5*noiz(vNormal.xy)+0.6)*vec3(1,1,1)));
	
	
	gl_FragColor = vec4(r,g,b,max(0.05, dot(vec3(r,g,b), (0.5*noiz(vNormal.xy)+0.6)*vec3(1,1,1))));
	
}
