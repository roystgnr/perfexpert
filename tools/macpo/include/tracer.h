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

#ifndef	TRACER_H_
#define	TRACER_H_

#include "generic_defs.h"
#include "inst_defs.h"

class tracer_t : public AstTopDownProcessing<attrib> {
    public:
        name_list_t& get_stream_list();

        virtual attrib evaluateInheritedAttribute(SgNode* node, attrib attr);
        virtual void atTraversalStart();
        virtual void atTraversalEnd();

        const statement_list_t::iterator stmt_begin();
        const statement_list_t::iterator stmt_end();

    private:
        statement_list_t statement_list;
        name_list_t stream_list;
};

#endif	/* TRACER_H_ */
