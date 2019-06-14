#include "dynloader.h"
#include "exception.h"
#include "map"

namespace frumul {

	void * __load_lib(const fs::path& path) {
		/* Load c++ library at the given path
		 */
		std::map<const fs::path, void*> libs_loaded;

		bool already_saved = libs_loaded.count(path);
#ifdef __linux__
		void* symb = already_saved ? libs_loaded[path] : dlopen(path.c_str(),RTLD_LAZY) ;
		if (!symb) {
			std::cerr << dlerror() << std::endl;
			throw BackException(exc::LIBRARY_NOT_FOUND);
		}
#endif

		if (! already_saved)
			libs_loaded.emplace(path,symb);
		return symb;
	}

	std::unique_ptr<Symbol> load_lib(const fs::path& path) {
		return load_object<std::unique_ptr<Symbol>>(path,"create_symbol");
	}
}
