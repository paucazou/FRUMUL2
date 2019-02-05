#include "cmdline.h"
#include <fstream>

namespace frumul {

	cxxopts::Options get_options() {
		/* get the options expected
		 * in the command line
		 */

		cxxopts::Options options {"FRUMUL 2", "Fast and Readable MarkUp Language 2"};
		// main options
		options.add_options()
			("h,help","Print help");
		options.add_options("Transpilation")
			("i,input","Input FRUMUL file",cxxopts::value<String>())
			("o,output","Output file.", cxxopts::value<String>())
			("l,lang","Language output",cxxopts::value<String>()->default_value("every"))
			("r,recursive","Transpile the output until output is not a valid FRUMUL file.",cxxopts::value<bool>()->default_value("false"));

		options.add_options("New file")
			("new-file","Create a new FRUMUL file with given path",cxxopts::value<std::string>())
			("header","Header + opening tag for a new file."
			 "Use a path for your own header, and a name for a standard header. "
			 "Example: --header json jj",
			 cxxopts::value<std::vector<String>>());

		return options;
	}

	void manage_args(cxxopts::Options& options, int argc, char ** argv) {
		/* Follow the args
		 */
		cxxopts::ParseResult results{options.parse(argc, argv)};

		if (results.count("help")){
			std::cout << options.help();
			return;
		}
		if (results.count("new-file")) {
			std::ofstream new_file { results["new-file"].as<std::string>(), std::ofstream::out};
			new_file << "___header___\n";
			if (results.count("header")) {
			}
			new_file << "___text___\n";
			new_file.close();


		}
	}

	bool check_args_compatibility(const cxxopts::Options& options, const cxxopts::ParseResult& results, const ParametersList parameters) {
		/* Check that the parameters entered are compatible
		 * The parameters argument contains vectors of parameters compatible
		 * with parameters of the same vector, but not compatible with 
		 * parameters of other vectors
		 */
	}
}


