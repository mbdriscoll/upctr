#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "rose.h"
#include "sageInterface.h"

#include "UpcLibrary.h"
#include "optimize.h"

using namespace std;

/*
 * Global variable to store (for_stmt -> upc_forall.aff_exp) mapping.
 */
aff_map_t aff_exp_map;

/**
 * Convert upc_stmt to a standard for statement, and save the affinity expression
 * for later retrieval.
 */
SgForStatement* translate_from_upc(SgUpcForAllStatement* upc_stmt) {
    SgForStatement* for_stmt = SageBuilder::buildForStatement_nfi(
            upc_stmt->get_for_init_stmt(), /* initialize stmt */
            upc_stmt->get_test(),          /* condition */
            upc_stmt->get_increment(),     /* increment */
            upc_stmt->get_loop_body()      /* loop body */
            );

    /* save the affinity expression in the global map */
    aff_exp_map[for_stmt] = upc_stmt->get_affinity();

    /* set file info */
    Sg_File_Info* old_file_info = upc_stmt->get_file_info();
    for_stmt->set_file_info( old_file_info );

    return for_stmt;
}

/**
 * Convert for_stmt to a upc_forall statement if it has an affinity exp
 * in the global map.
 */
SgStatement* translate_to_upc(SgForStatement* for_stmt) {
    /* return value is either SgUpcForAllStmt or SgForStmt */
    SgStatement* retval;

    /* translate if the for_stmt was upc_forall stmt */
    aff_map_t::iterator affinity_exp_it = aff_exp_map.find(for_stmt);
    if (affinity_exp_it != aff_exp_map.end()) {

        /* build a new upc_forall statement */
        retval = SageBuilder::buildUpcForAllStatement_nfi(
                for_stmt->get_for_init_stmt(), /* initialize stmt */
                for_stmt->get_test(),          /* condition */
                for_stmt->get_increment(),     /* increment */
                affinity_exp_it->second,       /* affinity */
                for_stmt->get_loop_body()      /* loop body */
                );

        /* set file info */
        Sg_File_Info* old_file_info = for_stmt->get_file_info();
        retval->set_file_info( old_file_info );

    } else {
        /* for_stmt was never a upc_forall stmt */
        retval = for_stmt;
    }

    return retval;
}

/**
 * True if LOOP is enclosed in another loop.
 */
bool isOuterLoop(SgForStatement* loop) {
    SgStatement* parent = isSgStatement(loop->get_parent());
    return SageInterface::findEnclosingLoop( parent ) == NULL;
}

int main(int argc, char* argv[])
{
    /* Processing cmd line args */
	vector<string> argvList(argv, argv+argc);
	UpcLibrary::processOptions(argvList);

    /* Parse the input file */
    SgProject* proj = frontend(argc, argv);

    /* collect all upc_forall stmts */
    vector<SgUpcForAllStatement*> forall_stmts =
        SageInterface::querySubTree<SgUpcForAllStatement>(proj);

    /* translate upc_forall stmts to normal c */
    foreach (SgUpcForAllStatement* forall_stmt, forall_stmts)
        SageInterface::replaceStatement(forall_stmt, translate_from_upc(forall_stmt));

    /* collect all for_stmts */
    vector<SgForStatement*> for_stmts =
        SageInterface::querySubTree<SgForStatement>(proj);

	/* Normalize C99-style for (int i=x, ...) to C89-style: int i; for(i=x, ...) */
    foreach (SgForStatement* for_stmt, for_stmts)
		SageInterface::normalizeForLoopInitDeclaration(for_stmt);

    /* Filter out the inner loops in each nest */
    vector<SgForStatement*> nests;
    foreach (SgForStatement* for_stmt, for_stmts)
        if (isOuterLoop(for_stmt))
            nests.push_back(for_stmt);

    /* optimize each nest */
    foreach (SgForStatement* nest, nests)
        optimize(nest);

    /* replace upc affinity expressions */
    foreach (SgForStatement* for_stmt, for_stmts)
        SageInterface::replaceStatement(for_stmt, translate_to_upc(for_stmt));

    proj->unparse();
    return 0;
}
