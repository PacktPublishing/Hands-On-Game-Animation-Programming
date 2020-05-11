#pragma warning(disable : 28251)
#pragma warning(disable : 26451)
#pragma warning(disable : 28182)
#pragma warning(disable : 6386)
#pragma warning(disable : 26812)
#pragma warning(disable : 6011)

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#undef APIENTRY
#include "glad.h"
#undef APIENTRY
#include <windows.h>

#define NK_IMPLEMENTATION

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"

#define NK_MAX_VERTEX_MEMORY 512 * 1024
#define NK_MAX_ELEMENT_MEMORY 128 * 1024

NK_API struct nk_context* nk_win32_init();
NK_API int nk_win32_handle_event(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, float scaleFactor);
NK_API void nk_win32_render(int width, int height, int display_width, int display_height);
NK_API void nk_win32_shutdown(void);
NK_API void nk_demo();

/*
 * ==============================================================
 *
 *                          IMPLEMENTATION
 *
 * ===============================================================
 */

NK_API void nk_win32_device_destroy(void);
NK_API void nk_win32_device_create(void);
NK_API void nk_win32_font_stash_begin(struct nk_font_atlas **atlas);
NK_API void nk_win32_font_stash_end(void);

#include <string.h>

struct nk_device {
	struct nk_buffer cmds;
	struct nk_draw_null_texture null;
	GLuint vbo, vao, ebo;
	GLuint prog;
	GLuint vert_shdr;
	GLuint frag_shdr;
	GLint attrib_pos;
	GLint attrib_uv;
	GLint attrib_col;
	GLint uniform_tex;
	GLint uniform_proj;
	GLuint font_tex;
};

struct nk_vertex {
	float position[2];
	float uv[2];
	nk_byte col[4];
};

static struct nk_ctx {
	struct nk_device ogl;
	struct nk_context ctx;
	struct nk_font_atlas atlas;
} ctx;

#ifdef __APPLE__
#define NK_SHADER_VERSION "#version 150\n"
#else
#define NK_SHADER_VERSION "#version 300 es\n"
#endif

NK_API void nk_win32_device_create(void) {
	GLint status;
	static const GLchar *vertex_shader =
		NK_SHADER_VERSION
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 TexCoord;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main() {\n"
		"   Frag_UV = TexCoord;\n"
		"   Frag_Color = Color;\n"
		"   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
		"}\n";
	static const GLchar *fragment_shader =
		NK_SHADER_VERSION
		"precision mediump float;\n"
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main(){\n"
		"   Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
		"}\n";

	struct nk_device *dev = &ctx.ogl;
	nk_buffer_init_default(&dev->cmds);
	dev->prog = glCreateProgram();
	dev->vert_shdr = glCreateShader(GL_VERTEX_SHADER);
	dev->frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(dev->vert_shdr, 1, &vertex_shader, 0);
	glShaderSource(dev->frag_shdr, 1, &fragment_shader, 0);
	glCompileShader(dev->vert_shdr);
	glCompileShader(dev->frag_shdr);
	glGetShaderiv(dev->vert_shdr, GL_COMPILE_STATUS, &status);
	assert(status == GL_TRUE);
	glGetShaderiv(dev->frag_shdr, GL_COMPILE_STATUS, &status);
	assert(status == GL_TRUE);
	glAttachShader(dev->prog, dev->vert_shdr);
	glAttachShader(dev->prog, dev->frag_shdr);
	glLinkProgram(dev->prog);
	glGetProgramiv(dev->prog, GL_LINK_STATUS, &status);
	assert(status == GL_TRUE);

	dev->uniform_tex = glGetUniformLocation(dev->prog, "Texture");
	dev->uniform_proj = glGetUniformLocation(dev->prog, "ProjMtx");
	dev->attrib_pos = glGetAttribLocation(dev->prog, "Position");
	dev->attrib_uv = glGetAttribLocation(dev->prog, "TexCoord");
	dev->attrib_col = glGetAttribLocation(dev->prog, "Color");

	{
		/* buffer setup */
		GLsizei vs = sizeof(struct nk_vertex);
		size_t vp = offsetof(struct nk_vertex, position);
		size_t vt = offsetof(struct nk_vertex, uv);
		size_t vc = offsetof(struct nk_vertex, col);

		glGenBuffers(1, &dev->vbo);
		glGenBuffers(1, &dev->ebo);
		glGenVertexArrays(1, &dev->vao);

		glBindVertexArray(dev->vao);
		glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

		glEnableVertexAttribArray((GLuint)dev->attrib_pos);
		glEnableVertexAttribArray((GLuint)dev->attrib_uv);
		glEnableVertexAttribArray((GLuint)dev->attrib_col);

		glVertexAttribPointer((GLuint)dev->attrib_pos, 2, GL_FLOAT, GL_FALSE, vs, (void*)vp);
		glVertexAttribPointer((GLuint)dev->attrib_uv, 2, GL_FLOAT, GL_FALSE, vs, (void*)vt);
		glVertexAttribPointer((GLuint)dev->attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, vs, (void*)vc);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

NK_INTERN void nk_win32_device_upload_atlas(const void *image, int width, int height) {
	struct nk_device *dev = &ctx.ogl;
	glGenTextures(1, &dev->font_tex);
	glBindTexture(GL_TEXTURE_2D, dev->font_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image);
}

NK_API void nk_win32_device_destroy(void) {
	struct nk_device *dev = &ctx.ogl;
	glDetachShader(dev->prog, dev->vert_shdr);
	glDetachShader(dev->prog, dev->frag_shdr);
	glDeleteShader(dev->vert_shdr);
	glDeleteShader(dev->frag_shdr);
	glDeleteProgram(dev->prog);
	glDeleteTextures(1, &dev->font_tex);
	glDeleteBuffers(1, &dev->vbo);
	glDeleteBuffers(1, &dev->ebo);
	nk_buffer_free(&dev->cmds);
}

NK_API void nk_blank_render(int width, int height, int display_width, int display_height) {
	nk_clear(&ctx.ctx);
}


NK_API void nk_win32_render(int width, int height, int display_width, int display_height) {

	int max_vertex_buffer = NK_MAX_VERTEX_MEMORY;
	int max_element_buffer = NK_MAX_ELEMENT_MEMORY;

	enum nk_anti_aliasing AA = NK_ANTI_ALIASING_ON;
	struct nk_device *dev = &ctx.ogl;
	struct nk_vec2 scale;
	GLfloat ortho[4][4] = {
		{2.0f, 0.0f, 0.0f, 0.0f},
		{0.0f,-2.0f, 0.0f, 0.0f},
		{0.0f, 0.0f,-1.0f, 0.0f},
		{-1.0f,1.0f, 0.0f, 1.0f},
	};
	ortho[0][0] /= (GLfloat)width;
	ortho[1][1] /= (GLfloat)height;

	scale.x = (float)display_width / (float)width;
	scale.y = (float)display_height / (float)height;

	/* setup global state */
	glViewport(0, 0, display_width, display_height);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	/* setup program */
	glUseProgram(dev->prog);
	glUniform1i(dev->uniform_tex, 0);
	glUniformMatrix4fv(dev->uniform_proj, 1, GL_FALSE, &ortho[0][0]);
	{
		/* convert from command queue into draw list and draw to screen */
		const struct nk_draw_command *cmd;
		void *vertices, *elements;
		const nk_draw_index *offset = NULL;
		struct nk_buffer vbuf, ebuf;

		/* allocate vertex and element buffer */
		glBindVertexArray(dev->vao);
		glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

		glBufferData(GL_ARRAY_BUFFER, max_vertex_buffer, NULL, GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_element_buffer, NULL, GL_STREAM_DRAW);

		/* load vertices/elements directly into vertex/element buffer */
		vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
		{
			/* fill convert configuration */
			struct nk_convert_config config;
			static const struct nk_draw_vertex_layout_element vertex_layout[] = {
				{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_vertex, position)},
				{NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_vertex, uv)},
				{NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_vertex, col)},
				{NK_VERTEX_LAYOUT_END}
			};
			NK_MEMSET(&config, 0, sizeof(config));
			config.vertex_layout = vertex_layout;
			config.vertex_size = sizeof(struct nk_vertex);
			config.vertex_alignment = NK_ALIGNOF(struct nk_vertex);
			config.null = dev->null;
			config.circle_segment_count = 22;
			config.curve_segment_count = 22;
			config.arc_segment_count = 22;
			config.global_alpha = 1.0f;
			config.shape_AA = AA;
			config.line_AA = AA;

			/* setup buffers to load vertices and elements */
			nk_buffer_init_fixed(&vbuf, vertices, (nk_size)max_vertex_buffer);
			nk_buffer_init_fixed(&ebuf, elements, (nk_size)max_element_buffer);
			nk_convert(&ctx.ctx, &dev->cmds, &vbuf, &ebuf, &config);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

		/* iterate over and execute each draw command */
		nk_draw_foreach(cmd, &ctx.ctx, &dev->cmds) {
			if (!cmd->elem_count) continue;
			glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
			glScissor((GLint)(cmd->clip_rect.x * scale.x),
				(GLint)((height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h)) * scale.y),
				(GLint)(cmd->clip_rect.w * scale.x),
				(GLint)(cmd->clip_rect.h * scale.y));
			glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset);
			offset += cmd->elem_count;
		}
		nk_clear(&ctx.ctx);
	}

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
	glDisable(GL_SCISSOR_TEST);
}


static void
nk_gdi_clipboard_paste(nk_handle usr, struct nk_text_edit *edit)
{
	(void)usr;
	if (IsClipboardFormatAvailable(CF_UNICODETEXT) && OpenClipboard(NULL))
	{
		HGLOBAL mem = GetClipboardData(CF_UNICODETEXT);
		if (mem)
		{
			SIZE_T size = GlobalSize(mem) - 1;
			if (size)
			{
				LPCWSTR wstr = (LPCWSTR)GlobalLock(mem);
				if (wstr)
				{
					int utf8size = WideCharToMultiByte(CP_UTF8, 0, wstr, (int)(size / sizeof(wchar_t)), NULL, 0, NULL, NULL);
					if (utf8size)
					{
						char* utf8 = (char*)malloc(utf8size);
						if (utf8)
						{
							WideCharToMultiByte(CP_UTF8, 0, wstr, (int)(size / sizeof(wchar_t)), utf8, utf8size, NULL, NULL);
							nk_textedit_paste(edit, utf8, utf8size);
							free(utf8);
						}
					}
					GlobalUnlock(mem);
				}
			}
		}
		CloseClipboard();
	}
}

static void
nk_gdi_clipboard_copy(nk_handle usr, const char *text, int len)
{
	if (OpenClipboard(NULL))
	{
		int wsize = MultiByteToWideChar(CP_UTF8, 0, text, len, NULL, 0);
		if (wsize)
		{
			HGLOBAL mem = (HGLOBAL)GlobalAlloc(GMEM_MOVEABLE, (wsize + 1) * sizeof(wchar_t));
			if (mem)
			{
				wchar_t* wstr = (wchar_t*)GlobalLock(mem);
				if (wstr)
				{
					MultiByteToWideChar(CP_UTF8, 0, text, len, wstr, wsize);
					wstr[wsize] = 0;
					GlobalUnlock(mem);

					SetClipboardData(CF_UNICODETEXT, mem);
				}
			}
		}
		CloseClipboard();
	}
}

NK_API struct nk_context* nk_win32_init() {
	// Stash begin
	nk_font_atlas_init_default(&ctx.atlas);
	nk_font_atlas_begin(&ctx.atlas);

	// Load font
	struct nk_font* font = nk_font_atlas_add_default(&ctx.atlas, 13, 0);

	// Stash end
	const void *image; int w, h;
	image = nk_font_atlas_bake(&ctx.atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
	nk_win32_device_upload_atlas(image, w, h);
	nk_font_atlas_end(&ctx.atlas, nk_handle_id((int)ctx.ogl.font_tex), &ctx.ogl.null);
	if (ctx.atlas.default_font) {
		nk_style_set_font(&ctx.ctx, &ctx.atlas.default_font->handle);
	}

	nk_init_default(&ctx.ctx, &font->handle);
	ctx.ctx.clip.copy = nk_gdi_clipboard_copy;
	ctx.ctx.clip.paste = nk_gdi_clipboard_paste;
	ctx.ctx.clip.userdata = nk_handle_ptr(0);
	nk_win32_device_create();

	return &ctx.ctx;
}

NK_API void nk_win32_font_stash_begin(struct nk_font_atlas **atlas) {
	nk_font_atlas_init_default(&ctx.atlas);
	nk_font_atlas_begin(&ctx.atlas);
	*atlas = &ctx.atlas;
}

NK_API void nk_win32_font_stash_end(void) {
	const void *image; int w, h;
	image = nk_font_atlas_bake(&ctx.atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
	nk_win32_device_upload_atlas(image, w, h);
	nk_font_atlas_end(&ctx.atlas, nk_handle_id((int)ctx.ogl.font_tex), &ctx.ogl.null);
	if (ctx.atlas.default_font)
		nk_style_set_font(&ctx.ctx, &ctx.atlas.default_font->handle);
}

#define LEFT_MOUSE_POS (short)((float)LOWORD(lparam) * scaleFactor)
#define RIGHT_MOUSE_POS (short)((float)HIWORD(lparam) * scaleFactor)

NK_API int nk_win32_handle_event(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam, float scaleFactor) {
	switch (msg)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	{
		int down = !((lparam >> 31) & 1);
		int ctrl = GetKeyState(VK_CONTROL) & (1 << 15);

		switch (wparam)
		{
		case VK_SHIFT:
		case VK_LSHIFT:
		case VK_RSHIFT:
			nk_input_key(&ctx.ctx, NK_KEY_SHIFT, down);
			return 1;

		case VK_DELETE:
			nk_input_key(&ctx.ctx, NK_KEY_DEL, down);
			return 1;

		case VK_RETURN:
			nk_input_key(&ctx.ctx, NK_KEY_ENTER, down);
			return 1;

		case VK_TAB:
			nk_input_key(&ctx.ctx, NK_KEY_TAB, down);
			return 1;

		case VK_LEFT:
			if (ctrl)
				nk_input_key(&ctx.ctx, NK_KEY_TEXT_WORD_LEFT, down);
			else
				nk_input_key(&ctx.ctx, NK_KEY_LEFT, down);
			return 1;

		case VK_RIGHT:
			if (ctrl)
				nk_input_key(&ctx.ctx, NK_KEY_TEXT_WORD_RIGHT, down);
			else
				nk_input_key(&ctx.ctx, NK_KEY_RIGHT, down);
			return 1;

		case VK_BACK:
			nk_input_key(&ctx.ctx, NK_KEY_BACKSPACE, down);
			return 1;

		case VK_HOME:
			nk_input_key(&ctx.ctx, NK_KEY_TEXT_START, down);
			nk_input_key(&ctx.ctx, NK_KEY_SCROLL_START, down);
			return 1;

		case VK_END:
			nk_input_key(&ctx.ctx, NK_KEY_TEXT_END, down);
			nk_input_key(&ctx.ctx, NK_KEY_SCROLL_END, down);
			return 1;

		case VK_NEXT:
			nk_input_key(&ctx.ctx, NK_KEY_SCROLL_DOWN, down);
			return 1;

		case VK_PRIOR:
			nk_input_key(&ctx.ctx, NK_KEY_SCROLL_UP, down);
			return 1;

		case 'C':
			if (ctrl) {
				nk_input_key(&ctx.ctx, NK_KEY_COPY, down);
				return 1;
			}
			break;

		case 'V':
			if (ctrl) {
				nk_input_key(&ctx.ctx, NK_KEY_PASTE, down);
				return 1;
			}
			break;

		case 'X':
			if (ctrl) {
				nk_input_key(&ctx.ctx, NK_KEY_CUT, down);
				return 1;
			}
			break;

		case 'Z':
			if (ctrl) {
				nk_input_key(&ctx.ctx, NK_KEY_TEXT_UNDO, down);
				return 1;
			}
			break;

		case 'R':
			if (ctrl) {
				nk_input_key(&ctx.ctx, NK_KEY_TEXT_REDO, down);
				return 1;
			}
			break;
		}
		return 0;
	}

	case WM_CHAR:
		if (wparam >= 32)
		{
			nk_input_unicode(&ctx.ctx, (nk_rune)wparam);
			return 1;
		}
		break;

	case WM_LBUTTONDOWN:
		nk_input_button(&ctx.ctx, NK_BUTTON_LEFT, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 1);
		SetCapture(wnd);
		return 1;

	case WM_LBUTTONUP:
		nk_input_button(&ctx.ctx, NK_BUTTON_DOUBLE, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 0);
		nk_input_button(&ctx.ctx, NK_BUTTON_LEFT, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 0);
		ReleaseCapture();
		return 1;

	case WM_RBUTTONDOWN:
		nk_input_button(&ctx.ctx, NK_BUTTON_RIGHT, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 1);
		SetCapture(wnd);
		return 1;

	case WM_RBUTTONUP:
		nk_input_button(&ctx.ctx, NK_BUTTON_RIGHT, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 0);
		ReleaseCapture();
		return 1;

	case WM_MBUTTONDOWN:
		nk_input_button(&ctx.ctx, NK_BUTTON_MIDDLE, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 1);
		SetCapture(wnd);
		return 1;

	case WM_MBUTTONUP:
		nk_input_button(&ctx.ctx, NK_BUTTON_MIDDLE, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 0);
		ReleaseCapture();
		return 1;

	case WM_MOUSEWHEEL:
		nk_input_scroll(&ctx.ctx, nk_vec2(0, (float)(short)HIWORD(wparam) / WHEEL_DELTA));
		return 1;

	case WM_MOUSEMOVE:
		nk_input_motion(&ctx.ctx, LEFT_MOUSE_POS, RIGHT_MOUSE_POS);
		return 1;

	case WM_LBUTTONDBLCLK:
		nk_input_button(&ctx.ctx, NK_BUTTON_DOUBLE, LEFT_MOUSE_POS, RIGHT_MOUSE_POS, 1);
		return 1;
	}

	return 0;
}


NK_API void nk_win32_shutdown(void) {
	nk_font_atlas_clear(&ctx.atlas);
	nk_free(&ctx.ctx);
	nk_win32_device_destroy();
	memset(&ctx, 0, sizeof(ctx));
}

NK_API void nk_demo() {
	enum { EASY, HARD };
	static int op = EASY;
	static float value = 0.6f;
	static int i = 20;

	if (nk_begin(&ctx.ctx, "Show", nk_rect(50, 50, 220, 220),
		NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE)) {
		/* fixed widget pixel width */
		nk_layout_row_static(&ctx.ctx, 30, 80, 1);
		if (nk_button_label(&ctx.ctx, "button")) {
			/* event handling */
		}

		/* fixed widget window ratio width */
		nk_layout_row_dynamic(&ctx.ctx, 30, 2);
		if (nk_option_label(&ctx.ctx, "easy", op == EASY)) op = EASY;
		if (nk_option_label(&ctx.ctx, "hard", op == HARD)) op = HARD;

		/* custom widget pixel width */
		nk_layout_row_begin(&ctx.ctx, NK_STATIC, 30, 2);
		{
			nk_layout_row_push(&ctx.ctx, 50);
			nk_label(&ctx.ctx, "Volume:", NK_TEXT_LEFT);
			nk_layout_row_push(&ctx.ctx, 110);
			nk_slider_float(&ctx.ctx, 0, &value, 1.0f, 0.1f);
		}
		nk_layout_row_end(&ctx.ctx);
	}
	nk_end(&ctx.ctx);
}