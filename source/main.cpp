#include <unistd.h>
#include <string.h>

#include "thirdParty/CLI11.hpp"

#include "HIDInterface.hpp"
#include "helpers.hpp"

bool IsRunningAsSudo() {
	// When effective uid is zero, the effective user has admin rights
	return !geteuid();
}

int main(int argc, char** argv) {
	CLI::App app { "A TAS program that can be run on unmodified hardware from a Raspi Zero" };

	std::string action = "run";
	app.add_option("-a,--action", action, "Choose action that the program should do");

	// Parse commands
	CLI11_PARSE(app, argc, argv);

	if (action == "update") {
		// Run update
		// Go to executable's path
		std::string exePath = GetExePath();
		puts(exePath.c_str());
		chdir(exePath.c_str());
		// Go back one folder
		chdir("..");
		// Make sure build tools and kernel build tools are present
		system("apt-get install build-essential raspberrypi-kernel-headers");
		puts("-----Starting Update-----");
		system("git reset --hard");
		system("git pull origin master");
		system("make");
		// Installs dummy_hcd
		puts("Installing dummy_hcd");
		chdir("source/dummy_hcd");
		// Run makefile (makefile moves file to appropriate place)
		system("make");
		puts("----Finished-----");
	} else if (action == "run") {
		// Start gadget
		if (IsRunningAsSudo()) {
			puts("-----Starting gadget-----");
			StartGadget();
		} else {
			puts("Admin rights (using SUDO or otherwise) are required to run this application");
		}
		// Just print the most recent logs
		system("tail -n 30 /var/log/syslog");
	}

	return 0;
}
