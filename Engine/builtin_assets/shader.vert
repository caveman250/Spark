port(InVertPos) in vec3 inPos;
port(InVertUV) in vec2 inUV;

port(FinalVertPos) out vec4 outPos;
port(FinalVertUV) out vec2 outUV;

uniform mat4 MVP;

void main()
{
    outPos = MVP * vec4(inPos, 1.0);
    outUV = inUV;
}