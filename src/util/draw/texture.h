#pragma once

#include "util/gc/file.h"
#include "util/misc.h"
#include "util/draw/render.h"
#include <ogc/gx.h>

class texture {
	// Encode width/height/format at start of file
	struct tex_header {
		u16 width;
		u16 height;
		u8 format;
	};

	// 32 byte align data
	struct tex_file : tex_header {
	private:
		u8 align[align_up(sizeof(tex_header), 32) - sizeof(tex_header)];
	public:
		char data[];
	};

	mutable GXTexObj tex_obj;
	const asset_file file;
	
	const tex_file *tex() const
	{
		return (tex_file*)file.data.get();
	}

public:
	texture(const std::string &path,
	        u8 wrap_s = GX_REPEAT,
	        u8 wrap_t = GX_REPEAT,
	        u8 mipmap = GX_FALSE
	) : file(path)
	{
		GX_InitTexObj(&tex_obj, (void*)tex()->data, tex()->width, tex()->height,
		              tex()->format, wrap_s, wrap_t, mipmap);
	}

	void apply() const
	{
		render_state::get().load_tex_obj(&tex_obj);
	}
	
	u16 width() const
	{
		return tex()->width;
	}

	u16 height() const
	{
		return tex()->height;
	}
};