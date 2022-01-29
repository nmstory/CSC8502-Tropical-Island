# C++ & OpenGL Graphics Coursework

For this coursework, I produced a tropical island scene by implementing a variety of algorithms, alongside speed and memory considerations to keep the game running at a stable 60FPS.

## Graphics Functionality
- Landscape
	- Using a heightmap
	- Multiple textures applied (dirt and rock)
- Lighting
	- Scene is lit with the use of a sun, achieved with calculations for ambient, diffuse and specular components.
	- Bump mapping
- Environment Mapping
	- Skybox for a realistic sky
- Scene Graph
	- Effective storage of game objects using a scene graph
	- Transform matrix hierarchy to apply relevant movement to children
- Meshes
	- Handling meshes within the scene graph consisting of various textures.
- Water
	- Water achieved with the use of geometry and tesselation shaders
- Skeletal Animations
	- Skeletal animation with interpolation of animation frames
- Post Processing Effect
	- Blur with the use of a frame buffer

## YouTube Video
Most of the functionality is demonstrated in the following [_demo video_](https://www.youtube.com/watch?v=HcqvJLa-9MI)

## OpenGLRendering Library
Many calls to OpenGL are handled by an intermediary library called the OpenGLRendering Library, developed by [_Newcastle University_](https://www.ncl.ac.uk, for support with low-level interactions.
