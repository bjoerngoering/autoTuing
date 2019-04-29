// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
\defgroup ROSEHPCT_PROFIR  ROSE-HPCT Profile IR
\ingroup ROSEHPCT

This submodule provides a basic abstract representation
of the HPCTookit profile data, which we refer to as the
"profile IR" (or ProfIR). The ProfIR represents a program in
terms of just the following few high-level constructs:
- Programs (executable programs; see \ref RoseCallPathHPCT::Program)
- Loader Modules (e.g., library modules linked into a
program; see \ref RoseCallPathHPCT::Module)
- Files (e.g., submodules of a library or program;
see \ref RoseCallPathHPCT::File)
- Procedures (e.g., functions; see \ref RoseCallPathHPCT::Procedure)
- Loops (see \ref RoseCallPathHPCT::Loop)
- Statements (see \ref RoseCallPathHPCT::Statement)

Each node in the profile IR may have a location (file name
and starting/ending line numbers) and "metrics" (observed
hardware counter values) associated with it.

$Id: profir.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
*/

/*!
 *  \file rosehpct/profir/profir.hh
 *  \brief Intermediate representation for HPCToolkit profile data.
 *  \ingroup ROSEHPCT_PROFIR
 *
 *  $Id: profir.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#if !defined(INC_PROFIR_PROFIR_HH)
//! rosehpct/profir/ir.h included.
#define INC_PROFIR_PROFIR_HH

#include <iostream>
#include <string>
#include <vector>

#include <rosecallpathhpct/util/tree2.hh>

namespace RoseCallPathHPCT
{
  //! \addtogroup ROSECALLPATHHPCT_PROFIR
  //@{

  //! \brief A class derived from Named has a name associated with it.
  class Named
  {
  public:
    //! Initialize with an empty name.
    Named (void);
    //! Initialize with a particular name.
    Named (const std::string& name);
    //! Copy another name.
    Named (const Named& n);
    virtual ~Named (void);

    const std::string& getName (void) const;
    virtual std::string toString(void) const;
    void setName (const std::string& new_name);

  private:
    std::string name_; //!< Name of this object.
  };

  //! Stores an observed and named value.
  class Metric : public Named
  {
  public:
    //! Initialize to a 0 value.
    Metric (void);
    //! Copy another name and value.
    Metric (const Metric& m);
    //! Initialize to a given name and value.
    Metric (const std::string& name, double value);
    virtual ~Metric (void);

//TODO maybe add replaceName here too
    virtual std::string toString() const;  
    double getValue (void) const;
    void setValue (double new_val);

  private:
    double value_; //!< Value of this metric
  };

  /*!
   *  \brief A class derived from Observable can have one or more items
   *  associated with it.
   */
  class Observable
  {
  public:
    Observable (void);
    Observable (const Observable& o);
    virtual ~Observable (void);

    //! \name Provide iterator-like semantics over child metrics.
    /*@{*/
    typedef std::vector<Metric>::iterator MetricIterator;
    typedef std::vector<Metric>::const_iterator ConstMetricIterator;
    
    unsigned int getNumMetrics (void) const;
    MetricIterator beginMetric(void);
    ConstMetricIterator beginMetric (void) const;
    MetricIterator endMetric (void);
    ConstMetricIterator endMetric (void) const;
    /*@}*/

    //! \name Observe metric fields.
    /*@{*/
    const Metric& getMetric (unsigned int i) const;
    Metric& getMetric (unsigned int i);
    double getMetricValue (unsigned int i) const;
    //Get a metric's value by its name
    double getMetricValue (const std::string& metric_name) const;
    std::string getMetricName (unsigned int i) const;
    /*@}*/

   /*!
    *  \brief Add a metric to the current list.
    *  \note Uniqueness of metric names is not enforced.
    */
    void addMetric (const Metric& m);

    /*!
      * replaces all id with the correct name (filename, loadmodule name, metric name, procedure name)
      * 
    */
    virtual void replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs ) { std::cerr << "no name to replace" << std::endl;};

    virtual std::string toString() const;

  private:
    std::vector<Metric> metrics_; //!< Contains child metrics.
  };

  //forward declaration
  class File;
  /*!
   *  \brief A class derived from Located has a source file position
   *  associated with it.
   */
  class Located
  {
  public:
    Located (void);
    Located (const Located& l);
    Located (unsigned int b, unsigned int e, File* f=NULL);
    virtual ~Located (void);

    File* getFileNode(void) const;
    void setFileNode(File* filenode);

    unsigned int getFirstLine (void) const;
    void setFirstLine (unsigned int l);

    unsigned int getLastLine (void) const;
    void setLastLine (unsigned int l);

    void setLines (unsigned int b, unsigned int e);
    virtual std::string toString() const;

  private:
    unsigned int begin_;
    unsigned int end_;
    File * filenode_; // file location info is not complete unless a file name is specified! Liao, 4/27/2009
  };
  /*@}*/

  /* -------------------------------------------------------------- */
  /*!
   *  \name Node classes.
   */
  /*@{*/
  /*!
   *  \brief Abstract base class for profile IR nodes.
   */
  class IRNode : public Named, public Observable
  {
    // DXN: not all IRNodes (e.g. Statement) produced by HPCToolkit profiler have matching SgNode.
    bool hasMatchedSgNode;
  public:
    bool hasMatchingSgNode() const { return hasMatchedSgNode; }
    void setHasMatchingSgNode(bool flag) { hasMatchedSgNode = flag; }
    virtual std::string toString() const;
     /*!
      * replaces all id with the correct name (filename, loadmodule name, metric name, procedure name)
      * 
    */
    virtual void replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs ) { std::cerr << "no name to replace" << std::endl;};
   protected:
    IRNode (void): hasMatchedSgNode(false) {}

 // DQ (12/6/2016): Eliminate warning we want to consider as error: -Wreorder.
 // IRNode (const std::string& name) : hasMatchedSgNode (false), Named (name)  { }
    IRNode (const std::string& name) : Named (name), hasMatchedSgNode (false)  { }

    virtual ~IRNode (void) {}
  };

  //! Program node ("PGM")
  class Program : public IRNode
  {
  public:
    Program (const std::string& name) : IRNode (name) {}
    virtual std::string toString() const;
  };

  //! Section Header node ("SecHeader")
  class SecHeader : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

  //! Metric Table node ("MetricTable")
  class MetricTable : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

  //! Metric Element node ("Metric")
  class MetricElement : public IRNode
  {
      long int id;
      std::string val_type;
      std::string type;
      long int show;
      long int show_percent;
  public:
    MetricElement(long int i, const std::string& name, const std::string& v,
            const std::string& t, long int s, long int sp):
        IRNode(name), id(i), val_type(v), type(t), show(s), show_percent(sp) {}
    virtual std::string toString() const;
    long int getId(void) const;
    void setId(long int new_id);
  };

    //! MetricFormula node ("MetricFormula")
  class MetricFormula : public IRNode
  {
      std::string type;
      std::string formula;

  public:
    MetricFormula(const std::string& t, const std::string& frm):
        type(t), formula(frm) {}
    virtual std::string toString() const;
  };

   //! MetricInfo node ("Info")
  class MetricInfo : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

     //! MetricData node ("NV")
  class MetricData : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

    //! Metric DB Table node ("MetricDBTable")
  class MetricDBTable : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

    //! Load Module Table node ("LoadModuleTable")
  class LoadModuleTable : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

    //! Loader module node ("LoadModule") DONE
  class LoadModule : public IRNode
  {
    long int id;
  public:
    LoadModule (const std::string& name, long int i) : IRNode (name), id(i) {}
    virtual std::string toString() const;
        long int getId(void) const;
    void setId(long int new_id);
  };

    //! File Table node ("FileTable")
  class FileTable : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

    //! File node ("File") DONE
  class File : public IRNode
  {
    long int id;
  public:
    File (const std::string& name, long int i) : IRNode (name) {}

    virtual void replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs )  override;


    virtual std::string toString() const;
    long int getId(void) const;
    void setId(long int new_id);
  };

    //! Procedure Table node ("ProcedureTable")
  class ProcedureTable : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

    //! Procedure node ("Procedure")
  class Procedure : public IRNode
  {
    long int id;
  public:
    Procedure (const std::string& name, long int i);
    virtual std::string toString() const;
    long int getId(void) const;
    void setId(long int new_id);
  };

  //! Section Flat Profile Data node ("SecFlatProfileData") DONE
  class SecFlatProfileData : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

    //! Section Call Path Profile Data node ("SecCallPathProfileData") DONE
  class SecCallPathProfileData : public IRNode
  {
  public:
    virtual std::string toString() const;
  };

  //! Group node ("G")
  class Group : public IRNode
  {
    public:
      virtual std::string toString() const;
  };
  
  //! Proc node ("Pr") 
  class Proc : public IRNode, public Located
  {
      long int id;
      std::string static_scope_id;
      unsigned int line_range;
      std::string load_module;
      std::string file_name;
      std::string name;
      std::string alien;
      std::string hpcstruct_node_id;
  public:
    Proc (long int i, std::string s, unsigned int l, std::string lm, std::string f, std::string n, std::string a, std::string str);

         /*!
      * replaces all id with the correct name (filename, loadmodule name, metric name, procedure name)
      * 
    */
    virtual void replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs )  override;

    virtual std::string toString() const;  
  };

  //! Procedure frame ("PF") DONE
  class ProcFrame: public IRNode, public Located
  {
      long int id;
      std::string static_scope_id;
      unsigned int line_range;
      std::string load_module;
      std::string file_name;
      std::string name;
      std::string hpcstruct_node_id;
  public:
      ProcFrame (long int i, std::string s, unsigned int l, std::string lm, std::string f, std::string n, std::string str)
      ;

         /*!
      * replaces all id with the correct name (filename, loadmodule name, metric name, procedure name)
      * 
    */
    virtual void replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs )  override;

    virtual std::string toString() const;
  };

  //! Call site ("C") DONE
  class CallSite: public IRNode
  {
      long int id;
      std::string static_scope_id;
      unsigned int line_range;
      std::string hpcstruct_node_id;
  public:
    CallSite (long int i, std::string s, unsigned int l, std::string str): id(i), static_scope_id(s), line_range(l), hpcstruct_node_id(str)  {}
    virtual std::string toString() const;
  };

  //! Loop node ("L") DONE
  class Loop : public IRNode  , public Located
  {
    long int id;
    std::string static_scope_id;
    std::string file_name;
    std::string hpcstruct_node_id;
  public:
    Loop (long int i, std::string s, unsigned int l, std::string f, std::string str);

         /*!
      * replaces all id with the correct name (filename, loadmodule name, metric name, procedure name)
      * 
    */
    virtual void replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs )  override;

    virtual std::string toString() const;  
  };

  //! Statement node ("S") DONE
  class Statement : public IRNode, public Located
  {
    long int id;
    std::string static_scope_id;
    std::string hpcstruct_node_id;
   public:
    typedef long int id_t;

    Statement (void);
    Statement (const Statement& s);
    Statement (id_t i, unsigned int l, std::string s, std::string str);

    ~Statement (void);

    virtual void replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs ) override;

    id_t getId (void) const;
    std::string getStaticScopeId (void) const;
    std::string getHpcstructNodeId (void) const;
    void setId (id_t new_id);
    void setStaticScopeId ( std::string new_static_scope_id);
    void setHpcstructNodeId(std::string new_hpcstruct_node_id);
    virtual std::string toString() const;  
  };

  //! IR tree representation
  typedef Trees::Tree<IRNode *> IRTree_t;

  /*@}*/

  //@}
  
  std::string replaceName(std::string &replaced_name, const std::map<std::string, std::string> &replace_map);
  
}//end namespace ROSECallPathHPCT

#endif

/* eof */
