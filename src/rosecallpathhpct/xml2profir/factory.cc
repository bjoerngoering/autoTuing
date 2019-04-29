// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/xml2profir/factory.cc
 *  \brief Implements a factory for instantiating profile IR nodes
 *  from XML elements.
 *  \ingroup ROSEHPCT_XML2PROFIR
 *
 *  $Id: factory.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#include <cassert>
#include <cstdlib>

#include "rosecallpathhpct/util/general.hh"
#include "rosecallpathhpct/xml2profir/xml2profir.hh"

using namespace std;
using namespace GenUtil;
using namespace XMLUtil;
using namespace RoseCallPathHPCT;

/*!
 *  \name "Factory" routines that convert XMLElem objects into the
 *  corresponding RoseCallPathHPCTIR nodes.
 */
/*@{*/
static IRNode *
newProgram(const XMLElem &e)
{
  return (e.name == "SecCallPathProfile") ? new Program(e.getAttr("n")) : NULL;
}

static IRNode *
newSecHeader(const XMLElem &e)
{
  return (e.name == "SecHeader") ? new SecHeader : NULL;
}

static IRNode *
newMetricTable(const XMLElem &e)
{
  return (e.name == "MetricTable") ? new MetricTable : NULL;
}

static IRNode *
newMetricElement(const XMLElem &e)
{
  return (e.name == "Metric") ? new MetricElement(atol(e.getAttr("i")), e.getAttr("n"), e.getAttr("v"), e.getAttr("t"), atol(e.getAttr("show")), atol(e.getAttr("show-percent"))) : NULL;
}

static IRNode *
newMetricInfo(const XMLElem &e) {
  return (e.name == "Info") ? new MetricInfo() : NULL;
}

static IRNode *
newMetricData(const XMLElem &e) {
  return (e.name == "NV") ? new MetricData(/*e.getAttr("n"), e.getAttr("v")*/) : NULL;
}

static IRNode *
newMetricFormula(const XMLElem &e) {
  return (e.name == "MetricFormula") ? new MetricFormula(e.getAttr("t"), e.getAttr("frm")) : NULL;
}

static IRNode *
newMetricDBTable(const XMLElem &e) {
  return (e.name == "MetricDBTable") ? new MetricDBTable() : NULL;
}

static IRNode *
newLoadModuleTable(const XMLElem &e)
{
  return (e.name == "LoadModuleTable") ? new LoadModuleTable : NULL;
}

static IRNode *
newLoadModule(const XMLElem &e)
{
  return (e.name == "LoadModule") ? new LoadModule(e.getAttr("n"), atol(e.getAttr("i"))) : NULL;
}

static IRNode *
newFileTable(const XMLElem &e)
{
  return (e.name == "FileTable") ? new FileTable : NULL;
}

static IRNode *
newFile(const XMLElem &e)
{
  return (e.name == "File") ? new File(e.getAttr("n"), atol(e.getAttr("i"))) : NULL;
}

static IRNode *
newProcedureTable(const XMLElem &e)
{
  return (e.name == "ProcedureTable") ? new ProcedureTable : NULL;
}

static IRNode *
newProcedure(const XMLElem &e)
{
  return (e.name == "Procedure") ? new Procedure(e.getAttr("n"), atol(e.getAttr("i"))) : NULL;
}

static IRNode *
newSecFlatProfileData(const XMLElem &e)
{
  return (e.name == "SecFlatProfileData") ? new SecFlatProfileData : NULL;
}

static IRNode *
newSecCallPathProfileData(const XMLElem &e)
{
  return (e.name == "SecCallPathProfileData") ? new SecFlatProfileData : NULL;
}

static IRNode *
newProc(const XMLElem &e)
{
  return (e.name == "Pr")
             ? new Proc(atol(e.getAttr("i")), e.getAttr("s"), atol(e.getAttr("l")), e.getAttr("lm"), e.getAttr("f"), e.getAttr("n"), e.getAttr("a"), e.getAttr("str"))
             : NULL;
}

static IRNode *
newProcFrame(const XMLElem &e)
{
  return (e.name == "PF")
             ? new ProcFrame(atol(e.getAttr("i")), e.getAttr("s"), atol(e.getAttr("l")), e.getAttr("lm"), e.getAttr("f"), e.getAttr("n"), e.getAttr("str"))
             : NULL;
}

static IRNode *
newCallSite(const XMLElem &e)
{
  return (e.name == "C")
             ? new CallSite(atol(e.getAttr("i")), e.getAttr("s"), atol(e.getAttr("l")), e.getAttr("str"))
             : NULL;
}

static IRNode *
newLoop(const XMLElem &e)
{
  return (e.name == "L")
             // ? new Loop (e.getAttr ("n"), atol (e.getAttr ("b")), atol (e.getAttr ("e")))
             ? new Loop(atol(e.getAttr("i")), e.getAttr("s"), atol(e.getAttr("l")), e.getAttr("f"), e.getAttr("str"))
             : NULL;
}

static IRNode *
newStatement(const XMLElem &e)
{
  return (e.name == "S")
             // ? new Statement (e.getAttr ("n"), atol (e.getAttr ("b")), atol (e.getAttr ("e")))
             ? new Statement(atol(e.getAttr("i")), atol(e.getAttr("l")), e.getAttr("s"), e.getAttr("str"))
             : NULL;
}

/*@}*/

ProfIRFactory::ProfIRFactory(void)
{
  registerDefaults();
}

void ProfIRFactory::setElement(const XMLElem &e)
{
  element_ = e;
}

IRNode *
ProfIRFactory::callCreate(const string &id, IRNodeCreator_t creator) const
{
  return (*creator)(element_);
}

void ProfIRFactory::registerDefaults(void)
{
  registerType(string("SecCallPathProfile"), ::newProgram);
  registerType(string("SecHeader"), ::newSecHeader);
  registerType(string("MetricTable"), ::newMetricTable);
  registerType(string("Metric"), ::newMetricElement);
  registerType(string("MetricFormula"), ::newMetricFormula);
  registerType(string("Info"), ::newMetricInfo);
  registerType(string("NV"), ::newMetricData);
  registerType(string("MetricDBTable"), ::newMetricDBTable);
  registerType(string("LoadModuleTable"), ::newLoadModuleTable);
  registerType(string("LoadModule"), ::newLoadModule);
  registerType(string("FileTable"), ::newFileTable);
  registerType(string("File"), ::newFile);
  registerType(string("ProcedureTable"), ::newProcedureTable);
  registerType(string("Procedure"), ::newProcedure);
  registerType(string("SecFlatProfileData"), ::newSecFlatProfileData);
  registerType(string("SecCallPathProfileData"), ::newSecCallPathProfileData);
  registerType(string("Pr"), ::newProc);
  registerType(string("PF"), ::newProcFrame);
  registerType(string("C"), ::newCallSite);
  registerType(string("L"), ::newLoop);
  registerType(string("S"), ::newStatement);
}

/* eof */
