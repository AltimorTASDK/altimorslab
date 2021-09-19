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
		data(std::make_unique<char[]>(align_up(length, 32)))
	{
		auto _length = length;
		OSReport("file len %d\n", _length);
		File_Read(path.c_str(), data.get(), &_length);
	}
};

constexpr char lab_root[] = "lab/";

class asset_file : public dvd_file {
public:
	asset_file(const std::string &path) : dvd_file(lab_root + path) {}
};