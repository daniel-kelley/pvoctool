/*
   pvoctool.h

   Copyright (c) 2021 by Daniel Kelley

*/

#ifndef   PVOCTOOL_H_
#define   PVOCTOOL_H_

struct info {
    int verbose;
    int frame_count;
    int bin_count;
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
    const char *filename,
    struct pvoctool_data **data);
extern int pvoctool_free_data(struct pvoctool_data *data);

extern int pvoctool_octave_specgram(
    const struct info *info,
    const struct pvoctool_data *data,
    int argc,
    const char *argv[]);

extern int pvoctool_gnuplot(
    const struct info *info,
    const struct pvoctool_data *data,
    int argc,
    const char *argv[]);

#endif /* PVOCTOOL_H_ */
