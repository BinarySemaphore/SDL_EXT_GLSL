varying vec4 v_color;
varying vec2 v_texCoord;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    v_color = gl_Color;
	v_texCoord = gl_MultiTexCoord0.st;
}