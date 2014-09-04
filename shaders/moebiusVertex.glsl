#ifdef GL_ES
precision highp float;
#endif

uniform float leftIsoclinic[ 4 ];
uniform float rightIsoclinic[ 4 ];
uniform float radius;
uniform float offset[ 3 ];
varying vec3 vNormal;

//for spherical env mapping, from http://www.clicktorelease.com/blog/creating-spherical-environment-mapping-shader
varying vec3 e;
varying vec3 n;

#ifdef USE_MORPHTARGETS
#ifndef USE_MORPHNORMALS
uniform float morphTargetInfluences[ 8 ];
#else
uniform float morphTargetInfluences[ 4 ];
#endif
#endif

void main() {
	vec3 off = vec3(offset[0],offset[1],offset[2]);
	vec3 pos = vec3(viewMatrix * vec4(position,1.0));
	#ifdef USE_MORPHTARGETS
	vec3 morphed = vec3( 0.0 );
	morphed += ( morphTarget0 - pos ) * morphTargetInfluences[ 0 ];
	morphed += ( morphTarget1 - pos ) * morphTargetInfluences[ 1 ];
	morphed += ( morphTarget2 - pos ) * morphTargetInfluences[ 2 ];
	morphed += ( morphTarget3 - pos ) * morphTargetInfluences[ 3 ];
	#ifndef USE_MORPHNORMALS
	morphed += ( morphTarget4 - pos ) * morphTargetInfluences[ 4 ];
	morphed += ( morphTarget5 - pos ) * morphTargetInfluences[ 5 ];
	morphed += ( morphTarget6 - pos ) * morphTargetInfluences[ 6 ];
	morphed += ( morphTarget7 - pos ) * morphTargetInfluences[ 7 ];
	#endif
	pos += morphed;
	gl_Position = projectionMatrix * modelViewMatrix * vec4( morphed, 1.0 );
	#endif
	
	#ifndef USE_MORPHTARGETS
	#ifndef USE_SKINNING
	pos = position;
	#endif
	#endif
	#ifdef USE_MORPHNORMALS
	vec3 morphedNormal = vec3( 0.0 );
	morphedNormal +=  ( morphNormal0 - normal ) * morphTargetInfluences[ 0 ];
	morphedNormal +=  ( morphNormal1 - normal ) * morphTargetInfluences[ 1 ];
	morphedNormal +=  ( morphNormal2 - normal ) * morphTargetInfluences[ 2 ];
	morphedNormal +=  ( morphNormal3 - normal ) * morphTargetInfluences[ 3 ];
	morphedNormal += normal;
	vec3 transformedNormal = normalMatrix * morphedNormal;
	#else
	vec3 transformedNormal = normalMatrix * normal;
	#endif

	float a = leftIsoclinic[0];
	float b = leftIsoclinic[1];
	float c = leftIsoclinic[2];
	float d = leftIsoclinic[3];
	float aa = rightIsoclinic[0];
	float bb = rightIsoclinic[1];
	float cc = rightIsoclinic[2];
	float dd = rightIsoclinic[3];
	
	mat4 hRot = mat4(a,-b,-c,-d,b,a,-d,c,c,d,a,-b,d,-c,b,a)*mat4(aa,-bb,-cc,-dd,bb,aa,dd,-cc,cc,-dd,aa,bb,dd,cc,-bb,aa);
	vec3 p = (pos+off)/radius;
	vec3 pn = p+(normal)/radius;
	float dist = p[0]*p[0]+p[1]*p[1]+p[2]*p[2];
	float distn = pn[0]*pn[0]+pn[1]*pn[1]+pn[2]*pn[2];
	vec4 h = vec4(2.0*p,(dist-1.0));
	vec4 hn = vec4(2.0*pn,(distn-1.0));
	h = h/(1.0+dist);
	hn = hn/(1.0+distn);
	h = hRot*h;
	hn = hRot*hn;
	
	vec3 newPosition = radius*(vec3(h[0],h[1],h[2])/(1.0-h[3]));
	vNormal = radius*(vec3(hn[0],hn[1],hn[2])/(1.0-hn[3]))-newPosition;
    
    vec4 mvmPos =  modelViewMatrix * vec4(newPosition-off,1.0);
	gl_Position = projectionMatrix * mvmPos;
	
	// env mapping			  
    e = normalize( mvmPos.xyz);
    n = normalize( normalMatrix*vNormal );
    
    }
