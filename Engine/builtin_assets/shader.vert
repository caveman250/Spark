port(InVertPos) in vec3 inPos;
port(InVertColour) in vec3 inColour;
port(InVertUV) in vec2 inUV;

port(FinalVertPos) out vec4 outPos;
port(FinalVertColour) out vec3 outColour;
port(FinalVertUV) out vec2 outUV;

uniform mat4 MVP;

void main()
{
    outColour = inColour;
    outPos = MVP * vec4(inPos, 1.0);
    outUV = inUV;
}