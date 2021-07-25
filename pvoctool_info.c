/*
   pvoctool_info.c

   Copyright (c) 2021 by Daniel Kelley

   Show PVOC basic info
*/

#include <stdio.h>
#include "pvoctool.h"

int pvoctool_info(int argc, const char *argv[], const struct info *info)
{
    int err = 1;
    struct pvocf *handle;
    struct pvoctool_data *data;
    const char *file;

    (void)info;
    do {
        if (argc != 2) {
            fprintf(stderr, "%s: <pvoc_file>\n", argv[0]);
            fprintf(stderr, "    Print basic header information.\n");
            break;
        }
        file = argv[1];
        handle = pvocf_open(file);

        if (!handle) {
            fprintf(stderr, "could not open %s\n", file);
            break;
        }

        err = pvoctool_get_data(handle, &data);

        if (err) {
            fprintf(stderr, "%s: error\n", file);
            break;
        }

        printf("sample_rate: %d\n", data->sample_rate);
        printf("frame_count: %d\n", data->frame_count);
        printf("bin_count: %d\n", data->bin_count);
        printf("channels: %d\n", data->channels);
        err = pvoctool_free_data(data);

        pvocf_close(handle);
    } while (0);

    return err;
}
