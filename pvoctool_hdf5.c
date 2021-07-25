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
    hid_t fid = -1;
    herr_t herr;
    hsize_t dim[2];

    do {
        fid = H5Fcreate(file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        if (fid < 0) {
            H5Eprint(H5E_DEFAULT, stderr);
            break;
        }

        herr = H5LTmake_dataset(fid,
                                "sample_rate",
                                0,
                                NULL,
                                H5T_NATIVE_INT,
                                &data->sample_rate);

        if (herr < 0) {
            H5Eprint(H5E_DEFAULT, stderr);
            break;
        }

        herr = H5LTmake_dataset(fid,
                                "frame_count",
                                0,
                                NULL,
                                H5T_NATIVE_INT,
                                &data->frame_count);

        if (herr < 0) {
            H5Eprint(H5E_DEFAULT, stderr);
            break;
        }

        herr = H5LTmake_dataset(fid,
                                "bin_count",
                                0,
                                NULL,
                                H5T_NATIVE_INT,
                                &data->bin_count);

        if (herr < 0) {
            H5Eprint(H5E_DEFAULT, stderr);
            break;
        }

        herr = H5LTmake_dataset(fid,
                                "channels",
                                0,
                                NULL,
                                H5T_NATIVE_INT,
                                &data->channels);

        if (herr < 0) {
            H5Eprint(H5E_DEFAULT, stderr);
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
            H5Eprint(H5E_DEFAULT, stderr);
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
            H5Eprint(H5E_DEFAULT, stderr);
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
            H5Eprint(H5E_DEFAULT, stderr);
            break;
        }

        herr = H5LTmake_dataset(fid,
                                "amp",
                                2,
                                dim,
                                H5T_NATIVE_FLOAT,
                                data->channel[0].amp);

        if (herr < 0) {
            H5Eprint(H5E_DEFAULT, stderr);
            break;
        }

        err = 0;
    } while (0);

    if (fid >= 0) {
        H5Fclose(fid);
    }

    return err;
}


int pvoctool_hdf5(int argc, const char *argv[], const struct info *info)
{
    int err = 1;
    struct pvocf *handle = NULL;
    struct pvoctool_data *data = NULL;
    const char *in_file;
    const char *out_file;

    (void)info;
    do {
        if (argc != 3) {
            fprintf(stderr, "%s: <pvoc_file> <hdf5_file>\n", argv[0]);
            fprintf(stderr, "    Convert to an HDF5 format file.\n");
            break;
        }

        in_file = argv[1];
        out_file = argv[2];

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

    } while (0);

    if (handle) {
        pvocf_close(handle);
    }

    if (data) {
        (void)pvoctool_free_data(data);
    }


    return err;
}
