#include "cmdline.h"
#include "exception.h"
#include "transpiler.h"
#include "util.h"
#include <fstream>

namespace frumul {

	const FString& language{__language};
        static unsigned int __recursive__ = 0;

	void header_validator (const std::vector<FString>& headers) {
		/* Checks there is the good number of pairs
		 * headers/name
		 */
		if (headers.size() % 2 != 0)
			throw CLI::ValidationError("An opening tag lacks for the headers");
	}

	std::shared_ptr<CLI::App> get_app() {
		/* get the options expected
		 * in the command line
		 */
		// Note: we keep some variable names in comments to avoid -Wunused-variable

		auto app_ptr = std::make_shared<CLI::App>("FRUMUL 2, Fast and Readable MarkUp Language 2");
		CLI::App& app = *app_ptr;

		// Transpilation
		auto transpilation = app.add_option_group("Transpilation");
		auto input = transpilation->add_option("-i,--input","Select a source file")->check(CLI::ExistingFile);
		//auto output = 
		transpilation->add_option("-o,--output","Select the output file. Default: stdout")->needs(input);
		//auto lang = 
		transpilation->add_option("-l,--lang",__language,"Select the language. Default: every")->needs(input);
		//auto is_recursive = 
		transpilation->add_flag("-r,--recursive",__recursive__,
                        "Transpile output. Options:\n"
                        "   - only one flag: transpiles as many times as possible.\n"
                        "   - flag repeated: transpiles as many times as requested.\n"
                        "   - flag + number: transpiles as many times as requested.\n"
                        "Examples:\n"
                        "   - --recursive / -r: ad libitum\n"
                        "   - --recursive --recursive: 2times\n"
                        "   - -rrr: 3 times\n"
                        "   - --recursive=4: 4 times"
                        )->needs(input);

		// New file
		auto new_file = app.add_option_group("New file");
		auto new_file_path = new_file->add_option("--new-file","Creates a new FRUMUL file with given path")->excludes(input);
		//auto header = 
		new_file->add_option_function<std::vector<FString>>("--header",header_validator,"Header + opening tag for a new file.\nUse a path for your own header and a name for a standard header. \nNo check will be done.\nExample: --header json jj")->needs(new_file_path);

		return app_ptr;


		/*
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
	*/
	}

	void manage_args(CLI::App& app) {
		/* Follow the args
		 */
		// transpilation
		if (*app["--input"]) {
			FString path = app["--input"]->as<FString>();
			std::ifstream fileopened { path.toUTF8String<std::string>() };
			FString source = slurp(fileopened);

			// transpilation
			Transpiler transpiler{source,path,language};
			FString output_text { transpiler.getOutput() };

			// recursive
			if (*app["--recursive"]) {
                                unsigned int rec_nb { __recursive__ == 1 ? ~0u : __recursive__};
				FString nsource, npath;
				for (unsigned int i=1 ;i < rec_nb; ++i) {
					nsource = output_text;
					npath = path + " - recursive: " + i;
					try {
						Transpiler transpiler{nsource, npath, language};
						output_text = transpiler.getOutput();
					}
					catch (const exc& e) {
                                            if (i <= rec_nb && __recursive__ != 1)
                                                throw e;
                                            break;
					}
				}
			}

			// output
			auto output_opt = app["--output"];
			if (*output_opt) {
				std::ofstream output { output_opt->as<std::string>() };
				output << output_text;
				output.close();
			}
			else
				std::cout << output_text;

		}
		// new file creation
		else if (*app["--new-file"]) {
			std::ofstream new_file { app["--new-file"]->as<std::string>() };
			new_file << "___header___\n";
			if (*app["--header"]) {
				auto headers = app["--header"]->as<std::vector<std::string>>();
				for (size_t i{0}; i < headers.size(); i+= 2) {
					new_file << headers[i+1] << " : file «" << headers[i] << "» \n";
				}
			}
			new_file << "___text___\n";
			new_file.close();

		}
	}

	/*
	void manage_args(cxxopts::Options& options, int argc, char ** argv) {
		* Follow the args
		 *
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
	*/

	/*
	bool check_args_compatibility(const cxxopts::ParseResult& results, const ParametersList parameters) {
		* Check that the parameters entered are compatible
		 * The parameters argument contains vectors of parameters compatible
		 * with parameters of the same vector, but not compatible with 
		 * parameters of other vectors
		 * parameters are supposed to be well formed and match the options set.
		 *
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
	*/
}


