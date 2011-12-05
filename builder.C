#include "rose.h"
#include "sageInterface.h"

#include "builder.h"

using namespace std;
using namespace SageBuilder;

SgVariableDeclaration*
UpctrBuilder::buildLocalArrayDecl(
        SgPntrArrRefExp* shared_ref, SgExpression* subscript) {
    return NULL;
}

/*
 * Build AST to reference the given local array. Will look
 * something like:
 *     __upctr_local_A[k]
 */
SgExpression*
UpctrBuilder::buildLocalReference(
        SgVariableDeclaration* local_array_decl, SgExpression* subscript) {
    return NULL;
}

/*
 * Build AST to execute fetch of shared_array into local_array. Will
 * look something like:
 *     bupc_memget_strided(local_array, ..., shared_array, ...);
 */
SgStatement*
UpctrBuilder::buildFetch(
        SgVariableDeclaration* local_array_decl,
        SgPntrArrRefExp* shared_ref,
        SgExpression* subscript) {
    return buildExprStatement( buildStringVal("this_is_the_fetch") );
}

/*
 * Build AST to execute store of local_array into shared ref. Will
 * look something like:
 *     bupc_memput_strided(shared_ref, ..., local_array, ...);
 */
SgStatement*
UpctrBuilder::buildStore(
        SgPntrArrRefExp* shared_ref,
        SgVariableDeclaration* local_array_decl,
        SgExpression* subscript) {
    return buildExprStatement( buildStringVal("this_is_the_store") );
}
