/*
   pvoctool_dlm.c

   Copyright (c) 2021 by Daniel Kelley

   Convert PVOC data to comma delimited test files
   suitable for GNU Octave dlmread.


|----------------------+-------+------------|
| path                 | type  | dataspace  |
|----------------------+-------+------------|
| <outdir>/sample_rate | int   | scalar     |
| <outdir>/frame_count | int   | scalar M   |
| <outdir>/bin_count   | int   | scalar N   |
| <outdir>/bin_freq    | float | N vector   |
| <outdir>/frame_time  | float | M vector   |
| <outdir>/channels    | int   | scalar     |
| <outdir>/amp         | float | NxM matrix |
| <outdir>/freq        | float | NxM matrix |

*/

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "pvoctool.h"

static int pvoctool_dlm_mkdir(const char *dir)
{
    int err = -1;

    do {
        err = access(dir, F_OK);
        if (err) {
            err = mkdir(dir, 0777);
            if (err) {
                break;
            }
        }
        err = chdir(dir);
    } while (0);

    return err;
}

static int pvoctool_dlm_mkscalar_int(const char *file, int value)
{
    int err = -1;
    FILE *f;

    do {
        f = fopen(file, "w+");
        if (f == NULL) {
            break;
        }
        fprintf(f, "%d\n", value);
        fclose(f);
        err = 0;
    } while (0);

    return err;
}


static int pvoctool_dlm_mk1darray_float(const char *file,
                                        int columns,
                                        float *value)
{
    int err = -1;
    FILE *f;
    int i;

    do {
        f = fopen(file, "w+");
        if (f == NULL) {
            break;
        }
        for (i=0; i<columns; i++) {
            fprintf(f, "%f%s", *value++, i<(columns-1)?",":"");
        }
        fprintf(f, "\n");
        fclose(f);
        err = 0;
    } while (0);

    return err;
}

static int pvoctool_dlm_mk2darray_float(const char *file,
                                        int rows,
                                        int columns,
                                        float *value)
{
    int err = -1;
    FILE *f;
    int i;
    int j;

    do {
        f = fopen(file, "w+");
        if (f == NULL) {
            break;
        }
        for (i=0; i<rows; i++) {
            for (j=0; j<columns; j++) {
                fprintf(f, "%f%s", *value++, j<(columns-1)?",":"");
            }
            fprintf(f, "\n");
        }
        fclose(f);
        err = 0;
    } while (0);

    return err;
}


static int pvoctool_dlm_create(const char *dir,
                               const struct pvoctool_data *data)
{
    int err = -1;

    do {
        err = pvoctool_dlm_mkdir(dir);
        if (err) {
            break;
        }

        err = pvoctool_dlm_mkscalar_int("sample_rate", data->sample_rate);
        if (err) {
            break;
        }

        err = pvoctool_dlm_mkscalar_int("frame_count", data->frame_count);
        if (err) {
            break;
        }

        err = pvoctool_dlm_mkscalar_int("bin_count",data->bin_count);

        if (err) {
            break;
        }

        err = pvoctool_dlm_mkscalar_int("channels",data->channels);

        if (err) {
            break;
        }

        err = pvoctool_dlm_mk1darray_float("bin_freq",
                                            data->bin_count,
                                            data->bin_freq);

        if (err) {
            break;
        }

        err = pvoctool_dlm_mk1darray_float("frame_time",
                                            data->frame_count,
                                            data->frame_time);

        if (err) {
            break;
        }

        err = pvoctool_dlm_mk2darray_float("freq",
                                data->frame_count,
                                data->bin_count,
                                data->channel[0].freq);

        if (err) {
            break;
        }

        err = pvoctool_dlm_mk2darray_float("amp",
                                data->frame_count,
                                data->bin_count,
                                data->channel[0].amp);

    } while (0);

    return err;
}


int pvoctool_dlm(int argc, char *argv[], const struct info *info)
{
    int err = 1;
    struct pvocf *handle;
    struct pvoctool_data *data;
    const char *in_file;
    const char *out_dir;

    (void)info;
    do {
        if (argc != 2) {
            fprintf(stderr, "usage: ...\n");
            break;
        }

        in_file = argv[0];
        out_dir = argv[1];

        handle = pvocf_open(in_file);

        if (!handle) {
            fprintf(stderr, "%s: could not open\n", in_file);
            break;
        }

        err = pvoctool_get_data(handle, &data);

        if (err) {
            fprintf(stderr, "%s: error\n", in_file);
            break;
        }

        err = pvoctool_dlm_create(out_dir, data);

        (void)pvoctool_free_data(data);

        pvocf_close(handle);
    } while (0);

    return err;
}
