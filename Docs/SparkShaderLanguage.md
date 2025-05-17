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
TODO
#### Outputs
TODO
### Uniforms
TODO
### Settings
TODO
### main
TODO
### Expressions
TODO
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
