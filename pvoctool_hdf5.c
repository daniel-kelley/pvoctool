/*
   pvoctool_hdf5.c

   Copyright (c) 2021 by Daniel Kelley

   Convert PVOC data to hdf5 file


|--------------------+-----------------+------------|
| path               | type            | dataspace  |
|--------------------+-----------------+------------|
| /pvoc/sample_rate  | int             | scalar     |
| /pvoc/frame_count  | int             | scalar M   |
| /pvoc/bin_count    | int             | scalar N   |
| /pvoc/bin_freq     | float           | N vector   |
| /pvoc/frame_time   | float           | M vector   |
| /pvoc/channels     | int             | scalar     |
| /pvoc/channel      | composite array | composite  |
| /pvoc/channel/amp  | float           | NxM matrix |
| /pvoc/channel/freq | float           | NxM matrix |


*/

#include <stdio.h>
#include <hdf5.h>
#include <hdf5_hl.h>
#include "pvoctool.h"

static int pvoctool_hdf5_create(const char *file,
                                const struct pvoctool_data *data)
{
    int err = -1;
    hid_t fid;
    herr_t herr;
    hsize_t dim[2];

    do {
        fid = H5Fcreate(file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        if (fid < 0) {
            break;
        }

        herr = H5LTmake_dataset(fid,
                                "sample_rate",
                                0,
                                NULL,
                                H5T_NATIVE_INT,
                                &data->sample_rate);

        if (herr < 0) {
            break;
        }

        herr = H5LTmake_dataset(fid,
                                "frame_count",
                                0,
                                NULL,
                                H5T_NATIVE_INT,
                                &data->frame_count);

        if (herr < 0) {
            break;
        }

        herr = H5LTmake_dataset(fid,
                                "bin_count",
                                0,
                                NULL,
                                H5T_NATIVE_INT,
                                &data->bin_count);

        if (herr < 0) {
            break;
        }

        herr = H5LTmake_dataset(fid,
                                "channels",
                                0,
                                NULL,
                                H5T_NATIVE_INT,
                                &data->channels);

        if (herr < 0) {
            break;
        }

        dim[0] = data->bin_count;
        herr = H5LTmake_dataset(fid,
                                "bin_freq",
                                1,
                                dim,
                                H5T_NATIVE_FLOAT,
                                data->bin_freq);

        if (herr < 0) {
            break;
        }

        dim[0] = data->frame_count;
        herr = H5LTmake_dataset(fid,
                                "frame_time",
                                1,
                                dim,
                                H5T_NATIVE_FLOAT,
                                data->frame_time);

        if (herr < 0) {
            break;
        }

        dim[0] = data->frame_count;
        dim[1] = data->bin_count;
        herr = H5LTmake_dataset(fid,
                                "freq",
                                2,
                                dim,
                                H5T_NATIVE_FLOAT,
                                data->channel[0].freq);

        if (herr < 0) {
            break;
        }

        herr = H5LTmake_dataset(fid,
                                "amp",
                                2,
                                dim,
                                H5T_NATIVE_FLOAT,
                                data->channel[0].amp);

        if (herr < 0) {
            break;
        }

        H5Fclose(fid);
        (void)data;
        err = 0;
    } while (0);

    return err;
}


int pvoctool_hdf5(int argc, char *argv[], const struct info *info)
{
    int err = 1;
    struct pvocf *handle;
    struct pvoctool_data *data;
    const char *in_file;
    const char *out_file;

    (void)info;
    do {
        if (argc != 2) {
            fprintf(stderr, "usage: ...\n");
            break;
        }

        in_file = argv[0];
        out_file = argv[1];

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

        err = pvoctool_hdf5_create(out_file, data);

        (void)pvoctool_free_data(data);

        pvocf_close(handle);
    } while (0);

    return err;
}
