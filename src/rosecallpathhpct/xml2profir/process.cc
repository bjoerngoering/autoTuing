// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/xml2profir/process.cc
 *  \brief Process command-line options and files.
 *  \ingroup ROSEHPCT_XML2PROFIR
 *
 *  Command-line processing routines use Brian Gunney's SLA library.
 *
 *  $Id: process.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#include "sage3basic.h"
//#include "rosehpct/xml2profir/xml2profir.hh"
#include "sla.h"
//#include "rosehpct/util/general.hh"
#include "rosecallpathhpct/rosecallpathhpct.hh"

using namespace RoseCallPathHPCT;

/* ------------------------------------------------------------ */

/*!
 *  \brief Abstract Class to translate id items to full names.
 */
class TranslatorInterface : public Trees::PreTraversal<IRTree_t, false>
{
public:
  typedef std::map<std::string, std::string> RenameMap_t;

  /*! \brief Initialize with a specific translation map */
  TranslatorInterface (const RenameMap_t& metrictab, 
  const RenameMap_t& filetab,
  const RenameMap_t& moduletab,
  const RenameMap_t& proctab);
  virtual ~TranslatorInterface (void);

  /*! \brief Returns 'true' if any paths have been translated. */
  bool didChange (void) const;

protected:
  /*! \brief Only visits observable nodes */
  virtual void visit (TreeParamPtr_t tree);

  /*! \brief Observable-node specific translator */
  virtual void visitObservable (Observable* node);

private:
  /*! \brief Associative map of item - name translations */
  RenameMap_t metrictab_;
  RenameMap_t filetab_;
  RenameMap_t moduletab_;
  RenameMap_t proctab_;

  /*! \brief True <==> any item names were translated during traversal */
  bool did_change_;
};

/* ------------------------------------------------------------ */

/*!
 *  \brief Abstract Class to collect item names.
 */
class CollectorInterface : public Trees::PreTraversal<IRTree_t>
{
public:
  //! Map of items to values
  typedef std::map<std::string, unsigned int> ItemMap_t;

  CollectorInterface (ItemMap_t& metric_store);

protected:
  //! Visits Observable nodes only.
  virtual void visit (TreeParamPtr_t tree);

  //! Observable node-specific visit.
  virtual void visitObservable (Observable* node);

private:
  //! List of item names.
  ItemMap_t& items_;
};

/* ------------------------------------------------------------ */

using namespace std;
using namespace GenUtil;
using namespace XMLUtil;

/* ---------------------------------------------------------------- */

ProgramTreeList_t
RoseCallPathHPCT::loadXMLTrees (const FilenameList_t& filenames)
{
  ProgramTreeList_t trees;
  for (FilenameList_t::const_iterator i = filenames.begin ();
       i != filenames.end ();
       ++i)
    {
      cerr << "[Reading profile: " << (*i) << "]" << endl;
      XMLParser p (*i);
      if (p.getDoc () != NULL) // valid file
	{ 
    // Collect all metrics, loadmodules, files, procedure available in the xml input

	  map<std::string, string> mettab = XMLQuery::getMetricTable (p.getDoc ());
    map<std::string, string> filetab = XMLQuery::getFileTable (p.getDoc ());
    map<std::string, string> lmtab = XMLQuery::getLoadModuleTable (p.getDoc ());
    map<std::string, string> proctab = XMLQuery::getProcedureTable (p.getDoc ());
    if(enable_debug) {
    auto output_maps = [](std::map<std::string, std::string> input_map, std::string&& name){
      std::cout << name << std::endl;
      for (auto entry : input_map) {
        std::cout << entry.first << ": " << entry.second << std::endl;
      }
    };
    output_maps(mettab, "Metrics");
    output_maps(filetab, "Files");
    output_maps(lmtab, "Load modules");
    output_maps(proctab, "Procedures");
    }
	  IRTree_t* prog_root = convertXMLToProfIR (p.getDoc ());
	  translateIdsToNames (prog_root, mettab, filetab, lmtab, proctab);
	  trees.push_back (prog_root);
	}
    }
  return trees;
}

/* ---------------------------------------------------------------- */

/*!
 *  \brief Class to translate paths in filenames embedded in an
 *  RoseCallPathHPCT-IR tree.It also fixes up the File Node pointer for each located node
 *
 *  Pre-order traversal, isConst = false
 */
class PathTranslator : public Trees::PreTraversal<IRTree_t, false>
{
public:
  /*! \brief Initialize with a specific path translation map */
  PathTranslator (const EquivPathMap_t& eqpaths);
  virtual ~PathTranslator (void);

  /*! \brief Returns 'true' if any paths have been translated. */
  bool didChange (void) const;
  static double wallclk_sum;
private:
  /*! \brief Only translates paths for File nodes */
  virtual void visit (TreeParamPtr_t tree);

  /*! \brief File-node specific translation */
  void visitFile (File* node) ;

  /*! \brief Associative map of paths and their translations */
  EquivPathMap_t eqpaths_;

  /*! \brief True <==> any paths were changed during traversal */
  bool did_change_;

  /*! \brief The current file node's value */
  File* file_;
};

double PathTranslator::wallclk_sum=0.0;

PathTranslator::PathTranslator (const EquivPathMap_t& eqpaths)
  : eqpaths_ (eqpaths), did_change_ (false)
{
}

PathTranslator::~PathTranslator (void)
{
}

bool
PathTranslator::didChange (void) const
{
  return did_change_;
}

void
PathTranslator::visit (TreeParamPtr_t tree)
{
  File* filenode = dynamic_cast<File *> (tree->value);
  if (filenode != NULL)
  {
    // Save file nodes for later use
    RoseCallPathHPCT::profFileNodes_.insert(filenode);
    visitFile (filenode);
  }
  else
  {  // for other located nodes, fix up file node pointer if necessary
     Located * locatedNode=  dynamic_cast<Located*> (tree->value);
     if (locatedNode)
     {
       Statement* s = dynamic_cast<Statement*> (tree->value);
       // Save non-scope statement nodes for later use
       if (s)
       {
         RoseCallPathHPCT::profStmtNodes_.insert(s);
         //Accumulate wall clock metrics so it can be used later on to calculate percentage
         if (!gprof_only) // gprof provides the percentage so no need to calculate here.
           wallclk_sum += s->getMetricValue(m_wallclock); 
       }
       if (locatedNode->getFileNode()==NULL)
       {
         ROSE_ASSERT(file_ != NULL);
         locatedNode->setFileNode(file_);
       }
       else
       {// if it has a file node pointer, double check it here
         ROSE_ASSERT(file_== locatedNode->getFileNode());
       }
     }
  }
}

void
PathTranslator::visitFile (File* node) 
{
  file_ = node; // save this node
  string filename = node->getName ();
  // find a matching entry for the eqpaths
#if 1 
  // Liao, 4/28/2009 we extend this function to replace a root dir of a source tree to another
  // for example: /root1/smg2000/file1 --> /root2/smg2000/file1
  // The original function only deals with full path replacement, not a portion of it(root path),
  // which is less useful in practice.
   EquivPathMap_t::const_iterator iter = eqpaths_.begin(); 
   for (;iter!=eqpaths_.end(); iter++)
   {
     string oldpath = iter->first;
     string newpath = iter->second;
     //replace the matching old root path of a file with the new root path
     unsigned int pos1= filename.find(oldpath);
     if (pos1==0) // ensure it is the root path
     {
       if (enable_debug)
         cout<<"Replacing a file path from:"<<filename;
       filename.replace(pos1,oldpath.size(), newpath);
       if (enable_debug)
       {
         cout<<" to: "<<filename<<endl;
       }
       node->setName (filename);  
     }
   }
#else  
  string dirname = getDirname (filename);
  EquivPathMap_t::const_iterator ep = eqpaths_.find (dirname);
  if (ep != eqpaths_.end ())
  {
    // replace the file path with the second path of the entry
    string basename = getBaseFilename (filename);
    string new_filename = ep->second + "/" + basename;
    node->setName (new_filename);
  }
#endif  
}

/* ---------------------------------------------------------------- */
// A class to calculate wall clock percentage for each statement Profile IR node with a wallclock metric
class PercentageCalculator: public Trees::PreTraversal <IRTree_t, false> 
{
public:
  PercentageCalculator(double sum):wallclock_sum(sum){};
  virtual ~PercentageCalculator(void){};

private:
  virtual void visit (TreeParamPtr_t tree);
   double wallclock_sum;
};

void PercentageCalculator::visit(TreeParamPtr_t tree)
{
  Statement* s = dynamic_cast<Statement*> (tree->value);
  if (s)
  {
    double w = s->getMetricValue(m_wallclock);
    s->addMetric(Metric(m_percentage,w/wallclock_sum));
  }

}

bool
RoseCallPathHPCT::postProcessingProfIR(IRTree_t* root, const EquivPathMap_t& eqpaths)
{
  PathTranslator xlator (eqpaths);
  xlator.traverse (root);
  if (!gprof_only) // gprof already provides such information
  {
    //if (enable_debug )
    {
      cout<<"RoseCallPathHPCT::PostProcessingProfIR() accumulated wallclock is:"<<PathTranslator::wallclk_sum<<endl;
    }
    // DXN: wallclock_sum is 0 if there is no WALLCLOCK metric.
    //      Only compute percentage if wallclk_sum != 0.
    if (PathTranslator::wallclk_sum > 0)
    {
      PercentageCalculator clator(PathTranslator::wallclk_sum);
      clator.traverse(root);
    }
  }
  return xlator.didChange ();
}

/* ---------------------------------------------------------------- */

TranslatorInterface::TranslatorInterface (const RenameMap_t& metrictab, 
  const RenameMap_t& filetab,
  const RenameMap_t& moduletab,
  const RenameMap_t& proctab)
  : metrictab_ (metrictab), filetab_(filetab), moduletab_(moduletab), proctab_(proctab), did_change_ (false)
{
}

TranslatorInterface::~TranslatorInterface (void)
{
}

bool
TranslatorInterface::didChange (void) const
{
  return did_change_;
}

void
TranslatorInterface::visit (TreeParamPtr_t tree)
{
  Observable* node = dynamic_cast<Observable *> (tree->value);
  if (node != NULL)
    visitObservable (node);
}

void
TranslatorInterface::visitObservable (Observable* node)
{
  //std::cout << "TranslatorInterface::visitObservable: node name: " <<  node->toString() << std::endl;
  node->replaceNames(metrictab_, filetab_, moduletab_, proctab_);
  if (node->getNumMetrics () <= 0) return;

  for (Observable::MetricIterator i = node->beginMetric ();
       i != node->endMetric ();
       ++i)
    {
      RenameMap_t::iterator m = metrictab_.find (i->getName ());
      if (m != metrictab_.end ())
	{
      i->setName (m->second);
	  did_change_ = true;
	}
    }

#if 0 // leave preprocessor block in for debugging later
  for (Observable::ConstMetricIterator i = node->beginMetric ();
       i != node->endMetric ();
       ++i)
    {
      cerr << "New metric name: " << i->getName () << endl;
    }
#endif
}

bool RoseCallPathHPCT::translateIdsToNames(IRTree_t * root, const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs ){
  TranslatorInterface xlator (metrics, files, modules, procs);
  xlator.traverse (root);
  return xlator.didChange ();
}

/* ---------------------------------------------------------------- */

CollectorInterface::CollectorInterface (ItemMap_t& item_store)
  : items_ (item_store)
{
}

void
CollectorInterface::visit (TreeParamPtr_t tree)
{
  Observable* node = dynamic_cast<Observable *> (tree->value);
  if (node != NULL)
    visitObservable (node);
}

void
CollectorInterface::visitObservable (Observable* node)
{
  if (node->getNumMetrics () <= 0) return;

  for (Observable::MetricIterator i = node->beginMetric ();
       i != node->endMetric ();
       ++i)
    {
      string key = i->getName ();
      ItemMap_t::iterator m = items_.find (key);
      if (m == items_.end ()) // first encounter
	items_[key] = 1;
      else
	items_[key] += 1;
    }

#if 0 // leave preprocessor block in for debugging later
  for (Observable::ConstMetricIterator i = node->beginMetric ();
       i != node->endMetric ();
       ++i)
    {
      cerr << "New metric name: " << i->getName () << endl;
    }
#endif
}

void
RoseCallPathHPCT::collectMetricNames (const IRTree_t* root,
                              map<string, unsigned int>& metrics)
{
  CollectorInterface collection (metrics);
  collection.traverse (root);
}

/* eof */
