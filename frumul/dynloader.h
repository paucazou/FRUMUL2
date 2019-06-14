#ifndef DYNLOADER_H
#define DYNLOADER_H
#include "symbol.h"
#include <filesystem>
#include <map>
#include <memory>
#ifdef __linux__
#include <dlfcn.h>
#endif
namespace fs = std::filesystem;

namespace frumul {
	/*
	typedef std::unique_ptr<Symbol> create_sym();
	*/
	void * __load_lib(const fs::path&);

	template <typename T>
		std::function<T> load_function(const fs::path& path, const char* func_name) {
			/* Loads a dynamic function
			 * and return it
			 */
			void* _f_ptr{ __load_lib(path) };
#ifdef __linux__
			T* fun = (T*) dlsym(_f_ptr,func_name);
			if (!fun) {
				std::cerr << dlerror();
				throw BackException(exc::DYNAMIC_FUNCTION_NOT_FOUND);
			}
#endif
			return *fun;
		}

	template <typename T>
		T load_object(const fs::path& path, const char* func_name) {
			/* Loads a dynamic object
			 */
			return load_function<T()>(path,func_name)();
		}

	std::unique_ptr<Symbol> load_lib(const fs::path& path);


}

#endif
