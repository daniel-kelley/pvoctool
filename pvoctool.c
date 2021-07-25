/*
   pvoctool.c

   Copyright (c) 2021 by Daniel Kelley

   Manipulate CSound PVOC files
*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "pvoctool.h"

static struct {
    const char *name;
    int (*func)(int argc, const char *argv[], const struct info *info);
} cmd[] = {
    {"info", pvoctool_info},
    {"hdf5", pvoctool_hdf5},
    {"dlm",  pvoctool_dlm},
};

#define COUNT(a) ((int)(sizeof((a))/sizeof((a)[0])))

static void usage(const char *prog)
{
    int i;
    const char *argv[1];

    fprintf(stderr,"%s [-vh] <tool> <tool_args>\n", prog);
    fprintf(stderr,"  -h        Print this message\n");
    fprintf(stderr,"  -v        Verbose\n");
    fprintf(stderr,"tools:\n");
    for (i=0; i<COUNT(cmd); i++) {
        argv[0] = cmd[i].name;
        cmd[i].func(0, argv, NULL);
    }
}

static int run_command(int argc, const char *argv[], const struct info *info)
{
    int i;
    int err = 1;
    for (i=0; i<COUNT(cmd); i++) {
        if (!strcmp(argv[0], cmd[i].name)) {
            err = cmd[i].func(argc, argv, info);
            break;
        }
    }

    if (i == COUNT(cmd)) {
        assert(err);
        fprintf(stderr, "No tool '%s'.\n", argv[0]);
    }

    return err;
}

int main(int argc, char *argv[])
{
    int err = 1;
    int c;
    struct info info;

    setenv("POSIXLY_CORRECT","1",1);
    memset(&info, 0, sizeof(info));
    while ((c = getopt(argc, argv, "vh")) != EOF) {
        switch (c) {
        case 'v':
            info.verbose = 1;
            break;
        case 'h':
            usage(argv[0]);
            err = EXIT_SUCCESS;
            break;
        default:
            break;
        }
    }

    if (optind < argc) {
        err = run_command(argc-optind, (const char **)argv+optind,&info);
    }

    return err;
}
