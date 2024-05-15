#include "front.h"
#include "arena.h"
#include "lex.yy.h"
#include "lexer.h"
#include "parser.h"
#include "shell.h"
#include "xalloc.h"

#include <errno.h>
#include <getopt.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <unistd.h>

char *prompt = 0;
extern int echo, parse_error; // from the parser

void free_the_parser(void *parser)
{
    ParseFree(parser, free);
}
void my_yy_delete_buffer(void *st)
{
    yy_delete_buffer((YY_BUFFER_STATE)st);
}

static void handle_command(char *cmd)
{
    void *parser;
    int yv;
    struct lex_token tok;
    YY_BUFFER_STATE st;

    // prepare a parser context.
    parser = ParseAlloc(malloc);
    arena_register_mem(parser, &free_the_parser);
    parse_error = 0;

    // prepare a lexer context.
    st = yy_scan_string(cmd);
    arena_register_mem(st, &my_yy_delete_buffer);

    // while there are some lexing tokens...
    while ((yv = yylex()) != 0) {
        tok.text = 0;
        tok.number = -1;

        // NUMBER and WORD are the only 2 token types
        // with a carried value.
        if (yv == NUMBER || yv == WORD) {
            tok.text = xstrdup(token_text);
            if (yv == NUMBER) tok.number = atoi(tok.text);
        }

        // process the token in the parser.
        Parse(parser, yv, tok);

        // if at end, finish the parsing.
        if (yv == END) break;
    }

    // complete parse.
    Parse(parser, 0, tok);
}

void my_yylex_destroy(void)
{
    yylex_destroy();
}

void test_func(void)
{
    printf("malloc: %p, calloc: %p\n", arena_malloc(2, sizeof(char)),
           arena_calloc(1, sizeof(int)));
}

int main(int argc, char *argv[])
{
    int opt;
    int save_history = 0;

    arena_push();
    atexit(&arena_pop_all);

    // command-line argument parsing.

    while ((opt = getopt(argc, argv, "hec:")) != -1) {
        switch (opt) {
            case 'h':
                printf("usage: %s [OPTS] [FILE]\n"
                       "options:\n"
                       " -h      print this help.\n"
                       " -e      echo commands before running them.\n"
                       " -c CMD  run this command then exit.\n"
                       " FILE    read commands from FILE.\n",
                       argv[0]);
                return EXIT_SUCCESS;

            case 'e': echo = 1; break;

            case 'c':
                initialize();
                handle_command(optarg);
                return 0;
        }
    }

    // reading commands from a script or stdin.

    if (optind >= argc) {
        // reading from stdin; handle history if terminal.

        if (isatty(0)) {
            using_history();
            read_history(0);
            prompt = "42sh$ ";
            save_history = 1;
        }
    } else {
        // reading from file.

        FILE *f = fopen(argv[optind], "r");
        if (f == 0) {
            perror(argv[optind]);
            exit(1);
        }
        rl_instream = f;
        prompt = 0;
    }

    // the main loop.

    initialize();
    char *line;
    while ((line = readline(prompt)) != 0) {
        arena_push();
        arena_register_mem(line, &free);
        if (save_history && line[0] != '\0') {
            add_history(line);
            write_history(0);
        }
        handle_command(line);
        arena_pop();
    }

    arena_pop();
    assert(arena_amount() == 0);

    return 0;
}
