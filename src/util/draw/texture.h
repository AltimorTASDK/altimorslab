#pragma once

#include "util/gc/file.h"
#include <ogc/gx.h>

class texture {
	GXTexObj tex_obj;
	dvd_file file;

public:
	texture(const std::string &path,
	        u16 width,
	        u16 height,
	        u8 format = GX_RGBA8,
	        u8 wrap_s = GX_REPEAT,
	        u8 wrap_t = GX_REPEAT,
	        u8 mipmap = GX_TRUE
	) : file(path)
	{
		GX_InitTexObj(&tex_obj, file.data.get(), width, height,
		              format, wrap_s, wrap_t, mipmap);
	}

	void apply()
	{
		GX_LoadTexObj(&tex_obj, 0);
	}
};