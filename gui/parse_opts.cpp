#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "parse_opts.hpp"

static struct option opts[] = {
	{"help", no_argument, 0, 'h'},
	{"engine", no_argument, 0, 'e'},
	{NULL, 0, 0, 0}
};

extern void print_help(char* command) {
	std::cout << 
"Usage: " << command << " [options...]\n" 	<<
"-h, --help         Get a list of all options\n" <<
"-e, --engine       Play against the engine, by default you are playing white\n" <<
"\n" << 
"This is a chess game an engine from bgfxc4, if you want to, you can visit the github repo \n" <<
"https://github.com/bgfxc4/chess" <<
	std::endl;
}

extern void usage(char *command, int exit_code) {
	print_help(command);
	exit(exit_code);
}

extern void parse_opts(int argc, char** argv, struct opts* out) {
	out->help = 0, out->engine = 0;

	while (1) {
		int opt = getopt_long(argc, argv, "he", opts, NULL);

		if (opt == -1) {
			break;
		}

		switch (opt) {
			case '?':
				usage(argv[0], 1);
				break;
			case 'h':
				out->help = 1;
				print_help(argv[0]);
				break;
			case 'e':
				out->engine = 1;
				break;
		}
	}
}
