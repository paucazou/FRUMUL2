#include "dynloader.h"
#include "exception.h"

namespace frumul {

	std::unique_ptr<Symbol> load_lib(const fs::path& path) {
		/* Load c++ library at the given path
		 */
		struct __special__pointers_manager {
			/* This class should not be used
			 * outside this file.
			 * It manages the pointer to dynamic
			 * classes already loaded
			 */
			// attributes
			std::vector<void*> libs_loaded;

			// destructor
			~__special__pointers_manager() {
				/* Destroys the libraries
				 * saved in libs_loaded
				 * Should be called at the end
				 * of the runtime
				 */
				for (auto ptr : libs_loaded)
#ifdef __linux__
					dlclose(ptr);
#endif
			}
		};

		static __special__pointers_manager ptr_manager;

#ifdef __linux__
		void* symb = dlopen(path.c_str(),RTLD_LAZY);
		if (!symb) {
			std::cerr << dlerror();
			throw BackException(exc::LIBRARY_NOT_FOUND);
		}
		create_sym* create_symbol = (create_sym*) dlsym(symb,"create_symbol");
		if (!create_symbol) {
			std::cerr << dlerror();
			throw BackException(exc::CREATE_SYMBOL_FUNCTION_NOT_FOUND);
		}
		ptr_manager.libs_loaded.push_back(symb);

		return create_symbol();
#endif
	}
}
