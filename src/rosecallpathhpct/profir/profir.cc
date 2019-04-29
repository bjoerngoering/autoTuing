// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/profir/profir.cc
 *  \brief Intermediate representation for HPCToolkit profile data.
 *  \ingroup ROSEHPCT_PROFIR
 *
 *  $Id: profir.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#include "rosecallpathhpct/profir/profir.hh"
#include <boost/lexical_cast.hpp> // DXN: converting int to string

using namespace std;
using namespace RoseCallPathHPCT;
#include <iostream>
#include <sstream>
#include <string>
#include <cassert>

/* ---------------------------------------------------------------- */

RoseCallPathHPCT::Named::Named(void)
    : name_("")
{
}

RoseCallPathHPCT::Named::Named(const string &name)
    : name_(name)
{
}

RoseCallPathHPCT::Named::Named(const Named &n)
{
  setName(n.getName());
}

RoseCallPathHPCT::Named::~Named(void)
{
}

const string &
RoseCallPathHPCT::Named::getName(void) const
{
  return name_;
}

void RoseCallPathHPCT::Named::setName(const std::string &new_name)
{
  name_ = new_name;
}

std::string RoseCallPathHPCT::Named::toString(void) const
{
  return name_;
}
/* ---------------------------------------------------------------- */

Metric::Metric(void)
    : value_(0)
{
}

Metric::Metric(const Metric &m)
{
  setName(m.getName());
  setValue(m.getValue());
}

Metric::Metric(const string &name, double value)
    : Named(name), value_(value)
{
}

Metric::~Metric(void)
{
}

double
Metric::getValue(void) const
{
  return value_;
}

void Metric::setValue(double new_val)
{
  value_ = new_val;
}

std::string Metric::toString(void) const
{
  std::ostringstream o;
  o << getValue();
  return RoseCallPathHPCT::Named::toString() + ":" + o.str();
}
/* ---------------------------------------------------------------- */

Observable::Observable(void)
{
}

Observable::Observable(const Observable &o)
{
  metrics_ = o.metrics_;
}

Observable::~Observable(void)
{
}

unsigned int
Observable::getNumMetrics(void) const
{
  return metrics_.size();
}

const Metric &
Observable::getMetric(unsigned int i) const
{
  return metrics_[i];
}

void Observable::addMetric(const Metric &m)
{
  for (ConstMetricIterator iter = beginMetric(); iter != endMetric(); iter++)
  {
    Metric current = *iter;
    if (current.getName() == m.getName())
    {
      cerr << "Fatal error: Observable::addMetric() tries to add the same type of metric twice." << endl;
      assert(false);
    }
  }

  metrics_.push_back(m);
}

double
Observable::getMetricValue(unsigned int i) const
{
  return getMetric(i).getValue();
}

double
Observable::getMetricValue(const string &mname) const
{
  double result = 0.0;
  for (ConstMetricIterator iter = beginMetric(); iter != endMetric(); iter++)
  {
    Metric m = *iter;
    if (m.getName() == mname)
    {
      result = m.getValue();
      break;
    }
  }
  return result;
}

std::string
Observable::getMetricName(unsigned int i) const
{
  return getMetric(i).getName();
}

Observable::MetricIterator
Observable::beginMetric(void)
{
  return metrics_.begin();
}

Observable::ConstMetricIterator
Observable::beginMetric(void) const
{
  return metrics_.begin();
}

Observable::MetricIterator
Observable::endMetric(void)
{
  return metrics_.end();
}

Observable::ConstMetricIterator
Observable::endMetric(void) const
{
  return metrics_.end();
}

std::string Observable::toString() const
{
  std::string result;
  ConstMetricIterator iter = beginMetric();
  for (; iter != endMetric(); iter++)
  {
    if (iter != beginMetric())
      result += " ";
    result += (*iter).toString();
  }
  return result;
}

/* ---------------------------------------------------------------- */

Located::Located(void)
{
  setLines(0, 0);
  setFileNode(0);
}

Located::Located(const Located &l)
{
  setLines(l.getFirstLine(), l.getLastLine());
  setFileNode(l.getFileNode());
}

Located::Located(unsigned int b, unsigned int e, File *f /*=NULL*/)
    : begin_(b), end_(e), filenode_(f)
{
}

Located::~Located(void)
{
}

File *Located::getFileNode(void) const
{
  return filenode_;
}
void Located::setFileNode(File *filenode)
{
  filenode_ = filenode;
}

unsigned int
Located::getFirstLine(void) const
{
  return begin_;
}

void Located::setFirstLine(unsigned int l)
{
  begin_ = l;
}

unsigned int
Located::getLastLine(void) const
{
  return end_;
}

void Located::setLastLine(unsigned int l)
{
  end_ = l;
}

void Located::setLines(unsigned int b, unsigned int e)
{
  setFirstLine(b);
  setLastLine(e);
}

std::string Located::toString() const
{
  std::string result;
  std::ostringstream o1, o2;
  o1 << begin_;
  o2 << end_;
  result = o1.str() + "-" + o2.str();
  if (filenode_)
    result = filenode_->toString() + " " + result;
  else
    result = " no_filenode " + result;
  return result;
}
/* ---------------------------------------------------------------- */
std::string IRNode::toString() const
{
  std::string result;
  result += Named::toString() + " " + Observable::toString();
  return result;
}
/* ---------------------------------------------------------------- */
std::string Program::toString() const
{
  std::string result;
  result = "Program " + IRNode::toString();
  return result;
}

std::string SecHeader::toString() const
{
  std::string result;
  result = "SecHeader " + IRNode::toString();
  return result;
}

std::string MetricTable::toString() const
{
  std::string result;
  result = "MetricTable " + IRNode::toString();
  return result;
}

std::string MetricElement::toString() const
{
  std::string result;
  result = "MetricElement " + IRNode::toString();
  return result;
}

long int MetricElement::getId(void) const
{
  return id;
}
void MetricElement::setId(long int new_id)
{
  id = new_id;
}

std::string MetricInfo::toString() const
{
  std::string result;
  result = "MetricInfo " + IRNode::toString();
  return result;
}

std::string MetricFormula::toString() const
{
  std::string result;
  result = "MetricFormula " + IRNode::toString();
  return result;
}

std::string MetricData::toString() const
{
  std::string result;
  result = "MetricData " + IRNode::toString();
  return result;
}

std::string MetricDBTable::toString() const
{
  std::string result;
  result = "MetricDBTable " + IRNode::toString();
  return result;
}

std::string LoadModuleTable::toString() const
{
  std::string result;
  result = "LoadModuleTable " + IRNode::toString();
  return result;
}

std::string LoadModule::toString() const
{
  std::string result;
  result = "LoadModule " + IRNode::toString();
  return result;
}

long int LoadModule::getId(void) const
{
  return id;
}
void LoadModule::setId(long int new_id)
{
  id = new_id;
}

std::string FileTable::toString() const
{
  std::string result;
  result = "FileTable " + IRNode::toString();
  return result;
}

void File::replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs ) {
  std::string id_file_name(getName());
  std::string real_file_name(replaceName(id_file_name, files));
  if(id_file_name == real_file_name) {
    std::cout << "File::replaceNames: file name could not be replaced." << std::endl;
  } else {
    setName(real_file_name);
  }
}

std::string File::toString() const
{
  std::string result;
  result = "File " + IRNode::toString();
  return result;
}

long int File::getId(void) const
{
  return id;
}
void File::setId(long int new_id)
{
  id = new_id;
}

std::string ProcedureTable::toString() const
{
  std::string result;
  result = "ProcedureTable " + IRNode::toString();
  return result;
}

Procedure::Procedure(const std::string &name, long int i)
    // DQ (12/6/2016): Eliminate warning we want to consider to be an error: -Wreorder.
    // : id(i), IRNode (name), Located(l, l)
    : IRNode(name), id(i)
{
}

std::string Procedure::toString() const {
  std::string result;
  result = "Procedure " + IRNode::toString();
  return result;
}

long int Procedure::getId(void) const
{
  return id;
}
void Procedure::setId(long int new_id)
{
  id = new_id;
}


std::string SecFlatProfileData::toString() const
{
  std::string result;
  result = "SecFlatProfileData " + IRNode::toString();
  return result;
}

std::string SecCallPathProfileData::toString() const
{
  std::string result;
  result = "SecCallPathProfileData " + IRNode::toString();
  return result;
}

std::string Group::toString() const
{
  std::string result;
  result = "Group " + IRNode::toString();
  return result;
}



Proc::Proc(long int i, std::string s, unsigned int l, std::string lm, std::string f, std::string n, std::string a, std::string str)
    : Located(l, l),
      id(i),
      static_scope_id(s),
      line_range(l),
      load_module(lm),
      file_name(f),
      name(n),
      alien(a),
      hpcstruct_node_id(str)
{
}



void Proc::replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs ) {
 
 load_module = replaceName(load_module, modules);
  file_name = replaceName(file_name, files);
  name = replaceName(name, procs);

};

std::string Proc::toString() const
{
  std::string result("Proc " + IRNode::toString() + " @ line " + boost::lexical_cast<std::string>(line_range));
  return result;
}

ProcFrame::ProcFrame(long int i, std::string s, unsigned int l, std::string lm, std::string f, std::string n, std::string str)
    : Located(l, l),
      id(i),
      static_scope_id(s),
      line_range(l),
      load_module(lm),
      file_name(f),
      name(n),
      hpcstruct_node_id(str)
{
}

void ProcFrame::replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs ) {
 //std::cout << "Load modules: " << load_module << std::endl;
 load_module = replaceName(load_module, modules);
 //std::cout << "file name: " << file_name << std::endl;

  file_name = replaceName(file_name, files);
 //std::cout << "procedure name: " << name << std::endl;

  name = replaceName(name, procs);

};

std::string ProcFrame::toString() const
{
  std::string result;
  result = "ProcFrame " + IRNode::toString() + " @ line " + boost::lexical_cast<std::string>(line_range);
  return result;
}

std::string CallSite::toString() const
{
  std::string result;
  result = "CallSite " + IRNode::toString() + " @ line " + boost::lexical_cast<std::string>(line_range);
  return result;
}

Loop::Loop(long int i, std::string s, unsigned int l, std::string f, std::string str) : 
  Located(l, l), id(i), static_scope_id(s), file_name(f), hpcstruct_node_id(str)
{
}


void Loop::replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs ) {}

std::string Loop::toString() const
{
  std::string result;
  result = "Loop " + IRNode::toString() + " @ " + Located::toString();
  // result = "Loop " + IRNode::toString() + " @ line" + boost::lexical_cast<std::string>(line);
  return result;
}

std::string Statement::toString() const
{
  std::string result;
  result = "Statement " + IRNode::toString() + "@" + Located::toString();
  // result = "Statement "+ IRNode::toString() + " @ line " + boost::lexical_cast<std::string>(line);
  return result;
}

// Statement::Statement (void): id(0), hasMatchedSgNode(false)
Statement::Statement(void) : id(0), static_scope_id(0), hpcstruct_node_id(0)
{
}

// Statement::Statement (const Statement& s): id(s.getId()), hasMatchedSgNode(s.hasMatchingSgNode()), Located (s)
Statement::Statement(const Statement &s)
    // DQ (12/6/2016): Eliminate warning we want to consider to be an error: -Wreorder.
    // : id(s.getId()), Located (s)
    : Located(s), id(s.getId()), static_scope_id(s.getStaticScopeId()), hpcstruct_node_id(s.getHpcstructNodeId())
{
  setHasMatchingSgNode(s.hasMatchingSgNode());
}

// Statement::Statement (const std::string& name, id_t i, unsigned int l):
//         id(i), hasMatchedSgNode(false), IRNode (name), Located(l, l)
Statement::Statement(id_t i, unsigned int l, std::string s, std::string str)
    // DQ (12/6/2016): Eliminate warning we want to consider to be an error: -Wreorder.
    // : id(i), IRNode (name), Located(l, l)
    : Located(l, l), id(i), static_scope_id(s), hpcstruct_node_id(str)
{
}

Statement::~Statement(void)
{
}

void Statement::replaceNames(const std::map<std::string, std::string> & metrics, 
const std::map<std::string, std::string> & files, 
const std::map<std::string, std::string> & modules, 
const std::map<std::string, std::string> & procs ) {}

Statement::id_t
Statement::getId(void) const
{
  return id;
}

void Statement::setId(Statement::id_t new_id)
{
  id = new_id;
}

std::string Statement::getStaticScopeId(void) const
{
  return static_scope_id;
}
std::string Statement::getHpcstructNodeId(void) const
{
  return hpcstruct_node_id;
}

void Statement::setStaticScopeId(std::string new_static_scope_id)
{
  static_scope_id = new_static_scope_id;
}
void Statement::setHpcstructNodeId(std::string new_hpcstruct_node_id)
{
  hpcstruct_node_id = new_hpcstruct_node_id;
}

std::string RoseCallPathHPCT::replaceName(std::string &replaced_name, const std::map<std::string, std::string> &replace_map)
{
  auto it = replace_map.find(replaced_name);
  if (it != replace_map.end())
  {
    //std::cout << "replacing " << replaced_name << " with " << it->second << std::endl;
    replaced_name = it->second;
  }
  else
  {
    //std::cerr << "replaceNames::Could not replace name: " << replaced_name << std::endl;
  }
  return replaced_name;
}

/* eof */
