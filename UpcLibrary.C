#include "UpcLibrary.h"
#include <iostream>

using namespace std;

bool UpcLibrary::debug = false;
bool UpcLibrary::phase2_only = false;

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

/**
 * True if this is a dependence type that we care about.
 */
bool
UpcLibrary::depFilter(DepType dt) {
    switch(dt) {
        case DEPTYPE_TRUE:
        case DEPTYPE_ANTI:
        case DEPTYPE_OUTPUT:
            return true;
        default:
            return false;
    }
}

/*
 * Print the dependence graph to FILENAME in the DOT format, to be read by
 * Graphviz, zgrviewer, etc.
 */
void
UpcLibrary::printDepGraphAsDot(LoopTreeDepGraph* depgraph, char* filename) {
    ROSE_ASSERT(depgraph);

    FILE* ofile = fopen(filename, "w");
    fprintf(ofile, "digraph G {\n");

    set<SgNode*> printed_nodes;
    GraphEdgeIterator<LoopTreeDepGraph> edgep(depgraph);
    for ( ; !edgep.ReachEnd(); ++edgep) {
        for (DepInfoConstIterator depIter = edgep.Current()->get_depIterator();
                !depIter.ReachEnd(); depIter++) {
            DepInfo d = depIter.Current();
            DepType type = d.GetDepType();

            /* continue if we don't care about this type of dependence. */
            if (!depFilter(type)) continue;

            SgNode* src =  (SgNode*) d.SrcRef().get_ptr();
            SgNode* sink = (SgNode*) d.SnkRef().get_ptr();
            if (printed_nodes.find(src) == printed_nodes.end()) {
                printed_nodes.insert(src);
                fprintf(ofile, "n%p [label=\"%s\\n%s\"];\n", src,
                        src->class_name().c_str(), src->unparseToString().c_str());
            }
            if (printed_nodes.find(sink) == printed_nodes.end()) {
                printed_nodes.insert(sink);
                fprintf(ofile, "n%p [label=\"%s\\n%s\"];\n", sink,
                        sink->class_name().c_str(), sink->unparseToString().c_str());
            }
            int level = d.CommonLevel();
            fprintf(ofile, "n%p -> n%p [label=\"level: %d\\n%s\"];\n",
                    src, sink, level, DepType2String(type).c_str());
        }
    }

    fprintf(ofile, "}\n");
    fclose(ofile);
}

// From AutoParallelization project
// Compute dependence graph for a loop, using ArrayInterface and ArrayAnnotation
// TODO generate dep graph for the entire function and reuse it for all loops
LoopTreeDepGraph*
UpcLibrary::ComputeDependenceGraph(SgNode* loop)
{
    // Replace operators with their equivalent counterparts defined
    // in "inline" annotations
    AstInterfaceImpl faImpl_1(loop);
    CPPAstInterface fa_body(&faImpl_1);
    OperatorInlineRewrite()(fa_body, AstNodePtrImpl(loop));

    // Pass annotations to arrayInterface and use them to collect
    // alias info. function info etc.
    SgFunctionDefinition* defn = SageInterface::getEnclosingProcedure(loop);
    ArrayAnnotation *annot = ArrayAnnotation::get_inst();
    ArrayInterface array_interface(*annot);
    array_interface.initialize(fa_body, AstNodePtrImpl(defn));
    array_interface.observe(fa_body);

    //FR(06/07/2011): aliasinfo was not set which caused segfault
    LoopTransformInterface::set_aliasInfo(&array_interface);

    // X. Loop normalization for all loops within body
    NormalizeForLoop(fa_body, AstNodePtrImpl(loop));

	//TODO check if its a canonical loop

	// Prepare AstInterface: implementation and head pointer
	AstInterfaceImpl faImpl_2 = AstInterfaceImpl(loop);
	//AstInterface fa(&faImpl); // Using CPP interface to handle templates etc.
	CPPAstInterface fa(&faImpl_2);
	AstNodePtr head = AstNodePtrImpl(loop);
	fa.SetRoot(head);

	LoopTransformInterface::set_astInterface(fa);
	LoopTransformInterface::set_arrayInfo(&array_interface);
	LoopTransformInterface::set_aliasInfo(&array_interface);
	LoopTransformInterface::set_sideEffectInfo(annot);
	LoopTreeDepCompCreate* comp = new LoopTreeDepCompCreate(head);// TODO when to release this?
	// Retrieve dependence graph here!
	if (UpcLibrary::debug)
	{
		cout<<"Debug: Dump the dependence graph for the loop in question:"<<endl;
		comp->DumpDep();
	}
	return comp->GetDepGraph();

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
//		//cout<<AstToString(ast_ptr)<<endl;
//		ROSE_ASSERT(ast_ptr!=NULL);
//		SgNode* sg_node = AstNodePtr2Sage(ast_ptr);
//		ROSE_ASSERT(sg_node == loop);
//		//cout<<"-------------Dump the loops in question------------"<<endl;
//		//cout<<sg_node->class_name()<<endl;
//		return comp->GetDepGraph();
//	}
//	else
//	{
//		cout<<"Skipping a loop not recognized by LoopTreeTraverseSelectLoop ..."<<endl;
//		return NULL;
//		// Not all loop can be collected by LoopTreeTraverseSelectLoop right now
//		// e.g: loops in template function bodies
//	}
}

// Modified from AutoParallelization project
void
UpcLibrary::processOptions(vector<string> &argvList){
	if(CommandlineProcessing::isOption(argvList, "", "-debug", true)){
		cout << "Enabling debugging..." << endl;
		debug = true;
	}
	if(CommandlineProcessing::isOption(argvList, "", "-phase2", true)){
		cout << "Skipping Phase I..." << endl;
		phase2_only = true;
	}

	//Save -debugdep, -annot file .. etc,
	// used internally in ReadAnnotation and Loop transformation
	CmdOptions::GetInstance()->SetOptions(argvList);
	bool dumpAnnot = CommandlineProcessing::isOption(argvList,"","-dumpannot",true);

	//Read in annotation files after -annot
	ArrayAnnotation* annot = ArrayAnnotation::get_inst();
	annot->register_annot();
	ReadAnnotation::get_inst()->read();
	if (dumpAnnot)
		annot->Dump();
	//Strip off custom options and their values to enable backend compiler
	CommandlineProcessing::removeArgsWithParameters(argvList,"-annot");

	// keep --help option after processing, let other modules respond also
	if ((CommandlineProcessing::isOption (argvList,"--help","",false)) ||
		(CommandlineProcessing::isOption (argvList,"-help","",false)))
	{
		cout << "upctr-specific options" << endl;
		cout << "\t-debug          		print debug info" << endl;
		cout << "\t-phase2         		run only phase II (dependence analysis)" << endl;
		cout << "\t-annot filename      specify annotation file for semantics of abstractions" << endl;
		cout << "\t-dumpannot           dump annotation file content" << endl;
		cout << "---------------------------------------------------------------" << endl;
	}
}

#if 0

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
 #endif
