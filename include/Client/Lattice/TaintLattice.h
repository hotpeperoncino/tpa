#ifndef TPA_INFOFLOW_LATTICE_H
#define TPA_INFOFLOW_LATTICE_H

#include "Client/Lattice/Lattice.h"

namespace llvm
{
	class raw_ostream;
}

namespace client
{

enum class TaintLattice: uint8_t
{
	Unknown,
	Untainted,
	Tainted,
	Either
};

template<> struct Lattice<TaintLattice>
{
	static LatticeCompareResult compare(TaintLattice lhs, TaintLattice rhs)
	{
		if (lhs == rhs)
			return LatticeCompareResult::Equal;
		else if (rhs == TaintLattice::Either || lhs == TaintLattice::Unknown)
			return LatticeCompareResult::LessThan;
		else if (lhs == TaintLattice::Either || rhs == TaintLattice::Unknown)
			return LatticeCompareResult::GreaterThan;
		else
			return LatticeCompareResult::Incomparable;
	}

	static TaintLattice merge(TaintLattice lhs, TaintLattice rhs)
	{
		auto cmpResult = compare(lhs, rhs);
		switch (cmpResult)
		{
			case LatticeCompareResult::Equal:
			case LatticeCompareResult::GreaterThan:
				return lhs;
			case LatticeCompareResult::LessThan:
				return rhs;
			case LatticeCompareResult::Incomparable:
				return TaintLattice::Either;
		}
	}

	static bool willMergeSmearTaintness(TaintLattice lhs, TaintLattice rhs)
	{
		if (lhs == TaintLattice::Either && rhs == TaintLattice::Either)
			return false;
		else if (lhs == TaintLattice::Unknown || rhs == TaintLattice::Unknown)
			return false;
		else
			return lhs != rhs;
	}
};

namespace taint
{
	llvm::raw_ostream& operator<<(llvm::raw_ostream&, TaintLattice);
}

}

#endif
