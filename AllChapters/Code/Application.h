#ifndef _H_APPLICATION_
#define _H_APPLICATION_

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"

class Application {
private:
	Application(const Application&);
	Application& operator=(const Application&);
public:
	inline Application() { }
	inline virtual ~Application() { }

	inline virtual void Initialize() {}
	inline virtual void Update(float inDeltaTime) { }
	inline virtual void Render(float inAspectRatio) { }
	inline virtual void ImGui(nk_context* inContext) { }
	inline virtual void Shutdown() { }
};

#endif