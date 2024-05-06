port(FragVertColour) in vec3 inColour;
port(FragVertUV) in vec2 inUV;
port(test) out vec3 color;

uniform sampler2D Texture;

void main()
{
    color = texture(Texture, inUV).rgb;
}