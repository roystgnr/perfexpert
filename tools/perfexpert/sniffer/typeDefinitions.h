/*
 * Copyright (C) 2013 The University of Texas at Austin
 *
 * This file is part of PerfExpert.
 *
 * PerfExpert is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * PerfExpert is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with PerfExpert. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Ashay Rane
 */

#ifndef TYPE_DEFINITIONS_H_
#define TYPE_DEFINITIONS_H_

typedef struct {
    int PAPI_event_code;
    unsigned int sampling_freq;
    unsigned short use, sampling_category_id;
    char* PAPI_event_name;
} t_event_entry;

// Lower numbering implying greater importance
enum {
    SAMPLING_CATEGORY_01 = 0,
    SAMPLING_CATEGORY_02,
    SAMPLING_CATEGORY_03,
    SAMPLING_CATEGORY_04,
    SAMPLING_CATEGORY_COUNT
};

unsigned int sampling_limits[SAMPLING_CATEGORY_COUNT + 1] = {
    99999999,
    9999999,
    5000000,
    500000,
    100000
};

enum {
    TOT_INS = 0,
    TOT_CYC,
    LD_INS,
    L1_DCA,
    L2_DCA,
    L2_TCA,
    L1_ICA,
    L2_ICA,
    L2_DCM,
    L2_ICM,
    L2_TCM,
    TLB_DM,
    TLB_IM,
    BR_INS,
    BR_MSP,
    FP_INS,
    FML_INS,
    FDV_INS,
    FAD_INS,
    L1I_CYCLES_STALLED,
    ARITH_CYCLES_DIV_BUSY,
    FP_COMP_OPS_EXE_X87,
    FP_COMP_OPS_EXE_SSE_FP,
    FP_COMP_OPS_EXE_SSE_FP_PACKED,
    FP_COMP_OPS_EXE_SSE_FP_SCALAR,
    FP_COMP_OPS_EXE_SSE_SINGLE_PRECISION,
    FP_COMP_OPS_EXE_SSE_DOUBLE_PRECISION,
    RETIRED_SSE_OPERATIONS_ALL,
    RETIRED_SSE_OPS_ALL,
    SSEX_UOPS_RETIRED_PACKED_DOUBLE,
    SSEX_UOPS_RETIRED_PACKED_SINGLE,
    SSEX_UOPS_RETIRED_SCALAR_DOUBLE,
    SSEX_UOPS_RETIRED_SCALAR_SINGLE,
    SIMD_COMP_INST_RETIRED_PACKED_DOUBLE,
    SIMD_COMP_INST_RETIRED_PACKED_SINGLE,
    SIMD_COMP_INST_RETIRED_SCALAR_DOUBLE,
    SIMD_COMP_INST_RETIRED_SCALAR_SINGLE,
    DTLB_LOAD_MISSES_CAUSES_A_WALK,
    DTLB_STORE_MISSES_CAUSES_A_WALK,
    FP_COMP_OPS_EXE_SSE_FP_PACKED_DOUBLE,
    FP_COMP_OPS_EXE_SSE_FP_SCALAR_SINGLE,
    FP_COMP_OPS_EXE_SSE_PACKED_SINGLE,
    FP_COMP_OPS_EXE_SSE_SCALAR_DOUBLE,
    ICACHE,
    SIMD_FP_256_PACKED_SINGLE,
    SIMD_FP_256_PACKED_DOUBLE,
    ARITH,
    PURE_EVENT_COUNT
};

enum {
    // Dummy counters
    SSEX_UOPS_RETIRED_SCALAR_DOUBLE_SCALAR_SINGLE = PURE_EVENT_COUNT,
    SIMD_COMP_INST_RETIRED_SCALAR_SINGLE_SCALAR_DOUBLE,
    FP_COMP_OPS_EXE_SSE_DOUBLE_PRECISION_SSE_FP_SSE_FP_PACKED_SSE_FP_SCALAR_SSE_SINGLE_PRECISION_X87,
    FP_COMP_OPS_EXE_SSE_FP_SCALAR_SINGLE_SSE_SCALAR_DOUBLE,
    EVENT_COUNT
};

t_event_entry event_list[EVENT_COUNT] = {
    -1, 0, 0, SAMPLING_CATEGORY_01, "PAPI_TOT_INS",
    -1, 0, 0, SAMPLING_CATEGORY_01, "PAPI_TOT_CYC",
    -1, 0, 0, SAMPLING_CATEGORY_02, "PAPI_LD_INS",
    -1, 0, 0, SAMPLING_CATEGORY_02, "PAPI_L1_DCA",
    -1, 0, 0, SAMPLING_CATEGORY_03, "PAPI_L2_DCA",
    -1, 0, 0, SAMPLING_CATEGORY_03, "PAPI_L2_TCA",
    -1, 0, 0, SAMPLING_CATEGORY_02, "PAPI_L1_ICA",
    -1, 0, 0, SAMPLING_CATEGORY_03, "PAPI_L2_ICA",
    -1, 0, 0, SAMPLING_CATEGORY_03, "PAPI_L2_DCM",
    -1, 0, 0, SAMPLING_CATEGORY_03, "PAPI_L2_ICM",
    -1, 0, 0, SAMPLING_CATEGORY_03, "PAPI_L2_TCM",
    -1, 0, 0, SAMPLING_CATEGORY_04, "PAPI_TLB_DM",
    -1, 0, 0, SAMPLING_CATEGORY_04, "PAPI_TLB_IM",
    -1, 0, 0, SAMPLING_CATEGORY_03, "PAPI_BR_INS",
    -1, 0, 0, SAMPLING_CATEGORY_03, "PAPI_BR_MSP",
    -1, 0, 0, SAMPLING_CATEGORY_02, "PAPI_FP_INS",
    -1, 0, 0, SAMPLING_CATEGORY_02, "PAPI_FML_INS",
    -1, 0, 0, SAMPLING_CATEGORY_02, "PAPI_FDV_INS",
    -1, 0, 0, SAMPLING_CATEGORY_02, "PAPI_FAD_INS",
    -1, 0, 0, SAMPLING_CATEGORY_01, "L1I_CYCLES_STALLED",
    -1, 0, 0, SAMPLING_CATEGORY_02, "ARITH:CYCLES_DIV_BUSY",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:X87",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:SSE_FP",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:SSE_FP_PACKED",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:SSE_FP_SCALAR",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:SSE_SINGLE_PRECISION",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:SSE_DOUBLE_PRECISION",
    -1, 0, 0, SAMPLING_CATEGORY_02, "RETIRED_SSE_OPERATIONS:ALL",
    -1, 0, 0, SAMPLING_CATEGORY_02, "RETIRED_SSE_OPS:ALL",
    -1, 0, 0, SAMPLING_CATEGORY_02, "SSEX_UOPS_RETIRED:PACKED_DOUBLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "SSEX_UOPS_RETIRED:PACKED_SINGLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "SSEX_UOPS_RETIRED:SCALAR_DOUBLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "SSEX_UOPS_RETIRED:SCALAR_SINGLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "SIMD_COMP_INST_RETIRED:PACKED_DOUBLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "SIMD_COMP_INST_RETIRED:PACKED_SINGLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "SIMD_COMP_INST_RETIRED:SCALAR_DOUBLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "SIMD_COMP_INST_RETIRED:SCALAR_SINGLE",
    -1, 0, 0, SAMPLING_CATEGORY_04, "DTLB_LOAD_MISSES:CAUSES_A_WALK",
    -1, 0, 0, SAMPLING_CATEGORY_04, "DTLB_STORE_MISSES:CAUSES_A_WALK",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:SSE_FP_PACKED_DOUBLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:SSE_FP_SCALAR_SINGLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:SSE_PACKED_SINGLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:SSE_SCALAR_DOUBLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "ICACHE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "SIMD_FP_256:PACKED_SINGLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "SIMD_FP_256:PACKED_DOUBLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "ARITH",
    /* dummy counters ahead */
    -1, 0, 0, SAMPLING_CATEGORY_02, "SSEX_UOPS_RETIRED:SCALAR_DOUBLE:SCALAR_SINGLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "SIMD_COMP_INST_RETIRED:SCALAR_SINGLE:SCALAR_DOUBLE",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:SSE_DOUBLE_PRECISION:SSE_FP:SSE_FP_PACKED:SSE_FP_SCALAR:SSE_SINGLE_PRECISION:X87",
    -1, 0, 0, SAMPLING_CATEGORY_02, "FP_COMP_OPS_EXE:SSE_FP_SCALAR_SINGLE:SSE_SCALAR_DOUBLE"
};

#endif /* TYPE_DEFINITIONS_H */
