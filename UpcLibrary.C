#include "UpcLibrary.h"

SgExpression*
UpcLibrary::buildThreadOfCall(
        SgExpression* exp,
        SgScopeStatement* scope)
{
    SgExprListExp* params = SageBuilder::buildExprListExp(exp);
    SgType* return_type = SageBuilder::buildIntType();
    SgName name("upc_threadof");
    SgExpression* threadof =
        SageBuilder::buildFunctionCallExp(name, return_type, params, scope);
    return threadof;
}
