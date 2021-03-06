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

#ifndef TOOLS_MACPO_INST_INCLUDE_LOOP_TRAVERSAL_H_
#define TOOLS_MACPO_INST_INCLUDE_LOOP_TRAVERSAL_H_

#include <rose.h>

#include "generic_defs.h"
#include "inst_defs.h"

class loop_traversal_t : public AstTopDownProcessing<attrib> {
 public:
    explicit loop_traversal_t(const du_table_t& def_table);

    loop_info_list_t& get_loop_info_list();
    void set_deep_search(bool _deep_search);
    virtual attrib evaluateInheritedAttribute(SgNode* node, attrib attr);

 private:
    bool deep_search;
    SgNode* initial_node;
    reference_list_t reference_list;
    loop_info_list_t loop_info_list;

    const du_table_t def_table;
};

#endif  // TOOLS_MACPO_INST_INCLUDE_LOOP_TRAVERSAL_H_
