
/*
	NVIDIA Fan Controller for Linux.
	- by mikejzx, Sun 29 Sep 2019

	This basic program adjusts the GPU's fan speeds based on
	the temperature. (Only single GPU's supported.)

	This command must be run prior:
	sudo nvidia-xconfig -a --cool-bits=28 --allow-empty-initial-configuration
*/

#include <iostream> // std::cout and other I/O.
#include <string.h> // For strcmp
#include <vector>   // std::vector
#include <fstream>  // std::ifstream
#include <thread>   // std::this_thread
#include <chrono>   // For ms, s, min, etc sleep literals.
#include <cmath>    // std::isfinite
#include <memory>   // std::unique_ptr
#include <array>    // std::array

#define CMD_SUCCESS 0 

int  parse_cmd   (const int,  char**); // Parses command line arguments.
bool parse_config(const std::string&); // Parse the input configuration file.
unsigned vector_ceil (const unsigned, unsigned* const); // Rounds x to nearest high number in vector.
unsigned vector_floor(const unsigned, unsigned* const); // Rounds x to nearest low  number in vector.
unsigned query_gpu_temp();

// Tells the user to pass --help
inline void print_help_stuff() { std::cout << "Pass --help for help." << std::endl; }

static std::string filepath = "./fan.config";
static std::vector<unsigned> points_temp, points_perc;
static unsigned points_count = 0;
static bool verbose = false;

// Entry point.
int main(int argc, char* argv[])
{
	// Print out a good old title to really annoy everyone.	
	std::cout << "\033[1;35m -- -- mikejzx's NVIDIA Fan Curve Controller for Linux -- -- \033[0m" << std::endl;
	std::cout << "\033[1;33m ----- https://mikejzx.github.io ----- \033[0m" << std::endl;
	std::cout << "Running..." << std::endl;

	if (parse_cmd(argc, argv) != CMD_SUCCESS // Parse command-line  args.
		|| parse_config(filepath) != true    // Parse configuration file.
	)
	{
		std::cout << "Aborting..." << std::endl;
		return -1;
	}
	
	// Allow user-defined fan control of the GPU
	std::string no_output = " > /dev/null"; // Great Unix trick to remove output of a command.
	system(std::string("nvidia-settings -a [gpu:0]/GPUFanControlState=1" + no_output).c_str());	
	std::string cmd = std::string("nvidia-settings -a [fan:0]/GPUTargetFanSpeed=");

	// Get access to the 'ms' literal.
	// (using namespace in local scope forever :D)
	using namespace std::literals::chrono_literals;

	// Main loop. Just program will run infinitely unless closed.
	// The for-loop will query the GPU temperature and use that to determine the fan speed
	// based off the given curve.
	unsigned cur_temp = 0;
	unsigned cur_percent = 0;
	for(;cur_temp = query_gpu_temp();)
	{	
		// Get the range.
		unsigned lo_temp, hi_temp;
		unsigned lo = vector_floor(cur_temp, &lo_temp);
		unsigned hi = vector_ceil (cur_temp, &hi_temp);

		// If we visualise what we had as a value that's been linearly-interpolated,
		// we are able to "undo" it and find a value determining the fraction of where
		// the current temperature is in relation to lo and hi.
		float unlerped = (float)(cur_temp - lo_temp) / (float)(hi_temp - lo_temp);
		unlerped = std::isfinite(unlerped) ? unlerped : 1.0f;
		cur_percent = lo + unlerped * (hi - lo); // Do a lerp with the unlerped value. (lerp = a + t * (b - a))

		// Print the current fan speed.
		if (verbose)
		{
			std::cout << cur_percent << "% " << std::endl;
		}

		// For debugging
		//std::cout << "T:" << cur_temp << ", P:" << cur_percent << "%. hi: " << hi << ", lo: " << lo << "-- unl:" << unlerped << " --- lotem: " << lo_temp << " ; hitem: " << hi_temp<< std::endl;

		// Set the fan speed!
		system(std::string(cmd + std::to_string(cur_percent) + no_output).c_str());

		// Sleep for 3 seconds
		std::this_thread::sleep_for(3000ms);
	}
	std::cout << std::endl;

	return 0;
}

// Captures stdout output from a terminal command and returns it as a string.
// https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
std::string exec_get_stdout(const char* cmd) 
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) 
	{
        return "";
		std::cout << "popen() failed" << std::endl;
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) 
	{
        result += buffer.data();
    }
    return result;
}

// Get the GPU temperature.
unsigned query_gpu_temp()
{
	std::string cmd = "nvidia-smi --query-gpu=temperature.gpu --format=csv,noheader";
	std::string outp = exec_get_stdout(cmd.c_str());
	
	// Convert to uint
	unsigned temp;
	sscanf(outp.c_str(), "%u", &temp);
//	std::cout << "Temperature is " << temp << std::endl;	
	return temp;
}

bool parse_config(const std::string& path)
{
	// Setup file stream.
	std::ifstream file;
	file.open(path);
	if (!file.is_open())
	{
		std::cout << "Error opening file at '" << path << "'. Check that the file exists." << std::endl;
		return false;
	}
 
	// Reserve to "optimise" a bit. (Prevents unnecessary reallocations)
	// We assume the user will have at least 6 points on their graph.
	points_temp.reserve(6);
	points_perc.reserve(6);

	// Read the file line-by-line.
	std::string line;
	unsigned temp, percent;
	while(std::getline(file, line))
	{
		// Read the line, store temperature and percent in variables.
		// (Two '%' will escape it so it is an actual percent sign.)
		if (sscanf(line.c_str(), "%u %u%%", &temp, &percent) != 2)
		{
			// No dice. Skip this line.
			continue;
		}

		// Push to the back of the vectors.
		points_temp.emplace_back(temp);
		points_perc.emplace_back(percent);

		// Add to points count.
		++points_count;

		// Print it out for debug purposes.
		//std::cout << "t: " << temp << ", per: " << percent << std::endl;
	}

	// Close. (I believe it's done automatically now but whatever.)
	file.close();

	if (points_count == 0)
	{
		std::cout << "The configuration file did not contain any points. Please use the following format:" << std::endl;
		std::cout << "<at temperature> <fan percentage 0-100>%" << std::endl;
		return false;
	}
	
	// Note that the vectors are not sorted from smallest to largest temperature.
	// The user should use their common sense and not write their configuration like this anyway...

	// All went well.
	return true;
}

int parse_cmd(const int argc, char* argv[])
{
	// Argc is 1 when no arguments are passed.
	if (argc < 2) 
	{
		// No commands passed.
		return CMD_SUCCESS;
	}

	// User asked for help.
	if (strcmp(argv[1], "--help") == 0
		|| strcmp(argv[1], "-h") == 0
		|| strcmp(argv[1], "/h") == 0)
	{
		const char* indent = "        ";
		std::cout << std::endl << "mikejzx's NVIDIA Fan Controller Help" << std::endl;
		std::cout << "Arguments:" << std::endl << std::endl;
		std::cout << "    -f, --config-path" << std::endl;
		std::cout << indent << "Specifies the location of the configuration path." << std::endl;
		std::cout << indent << "By default the program checks in the current directory." << std::endl << std::endl;

		std::cout << "    -v, --verbose" << std::endl;
		std::cout << indent << "Increase verbosity; i.e: show the fan speed as it changes." << std::endl << std::endl;

		std::cout << "    -h, --help" << std::endl;
		std::cout << indent << "Displays this help information." << std::endl << std::endl;
	}

	// Argument handler loop
	for (unsigned i = 0; i  < argc; ++i)
	{
		// Passing the file path for configuration
		if (strcmp(argv[i], "-f") == 0
			|| strcmp(argv[i], "--config-path") == 0)
		{
			// Next argument must be present.
			if (i + 1 >= argc) 
			{
				// Not enough arguments.
				std::cout << "Missing parameters." << std::endl;
				print_help_stuff();
				return -1;
			}

			// Assign the config filepath.
			filepath = std::string(argv[i + 1]);
		}
	
		// Verbose mode
		if (strcmp(argv[i], "-v") == 0
			|| strcmp(argv[i], "--verbose") == 0)
		{
			std::cout << "Passed -v argument. Verbose flag enabled." << std::endl;
			verbose = true;
		}
	}

	return CMD_SUCCESS;
}

// Finds the nearest integer to x in the temperatures array that is larger than it.
// This will return the higher percentile at that point.
unsigned vector_ceil(const unsigned in_temp, unsigned* const out_temp)
{
	for (unsigned i = 0; i < points_count; ++i)
	{
		unsigned p = points_temp[i];
		if (p > in_temp)
		{
			// Note that this is the percents array and not temperaturs!
			*out_temp = p;
			return points_perc[i];
		}
	}

	// Return the maximum if there are no remaining points.
	*out_temp = points_temp[points_count - 1];
	return points_perc[points_count - 1];
}

// Finds the nearest integer to x in the temperatures array that is LESS than it.
// Will return the LOWER percentile at that point.
unsigned vector_floor(const unsigned in_temp, unsigned* const out_temp)
{
	for (int i = (int)points_count - 1; i > -1; --i)
	{
		unsigned p = points_temp[i];
		if (p <= in_temp)
		{
			*out_temp = p;
			return points_perc[i];
		}
	}
	
	// Return the minimum if there are no remaining points.
	*out_temp = points_temp[0];
	return points_perc[0];
}
