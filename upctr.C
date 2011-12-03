#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "rose.h"
#include "sageInterface.h"

#include "UpcLibrary.h"

using namespace std;

/*
 * Global variable to store (for_stmt -> upc_forall.aff_exp) mapping.
 */
map<SgForStatement*,SgExpression*> aff_exp_map;


/**
 * Convert upc_stmt to a standard for statement, and save the affinity expression
 * for later retrieval.
 */
SgForStatement* translate(SgUpcForAllStatement* upc_stmt) {
    SgForStatement* for_stmt = SageBuilder::buildForStatement(
            upc_stmt->get_for_init_stmt(), /* initialize stmt */
            upc_stmt->get_test(),          /* condition */
            upc_stmt->get_increment(),     /* increment */
            upc_stmt->get_loop_body()      /* loop body */
            );

    /* save the affinity expression in the global map */
    aff_exp_map[for_stmt] = upc_stmt->get_affinity();
    return for_stmt;
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
        SageInterface::replaceStatement(forall_stmt, translate(forall_stmt));

    /* output the number of stmts found */
    cout << "Processed " << forall_stmts.size() << " upc_forall stmts" << endl;

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
    printf("Processing %d nests with %d total loops.\n",
            (int) nests.size(), (int) for_stmts.size());



#if 0
    Rose_STL_Container<SgNode*> loops  = NodeQuery::querySubTree(proj, V_SgForStatement);
    if(loops.size() == 0) continue;

    // Replace operators with their equivalent counterparts defined
    // in "inline" annotations
    AstInterfaceImpl faImpl_1(body);
    CPPAstInterface fa_body(&faImpl_1);
    OperatorInlineRewrite()(fa_body, AstNodePtrImpl(body));

    // Pass annotations to arrayInterface and use them to collect
    // alias info. function info etc.
    ArrayAnnotation *annot = ArrayAnnotation::get_inst();
    ArrayInterface array_interface(*annot);
    array_interface.initialize(fa_body, AstNodePtrImpl(defn));
    array_interface.observe(fa_body);

    //FR(06/07/2011): aliasinfo was not set which caused segfault
    LoopTransformInterface::set_aliasInfo(&array_interface);

    // X. Loop normalization for all loops within body
    NormalizeForLoop(fa_body, AstNodePtrImpl(body));

    for(Rose_STL_Container<SgNode*>::iterator iter = loops.begin();
            iter != loops.end(); iter++)
    {
        SgNode *current_loop = *iter;
        // X. Parallelize loop one by one
        // getLoopInvariant() will actually check if the loop has canonical forms
        // which can be handled by dependence analysis
        SgInitializedName *invarname = UpcLibrary::getLoopInvariant(current_loop);
        LoopTreeDepGraph *depgraph;
        if(true/*invarname != NULL*/){
            depgraph = UpcLibrary::ComputeDependenceGraph(current_loop, &array_interface, annot);
        }
        else { // Cannot grab loop index from a non-conforming looo, skip Dependence Analysis
            cout << "Skipping a non-canonical loop at line:"
                << current_loop->get_file_info()->get_line()
                << "..." << endl;
        }
    }
#endif

    return 0;
}
