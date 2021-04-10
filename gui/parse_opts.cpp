#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <iostream>

#include "parse_opts.hpp"

static struct option opts[] = {
	{"help", no_argument, 0, 'h'},
	{"engine", no_argument, 0, 'e'},
	{"threads", required_argument, 0, 't'},
	{"depth", required_argument, 0, 'd'},
	{NULL, 0, 0, 0}
};

extern void print_help(char* command) {
	std::cout << 
"Usage: " << command << " [options...]\n" 	<<
"\n" <<
"-h, --help             Get a list of all options\n" <<
"-e, --engine           Play against the engine, by default you are playing white\n" <<
"-t --threads <NUMBER>  Sets the number of threads the engine uses, default is all threads on your machine\n" << 
"-d --depth <NUMBER>    Sets the depth the engine searches to, default is 5\n" << 
"\n" << 
"This is a chess game an engine from bgfxc4, if you want to, you can visit the github repo \n" <<
"https://github.com/bgfxc4/chess" <<
	std::endl;
}

extern void usage(char* command, int exit_code) {
	print_help(command);
	exit(exit_code);
}

extern void error(const char* msg, int exit_code) {
	std::cerr << msg << std::endl;
	exit(exit_code);
}

bool is_number(char* str) {
	for (int i = 0; *(str + i) != '\0'; i++) {
		if (std::isdigit(*(str + i)) == 0) return false;
	}
	return true;
}

extern void parse_opts(int argc, char** argv, struct opts* out) {
	out->help = 0;
	out->engine = 0;
	out->threads = -1;
	out->depth = -1;

	while (1) {
		int opt = getopt_long(argc, argv, "het:d:", opts, NULL);

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
			case 't':
				if (!is_number(optarg)) return error("-t or --threads needs a number as an argument!", 1);
				out->threads = std::stoi(optarg);
			case 'd':
				if (!is_number(optarg)) return error("-d or --depth needs a number as an argument!", 1);
				out->depth = std::stoi(optarg);
		}
	}
}
