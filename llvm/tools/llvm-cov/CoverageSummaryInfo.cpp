//===- CoverageSummaryInfo.cpp - Coverage summary for function/file -------===//
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

#include "CoverageSummaryInfo.h"

using namespace llvm;
using namespace coverage;

static void sumBranches(size_t &NumBranches, size_t &CoveredBranches,
                        const ArrayRef<CountedRegion> &Branches) {
  for (const auto &BR : Branches) {
    // Skip folded branches.
    if (BR.Folded)
      continue;

    // "True" Condition Branches.
    ++NumBranches;
    if (BR.ExecutionCount > 0)
      ++CoveredBranches;
    // "False" Condition Branches.
    ++NumBranches;
    if (BR.FalseExecutionCount > 0)
      ++CoveredBranches;
  }
}

static void sumBranchExpansions(size_t &NumBranches, size_t &CoveredBranches,
                                const CoverageMapping &CM,
                                ArrayRef<ExpansionRecord> Expansions) {
  for (const auto &Expansion : Expansions) {
    auto CE = CM.getCoverageForExpansion(Expansion);
    sumBranches(NumBranches, CoveredBranches, CE.getBranches());
    sumBranchExpansions(NumBranches, CoveredBranches, CM, CE.getExpansions());
  }
}

// NOTE Aggregate decisions in a function instance

static std::pair<size_t, size_t>
sumMCDCPairs(const ArrayRef<MCDCRecord> &Records) {
  size_t NumPairs = 0, CoveredPairs = 0;
  for (const auto &Record : Records) {
    const auto NumConditions = Record.getNumConditions();
    for (unsigned C = 0; C < NumConditions; C++) {
      if (!Record.isCondFolded(C))
        ++NumPairs;
      if (Record.isConditionIndependencePairCovered(C))
        ++CoveredPairs;
    }
  }
  return {NumPairs, CoveredPairs};
}

// Rewrite the above function so that it passes information back to its caller
// through references

static void
sumMCDCPairs(const ArrayRef<MCDCRecord> &Records,
             size_t &NumPairs, size_t &CoveredPairs, size_t &NumDecisions,
             size_t &NumDecisions2, size_t &NumDecisions3, size_t &NumDecisions4,
             size_t &NumDecisions5, size_t &NumDecisions6) {
  NumPairs = 0;
  CoveredPairs = 0;
  NumDecisions = 0;
  for (const auto &Record : Records) {
    ++NumDecisions;
    const auto NumConditions = Record.getNumConditions();
    switch(NumConditions) {
    case 2:
      ++NumDecisions2;
      break;
    case 3:
      ++NumDecisions3;
      break;
    case 4:
      ++NumDecisions4;
      break;
    case 5:
      ++NumDecisions5;
      break;
    case 6:
      ++NumDecisions6;
      break;
    default:
      assert(false && "Encountered a decision whose num of conditions isn't {2, 3, 4, 5, 6}");
    }
    for (unsigned C = 0; C < NumConditions; C++) {
      if (!Record.isCondFolded(C))
        ++NumPairs;
      if (Record.isConditionIndependencePairCovered(C))
        ++CoveredPairs;
    }
  }
}

FunctionCoverageSummary
FunctionCoverageSummary::get(const CoverageMapping &CM, // NOTE A function instantiation
                             const coverage::FunctionRecord &Function) {
  // Compute the region coverage.
  size_t NumCodeRegions = 0, CoveredRegions = 0;
  for (auto &CR : Function.CountedRegions) {
    if (CR.Kind != CounterMappingRegion::CodeRegion)
      continue;
    ++NumCodeRegions;
    if (CR.ExecutionCount != 0)
      ++CoveredRegions;
  }

  // Compute the line coverage
  size_t NumLines = 0, CoveredLines = 0;
  CoverageData CD = CM.getCoverageForFunction(Function);
  for (const auto &LCS : getLineCoverageStats(CD)) {
    if (!LCS.isMapped())
      continue;
    ++NumLines;
    if (LCS.getExecutionCount())
      ++CoveredLines;
  }

  // Compute the branch coverage, including branches from expansions.
  size_t NumBranches = 0, CoveredBranches = 0;
  sumBranches(NumBranches, CoveredBranches, CD.getBranches());
  sumBranchExpansions(NumBranches, CoveredBranches, CM, CD.getExpansions());

  size_t NumPairs = 0, CoveredPairs = 0;
  size_t NumDecisions = 0;
  size_t NumDecisions2 = 0;
  size_t NumDecisions3 = 0;
  size_t NumDecisions4 = 0;
  size_t NumDecisions5 = 0;
  size_t NumDecisions6 = 0;
  // std::tie(NumPairs, CoveredPairs) = sumMCDCPairs(CD.getMCDCRecords());
  sumMCDCPairs(CD.getMCDCRecords(), NumPairs, CoveredPairs, NumDecisions,
                                    NumDecisions2, NumDecisions3, NumDecisions4,
                                    NumDecisions5, NumDecisions6);

  return FunctionCoverageSummary(
      Function.Name, Function.ExecutionCount,
      RegionCoverageInfo(CoveredRegions, NumCodeRegions),
      LineCoverageInfo(CoveredLines, NumLines),
      BranchCoverageInfo(CoveredBranches, NumBranches),
      MCDCCoverageInfo(CoveredPairs, NumPairs, NumDecisions,
                                     NumDecisions2,NumDecisions3,NumDecisions4,
                                     NumDecisions5,NumDecisions6));
}

FunctionCoverageSummary
FunctionCoverageSummary::get(const InstantiationGroup &Group,
                             ArrayRef<FunctionCoverageSummary> Summaries) { // NOTE Merge all function instances
  std::string Name;
  if (Group.hasName()) {
    Name = std::string(Group.getName());
  } else {
    llvm::raw_string_ostream OS(Name);
    OS << "Definition at line " << Group.getLine() << ", column "
       << Group.getColumn();
  }

  FunctionCoverageSummary Summary(Name);
  Summary.ExecutionCount = Group.getTotalExecutionCount();
  Summary.RegionCoverage = Summaries[0].RegionCoverage;
  Summary.LineCoverage = Summaries[0].LineCoverage;
  Summary.BranchCoverage = Summaries[0].BranchCoverage;
  Summary.MCDCCoverage = Summaries[0].MCDCCoverage;
  for (const auto &FCS : Summaries.drop_front()) {
    Summary.RegionCoverage.merge(FCS.RegionCoverage);
    Summary.LineCoverage.merge(FCS.LineCoverage);
    Summary.BranchCoverage.merge(FCS.BranchCoverage);
    Summary.MCDCCoverage.merge(FCS.MCDCCoverage);
  }
  return Summary;
}
