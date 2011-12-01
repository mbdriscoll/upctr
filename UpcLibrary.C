#include "UpcLibrary.h"
#include <iostream>

using namespace std;

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

// Return the loop invariant of a canonical loop
// Return NULL if the loop is not canonical
SgInitializedName*
UpcLibrary::getLoopInvariant(SgNode* loop)
{
	AstInterfaceImpl faImpl(loop);
	AstInterface fa(&faImpl);
	AstNodePtr ivar2 ;
	AstNodePtrImpl loop2(loop);
	bool result=fa.IsFortranLoop(loop2, &ivar2);
	if (!result)
	  return NULL;
	SgVarRefExp* invar = isSgVarRefExp(AstNodePtrImpl(ivar2).get_ptr());
	ROSE_ASSERT(invar);
	SgInitializedName* invarname = invar->get_symbol()->get_declaration();
	// cout<<"debug ivar:"<<invarname<< " name "
	// <<invarname->get_name().getString()<<endl;
	return invarname;
}

//Compute dependence graph for a loop, using ArrayInterface and ArrayAnnotation
// TODO generate dep graph for the entire function and reuse it for all loops
LoopTreeDepGraph*
UpcLibrary::ComputeDependenceGraph(SgNode* loop, ArrayInterface* array_interface, ArrayAnnotation* annot)
{
	bool enable_debug = true;
	ROSE_ASSERT(loop && array_interface&& annot);
	//TODO check if its a canonical loop

	// Prepare AstInterface: implementation and head pointer
	AstInterfaceImpl faImpl_2 = AstInterfaceImpl(loop);
	//AstInterface fa(&faImpl); // Using CPP interface to handle templates etc.
	CPPAstInterface fa(&faImpl_2);
	AstNodePtr head = AstNodePtrImpl(loop);
	fa.SetRoot(head);

	LoopTransformInterface::set_astInterface(fa);
	LoopTransformInterface::set_arrayInfo(array_interface);
	LoopTransformInterface::set_aliasInfo(array_interface);
	LoopTransformInterface::set_sideEffectInfo(annot);
	LoopTreeDepCompCreate* comp = new LoopTreeDepCompCreate(head);// TODO when to release this?
	// Retrieve dependence graph here!
	if (enable_debug)
	{
		cout<<"Debug: Dump the dependence graph for the loop in question:"<<endl;
		comp->DumpDep();
	}

//	// The following code was used when an entire function body with several loops
//	// is analyzed for dependence analysis. I keep it to double check the computation.
//
//	// Get the loop hierarchy :grab just a top one for now
//	// TODO consider complex loop nests like loop {loop, loop} and loop{loop {loop}}
//	LoopTreeNode * loop_root = comp->GetLoopTreeRoot();
//	ROSE_ASSERT(loop_root!=NULL);
//	//loop_root->Dump();
//	LoopTreeTraverseSelectLoop loop_nodes(loop_root, LoopTreeTraverse::PreOrder);
//	LoopTreeNode * cur_loop = loop_nodes.Current();
//	// three-level loop: i,j,k
//	AstNodePtr ast_ptr;
//	if (cur_loop)
//	{
//		//cur_loop->Dump();
//		//loop_nodes.Advance();
//		//loop_nodes.Current()->Dump();
//		//loop_nodes.Advance();
//		//loop_nodes.Current()->Dump();
//		ast_ptr = dynamic_cast<LoopTreeLoopNode*>(cur_loop)->GetOrigLoop();
//		// cout<<AstToString(ast_ptr)<<endl;
//		ROSE_ASSERT(ast_ptr!=NULL);
//		SgNode* sg_node = AstNodePtr2Sage(ast_ptr);
//		ROSE_ASSERT(sg_node == loop);
//		// cout<<"-------------Dump the loops in question------------"<<endl;
//		//   cout<<sg_node->class_name()<<endl;
//		return comp->GetDepGraph();
//	}
//	else
//	{
//		cout<<"Skipping a loop not recognized by LoopTreeTraverseSelectLoop ..."<<endl;
//		return NULL;
//		// Not all loop can be collected by LoopTreeTraverseSelectLoop right now
//		// e.g: loops in template function bodies
//		//ROSE_ASSERT(false);
//	}
}
