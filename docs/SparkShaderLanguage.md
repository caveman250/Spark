# Spark Shader Language

- [Spark Shader Language](#spark-shader-language)
  - [Language Syntax](#language-syntax)
    - [Shader Declaration](#shader-declaration)
    - [Ports](#ports)
      - [Named Ports](#named-ports)
        - [VertexPos (Vertex Shader only)](#vertexpos-vertex-shader-only)
        - [VertexUV (Vertex Shader only)](#vertexuv-vertex-shader-only)
        - [VertexNormal (Vertex Shader only)](#vertexnormal-vertex-shader-only)
        - [FinalVertexPos (Vertex Shader only)](#finalvertexpos-vertex-shader-only)
        - [FragmentUV](#fragmentuv)
        - [FragmentWorldPos (3d only)](#fragmentworldpos-3d-only)
        - [FragmentNormalCameraSpace (3d only)](#fragmentnormalcameraspace-3d-only)
        - [EyeDirCameraSpace (3d only)](#eyedircameraspace-3d-only)
        - [LightDirCameraSpace (3d only)](#lightdircameraspace-3d-only)
      - [Inputs](#inputs)
      - [Outputs](#outputs)
    - [Uniforms](#uniforms)
    - [Settings](#settings)
    - [main](#main)
    - [Expressions](#expressions)
      - [Operators](#operators)
      - [Variable Declaration](#variable-declaration)
      - [Method Calls](#method-calls)
      - [If Statements](#if-statements)
      - [For Loops](#for-loops)
  - [Types](#types)
    - [void](#void)
    - [bool](#bool)
    - [int](#int)
    - [float](#float)
    - [vec2](#vec2)
      - [Properties](#properties)
        - [x](#x)
        - [y](#y)
    - [vec3](#vec3)
      - [Properties](#properties-1)
        - [x](#x-1)
        - [y](#y-1)
        - [z](#z)
    - [vec4](#vec4)
      - [Properties](#properties-2)
        - [x](#x-2)
        - [y](#y-2)
        - [z](#z-1)
        - [w](#w)
    - [mat3](#mat3)
    - [mat4](#mat4)
    - [sampler2D](#sampler2d)
  - [Methods](#methods)
    - [clamp](#clamp)
      - [Parameters](#parameters)
        - [val](#val)
        - [minVal](#minval)
        - [maxVal](#maxval)
      - [Description](#description)
    - [dfdx](#dfdx)
      - [Parameters](#parameters-1)
        - [val](#val-1)
      - [Description](#description-1)
    - [dfdy](#dfdy)
      - [Parameters](#parameters-2)
        - [val](#val-2)
      - [Description](#description-2)
    - [dot](#dot)
      - [Parameters](#parameters-3)
        - [x](#x-3)
        - [y](#y-3)
      - [Description](#description-3)
    - [length](#length)
      - [Parameters](#parameters-4)
        - [val](#val-3)
      - [Description](#description-4)
    - [normalize](#normalize)
      - [Parameters](#parameters-5)
        - [val](#val-4)
      - [Description](#description-5)
    - [pow](#pow)
    - [reflect](#reflect)
    - [smoothstep](#smoothstep)
    - [texture](#texture)


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

```glsl
// port(<port_name>) in <var_type> <local_name>;
//
// eg:

port(VertexPos) in vec3 inPos;
```

#### Outputs

Outputs are defined with the following syntax:

```glsl
// port(<port_name>) out <var_type> <local_name>;
//
// eg:

port(FragmentColour) out vec4 outColour;
```

### Uniforms

Uniforms are variables that stay the same for every vertex / pixel in a given material instance. A uniform is set at runtime, and can change in between draw calls.

They can be declared with the following syntax:

```glsl
// uniform <var_type> <var_name>
//
// eg:

uniform sampler2d texture;
```

Unfiforms can be set from c++ with the following syntax:

```cpp
std::shared_ptr<render::MaterialInstance> materialInstance = ...
std::shared_ptr<asset::Texture> myTexture = ...
materialInstance->SetUniform("texture", asset::shader::ast::AstType::Sampler2D, 1, &myTexture);
```

### Settings

Settings are similar to Unfiforms in that they stay the same for every vertex / pixel in a given material instance. 
However, settings must be set at compile time for a given shader, at the Material level.

As settings are compile time constants, they may be used in array declarations and for loop conditions.

They can be declared with the following syntax:

```glsl
// setting <var_type> <var_name>
//
// eg:

setting int numLights;
```

Settings can be set from c++ with the following:

```cpp
std::shared_ptr<render::Material> material = ...
material->GetShaderSettings().SetSetting("numLights", 3);
```

Settings must be set before a material has created it's platform resources. This generally happens the first time a material is used to submit a draw call.

### main

Main defines  the entry point for the shaders execution. It can be declared ewith the following syntax:

```glsl
void main()
{
    // Shader logic goes here...
}
```

Failing to declare main will result in a compile error.

### Expressions

Expressions in spark shader language are performed by chaining multiple variables, literals, and functions calls together. Separated by operators (listed below).
Each expression must be ended with the `;` character.

Example:

```glsl
vec3 myVec = vec3(1.0, 1.0, 1.0);

outColor = diffuse + ambient * shadow;
```

#### Operators

All C style operators are supported, with the exception of ++ and -- as of writing.

The available operators are:

- \*
- *=
- /
- /=
- \+
- +=
- \-
- -=
- =
- ==
- \<
- \<=
- \>
- \>=

#### Variable Declaration

Variables are defined with the following syntax:

```glsl
// <var_type> <var_name> = <initial_value>
// eg:

float val = 1.0f;
vec3 vecVal = vec3(1.0f, 1.0f, 1.0f);
```

#### Method Calls

Method calls use standard c syntax.

```glsl
// <method_name>(<arg1>, <arg2>, ...);
// eg:

float val = clamp(val, 0.f, 1.f);
```

#### If Statements

If statements allow the user to control the execution of code, by branching on a conditional statement.

```glsl
// if (<condition>)
// {
// 	  ...Do things
// }

eg:

int i = 5;
if (i < 10)
{
	
}
```

#### For Loops

For loops allow to user to run a section of code multiple times, based on the user provided condition.

```glsl
// for (<iterator_declaration>; <for_loop_condition>; <post_condition>)
// {
// 	
// }

eg:

for (int i = 0; i < 5; i += 1)
{
	
}
```

## Types

### void

Represents a "none" type. Only supported as the return type of a function.

### bool

A true or false value.

A bool literal can be represented with the `true` or `false` keywords.

### int

A signed 32 bit integer value.

### float

A 32 bit floating point value.

> [!NOTE]
> Double precision floats not supported at this time.

### vec2

A two dimensional vector, consisting of two floats.

A vec2 literal can be represented with: `vec2(x, y)`

#### Properties

##### x

The x component of the vector.

##### y

The y component of the vector.


### vec3


A three dimensional vector, consisting of three floats.

A vec3 literal can be represented with: `vec3(x, y, z)`

#### Properties

##### x

The x component of the vector.

##### y

The y component of the vector.

##### z

The z component of the vector.

### vec4

A three dimensional vector, consisting of three floats.

A vec3 literal can be represented with: `vec4(x, y, z, w)`

#### Properties

##### x

The x component of the vector.

##### y

The y component of the vector.

##### z

The z component of the vector.

##### w

The w component of the vector.

### mat3

A 3x3 matrix, consisting of three vec3's. matrices in ssl are column major.

A mat3 literal can be represented with: `mat3()`. By defult a mat3 is constructed with an identity matrix.

You may access the individual columns with the [] operator. As each column is a vec3, you may use it's [] operator to access individual values.

```glsl
mat3 mat;
mat[0] = vec3(1.0f, 0.0f, 0.0f);
mat[1][1] = 1.0f;
```

### mat4

A 4x4 matrix, consisting of four vec4's. matrices in ssl are column major.

A mat4 literal can be represented with: `mat4()`. By defult a mat4 is constructed with an identity matrix.

You may access the individual columns with the [] operator. As each column is a vec4, you may use it's [] operator to access individual values.

```glsl
mat4 mat;
mat[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);
mat[1][1] = 1.0f;
```

### sampler2D

Represents a sampler for a 2d texture. Only supported as a uniform value.

```glsl
port(FragmentUV) in vec2 inUV;
uniform sampler2d mySampler;

void main()
{
  vec4 colour = texture(mySampler, inUV);
}
```

See [texture](#texture) for more details.

## Methods

Spark provides several built in methods outlined below.

### clamp

```glsl
float clamp(float val, float minVal, float maxVal);
int clamp(int val, int minVal, int maxVal);
```

#### Parameters

##### val

The value to clamp.

##### minVal

The lower end of the range into which to clamp val.

##### maxVal

The upper end of the range into which to clamp val. 

#### Description

clamp returns the value of val clamped to the range of minVal to maxVal. eg: 
```glsl
clamp(3, 1, 2) == 2
```

### dfdx

```glsl
float dfdx(float val);
```

#### Parameters

##### val

The value from which to take the partial deriviative.

#### Description
dfdx returns the partial derivative of val, with respect to the window x coordinate.

usage: 
```glsl
float some_expression = ...
float deriviative = dfdx(some_expression);
```

### dfdy

```glsl
float dfdy(float val);
```

#### Parameters

##### val

The value from which to take the partial deriviative.

#### Description
dfdx returns the partial derivative of val, with respect to the window y coordinate.

usage: 
```glsl
float some_expression = ...
float deriviative = dfdy(some_expression);
```

### dot

```glsl
float dot(vec2 x, vec2 y);
float dot(vec3 x, vec3 y);
float dot(vec4 x, vec4 y);
```

#### Parameters

##### x

the lhs vector used to calculate the dot product.

##### y

the rhs vector used to calculate the dot product.

#### Description
dot returns the dot product of two vectors, x and y.

> [!WARNING]
> The two vectors must be of the same type. mixing vector types will result in a compile error.

usage: 
```glsl
vec2 a = ...
vec2 b = ...
float dot = dot(a, b);
```

### length

```glsl
float length(vec2 val);
float length(vec3 val);
float length(vec4 val);
```

#### Parameters

##### val

the vector to calculate the length of.

#### Description
length returns the length (or magnitude) of a vector.

usage: 
```glsl
vec2 val = ...
float len = length(val);
```

### normalize

```glsl
float normalize(vec2 val);
float normalize(vec3 val);
float normalize(vec4 val);
```

#### Parameters

##### val

the vector to calculate the normalized value of.

#### Description

normalize returns a vector with the same direction as val, but with a unit length of 1.

usage: 
```glsl
vec2 val = ...
vec2 norm = normalize(val);
```

### pow

TODO

### reflect

TODO

### smoothstep

TODO

### texture

TODO
