#ifndef DYNLOADER_H
#define DYNLOADER_H
#include "symbol.h"
#include <filesystem>
#ifdef __linux__
#include <dlfcn.h>
#endif
namespace fs = std::filesystem;

namespace frumul {
	typedef std::unique_ptr<Symbol> create_sym();
	std::unique_ptr<Symbol> load_lib(const fs::path& path);
}

#endif
