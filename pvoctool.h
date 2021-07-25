/*
   pvoctool.h

   Copyright (c) 2021 by Daniel Kelley

*/

#ifndef   PVOCTOOL_H_
#define   PVOCTOOL_H_

#include <pvocf.h>

struct info {
    int verbose;
};

struct pvoc_channel {
    float *amp;             /* 2D(bin_count,frame_count) */
    float *freq;            /* 2D(bin_count,frame_count) */
};

struct pvoctool_data {
    int sample_rate;
    int frame_count;
    int bin_count;
    int channels;
    float *bin_freq;        /* 1D(bin_count) */
    float *frame_time;      /* 1D(frame_count) */
    struct pvoc_channel *channel;
};


extern int pvoctool_get_data(
    struct pvocf *handle,
    struct pvoctool_data **data);

extern int pvoctool_free_data(struct pvoctool_data *data);

extern int pvoctool_info(
    int argc,
    const char *argv[],
    const struct info *info);

extern int pvoctool_hdf5(
    int argc,
    const char *argv[],
    const struct info *info);

extern int pvoctool_dlm(
    int argc,
    const char *argv[],
    const struct info *info);

#endif /* PVOCTOOL_H_ */
