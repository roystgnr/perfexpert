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

#ifdef __cplusplus
extern "C" {
#endif

/* System standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <dirent.h>

/* Tools headers */
#include "perfexpert.h"
#include "perfexpert_types.h"
#include "perfexpert_options.h"

/* Modules headers */
#include "modules/perfexpert_module_base.h"

/* PerfExpert common headers */
#include "common/perfexpert_alloc.h"
#include "common/perfexpert_constants.h"
#include "common/perfexpert_list.h"
#include "common/perfexpert_output.h"
#include "common/perfexpert_string.h"
#include "common/perfexpert_util.h"
#include "install_dirs.h"

static struct argp argp = { options, parse_options, args_doc, doc };
static arg_options_t arg_options = { 0 };

/* parse_cli_params */
int parse_cli_params(int argc, char *argv[]) {
    int i = 0, k = 0;

    /* perfexpert_run_exp compatibility */
    if (0 == strcmp("perfexpert_run_exp", argv[0])) {
        OUTPUT(("%s running PerfExpert in compatibility mode",
            _BOLDRED("WARNING:")));
        OUTPUT((""));
        globals.compat_mode = PERFEXPERT_TRUE;
    }

    /* Set default values for globals */
    arg_options = (arg_options_t) {
        .modules           = NULL,
        .program           = NULL,
        .program_argv      = NULL,
        .program_argv_temp = { 0 },
        .prefix            = NULL,
        .before            = NULL,
        .after             = NULL,
        .knc_prefix        = NULL,
        .knc_before        = NULL,
        .knc_after         = NULL,
        .do_not_run        = PERFEXPERT_FALSE
    };

    /* If some environment variable is defined, use it! */
    if (PERFEXPERT_SUCCESS != parse_env_vars()) {
        OUTPUT(("%s", _ERROR("Error: parsing environment variables")));
        return PERFEXPERT_ERROR;
    }

    /* Parse arguments */
    argp_parse(&argp, argc, argv, 0, 0, &globals);

    /* If the list of modules is empty, load the default modules */
    if ((0 == perfexpert_list_get_size(&(module_globals.modules))) &&
        (0 == perfexpert_list_get_size(&(module_globals.compile))) &&
        (0 == perfexpert_list_get_size(&(module_globals.measurements))) &&
        (0 == perfexpert_list_get_size(&(module_globals.analysis)))) {
        OUTPUT_VERBOSE((5, "no modules set, loading default modules"));
        if (PERFEXPERT_SUCCESS != perfexpert_module_load("lcpi")) {
            OUTPUT(("%s", _ERROR("Error: unable to load default modules")));
        }
        if (PERFEXPERT_SUCCESS != perfexpert_module_load("hpctoolkit")) {
            OUTPUT(("%s", _ERROR("Error: unable to load default modules")));
        }
    }

    /* Expand AFTERs, BEFOREs, PREFIXs and program arguments */
    if (NULL != arg_options.after) {
        perfexpert_string_split(perfexpert_string_remove_spaces(
            arg_options.after), globals.after, ' ');
    }
    if (NULL != arg_options.before) {
        perfexpert_string_split(perfexpert_string_remove_spaces(
            arg_options.before), globals.before, ' ');
    }
    if (NULL != arg_options.prefix) {
        perfexpert_string_split(perfexpert_string_remove_spaces(
            arg_options.prefix), globals.prefix, ' ');
    }
    if (NULL != arg_options.knc_after) {
        perfexpert_string_split(perfexpert_string_remove_spaces(
            arg_options.knc_after), globals.knc_after, ' ');
    }
    if (NULL != arg_options.knc_before) {
        perfexpert_string_split(perfexpert_string_remove_spaces(
            arg_options.knc_before), globals.knc_before, ' ');
    }
    if (NULL != arg_options.knc_prefix) {
        perfexpert_string_split(perfexpert_string_remove_spaces(
            arg_options.knc_prefix), globals.knc_prefix, ' ');
    }
    while (NULL != arg_options.program_argv[i]) {
        int j = 0;

        perfexpert_string_split(perfexpert_string_remove_spaces(
            arg_options.program_argv[i]), arg_options.program_argv_temp, ' ');
        while (NULL != arg_options.program_argv_temp[j]) {
            globals.program_argv[k] = arg_options.program_argv_temp[j];
            j++;
            k++;
        }
        i++;
    }

    /* Sanity check: verbose level should be between 1-10 */
    if ((0 > globals.verbose) || (10 < globals.verbose)) {
        OUTPUT(("%s", _ERROR("Error: invalid verbose level")));
        return PERFEXPERT_ERROR;
    }

    /* Sanity check: NULL program */
    if (NULL != arg_options.program) {
        if (PERFEXPERT_SUCCESS != perfexpert_util_filename_only(
            arg_options.program, &(globals.program))) {
            OUTPUT(("%s", _ERROR("Error: unable to extract program name")));
            return PERFEXPERT_ERROR;
        }
        OUTPUT_VERBOSE((1, "   program only=[%s]", globals.program));

        if (PERFEXPERT_SUCCESS != perfexpert_util_path_only(arg_options.program,
            &(globals.program_path))) {
            OUTPUT(("%s", _ERROR("Error: unable to extract program path")));
            return PERFEXPERT_ERROR;
        }
        OUTPUT_VERBOSE((1, "   program path=[%s]", globals.program_path));

        PERFEXPERT_ALLOC(char, globals.program_full,
            (strlen(globals.program) + strlen(globals.program_path) + 2));
        sprintf(globals.program_full, "%s/%s", globals.program_path,
            globals.program);
        OUTPUT_VERBOSE((1, "   program full path=[%s]", globals.program_full));

        if ((NULL == globals.sourcefile) && (NULL == globals.target) &&
            (PERFEXPERT_SUCCESS != perfexpert_util_file_is_exec(
                globals.program_full))) {
            OUTPUT(("%s (%s)", _ERROR("Error: unable to find program"),
                globals.program_full));
            return PERFEXPERT_ERROR;
        }
    } else {
        OUTPUT(("%s", _ERROR("Error: undefined program")));
        return PERFEXPERT_ERROR;
    }

    /* Sanity check: target and sourcefile at the same time */
    if ((NULL != globals.target) && (NULL != globals.sourcefile)) {
        OUTPUT(("%s", _ERROR("Error: target and sourcefile are both defined")));
        return PERFEXPERT_ERROR;
    }

    /* Sanity check: MIC options without MIC */
    if ((NULL != globals.knc_prefix[0]) && (NULL == globals.knc)) {
        OUTPUT(("%s option -P selected but no MIC card was specified, ignoring",
            _BOLDRED("WARNING:")));
    }

    /* Sanity check: MIC options without MIC */
    if ((NULL != globals.knc_before[0]) && (NULL == globals.knc)) {
        OUTPUT(("%s option -B selected but no MIC card was specified, ignoring",
            _BOLDRED("WARNING:")));
    }

    /* Sanity check: MIC options without MIC */
    if ((NULL != globals.knc_after[0]) && (NULL == globals.knc)) {
        OUTPUT(("%s option -A selected but no MIC card was specified, ignoring",
            _BOLDRED("WARNING:")));
    }

    OUTPUT_VERBOSE((7, "%s", _BLUE("Summary of options")));
    OUTPUT_VERBOSE((7, "   Verbose level:       %d", globals.verbose));
    OUTPUT_VERBOSE((7, "   Colorful verbose?    %s", globals.colorful ?
        "yes" : "no"));
    OUTPUT_VERBOSE((7, "   Leave garbage?       %s", globals.leave_garbage ?
        "yes" : "no"));
    OUTPUT_VERBOSE((7, "   Only experiments?    %s", globals.only_exp ?
        "yes" : "no"));
    OUTPUT_VERBOSE((7, "   Database file:       %s", globals.dbfile));
    OUTPUT_VERBOSE((7, "   Recommendations      %d", globals.rec_count));
    OUTPUT_VERBOSE((7, "   Make target:         %s", globals.target));
    OUTPUT_VERBOSE((7, "   Program source file: %s", globals.sourcefile));
    OUTPUT_VERBOSE((7, "   Program executable:  %s", globals.program));
    OUTPUT_VERBOSE((7, "   Program path:        %s", globals.program_path));
    OUTPUT_VERBOSE((7, "   Program full path:   %s", globals.program_full));
    OUTPUT_VERBOSE((7, "   Program input file:  %s", globals.inputfile));

    if (7 <= globals.verbose) {
        printf("%s    Program arguments:  ", PROGRAM_PREFIX);
        if (NULL == globals.program_argv[0]) {
            printf(" (null)");
        } else {
            i = 0;
            while (NULL != globals.program_argv[i]) {
                printf(" [%s]", globals.program_argv[i]);
                i++;
            }
        }

        printf("\n%s    Prefix:             ", PROGRAM_PREFIX);
        if (NULL == globals.prefix[0]) {
            printf(" (null)");
        } else {
            i = 0;
            while (NULL != globals.prefix[i]) {
                printf(" [%s]", (char *)globals.prefix[i]);
                i++;
            }
        }

        printf("\n%s    Before each run:    ", PROGRAM_PREFIX);
        if (NULL == globals.before[0]) {
            printf(" (null)");
        } else {
            i = 0;
            while (NULL != globals.before[i]) {
                printf(" [%s]", (char *)globals.before[i]);
                i++;
            }
        }

        printf("\n%s    After each run:     ", PROGRAM_PREFIX);
        if (NULL == globals.after[0]) {
            printf(" (null)");
        } else {
            i = 0;
            while (NULL != globals.after[i]) {
                printf(" [%s]", (char *)globals.after[i]);
                i++;
            }
        }
        printf("\n");
        fflush(stdout);
    }

    #if HAVE_KNC_SUPPORT
    OUTPUT_VERBOSE((7, "   MIC card:            %s", globals.knc));

    if (7 <= globals.verbose) {
        printf("%s    MIC prefix:         ", PROGRAM_PREFIX);
        if (NULL == globals.knc_prefix[0]) {
            printf(" (null)");
        } else {
            i = 0;
            while (NULL != globals.knc_prefix[i]) {
                printf(" [%s]", (char *)globals.knc_prefix[i]);
                i++;
            }
        }

        printf("\n%s    MIC before each run:", PROGRAM_PREFIX);
        if (NULL == globals.knc_before[0]) {
            printf(" (null)");
        } else {
            i = 0;
            while (NULL != globals.knc_before[i]) {
                printf(" [%s]", (char *)globals.knc_before[i]);
                i++;
            }
        }

        printf("\n%s    MIC after each run: ", PROGRAM_PREFIX);
        if (NULL == globals.knc_after[0]) {
            printf(" (null)");
        } else {
            i = 0;
            while (NULL != globals.knc_after[i]) {
                printf(" [%s]", (char *)globals.knc_after[i]);
                i++;
            }
        }
        printf("\n");
        fflush(stdout);
    }
    #endif

    OUTPUT_VERBOSE((7, "   Sorting order:       %s", globals.order));

    if (7 <= globals.verbose) {
        perfexpert_module_t *module = NULL;
        perfexpert_ordered_module_t *ordered_module = NULL;

        printf("%s    Modules:            ", PROGRAM_PREFIX);
        module = (perfexpert_module_t *)perfexpert_list_get_first(
            &(module_globals.modules));
        while ((perfexpert_list_item_t *)module !=
            &(module_globals.modules.sentinel)) {
            printf(" [%s]", module->name);
            module = (perfexpert_module_t *)perfexpert_list_get_next(module);
        }
        printf("\n%s    Compile order:      ", PROGRAM_PREFIX);
        ordered_module = (perfexpert_ordered_module_t *)
            perfexpert_list_get_first(&(module_globals.compile));
        while ((perfexpert_list_item_t *)ordered_module !=
            &(module_globals.compile.sentinel)) {
            printf(" [%s]", ordered_module->name);
            ordered_module = (perfexpert_ordered_module_t *)
                perfexpert_list_get_next(ordered_module);
        }
        if (0 == perfexpert_list_get_size(&(module_globals.compile))) {
            printf(" undefined order");
        }
        printf("\n%s    Measurements order: ", PROGRAM_PREFIX);
        ordered_module = (perfexpert_ordered_module_t *)
            perfexpert_list_get_first(&(module_globals.measurements));
        while ((perfexpert_list_item_t *)ordered_module !=
            &(module_globals.measurements.sentinel)) {
            printf(" [%s]", ordered_module->name);
            ordered_module = (perfexpert_ordered_module_t *)
                perfexpert_list_get_next(ordered_module);
        }
        if (0 == perfexpert_list_get_size(&(module_globals.measurements))) {
            printf(" undefined order");
        }
        printf("\n%s    Analysis order:     ", PROGRAM_PREFIX);
        ordered_module = (perfexpert_ordered_module_t *)
            perfexpert_list_get_first(&(module_globals.analysis));
        while ((perfexpert_list_item_t *)ordered_module !=
            &(module_globals.analysis.sentinel)) {
            printf(" [%s]", ordered_module->name);
            ordered_module = (perfexpert_ordered_module_t *)
                perfexpert_list_get_next(ordered_module);
        }
        if (0 == perfexpert_list_get_size(&(module_globals.analysis))) {
            printf(" undefined order");
        }
        printf("\n");
        fflush(stdout);
    }

    /* Not using OUTPUT_VERBOSE because I want only one line */
    if (8 <= globals.verbose) {
        i = 0;
        printf("%s    %s", PROGRAM_PREFIX, _YELLOW("command line:"));
        for (i = 0; i < argc; i++) {
            printf(" [%s]", argv[i]);
        }
        printf("\n");
        fflush(stdout);
    }

    if (PERFEXPERT_TRUE == arg_options.do_not_run) {
        exit(0);
    }
    return PERFEXPERT_SUCCESS;
}

/* parse_options */
static error_t parse_options(int key, char *arg, struct argp_state *state) {
    switch (key) {
        /* Should I run some program after each execution? */
        case 'a':
            arg_options.after = arg;
            OUTPUT_VERBOSE((1, "option 'a' set [%s]", arg_options.after));
            break;

        /* Should I run on the KNC some program after each execution? */
        case 'A':
            arg_options.knc_after = arg;
            OUTPUT_VERBOSE((1, "option 'A' set [%s]", arg_options.knc_after));
            break;

        /* Should I run some program before each execution? */
        case 'b':
            arg_options.before = arg;
            OUTPUT_VERBOSE((1, "option 'b' set [%s]", arg_options.before));
            break;

        /* Should I run on the KNC some program before each execution? */
        case 'B':
            arg_options.knc_before = arg;
            OUTPUT_VERBOSE((1, "option 'B' set [%s]", arg_options.knc_before));
            break;

        /* Activate colorful mode */
        case 'c':
            globals.colorful = PERFEXPERT_TRUE;
            OUTPUT_VERBOSE((1, "option 'c' set"));
            break;

        /* MIC card */
        case 'C':
            globals.knc = arg;
            OUTPUT_VERBOSE((1, "option 'C' set [%s]", globals.knc));
            break;

        /* Which database file? */
        case 'd':
            globals.dbfile = arg;
            OUTPUT_VERBOSE((1, "option 'd' set [%s]", globals.dbfile));
            break;

        /* Only experiments */
        case 'e':
            globals.only_exp = PERFEXPERT_TRUE;
            OUTPUT_VERBOSE((1, "option 'e' set"));
            break;

        /* Leave the garbage there? */
        case 'g':
            globals.leave_garbage = PERFEXPERT_TRUE;
            OUTPUT_VERBOSE((1, "option 'g' set"));
            break;

        /* Show help */
        case 'h':
            OUTPUT_VERBOSE((1, "option 'h' set"));
            argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
            break;

        /* Which input file? */
        case 'i':
            globals.inputfile = arg;
            OUTPUT_VERBOSE((1, "option 'i' set [%s]", globals.inputfile));
            break;

        /* Verbose level (has an alias: v) */
        case 'l':
            globals.verbose = arg ? atoi(arg) : 5;
            OUTPUT_VERBOSE((1, "option 'l' set [%d]", globals.verbose));
            break;

        /* Use Makefile? */
        case 'm':
            globals.target = arg;
            OUTPUT_VERBOSE((1, "option 'm' set [%s]", globals.target));
            break;

        /* List of modules */
        case 'M':
            OUTPUT_VERBOSE((1, "option 'M' set [%s]", arg));
            if (PERFEXPERT_SUCCESS != load_module(arg)) {
                exit(1);
            }
            break;

        /* Do not run */
        case 'n':
            arg_options.do_not_run = PERFEXPERT_TRUE;
            OUTPUT_VERBOSE((1, "option 'n' set [%d]", arg_options.do_not_run));
            break;

        /* Sorting order */
        case 'o':
            globals.order = arg;
            OUTPUT_VERBOSE((1, "option 'o' set [%s]", globals.order));
            break;

        /* Set module option */
        case 'O':
            OUTPUT_VERBOSE((1, "option 'O' set [%s]", arg));
            if (PERFEXPERT_SUCCESS != set_module_option(arg)) {
                exit(1);
            }
            break;

        /* Should I add a program prefix to the command line? */
        case 'p':
            arg_options.prefix = arg;
            OUTPUT_VERBOSE((1, "option 'p' set [%s]", arg_options.prefix));
            break;

        /* Should I add a program prefix to the KNC command line? */
        case 'P':
            arg_options.knc_prefix = arg;
            OUTPUT_VERBOSE((1, "option 'P' set [%s]", arg_options.knc_prefix));
            break;

        /* Number of recommendation to output */
        case 'r':
            globals.rec_count = atoi(arg);
            OUTPUT_VERBOSE((1, "option 'r' set [%d]", globals.rec_count));
            break;

        /* What is the source code filename? */
        case 's':
            globals.sourcefile = arg;
            OUTPUT_VERBOSE((1, "option 's' set [%s]", globals.sourcefile));
            break;

        /* Verbose level (has an alias: l) */
        case 'v':
            globals.verbose = arg ? atoi(arg) : 5;
            OUTPUT_VERBOSE((1, "option 'v' set [%d]", globals.verbose));
            break;

        /* Compile modules order */
        case -1:
            OUTPUT_VERBOSE((1, "option 'compile-order' set [%s]", arg));
            set_module_order(arg, PERFEXPERT_MODULE_COMPILE);
            break;

        /* Measurements modules order */
        case -2:
            OUTPUT_VERBOSE((1, "option 'measurements-order' set [%s]", arg));
            set_module_order(arg, PERFEXPERT_MODULE_MEASUREMENTS);
            break;

        /* Analysis modules order */
        case -3:
            OUTPUT_VERBOSE((1, "option 'analysis-order' set [%s]", arg));
            set_module_order(arg, PERFEXPERT_MODULE_ANALYSIS);
            break;

        /* Show modules help */
        case -4:
            OUTPUT_VERBOSE((1, "option 'module-help' set [%s]", arg));
            module_help(arg);
            break;

        /* Arguments: threshold and target program and it's arguments */
        case ARGP_KEY_ARG:
            if (PERFEXPERT_TRUE == globals.compat_mode) {

                globals.only_exp = PERFEXPERT_TRUE;
                OUTPUT_VERBOSE((1, "option 'e' set"));

                globals.leave_garbage = PERFEXPERT_TRUE;
                OUTPUT_VERBOSE((1, "option 'g' set"));

                arg_options.program = arg;
                OUTPUT_VERBOSE((1, "option 'target_program' set [%s]",
                    arg_options.program));

                OUTPUT_VERBOSE((1, "option 'threshold' set [0.0]"));
                if (PERFEXPERT_SUCCESS != set_module_option(
                    "lcpi,threshold=%s")) {
                    return PERFEXPERT_ERROR;
                }

                arg_options.program_argv = &state->argv[state->next];
                OUTPUT_VERBOSE((1, "option 'program_arguments' set"));
                state->next = state->argc;
            } else {
                if (0 == state->arg_num) {
                    char str[MAX_BUFFER_SIZE];

                    bzero(str, MAX_BUFFER_SIZE);
                    sprintf(str, "lcpi,threshold=%s", arg);
                    OUTPUT_VERBOSE((1, "option 'threshold' set [%s]", arg));
                    if (PERFEXPERT_SUCCESS != set_module_option(str)) {
                        return PERFEXPERT_ERROR;
                    }
                }
                if (1 == state->arg_num) {
                    arg_options.program = arg;
                    OUTPUT_VERBOSE((1, "option 'target_program' set [%s]",
                        arg_options.program));
                    arg_options.program_argv = &state->argv[state->next];
                    OUTPUT_VERBOSE((1, "option 'program_arguments' set"));
                    state->next = state->argc;
                }
            }
            break;

        /* If the arguments are missing */
        case ARGP_KEY_NO_ARGS:
            argp_usage(state);

        /* Too few options */
        case ARGP_KEY_END:
            if (PERFEXPERT_TRUE == globals.compat_mode) {
                if (1 > state->arg_num) {
                    argp_usage(state);
                }
            } else {
                if (2 > state->arg_num) {
                    argp_usage(state);
                }
            }
            break;

        /* Unknown option */
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* parse_env_vars */
static int parse_env_vars(void) {
    if (NULL != getenv("PERFEXPERT_VERBOSE_LEVEL")) {
        globals.verbose = atoi(getenv("PERFEXPERT_VERBOSE_LEVEL"));
        OUTPUT_VERBOSE((1, "ENV: verbose_level=%d", globals.verbose));
    }

    if (NULL != getenv("PERFEXPERT_DATABASE_FILE")) {
        globals.dbfile = getenv("PERFEXPERT_DATABASE_FILE");
        OUTPUT_VERBOSE((1, "ENV: dbfile=%s", globals.dbfile));
    }

    if ((NULL != getenv("PERFEXPERT_REC_COUNT")) &&
        (0 <= atoi(getenv("PERFEXPERT_REC_COUNT")))) {
        globals.rec_count = atoi(getenv("PERFEXPERT_REC_COUNT"));
        OUTPUT_VERBOSE((1, "ENV: rec_count=%d", globals.rec_count));
    }

    if ((NULL != getenv("PERFEXPERT_COLORFUL")) &&
        (1 == atoi(getenv("PERFEXPERT_COLORFUL")))) {
        globals.colorful = PERFEXPERT_FALSE;
        OUTPUT_VERBOSE((1, "ENV: colorful=YES"));
    }

    if (NULL != getenv("PERFEXPERT_MAKE_TARGET")) {
        globals.target = getenv("PERFEXPERT_MAKE_TARGET");
        OUTPUT_VERBOSE((1, "ENV: target=%s", globals.target));
    }

    if (NULL != getenv("PERFEXPERT_SOURCE_FILE")) {
        globals.sourcefile = ("PERFEXPERT_SOURCE_FILE");
        OUTPUT_VERBOSE((1, "ENV: sourcefile=%s", globals.sourcefile));
    }

    if (NULL != getenv("PERFEXPERT_PREFIX")) {
        arg_options.prefix = ("PERFEXPERT_PREFIX");
        OUTPUT_VERBOSE((1, "ENV: prefix=%s", arg_options.prefix));
    }

    if (NULL != getenv("PERFEXPERT_BEFORE")) {
        arg_options.before = ("PERFEXPERT_BEFORE");
        OUTPUT_VERBOSE((1, "ENV: before=%s", arg_options.before));
    }

    if (NULL != getenv("PERFEXPERT_AFTER")) {
        arg_options.after = ("PERFEXPERT_AFTER");
        OUTPUT_VERBOSE((1, "ENV: after=%s", arg_options.after));
    }

    if (NULL != getenv("PERFEXPERT_KNC_CARD")) {
        globals.knc = ("PERFEXPERT_KNC_CARD");
        OUTPUT_VERBOSE((1, "ENV: knc=%s", globals.knc));
    }

    if (NULL != getenv("PERFEXPERT_KNC_PREFIX")) {
        arg_options.knc_prefix = ("PERFEXPERT_KNC_PREFIX");
        OUTPUT_VERBOSE((1, "ENV: knc_prefix=%s", arg_options.knc_prefix));
    }

    if (NULL != getenv("PERFEXPERT_KNC_BEFORE")) {
        arg_options.knc_before = ("PERFEXPERT_KNC_BEFORE");
        OUTPUT_VERBOSE((1, "ENV: knc_before=%s", arg_options.knc_before));
    }

    if (NULL != getenv("PERFEXPERT_KNC_AFTER")) {
        arg_options.knc_after = ("PERFEXPERT_KNC_AFTER");
        OUTPUT_VERBOSE((1, "ENV: knc)after=%s", arg_options.knc_after));
    }

    if (NULL != getenv("PERFEXPERT_SORTING_ORDER")) {
        globals.order = getenv("PERFEXPERT_SORTING_ORDER");
        OUTPUT_VERBOSE((1, "ENV: order=%s", globals.order));
    }

    if (NULL != getenv("PERFEXPERT_MODULES")) {
        arg_options.modules = getenv("PERFEXPERT_MODULES");
        OUTPUT_VERBOSE((1, "ENV: tool=%s", arg_options.modules));
    }

    return PERFEXPERT_SUCCESS;
}

/* load_module */
static int load_module(char *module) {
    char *names[MAX_ARGUMENTS_COUNT];
    int i = 0;

    /* Expand list of modules */
    perfexpert_string_split(module, names, ',');
    while (NULL != names[i]) {
        if (PERFEXPERT_SUCCESS != perfexpert_module_load(names[i])) {
            OUTPUT(("%s [%s]", _ERROR("Error: while adding module"), names[i]));
            return PERFEXPERT_ERROR;
        }
        PERFEXPERT_DEALLOC(names[i]);
        i++;
    }
    return PERFEXPERT_SUCCESS;
}

/* set_module_option */
static int set_module_option(char *option) {
    char *options[MAX_ARGUMENTS_COUNT];
    int i = 0;

    /* Expand list of modules options */
    perfexpert_string_split(option, options, ',');
    while ((NULL != options[i]) && (NULL != options[i + 1])) {
        if (PERFEXPERT_SUCCESS != perfexpert_module_set_option(options[i],
            options[i + 1])) {
            OUTPUT(("%s [%s,%s]", _ERROR("Error: while setting module options"),
                options[i], options[i + 1]));
            return PERFEXPERT_ERROR;
        }
        OUTPUT_VERBOSE((1, "module [%s] option '%s' set", _CYAN(options[i]),
            options[i + 1]));
        PERFEXPERT_DEALLOC(options[i]);
        PERFEXPERT_DEALLOC(options[i + 1]);
        i = i + 2;
    }
    return PERFEXPERT_SUCCESS;
}

/* set_module_order */
static int set_module_order(char *option, module_phase_t order) {
    perfexpert_ordered_module_t *module = NULL;
    char *modules[MAX_ARGUMENTS_COUNT];
    int i = 0;

    /* Expand list of modules options */
    perfexpert_string_split(option, modules, ',');
    while (NULL != modules[i]) {

        PERFEXPERT_ALLOC(perfexpert_ordered_module_t, module,
            sizeof(perfexpert_ordered_module_t));
        PERFEXPERT_ALLOC(char, module->name, (strlen(modules[i]) + 1));
        strcpy(module->name, modules[i]);

        if (PERFEXPERT_MODULE_COMPILE == order) {
            perfexpert_list_append(&(module_globals.compile),
                (perfexpert_list_item_t *)module);
        } else if (PERFEXPERT_MODULE_MEASUREMENTS == order) {
            perfexpert_list_append(&(module_globals.measurements),
                (perfexpert_list_item_t *)module);
        } else if (PERFEXPERT_MODULE_ANALYSIS == order) {
            perfexpert_list_append(&(module_globals.analysis),
                (perfexpert_list_item_t *)module);
        }

        PERFEXPERT_DEALLOC(modules[i]);
        i++;
    }
    return PERFEXPERT_SUCCESS;
}

/* module_help */
static void module_help(const char *name) {
    struct dirent *entry = NULL;
    DIR *directory = NULL;
    int x = 0, y = 0;
    char *m = NULL;

    /* Show my help message */
    argp_help(&argp, stdout, ARGP_HELP_STD_HELP, "perfexpert");

    /* Show module's help messages */
    if (0 == strcmp("all", name)) {
        if (NULL == (directory = opendir(PERFEXPERT_LIBDIR))) {
            OUTPUT(("%s [%s]", _ERROR("Error: unable to open libdir"),
                PERFEXPERT_LIBDIR));
            exit(0);
        }
        while (NULL != (entry = readdir(directory))) {
            if ((0 == strncmp(entry->d_name, "libperfexpert_module_", 21)) &&
                ('.' == entry->d_name[strlen(entry->d_name) - 3]) &&
                ('s' == entry->d_name[strlen(entry->d_name) - 2]) &&
                ('o' == entry->d_name[strlen(entry->d_name) - 1]) &&
                (0 != strcmp(entry->d_name, "libperfexpert_module_base.so"))) {

                PERFEXPERT_ALLOC(char, m, (strlen(entry->d_name) - 24));

                for (x = 21, y = 0; x < (strlen(entry->d_name) - 3); x++, y++) {
                    m[y] = entry->d_name[x];
                }

                if (PERFEXPERT_SUCCESS != perfexpert_module_load(m)) {
                    OUTPUT(("%s [%s]", _ERROR("Error: loading module"), m));
                    exit(0);
                }
                if (PERFEXPERT_SUCCESS != perfexpert_module_set_option(m,
                    "help")) {
                    OUTPUT(("%s", _ERROR("Error: setting module help")));
                    exit(0);
                }

                PERFEXPERT_DEALLOC(m);
            }
        }
        if (0 != closedir(directory)) {
            OUTPUT(("%s [%s]", _ERROR("Error: unable to close libdir")));
            exit(0);
        }
    } else {
        if (PERFEXPERT_SUCCESS != perfexpert_module_load(name)) {
            OUTPUT(("%s [%s]", _ERROR("Error: loading module"), name));
            exit(0);
        }
        if (PERFEXPERT_SUCCESS != perfexpert_module_set_option(name, "help")) {
            OUTPUT(("%s", _ERROR("Error: setting module help"), name));
            exit(0);
        }
    }

    if (PERFEXPERT_SUCCESS != perfexpert_module_init()) {
        OUTPUT(("%s", _ERROR("Error: initializing modules")));
        exit(0);
    }

    exit(0);
}

#ifdef __cplusplus
}
#endif

// EOF
