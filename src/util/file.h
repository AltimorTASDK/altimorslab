#pragma once

#include "hsd/memory.h"
#include "os/file.h"
#include "util/misc.h"
#include <memory>
#include <string>

class dvd_file {
	size_t length;
	std::unique_ptr<char[]> data;

public:
	dvd_file(const std::string &path)
	{
		length = File_GetLength(path.c_str());
		data = std::make_unique<char[]>(align(length, 32));
		File_Read(path.c_str(), data.get(), &length);
	}

	size_t get_length() const { return length; }
	char *get_data() const { return data.get(); }
};