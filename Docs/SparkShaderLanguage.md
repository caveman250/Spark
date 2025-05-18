# Spark Shader Language
## Language Syntax
### Shader Declaration
Each Spark shader begins with a declaration specifying its intended usage. the available options are:
- `SSL_VERTEX`: Vertex Shader
- `SSL_FRAGMENT`: Fragment Shader

Failing to declare a shader type will result in a compile error.
### Ports
ssl shaders are compiled in such a way, that multiple shaders may be combined to 
generate the final shader used by the platform. Ports define either an input or an output, 
and can be accessed from one or multiple shader "stages" that comprise a material.  
Several preixisting ports exist, mapping to functionality such as Vertex attributes 
and output colour. Users are also free to add in their own custom ports, should they need 
to share data accross multiple shader stages.
#### Named Ports
##### VertexPos (Vertex Shader only)
Vertex position attribute, if supplied by the mesh being rendered.
##### VertexUV (Vertex Shader only)
Vertex uv attribute, if supplied by the mesh being rendered.
##### VertexNormal (Vertex Shader only)
Vertex normal attribute, if supplied by the mesh being rendered.
##### FinalVertexPos (Vertex Shader only)
The output position of a vertex. 
##### FragmentUV
the uv co-ordinates the fragment shader should use. interpolated between vertices.
##### FragmentWorldPos (3d only)
The world space position of the fragment being rendered.
##### FragmentNormalCameraSpace (3d only)
The normal of the fragment being rendered in camera space.
##### EyeDirCameraSpace (3d only)
The direction of the camera from the fragment being rendered in camera space.
##### LightDirCameraSpace (3d only)
The direction of the scenes lights in camera space. This variable is an array, and it's size must be defined by a setting named numLights. 
#### Inputs
Inputs are defined with the following syntax:

```
port(<port_name>) in <var_type> <local_name>;

eg:

port(VertexPos) in vec3 inPos;
```
#### Outputs
Outputs are defined with the following syntax:

```
port(<port_name>) out <var_type> <local_name>;

eg:

port(FragmentColour) out vec4 outColour;
```
### Uniforms
Uniforms are variables that stay the same for every vertex / pixel in a given material instance. A uniform is set at runtime, and can change in between draw calls.

They can be declared with the following syntax:

```
uniform <var_type> <var_name>

eg:

uniform sampler2d texture;
```

Unfiforms can be set from c++ with the following syntax:
```
std::shared_ptr<render::MaterialInstance> materialInstance = ...
std::shared_ptr<asset::Texture> myTexture = ...
materialInstance->SetUniform("texture", asset::shader::ast::AstType::Sampler2D, 1, &myTexture);
```
### Settings
Settings are similar to Unfiforms in that they stay the same for every vertex / pixel in a given material instance. 
However, settings must be set at compile time for a given shader, at the Material level.

As settings are compile time constants, they may be used in array declarations and for loop conditions.

They can be declared with the following syntax:

```
setting <var_type> <var_name>

eg:

setting int numLights;
```

Settings can be set from c++ with the following:
```
std::shared_ptr<render::Material> material = ...
material->GetShaderSettings().SetSetting("numLights", 3);
```

Settings must be set before a material has created it's platform resources. This generally happens the first time a material is used to submit a draw call.
### main

Main defines  the entry point for the shaders execution. It can be declared ewith the following syntax:

```
void main()
{
    Shader logic goes here...
}
```

Failing to declare main will result in a compile error.
### Expressions
Expressions in spark shader language are performed by chaining multiple variables, literals, and functions calls together. Separated by operators (listed below).
Each expression must be ended with the `;` character.

Example:
```
vec3 myVec = vec3(1.0, 1.0, 1.0);

outColor = diffuse + ambient * shadow;
```
#### Operators
##### *
##### *=
##### /
##### /=
##### +
##### +=
##### -
##### -=
##### =
##### ==
##### \<
##### \<=
##### \>
##### \>=
#### Variable Declaration
TODO
#### Method Calls
TODO
## Types
### void
TODO
### bool
TODO
### int
TODO
### float
TODO
### vec2
TODO
### vec3
TODO
### vec4
TODO
### mat3
TODO
### mat4
TODO
### sampler2D
TODO
## Methods
TODO
### clamp
TODO
### dfdx
TODO
### dfdy
TODO
### dot
TODO
### length
TODO
### normalize
TODO
### pow
TODO
### reflect
TODO
### smoothstep
TODO
### texture
TODO
