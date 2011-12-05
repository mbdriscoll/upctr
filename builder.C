#include "rose.h"
#include "sageInterface.h"

#include "builder.h"

using namespace std;
using namespace SageBuilder;

string
getArrayName(SgPntrArrRefExp* ref) {
    SgExpression* oldname_exp;
    assert( SageInterface::isArrayReference(ref, &oldname_exp) );
    SgVarRefExp* var = isSgVarRefExp(oldname_exp);
    return var->get_symbol()->get_name().getString();
}

SgType*
getArrayType(SgPntrArrRefExp* ref, SgExpression* len) {
    /* TODO extract type of reference array. for now, use double* */
    return buildArrayType( buildDoubleType(), len );
}

SgType*
stripSharedQualifier(SgType* type) {
    /* TODO strip shared qualifier, for now leave it */
    return type;
}

/*
 * Build a declaration of a local array:
 * double* __upctr_local_A[N];
 */
SgVariableDeclaration*
UpctrBuilder::buildLocalArrayDecl(SgPntrArrRefExp* shared_ref,
                                  SgExpression* subscript,
                                  SgScopeStatement* scope) {

    string newname = "__upctr_local_" + getArrayName(shared_ref);
    SgExpression* len = buildIntVal(1000); // TODO determine array size
    SgType* newtype = stripSharedQualifier( getArrayType(shared_ref, len) );

    return buildVariableDeclaration(newname, newtype, NULL, scope);
}

/*
 * Build AST to reference the given local array. Will look
 * something like:
 *     __upctr_local_A[k]
 */
SgExpression*
UpctrBuilder::buildLocalReference(SgVariableDeclaration* local_array_decl,
                                  SgExpression* subscript_exp,
                                  SgScopeStatement* scope) {
    return buildPntrArrRefExp( buildVarRefExp(local_array_decl), subscript_exp );
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
        SgExpression* subscript,
        SgScopeStatement* scope) {

    /* extract the shared array name */
    SgExpression* name_exp;
    assert( SageInterface::isArrayReference(shared_ref, &name_exp) );

    /* build the argument list to bupc_memget_strided */
    SgExprListExp* arguments = buildExprListExp(
            buildVarRefExp(local_array_decl),
            buildSizeOfOp( buildDoubleType() ), // TODO use array base type
            buildSizeOfOp( buildDoubleType() ), // TODO use array base type
            buildIntVal(1000),                  // TODO use array dimension
            buildPointerDerefExp( name_exp ),
            buildSizeOfOp( buildDoubleType() ), // TODO use array base type
            buildSizeOfOp( buildDoubleType() ), // TODO use array base type
            buildIntVal(1000)                   // TODO use array dimension
            );

    /* build the call to bupc_memget_strided */
    SgFunctionCallExp* call = buildFunctionCallExp(
            SgName("bupc_memget_fstrided"), // const SgName &name,
            buildVoidType(),                // SgType *return_type,
            arguments,                      // SgExprListExp *parameters=NULL
            scope);

    return buildExprStatement(call);
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
