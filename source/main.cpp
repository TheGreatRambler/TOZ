#include <unistd.h>

#include "thirdParty/CLI11.hpp"
#include "thirdParty/json.hpp"

#include "HIDInterface.hpp"

int main(int argc, char** argv) {
	CLI::App app { "A TAS program that can be run on unmodified hardware from a Raspi Zero" };

	std::string action = "run";
	app.add_option("-a,--action", action, "Choose action that the program should do");

	// Go back one folder for later stuff
	chdir("..");

	if (action == "update") {
		// Run update
		system("git reset --hard");
		system("git pull origin master");
		system("make");
		puts("Program is updated");
	} else if (action == "run") {
		StartGadget();
	}

	CLI11_PARSE(app, argc, argv);
	return 0;
}