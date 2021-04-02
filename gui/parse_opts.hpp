#pragma once

struct opts {
		int help;
		int engine;
};

extern void parse_opts(int argc, char** argv, struct opts* out);
extern void print_help(char* command);
extern void usage(char* command, int exit_code);
