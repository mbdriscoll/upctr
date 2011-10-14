#ifndef _UPC_LIBRARY_H_
#define _UPC_LIBRARY_H_

#include "rose.h"
#include "sageInterface.h"

namespace UpcLibrary {

SgExpression* buildThreadOfCall(
        SgExpression* exp,
        SgScopeStatement* scope);

} /* end namespace UpcLibrary */

#endif _UPC_LIBRARY_H_
