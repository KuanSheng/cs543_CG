#version 150

in  vec2 texCoord;
in vec2 vST;
out vec4 fColor;

const float PI = 3.14159265;
uniform sampler2D texture;
uniform int textureMode;

void main() 
{ 

    ivec2 ires = textureSize(texture, 0);
	float ResS = float(ires.s);
	float ResT = float(ires.t);
    const vec3 LUMCOEFFS = vec3(0.2125,0.7154,0.0721);
	vec2 stp0 = vec2(1.0/ResS,0);
	vec2 st0p = vec2(0,1.0/ResT);
	vec2 stpp = vec2(1.0/ResS,1.0/ResT);
	vec2 stpm = vec2(1.0/ResS, -1.0/ResT);
	vec3 irgb = texture(texture,vST).rgb;

	float uD = 45.0;
	float uR = 0.5;
	float i00 = dot(texture2D(texture,vST).rgb,LUMCOEFFS );
	float im1m1 = dot(texture2D(texture,vST-stpp).rgb,LUMCOEFFS );
	float ip1p1 = dot(texture2D(texture,vST+stpp).rgb,LUMCOEFFS );
	float im1p1 = dot(texture2D(texture,vST-stpm).rgb,LUMCOEFFS );
	float ip1m1 = dot(texture2D(texture,vST+stpm).rgb,LUMCOEFFS );
	float im10 = dot(texture2D(texture,vST-stp0).rgb,LUMCOEFFS );
	float ip10 = dot(texture2D(texture,vST+stp0).rgb,LUMCOEFFS );
	float i0m1 = dot(texture2D(texture,vST-st0p).rgb,LUMCOEFFS );
	float i0p1 = dot(texture2D(texture,vST+st0p).rgb,LUMCOEFFS );

	float h = -1.0*im1p1 - 2.0*i0p1 - 1.0*ip1p1 + 1.0*im1m1 + 2.0*i0m1 + 1.0*ip1m1;
	float v = -1.0*im1m1 - 2.0*im10 - 1.0*im1p1 + 1.0*ip1m1 + 2.0*ip10 + 1.0*ip1p1;

    switch(textureMode){
	case 0:
	fColor = texture2D(texture, texCoord);
	break;
	case 1:
    const vec3 w = vec3(0.2125,0.7154,0.0721);
	
	float luminance = dot(irgb,w);
    fColor = vec4(luminance,luminance,luminance,1);
	break;
    case 2:
	fColor = 1-texture2D( texture, texCoord );
	break;
	case 3:
	float mag = sqrt(h*h + v*v);
	vec3 target = vec3(mag,mag,mag);
	fColor = vec4(mix(irgb,target,1.0),1.0);
	break;
	case 4:
	vec3 c00 = texture2D(texture,vST).rgb;
	vec3 cp1p1 = texture2D(texture,vST+stpp).rgb;
	vec3 diffs = c00 - cp1p1;
	float max = diffs.r;

	if(abs(diffs.g)>abs(max))
	  max = diffs.g;
	if(abs(diffs.b)>abs(max))
	  max = diffs.b;
   
    float gray = clamp(max+0.5,0,1.0);
	vec4 grayversion = vec4(gray,gray,gray,1.0);
	vec4 colorversion = vec4(gray*c00,1.0);
	fColor = mix(grayversion,colorversion,0.1);
    break;

	case 5:
	
    float Res = float( ires.s ); 
    vec2 st = vST;
    float Radius = Res * uR;
    vec2 xy = Res * st; 
    vec2 dxy = xy-Res/2.; 
    float r = length( dxy );
    float beta = atan( dxy.y, dxy.x) + radians(uD) * (Radius-r)/Radius;
	vec2 xy1 = xy;
    if(r <= Radius)
    {
       xy1 = Res/2. + r * vec2( 
       cos(beta), sin(beta) );
    }
    st = xy1/Res; 
    irgb = texture( texture, st ).rgb;
    fColor = vec4( irgb, 1. );
	break;

	case 6:
    Res = float( ires.s );
	 st = vST;
	 xy = Res * st; 
    float beta1 = 2*PI*xy.y/120.0;
	float beta2 = 2*PI*xy.x/250.0;
	 xy1 = xy + vec2(10*sin(beta1),15*sin(beta2));

	st = xy1/Res; 
	irgb = texture( texture, st ).rgb;
    fColor = vec4( irgb, 1. );
	break;

	case 7:
	
	break;
	}

	}