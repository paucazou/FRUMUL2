#include "cmdline.h"
#include "transpiler.h"
#include "util.h"
#include <fstream>

namespace frumul {

	const FString& language{__language};

	cxxopts::Options get_options() {
		/* get the options expected
		 * in the command line
		 */

		cxxopts::Options options {"FRUMUL 2", "Fast and Readable MarkUp Language 2"};
		// main options
		options.add_options()
			("h,help","Print help");
		options.add_options("Transpilation")
			("i,input","Input FRUMUL file",cxxopts::value<std::string>())
			("o,output","Output file.", cxxopts::value<std::string>())
			("l,lang","Language output",cxxopts::value<FString>()->default_value("every"))
			("r,recursive","Transpile the output until output is not a valid FRUMUL file.",cxxopts::value<bool>()->default_value("false"));

		options.add_options("New file")
			("new-file","Create a new FRUMUL file with given path",cxxopts::value<std::string>())
			("header","Header + opening tag for a new file."
			 "Use a path for your own header, and a name for a standard header. "
			 "Example: --header json jj",
			 cxxopts::value<std::vector<FString>>());

		return options;
	}

	void manage_args(cxxopts::Options& options, int argc, char ** argv) {
		/* Follow the args
		 */
		cxxopts::ParseResult results{options.parse(argc, argv)};

		using s_vec = std::vector<std::string>;
		if (!check_args_compatibility(results,
				s_vec({"help"}),
				s_vec({"input","output","lang","recursive"}),
				s_vec({"new-file","header"})
				)) {
			std::cerr << "Incompatible arguments\n";
			std::cerr << options.help();
		}

		if (results.count("help")){
			std::cout << options.help();
			return;
		}
		if (results.count("new-file")) {
			std::ofstream new_file { results["new-file"].as<std::string>(), std::ofstream::out};
			new_file << "___header___\n";
			if (results.count("header")) {
#pragma message "header not yet set"
			}
			new_file << "___text___\n";
		}

		if (results.count("input")) {
#pragma message "Add some security checks"
				// get the source
				std::ifstream fileopened(results["input"].as<std::string>());
				FString source {slurp(fileopened)};
				FString filepath {results["input"].as<std::string>()};
				// get the language
				FString lang {
					results.count("lang") ?
					results["lang"].as<FString>() :
					"every" };

				// transpilation
				Transpiler transpiler{source,filepath,lang};

				// get the output
				if (results.count("output")) {
					std::ofstream output {results["output"].as<std::string>()};
					output << transpiler.getOutput();
					output.close();
				}
				else
				std::cout << transpiler.getOutput();


		}
	}

	bool check_args_compatibility(const cxxopts::ParseResult& results, const ParametersList parameters) {
		/* Check that the parameters entered are compatible
		 * The parameters argument contains vectors of parameters compatible
		 * with parameters of the same vector, but not compatible with 
		 * parameters of other vectors
		 * parameters are supposed to be well formed and match the options set.
		 */
		constexpr int not_yet_set { -1 };
		int compatibility_index{not_yet_set};

		for (const auto& kv : results.arguments()) {
			const auto& key { kv.key() };

			int i{0};
			for (auto it {parameters.begin()}; it != parameters.end(); ++it,++i) {
				for (const auto& parm : *it) {
					if (parm == key && compatibility_index == not_yet_set) {
						compatibility_index = i;
					}
					else if (parm == key && compatibility_index != i) {
						return false;
					}
					else if (parm == key && compatibility_index == i)
						break;
				}
			}
		}
		return true;

	}
}


