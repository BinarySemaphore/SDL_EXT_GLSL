varying vec2 v_texCoord;
uniform sampler2D tex0;

void main()
{
    vec4 texture = texture2D(tex0, v_texCoord);
    gl_FragColor = texture;
}