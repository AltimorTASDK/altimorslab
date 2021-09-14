#pragma once

#include "hsd/memory.h"
#include "os/file.h"
#include "util/misc.h"
#include <memory>
#include <string>

class dvd_file {
public:
	const size_t length;
	const std::unique_ptr<char[]> data;

	dvd_file(const std::string &path) :
		length(File_GetLength(path.c_str())),
		data(std::make_unique<char[]>(align(length, 32)))
	{
		auto _length = length;
		File_Read(path.c_str(), data.get(), &_length);
	}
};