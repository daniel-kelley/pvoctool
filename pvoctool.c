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
#include "pvocf.h"
#include "pvoctool.h"

int pvoc_free_data(struct pvoctool_data *data)
{
    free(data);

    return 0;
}

int pvoc_get_data(struct pvocf *handle, struct pvoctool_data **data)
{
    int err = 1;
    int frame_count = pvocf_frame_count(handle);
    int frame_size = pvocf_frame_size(handle);
    int channels;
    const struct pvoc_info *info = pvocf_get_info(handle);
    int bin_count = info->pvoc.nAnalysisBins;
    void *pool;
    void *pool_end;
    size_t pool_size;
    size_t channel_size;
    size_t mtx_size;
    size_t bin_freq_size;
    size_t frame_time_size;
    struct pvoctool_data *pvd = NULL;
    float *frame = NULL;
    float *f;
    int i;
    int j;
    float v;
    float bin_freq_incr = 0;
    float frame_time_incr = 0;

    do {
        if (frame_count < 0) {
            /* Error getting frame count. */
            break;
        }

        if (frame_size < 0) {
            /* Error getting frame size. */
            break;
        }

        channels = info->fmt.nChannels;

        pool_size = sizeof(struct pvoctool_data);

        /* amp, freq 2D matrix */
        mtx_size = bin_count * frame_count * sizeof(float);
        /* bin_freq 1D array */
        bin_freq_size = bin_count * sizeof(float);
        /* frame_time 1D array */
        frame_time_size = frame_count * sizeof(float);
        pool_size += bin_freq_size;
        pool_size += frame_time_size;

        /* element of channel array */
        channel_size = sizeof(struct pvoc_channel);
        /* amp,freq mtx per channel */
        channel_size += 2*mtx_size;

        pool_size += channel_size * channels;

        pool = calloc(1, pool_size);
        if (!pool) {
            break;
        }
        pool_end = pool + pool_size;

        pvd = (struct pvoctool_data *)pool;

        frame = calloc(frame_count, frame_size);
        if (!frame) {
            break;
        }

        err = pvocf_get_frame(handle,
                              0,
                              frame_count,
                              frame_count * frame_size,
                              frame);
        if (err) {
            break;
        }


        pvd->sample_rate = info->fmt.nSamplesPerSec;
        pvd->frame_count = frame_count;
        pvd->bin_count = bin_count;
        pvd->channels = channels;
        pool += sizeof(struct pvoctool_data);
        assert(pool < pool_end);

        bin_freq_incr =
            (((float)info->fmt.nSamplesPerSec)/2.0)/((float)bin_count-1);
        frame_time_incr =
            ((float)info->pvoc.dwOverlap)/((float)info->fmt.nSamplesPerSec);

        pvd->bin_freq = (float *)pool;
        pool += bin_freq_size;
        assert(pool < pool_end);

        pvd->frame_time = (float *)pool;
        pool += frame_time_size;
        assert(pool < pool_end);

        pvd->channel = (struct pvoc_channel *)pool;
        pool += sizeof(struct pvoc_channel) * channels;
        assert(pool < pool_end);

        /* Allocate buffers from pool */
        for (i=0; i<channels; i++) {
            pvd->channel[i].amp = (float *)pool;
            pool += mtx_size;
            assert(pool < pool_end);

            pvd->channel[i].freq = (float *)pool;
            pool += mtx_size;
            assert(pool <= pool_end);
        }
        assert(pool == pool_end);

        /* fill out amp,freq matricies */
        f = frame;
        assert((int)(bin_count*channels*sizeof(float)*2) == frame_size);

        for (i=0; i<(bin_count*frame_count); i++) {
            for (j=0; j<channels; j++) {
                float amp = *f++;
                float freq = *f++;

                assert(i < (int)(mtx_size/sizeof(float)));
                pvd->channel[j].amp[i] = amp;
                pvd->channel[j].freq[i] = freq;
            }
        }
        /* fill out bin_freq */
        v = 0.0;
        for (i=0; i<bin_count; i++) {
            pvd->bin_freq[i] = v;
            v += bin_freq_incr;
        }

        /* fill out frame_time */
        v = 0.0;
        for (i=0; i<frame_count; i++) {
            pvd->frame_time[i] = v;
            v += frame_time_incr;
        }

    } while (0);

    if (err) {
        if (pvd) {
            free(pvd);
        }
    } else {
        *data = pvd;
    }

    if (frame) {
        free(frame);
    }

    return err;
}

static int pvocf_info(const char *file)
{
    int err = 1;
    struct pvocf *handle = pvocf_open(file);
    if (handle) {
        struct pvoctool_data *data;
        err = pvoc_get_data(handle, &data);
        if (!err) {
            printf("sample_rate: %d\n", data->sample_rate);
            printf("frame_count: %d\n", data->frame_count);
            printf("bin_count: %d\n", data->bin_count);
            printf("channels: %d\n", data->channels);
            err = pvoc_free_data(data);
        }
    } else {
        fprintf(stderr, "%s: error\n", file);
    }
    pvocf_close(handle);

    return err;
}

static void usage(const char *prog)
{
    fprintf(stderr,"%s -f n [-vh] <input>\n", prog);
    fprintf(stderr,"  -h        Print this message\n");
    fprintf(stderr,"  -n n      Start with analysis frame n\n");
    fprintf(stderr,"  -a n      Show n analysis frames\n");
    fprintf(stderr,"  -b n      Show first n analysis bins\n");
    fprintf(stderr,"  -A        Show analysis frames\n");
    fprintf(stderr,"  -F        Use %%.3f for analysis float format\n");
    fprintf(stderr,"  -v        Verbose\n");
}

int main(int argc, char *argv[])
{
    int err = 1;
    int i;
    int c;
    struct info info;

    memset(&info, 0, sizeof(info));
    while ((c = getopt(argc, argv, "a:n:b:AFvh")) != EOF) {
        switch (c) {
        case 'a':
            info.frame_count = strtoul(optarg, NULL, 0);
            break;
        case 'b':
            info.bin_count = strtoul(optarg, NULL, 0);
            break;
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

    for (i=optind; i<argc; i++) {
        err = pvocf_info(argv[i]);
        if (err) {
            break;
        }
    }

    return err;
}
