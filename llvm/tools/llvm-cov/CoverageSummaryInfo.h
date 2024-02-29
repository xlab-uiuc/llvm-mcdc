//===- CoverageSummaryInfo.h - Coverage summary for function/file ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// These structures are used to represent code coverage metrics
// for functions/files.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_COV_COVERAGESUMMARYINFO_H
#define LLVM_COV_COVERAGESUMMARYINFO_H

#include "llvm/ProfileData/Coverage/CoverageMapping.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {

/// Provides information about region coverage for a function/file.
class RegionCoverageInfo {
  /// The number of regions that were executed at least once.
  size_t Covered;

  /// The total number of regions in a function/file.
  size_t NumRegions;

public:
  RegionCoverageInfo() : Covered(0), NumRegions(0) {}

  RegionCoverageInfo(size_t Covered, size_t NumRegions)
      : Covered(Covered), NumRegions(NumRegions) {
    assert(Covered <= NumRegions && "Covered regions over-counted");
  }

  RegionCoverageInfo &operator+=(const RegionCoverageInfo &RHS) {
    Covered += RHS.Covered;
    NumRegions += RHS.NumRegions;
    return *this;
  }

  void merge(const RegionCoverageInfo &RHS) {
    Covered = std::max(Covered, RHS.Covered);
    NumRegions = std::max(NumRegions, RHS.NumRegions);
  }

  size_t getCovered() const { return Covered; }

  size_t getNumRegions() const { return NumRegions; }

  bool isFullyCovered() const { return Covered == NumRegions; }

  double getPercentCovered() const {
    assert(Covered <= NumRegions && "Covered regions over-counted");
    if (NumRegions == 0)
      return 0.0;
    return double(Covered) / double(NumRegions) * 100.0;
  }
};

/// Provides information about line coverage for a function/file.
class LineCoverageInfo {
  /// The number of lines that were executed at least once.
  size_t Covered;

  /// The total number of lines in a function/file.
  size_t NumLines;

public:
  LineCoverageInfo() : Covered(0), NumLines(0) {}

  LineCoverageInfo(size_t Covered, size_t NumLines)
      : Covered(Covered), NumLines(NumLines) {
    assert(Covered <= NumLines && "Covered lines over-counted");
  }

  LineCoverageInfo &operator+=(const LineCoverageInfo &RHS) {
    Covered += RHS.Covered;
    NumLines += RHS.NumLines;
    return *this;
  }

  void merge(const LineCoverageInfo &RHS) {
    Covered = std::max(Covered, RHS.Covered);
    NumLines = std::max(NumLines, RHS.NumLines);
  }

  size_t getCovered() const { return Covered; }

  size_t getNumLines() const { return NumLines; }

  bool isFullyCovered() const { return Covered == NumLines; }

  double getPercentCovered() const {
    assert(Covered <= NumLines && "Covered lines over-counted");
    if (NumLines == 0)
      return 0.0;
    return double(Covered) / double(NumLines) * 100.0;
  }
};

/// Provides information about branches coverage for a function/file.
class BranchCoverageInfo {
  /// The number of branches that were executed at least once.
  size_t Covered;

  /// The total number of branches in a function/file.
  size_t NumBranches;

public:
  BranchCoverageInfo() : Covered(0), NumBranches(0) {}

  BranchCoverageInfo(size_t Covered, size_t NumBranches)
      : Covered(Covered), NumBranches(NumBranches) {
    assert(Covered <= NumBranches && "Covered branches over-counted");
  }

  BranchCoverageInfo &operator+=(const BranchCoverageInfo &RHS) {
    Covered += RHS.Covered;
    NumBranches += RHS.NumBranches;
    return *this;
  }

  void merge(const BranchCoverageInfo &RHS) {
    Covered = std::max(Covered, RHS.Covered);
    NumBranches = std::max(NumBranches, RHS.NumBranches);
  }

  size_t getCovered() const { return Covered; }

  size_t getNumBranches() const { return NumBranches; }

  bool isFullyCovered() const { return Covered == NumBranches; }

  double getPercentCovered() const {
    assert(Covered <= NumBranches && "Covered branches over-counted");
    if (NumBranches == 0)
      return 0.0;
    return double(Covered) / double(NumBranches) * 100.0;
  }
};

/// Provides information about MC/DC coverage for a function/file.
class MCDCCoverageInfo {
  /// The number of Independence Pairs that were covered.
  size_t CoveredPairs;

  /// The total number of Independence Pairs in a function/file.
  size_t NumPairs;

  size_t NumDecisions;

  size_t NumDecisions2;
  size_t NumDecisions3;
  size_t NumDecisions4;
  size_t NumDecisions5;
  size_t NumDecisions6;

  size_t NumPairsAll;

  size_t NumDecisionsWithAtLeastTwoNonConstCond;

public:
  MCDCCoverageInfo() : CoveredPairs(0), NumPairs(0), NumDecisions(0),
                       NumDecisions2(0), NumDecisions3(0),  NumDecisions4(0), NumDecisions5(0), NumDecisions6(0),
                       NumPairsAll(0),
                       NumDecisionsWithAtLeastTwoNonConstCond(0) {}

  MCDCCoverageInfo(size_t CoveredPairs, size_t NumPairs, size_t NumDecisions,
                   size_t NumDecisions2, size_t NumDecisions3, size_t NumDecisions4,
                   size_t NumDecisions5, size_t NumDecisions6,
                   size_t NumPairsAll,
                   size_t NumDecisionsWithAtLeastTwoNonConstCond)
      : CoveredPairs(CoveredPairs), NumPairs(NumPairs), NumDecisions(NumDecisions),
        NumDecisions2(NumDecisions2), NumDecisions3(NumDecisions3), NumDecisions4(NumDecisions4),
        NumDecisions5(NumDecisions5), NumDecisions6(NumDecisions6),
        NumPairsAll(NumPairsAll),
        NumDecisionsWithAtLeastTwoNonConstCond(NumDecisionsWithAtLeastTwoNonConstCond) {
    assert(CoveredPairs <= NumPairs && "Covered pairs over-counted");
  }

  MCDCCoverageInfo &operator+=(const MCDCCoverageInfo &RHS) {
    CoveredPairs += RHS.CoveredPairs;
    NumPairs += RHS.NumPairs;
    NumDecisions += RHS.NumDecisions;
    NumDecisions2 += RHS.NumDecisions2;
    NumDecisions3 += RHS.NumDecisions3;
    NumDecisions4 += RHS.NumDecisions4;
    NumDecisions5 += RHS.NumDecisions5;
    NumDecisions6 += RHS.NumDecisions6;
    NumDecisionsWithAtLeastTwoNonConstCond += RHS.NumDecisionsWithAtLeastTwoNonConstCond;
    NumPairsAll += RHS.NumPairsAll;
    return *this;
  }

  // NOTE Merge function instances
  void merge(const MCDCCoverageInfo &RHS, bool &SomethingWrong) {

    SomethingWrong = false;

    CoveredPairs = std::max(CoveredPairs, RHS.CoveredPairs);

    // It's possible that the numbers for a function instance are zero when an
    // inline function is defined but never *invoked* anywhere else. Note being
    // "invoked" is different from being "executed". For example
    //
    // inline.h
    //
    //   static inline void func(void) { int x; x && x; }
    //
    // foo.c
    //
    //   #include "inline.h"
    //   void foo(void) { if (0) func(); }
    //
    // bar.c
    //
    //   #include "inline.h"
    //   void bar(void) { }
    //
    // In the instance in foo.c, func() is not executed: 1 decision, 2 conditions
    // In the instance in bar.c, func() is not invoked:  0 decision, 0 condition

    if (NumPairs && RHS.NumPairs)
      // assert(NumPairs == RHS.NumPairs);
      if (NumPairs != RHS.NumPairs) {
        SomethingWrong = true;
        printf("#cond unexpected: %lu %lu\n", NumPairs, RHS.NumPairs);
      }
    if (NumDecisions && RHS.NumDecisions)
      // assert(NumDecisions == RHS.NumDecisions);
      if (NumDecisions != RHS.NumDecisions) {
        SomethingWrong = true;
        printf("#decision unexpected: %lu %lu\n", NumDecisions, RHS.NumDecisions);
      }

    NumPairs = std::max(NumPairs, RHS.NumPairs);
    NumDecisions = std::max(NumDecisions, RHS.NumDecisions);

    NumDecisions2 = std::max(NumDecisions2, RHS.NumDecisions2);
    NumDecisions3 = std::max(NumDecisions3, RHS.NumDecisions3);
    NumDecisions4 = std::max(NumDecisions4, RHS.NumDecisions4);
    NumDecisions5 = std::max(NumDecisions5, RHS.NumDecisions5);
    NumDecisions6 = std::max(NumDecisions6, RHS.NumDecisions6);

    NumDecisionsWithAtLeastTwoNonConstCond = std::max(NumDecisionsWithAtLeastTwoNonConstCond, RHS.NumDecisionsWithAtLeastTwoNonConstCond);

    NumPairsAll = std::max(NumPairsAll, RHS.NumPairsAll);
  }

  size_t getCoveredPairs() const { return CoveredPairs; }

  size_t getNumPairs() const { return NumPairs; }

  size_t getNumDecisions() const { return NumDecisions; }

  size_t getNumDecisions2() const { return NumDecisions2; }
  size_t getNumDecisions3() const { return NumDecisions3; }
  size_t getNumDecisions4() const { return NumDecisions4; }
  size_t getNumDecisions5() const { return NumDecisions5; }
  size_t getNumDecisions6() const { return NumDecisions6; }

  size_t getNumDecisionsWithAtLeastTwoNonConstCond() const { return NumDecisionsWithAtLeastTwoNonConstCond; }

  size_t getNumPairsAll() const { return NumPairsAll; }

  bool isFullyCovered() const { return CoveredPairs == NumPairs; }

  double getPercentCovered() const {
    assert(CoveredPairs <= NumPairs && "Covered pairs over-counted");
    if (NumPairs == 0)
      return 0.0;
    return double(CoveredPairs) / double(NumPairs) * 100.0;
  }
};

/// Provides information about function coverage for a file.
class FunctionCoverageInfo {
  /// The number of functions that were executed.
  size_t Executed;

  /// The total number of functions in this file.
  size_t NumFunctions;

public:
  FunctionCoverageInfo() : Executed(0), NumFunctions(0) {}

  FunctionCoverageInfo(size_t Executed, size_t NumFunctions)
      : Executed(Executed), NumFunctions(NumFunctions) {}

  FunctionCoverageInfo &operator+=(const FunctionCoverageInfo &RHS) {
    Executed += RHS.Executed;
    NumFunctions += RHS.NumFunctions;
    return *this;
  }

  void addFunction(bool Covered) {
    if (Covered)
      ++Executed;
    ++NumFunctions;
  }

  size_t getExecuted() const { return Executed; }

  size_t getNumFunctions() const { return NumFunctions; }

  bool isFullyCovered() const { return Executed == NumFunctions; }

  double getPercentCovered() const {
    assert(Executed <= NumFunctions && "Covered functions over-counted");
    if (NumFunctions == 0)
      return 0.0;
    return double(Executed) / double(NumFunctions) * 100.0;
  }
};

/// A summary of function's code coverage.
struct FunctionCoverageSummary {
  std::string Name;
  uint64_t ExecutionCount;
  RegionCoverageInfo RegionCoverage;
  LineCoverageInfo LineCoverage;
  BranchCoverageInfo BranchCoverage;
  MCDCCoverageInfo MCDCCoverage;

  FunctionCoverageSummary(const std::string &Name)
      : Name(Name), ExecutionCount(0) {}

  FunctionCoverageSummary(const std::string &Name, uint64_t ExecutionCount,
                          const RegionCoverageInfo &RegionCoverage,
                          const LineCoverageInfo &LineCoverage,
                          const BranchCoverageInfo &BranchCoverage,
                          const MCDCCoverageInfo &MCDCCoverage)
      : Name(Name), ExecutionCount(ExecutionCount),
        RegionCoverage(RegionCoverage), LineCoverage(LineCoverage),
        BranchCoverage(BranchCoverage), MCDCCoverage(MCDCCoverage) {}

  /// Compute the code coverage summary for the given function coverage
  /// mapping record.
  static FunctionCoverageSummary get(const coverage::CoverageMapping &CM,
                                     const coverage::FunctionRecord &Function);

  /// Compute the code coverage summary for an instantiation group \p Group,
  /// given a list of summaries for each instantiation in \p Summaries.
  static FunctionCoverageSummary
  get(const coverage::InstantiationGroup &Group,
      ArrayRef<FunctionCoverageSummary> Summaries, bool &SomethingWrong);
};

/// A summary of file's code coverage.
struct FileCoverageSummary {
  StringRef Name;
  RegionCoverageInfo RegionCoverage;
  LineCoverageInfo LineCoverage;
  BranchCoverageInfo BranchCoverage;
  MCDCCoverageInfo MCDCCoverage;
  FunctionCoverageInfo FunctionCoverage;
  FunctionCoverageInfo InstantiationCoverage;

  FileCoverageSummary() = default;
  FileCoverageSummary(StringRef Name) : Name(Name) {}

  FileCoverageSummary &operator+=(const FileCoverageSummary &RHS) {
    RegionCoverage += RHS.RegionCoverage;
    LineCoverage += RHS.LineCoverage;
    FunctionCoverage += RHS.FunctionCoverage;
    BranchCoverage += RHS.BranchCoverage;
    MCDCCoverage += RHS.MCDCCoverage;
    InstantiationCoverage += RHS.InstantiationCoverage;
    return *this;
  }

  void addFunction(const FunctionCoverageSummary &Function) { // NOTE Add function (all instances merged)
    RegionCoverage += Function.RegionCoverage;
    LineCoverage += Function.LineCoverage;
    BranchCoverage += Function.BranchCoverage;
    MCDCCoverage += Function.MCDCCoverage;
    FunctionCoverage.addFunction(/*Covered=*/Function.ExecutionCount > 0);
  }

  void addInstantiation(const FunctionCoverageSummary &Function) {
    InstantiationCoverage.addFunction(/*Covered=*/Function.ExecutionCount > 0);
  }
};

/// A cache for demangled symbols.
struct DemangleCache {
  StringMap<std::string> DemangledNames;

  /// Demangle \p Sym if possible. Otherwise, just return \p Sym.
  StringRef demangle(StringRef Sym) const {
    const auto DemangledName = DemangledNames.find(Sym);
    if (DemangledName == DemangledNames.end())
      return Sym;
    return DemangledName->getValue();
  }
};

} // namespace llvm

#endif // LLVM_COV_COVERAGESUMMARYINFO_H
