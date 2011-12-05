#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "optimize.h"
#include "builder.h"
#include "UpcLibrary.h"
#include "AstDOTGeneration.h"

using namespace std;
using namespace SageInterface;

/**
 * True if REF is a top-level array reference.
 */
bool isOuterReference(SgPntrArrRefExp* ref) {
    ROSE_ASSERT(ref);
    return !isSgPntrArrRefExp( ref->get_parent() );
}

/**
 * The number of loops enclosing the given expression.
 */
int getLoopLevel(SgExpression* exp) {
    int level = 0;
    SgForStatement* for_stmt = getEnclosingNode<SgForStatement>(exp);
    while (for_stmt != NULL) {
        level += 1;
        for_stmt = getEnclosingNode<SgForStatement>(for_stmt);
    }
    return level;
}

/**
 * TODO True if this reference is a write.
 */
bool isWrite(SgPntrArrRefExp* reference) {
    cerr << "Warning: isWrite() is unimplemented." << endl;
    return true;
}

/**
 * True if this reference is a read.
 */
bool isRead(SgPntrArrRefExp* reference) {
    return !isWrite(reference);
}

/**
 * Localize REF. Must be localizable.
 */
void localize(SgPntrArrRefExp* shared_ref,
              SgExpression* subscript,
              SgForStatement* target) {

    bool written = isWrite(shared_ref);
    bool read = isRead(shared_ref);

    /* build new local array __upctr_local_NAME */
    SgVariableDeclaration* local_array_decl =
        UpctrBuilder::buildLocalArrayDecl(shared_ref, subscript, target->get_scope());

    /* build reference to the local array */
    SgExpression* local_ref = UpctrBuilder::buildLocalReference(
            local_array_decl, subscript, target->get_scope());

    /* if subscript is a read, build bupc_memget_strided call */
    SgStatement* fetch_stmt;
    if (read)
        fetch_stmt = UpctrBuilder::buildFetch(local_array_decl, shared_ref, subscript);

    /* if subscript is a write, build bupc_memput_strided call */
    SgStatement* store_stmt;
    if (written)
        store_stmt = UpctrBuilder::buildStore(shared_ref, local_array_decl, subscript);

    /* actually transform the AST */
    prependStatement(local_array_decl, target->get_scope());
    replaceExpression(shared_ref, local_ref, true);
    if (read) insertStatementAfter(local_array_decl, fetch_stmt);
    if (written) appendStatement(store_stmt, target->get_scope());
}

bool
hasLoopCarriedDepsAtLevel(LoopTreeDepGraph* depgraph,
                          SgPntrArrRefExp* reference,
                          int level) {
    /* TODO implement hasLoopCarriedDepsAtLevel. Do not
     * worry about deps at other levels, or deps that
     * yield false when passed to UpcLibrary::depFilter */

    cerr << "Warning: hasLoopCarriedDepsAtLevel() is unimplemented." << endl;

    /* for now, true if the array referenced is not named 'C' */
    reference = isSgPntrArrRefExp(reference->get_lhs_operand());
    if (reference == NULL) return true;
    SgVarRefExp* var = isSgVarRefExp(reference->get_lhs_operand());
    if (var == NULL) return true;
    return "C" == var->get_symbol()->get_name().getString();
}

/**
 * True if LOOP was a upc_forall loop that we converted to a plain loop.
 */
bool
loopWasUpcForall(SgForStatement* loop) {
    return aff_exp_map.find(loop) != aff_exp_map.end();
}

/**
 * True if SUBSCRIPT is the same as the loop index variable.
 */
bool
loopIndexIsSubscript(SgForStatement* loop, SgExpression* subscript) {
    /* extract the subscript's symbol */
    SgVarRefExp* varref = isSgVarRefExp(subscript);
    if (varref == NULL) {
        cerr << "Warning: complicated subscript: "
            << subscript->unparseToString() << endl;
        assert(!"Bad input program.");
    }
    SgVariableSymbol* subscript_sym = varref->get_symbol();

    /* lookup the loop index symbol */
    SgInitializedName* loop_iname = getLoopIndexVariable(loop);
    SgVariableSymbol* index_sym =
        isSgVariableSymbol( loop_iname->get_symbol_from_symbol_table() );

    /* compare the symbols */
    return subscript_sym == index_sym;
}


/**
 * For the given reference, test if it can be localized. If it can,
 * localize it.
 *
 * Localization is legal at given level if the reference has no
 * loop-carried dependences at or inside the level in question.
 *
 * Localization is most profitable at the level just inside the
 * innermost upc_forall loop.
 *
 * 1-dimensional localization must occur inside the loop iterating
 * over the second dimension.
 */
void
find_and_localize(LoopTreeDepGraph* depgraph,
                  SgPntrArrRefExp*  reference,
                  vector<SgExpression*>* subscript_exps) {

    /* Walk outward until we can't keep the local array 1d. if we find a
     * subscript that can be localized, set SUBSCRIPT  */
    SgExpression* subscript = NULL;
    SgForStatement* target = NULL;
    SgForStatement* loop = getEnclosingNode<SgForStatement>(reference);
    SgForStatement* next_loop = getEnclosingNode<SgForStatement>(loop);
    for (int level = getLoopLevel(reference); level > 0; level--) {

        /* return if reference carried deps at this level */
        if (hasLoopCarriedDepsAtLevel(depgraph, reference, level))
            return;

        /* find possibly 1 subscript that is controlled by this loop level */
        foreach (SgExpression* subscript_exp, *subscript_exps) {

            /* if we haven't found the first dimension, look for it */
            if (subscript == NULL && loopIndexIsSubscript(loop, subscript_exp))
                subscript = subscript_exp;

            /* if we have found the first dimension, look for the target loop */
            if (subscript != NULL &&
                (next_loop == NULL || loopIndexIsSubscript(next_loop, subscript_exp))) {
                target = loop;
                goto identified;
            }
        }

        /* notice if the next loop level is a upc_forall loop */
        bool outermost_within_upc_forall = loopWasUpcForall(next_loop);
        if (outermost_within_upc_forall) {
            target = loop;
            goto identified;
        }

        loop = next_loop;
        next_loop = getEnclosingNode<SgForStatement>(loop);
    }

identified:

    /* if we found a target, make the transformation */
    if (subscript != NULL && target != NULL)
        localize(reference, subscript, target);
}

/**
 * Optimize this loop nest.
 */
void optimize(SgForStatement* nest) {

    /* build the dependence graph */
    LoopTreeDepGraph* depgraph = UpcLibrary::ComputeDependenceGraph(nest);

    /* debugging: print dep graph and ast */
    if (UpcLibrary::debug) {
        UpcLibrary::printDepGraphAsDot(depgraph, (char*) "graph.dot");
        AstDOTGeneration dot; dot.generate(nest, "tree");
    }

    /* collect all array references, including non-top-level ones */
    vector<SgPntrArrRefExp*> all_array_refs =
        SageInterface::querySubTree<SgPntrArrRefExp>(nest);

    /* filter out non-top-level array references */
    vector<SgPntrArrRefExp*> array_refs;
    foreach (SgPntrArrRefExp* array_ref, all_array_refs)
        if (isOuterReference(array_ref))
            array_refs.push_back(array_ref);

    /* test and localize each subscript */
    foreach (SgPntrArrRefExp* array_ref, array_refs) {

        /* extract all subscript expressions */
        SgExpression* name_exp;
        vector<SgExpression*>* subscript_exps = new vector<SgExpression*>();;
        assert(SageInterface::isArrayReference(array_ref, &name_exp, &subscript_exps));

        /* find the outermost for-stmt around which this subscript
         * can be localized, and localize it */
        find_and_localize(depgraph, array_ref, subscript_exps);

        /* cleanup */
        delete subscript_exps;
    }
}
