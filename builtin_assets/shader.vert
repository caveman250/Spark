port(InVertPos) in vec3 vertPos;
port(FinalVertPos) out vec4 outPos;

uniform mat4 MVP;

void main()
{
    outPos = MVP * vec4(vertPos, 1.0);
}