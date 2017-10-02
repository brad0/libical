/*======================================================================
 FILE: copycluster.c
 CREATOR: eric 15 January 2000

 (C) COPYRIGHT 2000 Eric Busboom <eric@softwarestudio.org>
     http://www.softwarestudio.org

 This library is free software; you can redistribute it and/or modify
 it under the terms of either:

    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html

 Or:

    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libical/ical.h"
#include "libicalss/icalss.h"

#include <stdlib.h>

#if defined(HAVE_SIGNAL) && defined(HAVE_ALARM)
static void sig_alrm(int i)
{
    _unused(i);
    fprintf(stderr, "Could not get lock on file\n");
    exit(1);
}

#endif

/* This program copies a file that holds iCal components to an other file. */

void usage(char *arg0)
{
    printf("usage: %s cluster-file1 cluster-file2\n", arg0);
}

int main(int c, char *argv[])
{
    icalset *clusterin, *clusterout = NULL;
    icalcomponent *itr;
    int count = 0;
    int tostdout = 0;

    if (c < 2 || c > 3) {
        usage(argv[0]);
        exit(1);
    }

    if (c == 2) {
        tostdout = 1;
    }

    /*icalerror_set_error_state(ICAL_PARSE_ERROR, ICAL_ERROR_NONFATAL); */

#if defined(HAVE_SIGNAL) && defined(HAVE_ALARM)
    (void)signal(SIGALRM, sig_alrm);
    alarm(10);
#endif
    clusterin = icalfileset_new(argv[1]);
#if defined(HAVE_SIGNAL) && defined(HAVE_ALARM)
    alarm(0);
#endif
    if (clusterin == 0) {
        printf("Could not open input cluster \"%s\"\n", argv[1]);
        if (icalerrno != ICAL_NO_ERROR) {
            printf("Error: %s\n", icalerror_strerror(icalerrno));
        }
        exit(1);
    }

    if (!tostdout) {
#if defined(HAVE_SIGNAL) && defined(HAVE_ALARM)
        alarm(10);
#endif
        clusterout = icalfileset_new(argv[2]);
#if defined(HAVE_SIGNAL) && defined(HAVE_ALARM)
        alarm(0);
#endif
        if (clusterout == 0) {
            printf("Could not open output cluster \"%s\"\n", argv[2]);
            exit(1);
        }
    }

    for (itr = icalset_get_first_component(clusterin);
         itr != 0; itr = icalset_get_next_component(clusterin)) {

        icalerror_set_error_state(ICAL_BADARG_ERROR, ICAL_ERROR_NONFATAL);
        icalrestriction_check(itr);
        icalerror_set_error_state(ICAL_BADARG_ERROR, ICAL_ERROR_DEFAULT);

        if (tostdout) {
            printf("--------------\n%s\n", icalcomponent_as_ical_string(itr));
        } else {
            (void)icalfileset_add_component(clusterout, icalcomponent_new_clone(itr));
        }
        count++;
    }

    printf("Transferred %d components\n", count);

    icalset_free(clusterin);

    if (!tostdout) {
        icalfileset_mark(clusterout);
        icalset_free(clusterout);
    }

    return 0;
}
