#include <iostream>
#include <vector>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "rose.h"
#include "sageInterface.h"

#include "UpcLibrary.h"

using namespace std;

/**
 * Construct an expression which is true when MYTHREAD has affinity to the
 * given integer expression.
 *
 * TODO: check type of elem_exp (int) to sanitize
 */
SgExpression* buildHasAffinityExp_Int(SgExpression* int_exp, SgScopeStatement* scope) {
    SgUpcMythread* mythread = SageBuilder::buildUpcMythread();
    SgModOp* mod_exp = SageBuilder::buildModOp(int_exp, mythread);
    SgIntVal* zero = SageBuilder::buildIntVal(0);
    return SageBuilder::buildEqualityOp(mod_exp, zero);
}

/**
 * Construct an expression which is true when MYTHREAD has affinity to the
 * given pointer-to-shared expression.
 *
 * TODO: check type of elem_exp (pointer-to-shared) to sanitize
 */
SgExpression* buildHasAffinityExp_Ptr(SgExpression* elem_exp, SgScopeStatement* scope) {
    SgUpcMythread* mythread = SageBuilder::buildUpcMythread();
    SgExpression* elem_affinity = UpcLibrary::buildThreadOfCall(elem_exp, scope);
    return SageBuilder::buildEqualityOp(mythread, elem_affinity);
}

/**
 * Construct an expression which is true when MYTHREAD has affinity to exp, which
 * must either be an integer or pointer-to-shared expression.
 */
SgExpression* buildHasAffinityExp(SgExpression* exp, SgScopeStatement* scope) {
    if (exp->get_type()->isIntegerType())
        return buildHasAffinityExp_Int(exp, scope);
    else /* pointer-to-shared */
        return buildHasAffinityExp_Ptr(exp, scope);
}

/**
 * Convert upc_stmt to a standard for statement.
 */
SgForStatement* translate(SgUpcForAllStatement* upc_stmt) {
    SgScopeStatement* scope = upc_stmt->get_scope();

    SgExprStatement* old_test_stmt = isSgExprStatement(upc_stmt->get_test());
    SgExpression* old_test = old_test_stmt->get_expression();
    ROSE_ASSERT(old_test);

    SgStatement* new_test_stmt;
    if (isSgNullExpression( upc_stmt->get_affinity() )) {
        new_test_stmt = old_test_stmt;
    } else {
        SgExpression* has_affinity = buildHasAffinityExp(upc_stmt->get_affinity(), scope);
        SgExpression* new_test = SageBuilder::buildAndOp(old_test, has_affinity);
        new_test_stmt = SageBuilder::buildExprStatement(new_test);
    }

    SgForStatement* for_stmt = SageBuilder::buildForStatement(
            upc_stmt->get_for_init_stmt(), /* initialize stmt */
            new_test_stmt,                 /* test exp */
            upc_stmt->get_increment(),     /* increment */
            upc_stmt->get_loop_body()      /* loop body */
            );
    return for_stmt;
}

int main(int argc, char* argv[])
{
    /* parse the input file */
    SgProject* proj = frontend(argc, argv);

    /* collect all upc_forall stmts */
    vector<SgUpcForAllStatement*> for_stmts =
        SageInterface::querySubTree<SgUpcForAllStatement>(proj);

    /* translate upc_forall stmts to normal c */
    foreach (SgUpcForAllStatement* upc_stmt, for_stmts) {
        SageInterface::replaceStatement(upc_stmt, translate(upc_stmt));
    }

    /* output the number of stmts found */
    cout << "Processed " << for_stmts.size() << " upc_forall stmts" << endl;

    /* unparse the transformed AST */
    proj->unparse();
    return 0;
}
