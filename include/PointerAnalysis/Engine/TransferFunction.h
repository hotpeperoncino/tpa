#pragma once

#include "PointerAnalysis/Engine/EvalResult.h"
#include "PointerAnalysis/Program/CFG/CFGNode.h"

namespace context
{
	class Context;
}

namespace llvm
{
	class Function;
	class Instruction;
}

namespace annotation
{
	class APosition;
	class CopyDest;
	class CopySource;
	class PointerAllocEffect;
	class PointerCopyEffect;
	class PointerEffect;
}

namespace tpa
{


class FunctionContext;
class GlobalState;
class ProgramPoint;

class TransferFunction
{
private:
	// The global state
	GlobalState& globalState;

	// The local state
	const Store* localState;

	// Successor helper
	void addTopLevelSuccessors(const ProgramPoint&, EvalResult&);
	void addMemLevelSuccessors(const ProgramPoint&, EvalResult&);

	// evalAlloc helper
	bool evalMemoryAllocation(const context::Context*, const llvm::Instruction*, const TypeLayout*, bool);
	// evalOffset helper
	bool copyWithOffset(const Pointer*, const Pointer*, size_t, bool);
	PtsSet offsetMemory(const MemoryObject*, size_t, bool);
	// evalLoad helper
	PtsSet loadFromPointer(const Pointer*, const Store&);
	// evalStore helper
	bool evalStore(const Pointer*, const Pointer*, Store&);
	void strongUpdateStore(const MemoryObject*, PtsSet, Store&);
	void weakUpdateStore(PtsSet, PtsSet, Store&);
	// evalCall helper
	std::vector<const llvm::Function*> findFunctionInPtsSet(PtsSet, const CallCFGNode&);
	std::vector<const llvm::Function*> resolveCallTarget(const context::Context*, const CallCFGNode&);
	std::vector<PtsSet> collectArgumentPtsSets(const context::Context*, const CallCFGNode&, size_t);
	bool updateParameterPtsSets(const FunctionContext&, const std::vector<PtsSet>&);
	bool evalCallArguments(const context::Context*, const CallCFGNode&, const FunctionContext&);
	void evalExternalCall(const context::Context*, const CallCFGNode&, const FunctionContext&, EvalResult&);
	void evalInternalCall(const context::Context*, const CallCFGNode&, const FunctionContext&, EvalResult&, bool);
	// evalReturn helper
	std::pair<bool, bool> evalReturnValue(const context::Context*, const ReturnCFGNode&, const ProgramPoint&);
	void evalReturn(const context::Context*, const ReturnCFGNode&, const ProgramPoint&, EvalResult&);
	// evalExternalCall helper
	bool evalExternalAlloc(const context::Context*, const CallCFGNode&, const annotation::PointerAllocEffect&);
	void evalMemcpyPtsSet(const MemoryObject*, const std::vector<const MemoryObject*>&, size_t, Store&);
	bool evalMemcpyPointer(const Pointer*, const Pointer*, Store&);
	bool evalMemcpy(const context::Context*, const CallCFGNode&, Store&, const annotation::APosition&, const annotation::APosition&);
	void fillPtsSetWith(const Pointer*, PtsSet, Store&);
	PtsSet evalExternalCopySource(const context::Context*, const CallCFGNode&, Store&, const annotation::CopySource&);
	std::pair<bool, bool> evalExternalCopyDest(const context::Context*, const CallCFGNode&, Store&, const annotation::CopyDest&, PtsSet);
	std::pair<bool, bool> evalExternalCopy(const context::Context*, const CallCFGNode&, Store&, const annotation::PointerCopyEffect&);
	void evalExternalCallByEffect(const context::Context*, const CallCFGNode&, const annotation::PointerEffect&, EvalResult&);

	bool evalAllocNode(const context::Context*, const AllocCFGNode&);
	bool evalCopyNode(const context::Context*, const CopyCFGNode&);
	bool evalOffsetNode(const context::Context*, const OffsetCFGNode&);
	std::pair<bool, bool> evalLoadNode(const context::Context*, const LoadCFGNode&, const Store&);
	bool evalStoreNode(const context::Context*, const StoreCFGNode&, Store&);
	void evalCallNode(const context::Context*, const CallCFGNode&, EvalResult&);
	void evalReturnNode(const context::Context*, const ReturnCFGNode&, EvalResult&);
public:
	TransferFunction(GlobalState& g, const Store* s): globalState(g), localState(s) {}

	EvalResult eval(const ProgramPoint&);
};

}
