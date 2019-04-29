
#include "rose.h"
#include "autoTuningSupport.h"
#include <algorithm>

using namespace std;
using namespace GenUtil;
using namespace RoseHPCT;
using namespace Outliner;
using namespace autoTuning;

int main(int argc, char * argv[]) 
{
  vector<string> argvList (argv, argv+argc);
  // recognize options for autotuning
  autotuning_command_processing(argvList);
  // Prepare AST with performance metrics attached.
  // -------------------------------------------------
  // Read into the profiling result files
  RoseHPCT::ProgramTreeList_t profiles = RoseHPCT::loadProfilingFiles(argvList);
  // Create the AST tree
  SgProject * project = frontend(argvList);
  //Attach metrics to AST , last parameter is for verbose
  RoseHPCT::attachMetrics(profiles, project,project->get_verbose()>0);

  // Performance code triage based on annotated AST
  // -------------------------------------------------
  std::set<SgLocatedNode*> candidateSgStmts;
  std::set<SgForStatement*> candidateSgLoops;
  code_triage (candidateSgStmts, candidateSgLoops);
  if (!triage_only)
  {
    // Finally outline the target loops one by one
    // -------------------------------------------------
    // What if the loops are nested together?
    // We rank candidates in descending order so the most costly (inner most loops) will be outlined first.
    // It is an implicit way to do bottom up traversal.
    // TODO: A safer way is to do a bottom up traversal and outline matched loops one by one
    for (std::set <SgForStatement*>::iterator iter = candidateSgLoops.begin(); iter!=candidateSgLoops.end(); iter++)
    {
      outline(*iter);
    }
  }
  // -------------------------------------------------------
  AstTests::runAllTests(project);
  //project->unparse();
  return backend (project);
}
