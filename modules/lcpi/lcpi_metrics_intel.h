/*
 * Copyright (c) 2011-2013  University of Texas at Austin. All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * This file is part of PerfExpert.
 *
 * PerfExpert is free software: you can redistribute it and/or modify it under
 * the terms of the The University of Texas at Austin Research License
 *
 * PerfExpert is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.
 *
 * Authors: Leonardo Fialho and Ashay Rane
 *
 * $HEADER$
 */

#ifndef PERFEXPERT_MODULE_LCPI_METRICS_INTEL_H_
#define PERFEXPERT_MODULE_LCPI_METRICS_INTEL_H_

#define MAX_LCPI 1024

/* PerfExpert common headers */
#include "common/perfexpert_constants.h"

#define USE_EVENT(X)                                                   \
    if (PERFEXPERT_SUCCESS != my_module_globals.vtune->set_event(X)) { \
        return PERFEXPERT_ERROR;                                       \
    }

#define EVENT_AVAIL(X) my_module_globals.vtune->query_event(X)

/* Function declarations */
static int generate_ratio_floating_point(void);
static int generate_ratio_data_accesses(void);
static int generate_gflops(void);
static int generate_overall(void);
static int generate_data_accesses(void);
static int generate_instruction_accesses(void);
static int generate_tlb_metrics(void);
static int generate_branch_metrics(void);
static int generate_floating_point_instr(void);

#endif /* PERFEXPERT_MODULE_LCPI_METRICS_INTEL_H_ */
