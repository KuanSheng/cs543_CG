#version 150

in  vec2 texCoord;
in vec2 vST;
out vec4 fColor;

uniform sampler2D texture;
uniform int textureMode;

void main() 
{ 
    switch(textureMode){
	case 0:
	fColor = texture2D(texture, texCoord);
	break;
	case 1:
    const vec3 w = vec3(0.2125,0.7154,0.0721);
	vec3 irgb = texture(texture,vST).rgb;
	float luminance = dot(irgb,w);
    fColor = vec4(luminance,luminance,luminance,1);
	break;
    case 2:
	fColor = 1-texture2D( texture, texCoord );
	break;
	case 3:

	}

	}