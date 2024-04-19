port(InVertPos) in vec3 vertPos;
port(InVertColour) in vec3 vertColour;
port(FinalVertPos) out vec4 outPos;
port(FinalVertColour) out vec3 outColour;

uniform mat4 MVP;

void main()
{
    outColour = vertColour;
    outPos = MVP * vec4(vertPos, 1.0);
}