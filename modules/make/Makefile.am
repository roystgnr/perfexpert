#
# Copyright (c) 2011-2016  University of Texas at Austin. All rights reserved.
#
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# This file is part of PerfExpert.
#
# PerfExpert is free software: you can redistribute it and/or modify it under
# the terms of the The University of Texas at Austin Research License
#
# PerfExpert is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.
#
# Authors: Antonio Gomez-Iglesias, Leonardo Fialho and Ashay Rane
#
# $HEADER$
#

lib_LTLIBRARIES = libperfexpert_module_make.la
libperfexpert_module_make_la_CPPFLAGS = -I$(srcdir)/../..
libperfexpert_module_make_la_LDFLAGS = -module -version-info 1:0:0 \
	-export-symbols $(srcdir)/make_module.sym
libperfexpert_module_make_la_SOURCES = make_module.c make_options.c make.c

# EOF
