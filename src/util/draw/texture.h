#pragma once

#include "util/gc/file.h"
#include <ogc/gx.h>

class texture {
	// Encode width/height/format at start of file
	struct tex_file {
		u16 width;
		u16 height;
		u8 format;
	private:
		u8 align[32 - 5];
	public:
		char data[];
	};

	GXTexObj tex_obj;
	const asset_file file;

public:
	texture(const std::string &path,
	        u8 wrap_s = GX_REPEAT,
	        u8 wrap_t = GX_REPEAT,
	        u8 mipmap = GX_TRUE
	) : file(path)
	{
		GX_InitTexObj(&tex_obj, (void*)info()->data, info()->width, info()->height,
		              GX_TF_RGBA8, wrap_s, wrap_t, mipmap);
	}
	
	const tex_file *info() const
	{
		return (tex_file*)file.data.get();
	}

	void apply()
	{
		GX_LoadTexObj(&tex_obj, GX_TEXMAP0);
	}
};