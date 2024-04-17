port(VertexPosition) in vec3 vertPos;

void main()
{
  gl_Position.xyz = vertPos;
  gl_Position.w = 1.0;
}