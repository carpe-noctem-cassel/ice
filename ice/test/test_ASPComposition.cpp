#include <gtest/gtest.h>
#include "ClingWrapper.h"
#include "External.h"
#include "BaseLiteral.h"
#include "BoolLiteral.h"
#include "clasp/solver.h"
#include <chrono>
#include <set>
#include <vector>

using namespace std;

TEST(ASPComposition, simpleTest)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // ontology
  cw->add("base", {}, "entityType(robot).");
  cw->add("base", {}, "scope(scope1).");
  cw->add("base", {}, "scope(scope2).");
  cw->add("base", {}, "scope(scope3).");
  cw->add("base", {}, "representation(rep1).");
  cw->add("base", {}, "hasScope(robot,scope1).");
  cw->add("base", {}, "hasScope(robot,scope2).");
  cw->add("base", {}, "hasScope(robot,scope3).");
  cw->add("base", {}, "hasRepresentation(scope1,rep1).");
  cw->add("base", {}, "hasRepresentation(scope2,rep1).");
  cw->add("base", {}, "hasRepresentation(scope3,rep1).");
  cw->ground("base", {});

  // entities
  cw->ground("entity", {"entity1", "robot"});

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2"}, "system", {"system1", 100}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);
  cw->ground("sourceNode", {"in2", "system1", "system1", "entity1", "scope2", "rep1", "none", 5, 90, 1});
  auto input2 = cw->getExternal("sourceNode", {"system1", "in2", "entity1"}, true);
  cw->ground("sourceNode", {"in3", "system2", "system2", "entity1", "scope2", "rep1", "none", 0, 99, 1});
  auto input3 = cw->getExternal("sourceNode", {"system2", "in3", "entity1"}, false);

  // requireds
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"}),
                                                     10000, -100},
                                  true);

  // add transfer
  auto transfer = cw->getExternal("transfer", {"system2", "system1"}, "transfer", {"system2", "system1", 1, 2}, true);

  // add node1
  cw->add("node1", {}, "#external nodeTemplate(system1,node1,any).");
  auto node1 = cw->getExternal("nodeTemplate", {"system1", "node1", "any"}, "node1", {}, true);
  cw->add("node1", {}, "input(system1,node1,scope1,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "input(system1,node1,scope2,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "output(system1,node1,scope3,rep1,none).");
  cw->add("node1", {}, "metadataOutput(delay,system1,node1,max,1,0).");
  cw->add("node1", {}, "metadataProcessing(cost,system1,node1,10).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node1,max,0,0).");
  cw->ground("node1", {});

  // add node2
  cw->add("node2", {}, "#external nodeTemplate(system1,node2,any).");
  auto node2 = cw->getExternal("nodeTemplate", {"system1", "node2", "any"}, "node2", {}, true);
  cw->add("node2", {}, "input(system1,node2,scope1,rep1,none,1,1) :- nodeTemplate(system1,node2,any).");
  cw->add("node2", {}, "input(system1,node2,scope2,rep1,none,2,2) :- nodeTemplate(system1,node2,any).");
  cw->add("node2", {}, "output(system1,node2,scope3,rep1,none).");
  cw->add("node2", {}, "metadataOutput(delay,system1,node2,max,1,0).");
  cw->add("node2", {}, "metadataProcessing(cost,system1,node2,5).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node2,avg,0,1).");
  cw->ground("node2", {});

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),2),6)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),2),90)"));

  input3->assign(true);
  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(false, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system2,in3,entity1,none),information(entity1,scope2,rep1,none),2))"));
  EXPECT_EQ(false, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),3),6)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),3),2)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),3),99)"));

  node1->assign(false);
  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(false, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(true, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node2,entity1,none),stream(1,system1,node(1,system1,in1,entity1,none),information(entity1,scope1,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node2,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node2,entity1,none),stream(1,system1,node(1,system2,in3,entity1,none),information(entity1,scope2,rep1,none),2))"));
  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node2,entity1,none),information(entity1,scope3,rep1,none),3),6)"));
  EXPECT_EQ(false, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node2,entity1,none),information(entity1,scope3,rep1,none),3),2)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node2,entity1,none),information(entity1,scope3,rep1,none),3),96)"));
}

TEST(ASPComposition, simpleTestQuery)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // ontology
  cw->add("base", {}, "entityType(robot).");
  cw->add("base", {}, "scope(scope1).");
  cw->add("base", {}, "scope(scope2).");
  cw->add("base", {}, "scope(scope3).");
  cw->add("base", {}, "representation(rep1).");
  cw->add("base", {}, "hasScope(robot,scope1).");
  cw->add("base", {}, "hasScope(robot,scope2).");
  cw->add("base", {}, "hasScope(robot,scope3).");
  cw->add("base", {}, "hasRepresentation(scope1,rep1).");
  cw->add("base", {}, "hasRepresentation(scope2,rep1).");
  cw->add("base", {}, "hasRepresentation(scope3,rep1).");
  cw->ground("base", {});

  // entities
  cw->ground("entity", {"entity1", "robot"});

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2"}, "system", {"system1", 100}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);
  cw->ground("sourceNode", {"in2", "system1", "system1", "entity1", "scope2", "rep1", "none", 5, 90, 1});
  auto input2 = cw->getExternal("sourceNode", {"system1", "in2", "entity1"}, true);
  cw->ground("sourceNode", {"in3", "system2", "system2", "entity1", "scope2", "rep1", "none", 0, 99, 1});
  auto input3 = cw->getExternal("sourceNode", {"system2", "in3", "entity1"}, false);

  // requireds
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"}),
                                                     10000, -100},
                                  true);

  // add transfer
  auto transfer = cw->getExternal("transfer", {"system2", "system1"}, "transfer", {"system2", "system1", 1, 2}, true);

  // add node1
  cw->add("node1", {}, "#external nodeTemplate(system1,node1,any).");
  auto node1 = cw->getExternal("nodeTemplate", {"system1", "node1", "any"}, "node1", {}, true);
  cw->add("node1", {}, "input(system1,node1,scope1,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "input(system1,node1,scope2,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "output(system1,node1,scope3,rep1,none).");
  cw->add("node1", {}, "metadataOutput(delay,system1,node1,max,1,0).");
  cw->add("node1", {}, "metadataProcessing(cost,system1,node1,10).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node1,max,0,0).");
  cw->ground("node1", {});

  // add node2
  cw->add("node2", {}, "#external nodeTemplate(system1,node2,any).");
  auto node2 = cw->getExternal("nodeTemplate", {"system1", "node2", "any"}, "node2", {}, true);
  cw->add("node2", {}, "input(system1,node2,scope1,rep1,none,1,1) :- nodeTemplate(system1,node2,any).");
  cw->add("node2", {}, "input(system1,node2,scope2,rep1,none,2,2) :- nodeTemplate(system1,node2,any).");
  cw->add("node2", {}, "output(system1,node2,scope3,rep1,none).");
  cw->add("node2", {}, "metadataOutput(delay,system1,node2,max,1,0).");
  cw->add("node2", {}, "metadataProcessing(cost,system1,node2,5).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node2,avg,0,1).");
  cw->ground("node2", {});

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),2),6)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),2),90)"));

  query1->assign(false);
  query1->release();
  auto query2 = cw->getExternal("query", {2}, "query", {2,3,10}, true);

  input3->assign(true);
  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(2,system1,node1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(2,system1,node2,entity1,none)"));
  EXPECT_EQ(false, cw->query("connectToNode(node(2,system1,node1,entity1,none),stream(2,system1,node(2,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(2,system1,node1,entity1,none),stream(2,system1,node(2,system2,in3,entity1,none),information(entity1,scope2,rep1,none),2))"));
  EXPECT_EQ(false, cw->query("metadataStream(2,delay,stream(2,system1,node(2,system1,node1,entity1,none),information(entity1,scope3,rep1,none),3),6)"));
  EXPECT_EQ(true, cw->query("metadataStream(2,delay,stream(2,system1,node(2,system1,node1,entity1,none),information(entity1,scope3,rep1,none),3),2)"));
  EXPECT_EQ(true, cw->query("metadataStream(2,accuracy,stream(2,system1,node(2,system1,node1,entity1,none),information(entity1,scope3,rep1,none),3),99)"));

  node1->assign(false);
  query2->assign(false);
  query2->release();
//  query2->assign(false);
  auto query3 = cw->getExternal("query", {3}, "query", {3,3,10}, true);
  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(false, cw->query("node(3,system1,node1,entity1,none)"));
  EXPECT_EQ(true, cw->query("node(3,system1,node2,entity1,none)"));
  EXPECT_EQ(true, cw->query("connectToNode(node(3,system1,node2,entity1,none),stream(3,system1,node(3,system1,in1,entity1,none),information(entity1,scope1,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(3,system1,node2,entity1,none),stream(3,system1,node(3,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(3,system1,node2,entity1,none),stream(3,system1,node(3,system2,in3,entity1,none),information(entity1,scope2,rep1,none),2))"));
  EXPECT_EQ(true, cw->query("metadataStream(3,delay,stream(3,system1,node(3,system1,node2,entity1,none),information(entity1,scope3,rep1,none),3),6)"));
  EXPECT_EQ(false, cw->query("metadataStream(3,delay,stream(3,system1,node(3,system1,node2,entity1,none),information(entity1,scope3,rep1,none),3),2)"));
  EXPECT_EQ(true, cw->query("metadataStream(3,accuracy,stream(3,system1,node(3,system1,node2,entity1,none),information(entity1,scope3,rep1,none),3),96)"));
}

TEST(ASPComposition, informationTranslation)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("data/asp/ontology.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // entities
  cw->ground("entity", {"nase", "robot"});

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2"}, "system", {"system1", 100}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "nase", "position", "coords", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "nase"}, true);

  // requires
  auto required = cw->getExternal("requiredStream", {"system2", Gringo::Value("information", {"nase", "position",
                                                                                              "wgs84", "none"})},
                                  "requiredStream", {"system2", Gringo::Value("information", {"nase", "position",
                                                                                              "wgs84", "none"}),
                                                     10000, -100},
                                  true);

  // add transfer
  auto transfer1_2 = cw->getExternal("transfer", {"system2", "system1"}, "transfer", {"system2", "system1", 1, 2}, true);

  // add translation
  cw->add("coords2Wgs84", {}, "#external transformation(system1,coords2Wgs84,any,none).");
  auto coords2Wgs84 = cw->getExternal("transformation", {"system1", "coords2Wgs84", "any", "none"}, "coords2Wgs84", {}, true);
  cw->add("coords2Wgs84", {}, "input(system1,coords2Wgs84,position,coords,none,1,1).");
  cw->add("coords2Wgs84", {}, "output(system1,coords2Wgs84,position,wgs84,none).");
  cw->add("coords2Wgs84", {}, "metadataOutput(delay,system1,coords2Wgs84,max,1,0).");
  cw->add("coords2Wgs84", {}, "metadataOutput(accuracy,system1,coords2Wgs84,avg,1,0).");
  cw->add("coords2Wgs84", {}, "metadataProcessing(cost,system1,coords2Wgs84,1).");
  cw->ground("coords2Wgs84", {});


  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
// cw->printLastModel();

  EXPECT_EQ(true, cw->query("stream(1,system2,node(1,system1,coords2Wgs84,nase,none),information(nase,position,wgs84,none),3)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system2,node(1,system1,coords2Wgs84,nase,none),information(nase,position,wgs84,none),3),2)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system2,node(1,system1,coords2Wgs84,nase,none),information(nase,position,wgs84,none),3),91)"));
}

TEST(ASPComposition, ego2allo)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("data/asp/ontology.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // entities
  cw->ground("entity", {"nase", "robot"});
  cw->ground("entity", {"bart", "robot"});

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2"}, "system", {"system1", 100}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "nase", "position", "coords", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "nase"}, true);
  cw->ground("sourceNode", {"in2", "system2", "system2", "bart", "position", "coords", "none", 0, 90, 1});
  auto input2 = cw->getExternal("sourceNode", {"system2", "in2", "bart"}, true);

  // requires
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"bart", "position",
                                                                                              "egoCoords", "nase"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"bart", "position",
                                                                                              "egoCoords", "nase"}),
                                                     10000, -100},
                                  true);

  // add transfer
  auto transfer1_2 = cw->getExternal("transfer", {"system2", "system1"}, "transfer", {"system2", "system1", 2, 1}, true);

  // add translation
  cw->add("allo2ego", {}, "#external transformation(system1,allo2ego,any,any).");
  auto allo2ego = cw->getExternal("transformation", {"system1", "allo2ego", "any", "any"}, "allo2ego", {},
                                  true);
  cw->add("allo2ego", {},
          "input2(system1,allo2ego,position,coords,none,1,1) :- transformation(system1,allo2ego,any,any).");
  cw->add("allo2ego", {},
          "input(system1,allo2ego,position,coords,none,1,1) :- transformation(system1,allo2ego,any,any).");
  cw->add("allo2ego", {}, "output(system1,allo2ego,position,egoCoords,any).");
  cw->add("allo2ego", {}, "metadataOutput(delay,system1,allo2ego,max,0,0).");
  cw->add("allo2ego", {}, "metadataOutput(accuracy,system1,allo2ego,avg,0,1).");
  cw->ground("allo2ego", {});

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//   cw->printLastModel();

  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system1,allo2ego,bart,nase),information(bart,position,egoCoords,nase),3)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,allo2ego,bart,nase),information(bart,position,egoCoords,nase),3),2)"));
  EXPECT_EQ(false, cw->query("metadataStream(1,delay,stream(1,system1,allo2ego,system1,information(bart,position,egoCoords,nase),3),3)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,allo2ego,bart,nase),information(bart,position,egoCoords,nase),3),92)"));
}

TEST(ASPComposition, requiredStreamsByEntityType)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // entities
  cw->ground("entity", {"entity1", "type"});
  cw->ground("entity", {"entity2", "type"});
  cw->ground("entity", {"entity3", "type"});
  cw->ground("entity", {"entity4", "type"});
  cw->ground("entity", {"entity5", "type"});
  cw->add("base", {}, "hasScope(type,scope1).");

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2"}, "system", {"system1", 10}, true);
  auto system3 = cw->getExternal("system", {"system3"}, "system", {"system1", 10}, true);
  auto system4 = cw->getExternal("system", {"system4"}, "system", {"system1", 10}, true);
  auto system5 = cw->getExternal("system", {"system5"}, "system", {"system1", 10}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);
  cw->ground("sourceNode", {"in2", "system2", "system1", "entity2", "scope1", "rep1", "none", 0, 90, 1});
  auto input2 = cw->getExternal("sourceNode", {"system2", "in2", "entity2"}, true);
  cw->ground("sourceNode", {"in3", "system3", "system1", "entity3", "scope1", "rep1", "none", 0, 90, 1});
  auto input3 = cw->getExternal("sourceNode", {"system3", "in3", "entity3"}, true);
  cw->ground("sourceNode", {"in4", "system4", "system1", "entity4", "scope1", "rep1", "none", 0, 90, 1});
  auto input4 = cw->getExternal("sourceNode", {"system4", "in4", "entity4"}, true);
  cw->ground("sourceNode", {"in5", "system5", "system1", "entity5", "scope1", "rep1", "none", 0, 90, 1});
  auto input5 = cw->getExternal("sourceNode", {"system5", "in5", "entity5"}, true);

  // sets
  cw->add("setNode", {}, "#external setNodeTemplate(system1,setNode,type).");
  auto node2 = cw->getExternal("setNodeTemplate", {"system1", "setNode", "type"}, "nodeTemplate", {}, true);
  cw->add("setNode", {}, "input(system1,setNode,scope1,rep1,none,0,5) :- setNodeTemplate(system1,setNode,type).");
  cw->add("setNode", {}, "outputSet(system1,setNode,type,scope1,rep1,none).");
  cw->add("setNode", {}, "metadataOutput(delay,system1,setNode,max,1,0).");
  cw->add("setNode", {}, "metadataProcessing(cost,system1,setNode,1).");
  cw->add("setNode", {}, "metadataOutput(accuracy,system1,setNode,avg,0,4).");
  cw->add("setNode", {}, "metadataOutput(density,system1,setNode,sum,0,1).");
  cw->ground("setNode", {});

  // requires
  // requiredSet(system,entity_type,scope,representation,entity2).
  auto required = cw->getExternal("requiredSet", {"system1", Gringo::Value("informationType", {"type", "scope1", "rep1", "none"})}, true);

  // add transfer
  auto transfer1_2 = cw->getExternal("transfer", {"system1", "system2"}, "transfer", {"system1", "system2", 400, 2}, true);
  auto transfer1_3 = cw->getExternal("transfer", {"system1", "system3"}, "transfer", {"system1", "system3", 400, 3}, true);
  auto transfer1_4 = cw->getExternal("transfer", {"system1", "system4"}, "transfer", {"system1", "system4", 400, 4}, true);
  auto transfer1_5 = cw->getExternal("transfer", {"system1", "system5"}, "transfer", {"system1", "system5", 400, 5}, true);
  auto transfer2_3 = cw->getExternal("transfer", {"system2", "system3"}, "transfer", {"system2", "system3", 400, 5}, true);
  auto transfer2_4 = cw->getExternal("transfer", {"system2", "system4"}, "transfer", {"system2", "system4", 400, 5}, true);
  auto transfer2_5 = cw->getExternal("transfer", {"system2", "system5"}, "transfer", {"system2", "system5", 400, 5}, true);
  auto transfer3_4 = cw->getExternal("transfer", {"system3", "system4"}, "transfer", {"system3", "system4", 400, 5}, true);
  auto transfer3_5 = cw->getExternal("transfer", {"system3", "system5"}, "transfer", {"system3", "system5", 400, 5}, true);
  auto transfer5_5 = cw->getExternal("transfer", {"system4", "system5"}, "transfer", {"system4", "system5", 400, 5}, true);

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system1,in1,entity1,none),information(entity1,scope1,rep1,none),1)"));
  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system2,in2,entity2,none),information(entity2,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system3,in3,entity3,none),information(entity3,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system4,in4,entity4,none),information(entity4,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system5,in5,entity5,none),information(entity5,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,accuracy,set(1,system1,setNode(1,system1,setNode,type,none),informationType(type,scope1,rep1,none),3),110)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,delay,set(1,system1,setNode(1,system1,setNode,type,none),informationType(type,scope1,rep1,none),3),401)"));
}

TEST(ASPComposition, setFusion)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // entities
  cw->add("base", {}, "hasScope(type,scope1).");
  cw->add("base", {}, "entityType(type).");

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2"}, "system", {"system1", 10}, true);
  auto system3 = cw->getExternal("system", {"system3"}, "system", {"system1", 10}, true);

  // input sets
  cw->add("setInput1", {}, "#external setNodeTemplate(system2,setInput1,type).");
  auto setInput1 = cw->getExternal("setNodeTemplate", {"system2", "setInput1", "type"}, "nodeTemplate", {}, true);
  cw->add("setInput1", {}, "outputSet(system2,setInput1,type,scope1,rep1,none).");
  cw->add("setInput1", {}, "metadataProcessing(cost,system2,setInput1,1).");
  cw->add("setInput1", {}, "metadataOutput(delay,system2,setInput1,fix,10,0).");
  cw->add("setInput1", {}, "metadataOutput(accuracy,system2,setInput1,fix,90,0).");
  cw->add("setInput1", {}, "metadataOutput(density,system2,setInput1,fix,3,0).");
  cw->ground("setInput1", {});

  cw->add("setInput2", {}, "#external setNodeTemplate(system3,setInput2,type).");
  auto setInput2 = cw->getExternal("setNodeTemplate", {"system3", "setInput2", "type"}, "nodeTemplate", {}, true);
  cw->add("setInput2", {}, "outputSet(system3,setInput2,type,scope1,rep1,none).");
  cw->add("setInput2", {}, "metadataProcessing(cost,system3,setInput2,1).");
  cw->add("setInput2", {}, "metadataOutput(delay,system3,setInput2,fix,10,0).");
  cw->add("setInput2", {}, "metadataOutput(accuracy,system3,setInput2,fix,90,0).");
  cw->add("setInput2", {}, "metadataOutput(density,system3,setInput2,fix,4,0).");
  cw->ground("setInput2", {});

  // set fusion
  cw->add("setFusionNode", {}, "#external setNodeTemplate(system1,setFusionNode,type).");
  auto nodeFusion = cw->getExternal("setNodeTemplate", {"system1", "setFusionNode", "type"}, "nodeTemplate", {}, true);
  cw->add("setFusionNode", {}, "inputSet(system1,setFusionNode,type,scope1,rep1,none,1,3) :- setNodeTemplate(system1,setFusionNode,type).");
  cw->add("setFusionNode", {}, "outputSet(system1,setFusionNode,type,scope1,rep1,none).");
  cw->add("setFusionNode", {}, "metadataProcessing(cost,system1,setFusionNode,1).");
  cw->add("setFusionNode", {}, "metadataOutput(delay,system1,setFusionNode,max,1,0).");
  cw->add("setFusionNode", {}, "metadataOutput(accuracy,system1,setFusionNode,avg,0,4).");
  cw->add("setFusionNode", {}, "metadataOutput(density,system1,setFusionNode,sum,0,0).");
  cw->ground("setFusionNode", {});

  // requires
  // requiredSet(system,entity_type,scope,representation,entity2).
  auto required = cw->getExternal("requiredSet", {"system1", Gringo::Value("informationType", {"type", "scope1", "rep1", "none"})}, true);

  // add transfer
  auto transfer1_2 = cw->getExternal("transfer", {"system1", "system2"}, "transfer", {"system1", "system2", 10, 1}, true);
  auto transfer1_3 = cw->getExternal("transfer", {"system1", "system3"}, "transfer", {"system1", "system3", 10, 1}, true);
  auto transfer2_3 = cw->getExternal("transfer", {"system2", "system3"}, "transfer", {"system2", "system3", 10, 1}, true);

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("set(1,system2,setNode(1,system2,setInput1,type,none),informationType(type,scope1,rep1,none),1)"));
  EXPECT_EQ(true, cw->query("set(1,system3,setNode(1,system3,setInput2,type,none),informationType(type,scope1,rep1,none),1)"));
  EXPECT_EQ(true, cw->query("set(1,system1,setNode(1,system2,setInput1,type,none),informationType(type,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("set(1,system1,setNode(1,system3,setInput2,type,none),informationType(type,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,accuracy,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),98)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,delay,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),21)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,density,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),7)"));
}


TEST(ASPComposition, setFusionSubType)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // entities
  cw->add("base", {}, "hasScope(type,scope1).");
  cw->add("base", {}, "entityType(type).");
  cw->add("base", {}, "entityType(type2,type).");

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2"}, "system", {"system1", 10}, true);
  auto system3 = cw->getExternal("system", {"system3"}, "system", {"system1", 10}, true);

  // input sets
  cw->add("setInput1", {}, "#external setNodeTemplate(system2,setInput1,type).");
  auto setInput1 = cw->getExternal("setNodeTemplate", {"system2", "setInput1", "type"}, "nodeTemplate", {}, true);
  cw->add("setInput1", {}, "outputSet(system2,setInput1,type,scope1,rep1,none).");
  cw->add("setInput1", {}, "metadataProcessing(cost,system2,setInput1,1).");
  cw->add("setInput1", {}, "metadataOutput(delay,system2,setInput1,fix,10,0).");
  cw->add("setInput1", {}, "metadataOutput(accuracy,system2,setInput1,fix,90,0).");
  cw->add("setInput1", {}, "metadataOutput(density,system2,setInput1,fix,3,0).");
  cw->ground("setInput1", {});

  cw->add("setInput2", {}, "#external setNodeTemplate(system3,setInput2,type2).");
  auto setInput2 = cw->getExternal("setNodeTemplate", {"system3", "setInput2", "type2"}, "nodeTemplate", {}, true);
  cw->add("setInput2", {}, "outputSet(system3,setInput2,type2,scope1,rep1,none).");
  cw->add("setInput2", {}, "metadataProcessing(cost,system3,setInput2,1).");
  cw->add("setInput2", {}, "metadataOutput(delay,system3,setInput2,fix,10,0).");
  cw->add("setInput2", {}, "metadataOutput(accuracy,system3,setInput2,fix,90,0).");
  cw->add("setInput2", {}, "metadataOutput(density,system3,setInput2,fix,4,0).");
  cw->ground("setInput2", {});

  // set fusion
  cw->add("setFusionNode", {}, "#external setNodeTemplate(system1,setFusionNode,type).");
  auto nodeFusion = cw->getExternal("setNodeTemplate", {"system1", "setFusionNode", "type"}, "nodeTemplate", {}, true);
  cw->add("setFusionNode", {}, "inputSet(system1,setFusionNode,type,scope1,rep1,none,1,3).");
  cw->add("setFusionNode", {}, "outputSet(system1,setFusionNode,type,scope1,rep1,none).");
  cw->add("setFusionNode", {}, "metadataProcessing(cost,system1,setFusionNode,1).");
  cw->add("setFusionNode", {}, "metadataOutput(delay,system1,setFusionNode,max,1,0).");
  cw->add("setFusionNode", {}, "metadataOutput(accuracy,system1,setFusionNode,avg,0,4).");
  cw->add("setFusionNode", {}, "metadataOutput(density,system1,setFusionNode,sum,0,0).");
  cw->ground("setFusionNode", {});

  // requires
  // requiredSet(system,entity_type,scope,representation,entity2).
  auto required = cw->getExternal("requiredSet", {"system1", Gringo::Value("informationType", {"type", "scope1", "rep1", "none"})}, true);

  // add transfer
  auto transfer1_2 = cw->getExternal("transfer", {"system1", "system2"}, "transfer", {"system1", "system2", 10, 1}, true);
  auto transfer1_3 = cw->getExternal("transfer", {"system1", "system3"}, "transfer", {"system1", "system3", 10, 1}, true);
  auto transfer2_3 = cw->getExternal("transfer", {"system2", "system3"}, "transfer", {"system2", "system3", 10, 1}, true);

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("set(1,system2,setNode(1,system2,setInput1,type,none),informationType(type,scope1,rep1,none),1)"));
  EXPECT_EQ(true, cw->query("set(1,system3,setNode(1,system3,setInput2,type2,none),informationType(type2,scope1,rep1,none),1)"));
  EXPECT_EQ(true, cw->query("set(1,system1,setNode(1,system2,setInput1,type,none),informationType(type,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("set(1,system1,setNode(1,system3,setInput2,type2,none),informationType(type2,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,accuracy,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),98)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,delay,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),21)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,density,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),7)"));
}

TEST(ASPComposition, fusion)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // entities
  cw->add("base", {}, "entityType(robot).");
  cw->add("base", {}, "scope(scope1).");
  cw->add("base", {}, "scope(scope2).");
  cw->add("base", {}, "scope(scope3).");
  cw->add("base", {}, "representation(rep1).");
  cw->add("base", {}, "hasScope(robot,scope1).");
  cw->add("base", {}, "hasScope(robot,scope2).");
  cw->add("base", {}, "hasScope(robot,scope3).");
  cw->add("base", {}, "hasRepresentation(scope1,rep1).");
  cw->add("base", {}, "hasRepresentation(scope2,rep1).");
  cw->add("base", {}, "hasRepresentation(scope3,rep1).");
  cw->ground("base", {});

  // entities
  cw->ground("entity", {"entity1", "robot"});

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2"}, "system", {"system1", 100}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);
  cw->ground("sourceNode", {"in2", "system1", "system1", "entity1", "scope1", "rep1", "none", 5, 90, 1});
  auto input2 = cw->getExternal("sourceNode", {"system1", "in2", "entity1"}, true);
  cw->ground("sourceNode", {"in3", "system2", "system2", "entity1", "scope1", "rep1", "none", 0, 99, 1});
  auto input3 = cw->getExternal("sourceNode", {"system2", "in3", "entity1"}, false);

  // requireds
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"}),
                                                     10000, -100},
                                  true);

  // add transfer
  auto transfer = cw->getExternal("transfer", {"system2", "system1"}, "transfer", {"system2", "system1", 1, 2}, true);

  // add node1
  cw->add("node1", {}, "#external nodeTemplate(system1,node1,any).");
  auto node1 = cw->getExternal("nodeTemplate", {"system1", "node1", "any"}, "node1", {}, true);
  cw->add("node1", {}, "input(system1,node1,scope1,rep1,none,1,3) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "output(system1,node1,scope3,rep1,none).");
  cw->add("node1", {}, "metadataOutput(delay,system1,node1,max,1,0).");
  cw->add("node1", {}, "metadataProcessing(cost,system1,node1,10).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node1,max,0,0).");
  cw->ground("node1", {});

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system1,in1,entity1,none),information(entity1,scope1,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),2),90)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),2),1)"));

}

TEST(ASPComposition, noInputTest)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // ontology
  cw->add("base", {}, "entityType(robot).");
  cw->add("base", {}, "scope(scope1).");
  cw->add("base", {}, "representation(rep1).");
  cw->add("base", {}, "hasScope(robot,scope1).");
  cw->add("base", {}, "hasRepresentation(scope1,rep1).");


  cw->ground("base", {});

  // entities
  cw->ground("entity", {"entity1", "robot"});

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 100}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);

  // requireds
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope1",
                                                                                              "rep1", "none"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope1",
                                                                                              "rep1", "none"}),
                                                     10000, -100},
                                  true);

  // add node1
  cw->add("node1", {}, "#external nodeTemplate(system1,node1,any).");
  auto node1 = cw->getExternal("nodeTemplate", {"system1", "node1", "any"}, "node1", {}, true);
  cw->add("node1", {}, "input(system1,node1,scope1,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "output(system1,node1,scope1,rep1,none).");
  cw->add("node1", {}, "metadataOutput(delay,system1,node1,min,1,0).");
  cw->add("node1", {}, "metadataProcessing(cost,system1,node1,3).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node1,min,5,0).");
  cw->ground("node1", {});

  // add node2
  cw->add("node2", {}, "#external nodeTemplate(system1,node2,any).");
  auto node2 = cw->getExternal("nodeTemplate", {"system1", "node2", "any"}, "node2", {}, true);
  cw->add("node2", {}, "input(system1,node2,scope1,rep1,none,1,1) :- nodeTemplate(system1,node2,any).");
  cw->add("node2", {}, "output(system1,node2,scope1,rep1,none).");
  cw->add("node2", {}, "metadataOutput(delay,system1,node2,min,1,0).");
  cw->add("node2", {}, "metadataProcessing(cost,system1,node2,1).");
  cw->add("node2", {}, "metadataOutput(accuracy,system1,node2,min,5,0).");
  cw->ground("node2", {});

  // add node3
  cw->add("node3", {}, "#external nodeTemplate(system1,node3,any).");
  auto node3 = cw->getExternal("nodeTemplate", {"system1", "node3", "any"}, "node3", {}, true);
  cw->add("node3", {}, "input(system1,node3,scope1,rep1,none,1,1) :- nodeTemplate(system1,node3,any).");
  cw->add("node3", {}, "output(system1,node3,scope1,rep1,none).");
  cw->add("node3", {}, "metadataOutput(delay,system1,node3,min,1,0).");
  cw->add("node3", {}, "metadataProcessing(cost,system1,node3,1).");
  cw->add("node3", {}, "metadataOutput(accuracy,system1,node3,min,1,2).");
  cw->ground("node3", {});

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,in1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(true, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node3,entity1,none)"));

}

TEST(ASPComposition, simpleChainTest)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // ontology
  cw->add("base", {}, "entityType(robot).");
  cw->add("base", {}, "scope(scope1).");
  cw->add("base", {}, "representation(rep1).");
  cw->add("base", {}, "hasScope(robot,scope1).");
  cw->add("base", {}, "hasRepresentation(scope1,rep1).");

  cw->ground("base", {});

  // entities
  cw->ground("entity", {"entity1", "robot"});

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 11}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);

  // requireds
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope1",
                                                                                              "rep1", "none"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope1",
                                                                                              "rep1", "none"}),
                                                     10000, -100},
                                  true);

  // add node1
  cw->add("node1", {}, "#external nodeTemplate(system1,node1,any).");
  auto node1 = cw->getExternal("nodeTemplate", {"system1", "node1", "any"}, "node1", {}, true);
  cw->add("node1", {}, "input(system1,node1,scope1,rep1,none,1,1).");
  cw->add("node1", {}, "output(system1,node1,scope1,rep1,none).");
  cw->add("node1", {}, "metadataOutput(delay,system1,node1,min,1,0).");
  cw->add("node1", {}, "metadataProcessing(cost,system1,node1,8).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node1,min,5,0).");
  cw->ground("node1", {});

  // add node2
  cw->add("node2", {}, "#external nodeTemplate(system1,node2,any).");
  auto node2 = cw->getExternal("nodeTemplate", {"system1", "node2", "any"}, "node2", {}, true);
  cw->add("node2", {}, "input(system1,node2,scope1,rep1,none,1,1).");
  cw->add("node2", {}, "output(system1,node2,scope1,rep1,none).");
  cw->add("node2", {}, "metadataOutput(delay,system1,node2,min,1,0).");
  cw->add("node2", {}, "metadataProcessing(cost,system1,node2,5).");
  cw->add("node2", {}, "metadataOutput(accuracy,system1,node2,min,5,0).");
  cw->ground("node2", {});

  // add node3
  cw->add("node3", {}, "#external nodeTemplate(system1,node3,any).");
  auto node3 = cw->getExternal("nodeTemplate", {"system1", "node3", "any"}, "node3", {}, true);
  cw->add("node3", {}, "input(system1,node3,scope1,rep1,none,1,1).");
  cw->add("node3", {}, "output(system1,node3,scope1,rep1,none).");
  cw->add("node3", {}, "metadataOutput(delay,system1,node3,min,1,0).");
  cw->add("node3", {}, "metadataProcessing(cost,system1,node3,5).");
  cw->add("node3", {}, "metadataOutput(accuracy,system1,node3,min,1,2).");
  cw->ground("node3", {});

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,in1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(true, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node3,entity1,none)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node2,entity1,none),information(entity1,scope1,rep1,none),2),95)"));
}

TEST(ASPComposition, nodeUsedTwice)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // ontology
  cw->add("base", {}, "entityType(robot).");
  cw->add("base", {}, "scope(scope1).");
  cw->add("base", {}, "representation(rep1).");
  cw->add("base", {}, "hasScope(robot,scope1).");
  cw->add("base", {}, "hasRepresentation(scope1,rep1).");

  cw->ground("base", {});

  // entities
  cw->ground("entity", {"entity1", "robot"});
  cw->ground("entity", {"entity2", "robot"});

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 100}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);
  cw->ground("sourceNode", {"in2", "system1", "system1", "entity2", "scope1", "rep1", "none", 0, 90, 1});
  auto input2 = cw->getExternal("sourceNode", {"system1", "in2", "entity2"}, true);

  // requireds
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope1",
                                                                                              "rep1", "none"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope1",
                                                                                              "rep1", "none"}),
                                                     10000, -100},
                                  true);
  auto required2 = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity2", "scope1",
                                                                                              "rep1", "none"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"entity2", "scope1",
                                                                                              "rep1", "none"}),
                                                     10000, -100},
                                  true);

  // add node1
  cw->add("node1", {}, "#external nodeTemplate(system1,node1,any).");
  auto node1 = cw->getExternal("nodeTemplate", {"system1", "node1", "any"}, "node1", {}, true);
  cw->add("node1", {}, "input(system1,node1,scope1,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "output(system1,node1,scope1,rep1,none).");
  cw->add("node1", {}, "metadataOutput(delay,system1,node1,min,1,0).");
  cw->add("node1", {}, "metadataProcessing(cost,system1,node1,8).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node1,min,5,0).");
  cw->ground("node1", {});

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,in1,entity1,none)"));
  EXPECT_EQ(true, cw->query("node(1,system1,in2,entity2,none)"));
  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity2,none)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope1,rep1,none),2),95)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node1,entity2,none),information(entity2,scope1,rep1,none),2),95)"));
}

TEST(ASPComposition, localSimpleTest)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/localOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // ontology
  cw->add("base", {}, "entityType(robot).");
  cw->add("base", {}, "scope(scope1).");
  cw->add("base", {}, "scope(scope2).");
  cw->add("base", {}, "scope(scope3).");
  cw->add("base", {}, "representation(rep1).");
  cw->add("base", {}, "hasScope(robot,scope1).");
  cw->add("base", {}, "hasScope(robot,scope2).");
  cw->add("base", {}, "hasScope(robot,scope3).");
  cw->add("base", {}, "hasRepresentation(scope1,rep1).");
  cw->add("base", {}, "hasRepresentation(scope2,rep1).");
  cw->add("base", {}, "hasRepresentation(scope3,rep1).");
  cw->ground("base", {});

  // entities
  cw->ground("entity", {"entity1", "robot"});

  // systems
  auto system1 = cw->getExternal("system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2", 10}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);
  cw->ground("sourceNode", {"in2", "system1", "system1", "entity1", "scope2", "rep1", "none", 5, 90, 1});
  auto input2 = cw->getExternal("sourceNode", {"system1", "in2", "entity1"}, true);
  cw->ground("sourceNode", {"in3", "system2", "system2", "entity1", "scope2", "rep1", "none", 0, 99, 1});
  auto input3 = cw->getExternal("sourceNode", {"system2", "in3", "entity1"}, false);

  // requireds
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"}),
                                                     10000, -100},
                                  true);

  // add transfer
  auto transfer = cw->getExternal("transfer", {"system2", "system1"}, "transfer", {"system2", "system1", 1, 2}, true);

  // add node1
  cw->add("node1", {}, "#external nodeTemplate(system1,node1,any).");
  auto node1 = cw->getExternal("nodeTemplate", {"system1", "node1", "any"}, "node1", {}, true);
  cw->add("node1", {}, "input(system1,node1,scope1,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "input(system1,node1,scope2,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "output(system1,node1,scope3,rep1,none).");
  cw->add("node1", {}, "metadataOutput(delay,system1,node1,max,1,0).");
  cw->add("node1", {}, "metadataProcessing(cost,system1,node1,10).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node1,max,0,0).");
  cw->ground("node1", {});

  // add node2
  cw->add("node2", {}, "#external nodeTemplate(system1,node2,any).");
  auto node2 = cw->getExternal("nodeTemplate", {"system1", "node2", "any"}, "node2", {}, true);
  cw->add("node2", {}, "input(system1,node2,scope1,rep1,none,1,1) :- nodeTemplate(system1,node2,any).");
  cw->add("node2", {}, "input(system1,node2,scope2,rep1,none,2,2) :- nodeTemplate(system1,node2,any).");
  cw->add("node2", {}, "output(system1,node2,scope3,rep1,none).");
  cw->add("node2", {}, "metadataOutput(delay,system1,node2,max,1,0).");
  cw->add("node2", {}, "metadataProcessing(cost,system1,node2,5).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node2,avg,0,1).");
  cw->ground("node2", {});

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
 // cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));

  input3->assign(true);
  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(false, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system2,in3,entity1,none),information(entity1,scope2,rep1,none),2))"));

  node1->assign(false);
  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(false, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(true, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node2,entity1,none),stream(1,system1,node(1,system1,in1,entity1,none),information(entity1,scope1,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node2,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node2,entity1,none),stream(1,system1,node(1,system2,in3,entity1,none),information(entity1,scope2,rep1,none),2))"));
}

TEST(ASPComposition, localChainTest)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/localOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // ontology
  cw->add("base", {}, "entityType(robot).");
  cw->add("base", {}, "scope(scope1).");
  cw->add("base", {}, "representation(rep1).");
  cw->add("base", {}, "hasScope(robot,scope1).");
  cw->add("base", {}, "hasRepresentation(scope1,rep1).");

  cw->ground("base", {});

  // entities
  cw->ground("entity", {"entity1", "robot"});

  // systems
  auto system1 = cw->getExternal("system", {"system1", 11}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);

  // requireds
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope1",
                                                                                              "rep1", "none"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope1",
                                                                                              "rep1", "none"}),
                                                     10000, -100},
                                  true);

  // add node1
  cw->add("node1", {}, "#external nodeTemplate(system1,node1,any).");
  auto node1 = cw->getExternal("nodeTemplate", {"system1", "node1", "any"}, "node1", {}, true);
  cw->add("node1", {}, "input(system1,node1,scope1,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "output(system1,node1,scope1,rep1,none).");
  cw->add("node1", {}, "metadataOutput(delay,system1,node1,min,1,0).");
  cw->add("node1", {}, "metadataProcessing(cost,system1,node1,8).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node1,min,5,0).");
  cw->ground("node1", {});

  // add node2
  cw->add("node2", {}, "#external nodeTemplate(system1,node2,any).");
  auto node2 = cw->getExternal("nodeTemplate", {"system1", "node2", "any"}, "node2", {}, true);
  cw->add("node2", {}, "input(system1,node2,scope1,rep1,none,1,1) :- nodeTemplate(system1,node2,any).");
  cw->add("node2", {}, "output(system1,node2,scope1,rep1,none).");
  cw->add("node2", {}, "metadataOutput(delay,system1,node2,min,1,0).");
  cw->add("node2", {}, "metadataProcessing(cost,system1,node2,5).");
  cw->add("node2", {}, "metadataOutput(accuracy,system1,node2,min,5,0).");
  cw->ground("node2", {});

  // add node3
  cw->add("node3", {}, "#external nodeTemplate(system1,node3,any).");
  auto node3 = cw->getExternal("nodeTemplate", {"system1", "node3", "any"}, "node3", {}, true);
  cw->add("node3", {}, "input(system1,node3,scope1,rep1,none,1,1) :- nodeTemplate(system1,node3,any).");
  cw->add("node3", {}, "output(system1,node3,scope1,rep1,none).");
  cw->add("node3", {}, "metadataOutput(delay,system1,node3,min,1,0).");
  cw->add("node3", {}, "metadataProcessing(cost,system1,node3,5).");
  cw->add("node3", {}, "metadataOutput(accuracy,system1,node3,min,1,2).");
  cw->ground("node3", {});

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,in1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node3,entity1,none)"));
}

TEST(ASPComposition, simpleIslandTest)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/nicer.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // ontology
  cw->add("base", {}, "entityType(robot).");
  cw->add("base", {}, "scope(scope1).");
  cw->add("base", {}, "scope(scope2).");
  cw->add("base", {}, "scope(scope3).");
  cw->add("base", {}, "representation(rep1).");
  cw->add("base", {}, "hasScope(robot,scope1).");
  cw->add("base", {}, "hasScope(robot,scope2).");
  cw->add("base", {}, "hasScope(robot,scope3).");
  cw->add("base", {}, "hasRepresentation(scope1,rep1).");
  cw->add("base", {}, "hasRepresentation(scope2,rep1).");
  cw->add("base", {}, "hasRepresentation(scope3,rep1).");
  cw->ground("base", {});

  // entities
  cw->ground("entity", {"entity1", "robot"});

  // islands
  auto island1 = cw->getExternal("island", {"island1"}, true);
  auto island2 = cw->getExternal("island", {"island2"}, true);
  auto bridge = cw->getExternal("bridge", {"island1", "island2"}, "bridge", {"island1", "island2", 100, 2}, true);

  // systems
  auto system1 = cw->getExternal("system", {"system1", "island1"}, "system", {"system1", 100, "island1"}, true);
  auto system2 = cw->getExternal("system", {"system2", "island2"}, "system", {"system2", 100, "island2"}, true);

  auto cb12 = cw->getExternal("connectToBridge", {"system1", "island2"}, "connectToBridge", {"system1", "island2", 2, 2}, true);
  auto cb21 = cw->getExternal("connectToBridge", {"system2", "island1"}, "connectToBridge", {"system2", "island1", 2, 2}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system2", "system2", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system2", "in1", "entity1"}, true);

  // requireds
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"}),
                                                     10000, -100},
                                  true);

  // add node1
  cw->add("node1", {}, "#external nodeTemplate(system1,node1,any).");
  auto node1 = cw->getExternal("nodeTemplate", {"system1", "node1", "any"}, "node1", {}, true);
  cw->add("node1", {}, "input(system1,node1,scope1,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "output(system1,node1,scope3,rep1,none).");
  cw->add("node1", {}, "metadataOutput(delay,system1,node1,max,1,0).");
  cw->add("node1", {}, "metadataProcessing(cost,system1,node1,10).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node1,max,0,0).");
  cw->ground("node1", {});

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("transfer(system1,system2)"));
  EXPECT_EQ(true, cw->query("metadataOutput(delay,system1,system2,104)"));
  EXPECT_EQ(true, cw->query("metadataOutput(delay,system2,system1,104)"));
  EXPECT_EQ(true, cw->query("metadataProcessing(cost,system1,system2,6)"));
  EXPECT_EQ(true, cw->query("metadataProcessing(cost,system2,system1,6)"));
  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system2,in1,entity1,none),information(entity1,scope1,rep1,none),2))"));

  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),3),105)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),3),90)"));
}

TEST(ASPComposition, setFusionIslandTest)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/nicer.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // entities
  cw->ground("entity", {"entity1", "type"});
  cw->ground("entity", {"entity2", "type"});
  cw->ground("entity", {"entity3", "type"});
  cw->ground("entity", {"entity4", "type"});
  cw->ground("entity", {"entity5", "type"});
  cw->add("base", {}, "hasScope(type,scope1).");
  // islands
  auto island1 = cw->getExternal("island", {"island1"}, true);
  auto island2 = cw->getExternal("island", {"island2"}, true);
  auto island3 = cw->getExternal("island", {"island3"}, true);
  auto bridge12 = cw->getExternal("bridge", {"island1", "island2"}, "bridge", {"island1", "island2", 100, 22}, true);
  auto bridge13 = cw->getExternal("bridge", {"island1", "island3"}, "bridge", {"island1", "island3", 200, 22}, true);

  // systems
  auto system1 = cw->getExternal("system", {"system1", "island1"}, "system", {"system1", 100, "island1"}, true);
  auto system2 = cw->getExternal("system", {"system2", "island1"}, "system", {"system2", 100, "island1"}, true);
  auto system3 = cw->getExternal("system", {"system3", "island2"}, "system", {"system3", 100, "island2"}, true);
  auto system4 = cw->getExternal("system", {"system4", "island2"}, "system", {"system4", 100, "island2"}, true);
  auto system5 = cw->getExternal("system", {"system5", "island3"}, "system", {"system5", 100, "island3"}, true);

  auto cb12 = cw->getExternal("connectToBridge", {"system1", "island2"}, "connectToBridge", {"system1", "island2", 2, 2}, true);
  auto cb13 = cw->getExternal("connectToBridge", {"system1", "island3"}, "connectToBridge", {"system1", "island3", 2, 2}, true);
  auto cb31 = cw->getExternal("connectToBridge", {"system3", "island1"}, "connectToBridge", {"system3", "island1", 2, 2}, true);
  auto cb41 = cw->getExternal("connectToBridge", {"system4", "island1"}, "connectToBridge", {"system4", "island1", 2, 2}, true);
  auto cb51 = cw->getExternal("connectToBridge", {"system5", "island1"}, "connectToBridge", {"system5", "island1", 2, 2}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);
  cw->ground("sourceNode", {"in2", "system2", "system1", "entity2", "scope1", "rep1", "none", 0, 90, 1});
  auto input2 = cw->getExternal("sourceNode", {"system2", "in2", "entity2"}, true);
  cw->ground("sourceNode", {"in3", "system3", "system1", "entity3", "scope1", "rep1", "none", 0, 90, 1});
  auto input3 = cw->getExternal("sourceNode", {"system3", "in3", "entity3"}, true);
  cw->ground("sourceNode", {"in4", "system4", "system1", "entity4", "scope1", "rep1", "none", 0, 85, 1});
  auto input4 = cw->getExternal("sourceNode", {"system4", "in4", "entity4"}, true);
  cw->ground("sourceNode", {"in5", "system5", "system1", "entity5", "scope1", "rep1", "none", 0, 90, 1});
  auto input5 = cw->getExternal("sourceNode", {"system5", "in5", "entity5"}, true);

  // set fusion
  cw->add("setFusionNode", {}, "#external setNodeTemplate(system1,setFusionNode,type).");
  auto nodeFusion = cw->getExternal("setNodeTemplate", {"system1", "setFusionNode", "type"}, "nodeTemplate", {}, true);
  cw->add("setFusionNode", {}, "input(system1,setFusionNode,scope1,rep1,none,0,3) :- setNodeTemplate(system1,setFusionNode,type).");
  cw->add("setFusionNode", {}, "outputSet(system1,setFusionNode,type,scope1,rep1,none).");
  cw->add("setFusionNode", {}, "metadataProcessing(cost,system1,setFusionNode,1).");
  cw->add("setFusionNode", {}, "metadataOutput(delay,system1,setFusionNode,max,1,0).");
  cw->add("setFusionNode", {}, "metadataOutput(accuracy,system1,setFusionNode,avg,0,4).");
  cw->add("setFusionNode", {}, "metadataOutput(density,system1,setFusionNode,sum,0,1).");
  cw->ground("setFusionNode", {});

  // requires
  // requiredSet(system,entity_type,scope,representation,entity2).
  auto required = cw->getExternal("requiredSet", {"system1", Gringo::Value("informationType", {"type", "scope1", "rep1", "none"})}, true);

  // add transfer
  auto transfer1_2 = cw->getExternal("transfer", {"system1", "system2"}, "transfer", {"system1", "system2", 10, 1}, true);

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system1,in1,entity1,none),information(entity1,scope1,rep1,none),1)"));
  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system2,in2,entity2,none),information(entity2,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system3,in3,entity3,none),information(entity3,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,accuracy,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),102)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,density,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),3)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,delay,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),105)"));
}

TEST(ASPComposition, islandCoverageTest)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/nicer.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // entities
  cw->ground("entity", {"entity1", "type"});
  cw->ground("entity", {"entity2", "type"});
  cw->ground("entity", {"entity3", "type"});
  cw->ground("entity", {"entity4", "type"});
  cw->ground("entity", {"entity5", "type"});
  cw->add("base", {}, "hasScope(type,scope1).");

  // islands
  auto island1 = cw->getExternal("island", {"island1"}, true);
  auto island2 = cw->getExternal("island", {"island2"}, true);
  auto island3 = cw->getExternal("island", {"island3"}, true);
  auto bridge12 = cw->getExternal("bridge", {"island1", "island2"}, "bridge", {"island1", "island2", 100, 22}, true);
  auto bridge13 = cw->getExternal("bridge", {"island1", "island3"}, "bridge", {"island1", "island3", 400, 22}, true);

  // systems
  cw->add("base", {}, "system(system1).");
  cw->add("base", {}, "system(system2).");
  cw->add("base", {}, "system(system3).");
  cw->add("base", {}, "system(system4).");
  cw->add("base", {}, "system(system5).");
  auto system1 = cw->getExternal("system", {"system1", "island1"}, "system", {"system1", 100, "island1"}, true);
  auto system2 = cw->getExternal("system", {"system2", "island1"}, "system", {"system2", 100, "island1"}, true);
  auto system3 = cw->getExternal("system", {"system3", "island2"}, "system", {"system3", 100, "island2"}, true);
  auto system4 = cw->getExternal("system", {"system4", "island2"}, "system", {"system4", 100, "island2"}, true);
  auto system5 = cw->getExternal("system", {"system5", "island3"}, "system", {"system5", 100, "island3"}, true);

  auto cb12 = cw->getExternal("connectToBridge", {"system1", "island2"}, "connectToBridge", {"system1", "island2", 2, 2}, true);
  auto cb13 = cw->getExternal("connectToBridge", {"system1", "island3"}, "connectToBridge", {"system1", "island3", 2, 2}, true);
  auto cb31 = cw->getExternal("connectToBridge", {"system3", "island1"}, "connectToBridge", {"system3", "island1", 2, 2}, true);
  auto cb41 = cw->getExternal("connectToBridge", {"system4", "island1"}, "connectToBridge", {"system4", "island1", 2, 2}, true);
  auto cb51 = cw->getExternal("connectToBridge", {"system5", "island1"}, "connectToBridge", {"system5", "island1", 2, 2}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 95, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);
  cw->ground("sourceNode", {"in2", "system2", "system1", "entity2", "scope1", "rep1", "none", 0, 90, 1});
  auto input2 = cw->getExternal("sourceNode", {"system2", "in2", "entity2"}, true);
  cw->ground("sourceNode", {"in3", "system3", "system1", "entity3", "scope1", "rep1", "none", 0, 95, 1});
  auto input3 = cw->getExternal("sourceNode", {"system3", "in3", "entity3"}, true);
  cw->ground("sourceNode", {"in4", "system4", "system1", "entity4", "scope1", "rep1", "none", 0, 90, 1});
  auto input4 = cw->getExternal("sourceNode", {"system4", "in4", "entity4"}, true);
  cw->ground("sourceNode", {"in5", "system5", "system1", "entity5", "scope1", "rep1", "none", 0, 90, 1});
  auto input5 = cw->getExternal("sourceNode", {"system5", "in5", "entity5"}, true);

  // set fusion
  cw->add("setFusionNode", {}, "#external setNodeTemplate(system1,setFusionNode,type).");
  auto nodeFusion = cw->getExternal("setNodeTemplate", {"system1", "setFusionNode", "type"}, "nodeTemplate", {}, true);
  cw->add("setFusionNode", {}, "input(system1,setFusionNode,scope1,rep1,none,0,3) :- setNodeTemplate(system1,setFusionNode,type).");
  cw->add("setFusionNode", {}, "outputSet(system1,setFusionNode,type,scope1,rep1,none).");
  cw->add("setFusionNode", {}, "metadataProcessing(cost,system1,setFusionNode,1).");
  cw->add("setFusionNode", {}, "metadataOutput(delay,system1,setFusionNode,max,1,0).");
  cw->add("setFusionNode", {}, "metadataOutput(accuracy,system1,setFusionNode,avg,0,4).");
  cw->add("setFusionNode", {}, "metadataOutput(islandCoverage,system1,setFusionNode,sum,0,1).");
  cw->ground("setFusionNode", {});

  // requires
  // requiredSet(system,entity_type,scope,representation,entity2).
  auto required = cw->getExternal("requiredSet", {"system1", Gringo::Value("informationType", {"type", "scope1", "rep1", "none"})}, true);

  // add transfer
  auto transfer1_2 = cw->getExternal("transfer", {"system1", "system2"}, "transfer", {"system1", "system2", 10, 1}, true);

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system1,in1,entity1,none),information(entity1,scope1,rep1,none),1)"));
  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system3,in3,entity3,none),information(entity3,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("stream(1,system1,node(1,system5,in5,entity5,none),information(entity5,scope1,rep1,none),2)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,accuracy,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),105)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,delay,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),405)"));
  EXPECT_EQ(true, cw->query("metadataSet(1,islandCoverage,set(1,system1,setNode(1,system1,setFusionNode,type,none),informationType(type,scope1,rep1,none),3),3)"));
}

TEST(ASPComposition, localProcessingOnly)
{
  std::shared_ptr<supplementary::ClingWrapper> cw = std::make_shared<supplementary::ClingWrapper>();
  cw->addKnowledgeFile("../asp/informationProcessing/processing.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/searchBottomUp.lp");
  cw->addKnowledgeFile("../asp/informationProcessing/globalOptimization.lp");
  cw->addKnowledgeFile("../test/data/asp/processingStuff.lp");
  cw->init();

  // ontology
  cw->add("base", {}, "entityType(robot).");
  cw->add("base", {}, "scope(scope1).");
  cw->add("base", {}, "scope(scope2).");
  cw->add("base", {}, "scope(scope3).");
  cw->add("base", {}, "representation(rep1).");
  cw->add("base", {}, "hasScope(robot,scope1).");
  cw->add("base", {}, "hasScope(robot,scope2).");
  cw->add("base", {}, "hasScope(robot,scope3).");
  cw->add("base", {}, "hasRepresentation(scope1,rep1).");
  cw->add("base", {}, "hasRepresentation(scope2,rep1).");
  cw->add("base", {}, "hasRepresentation(scope3,rep1).");
  cw->ground("base", {});

  // entities
  cw->ground("entity", {"entity1", "robot"});

  // systems
  auto system1 = cw->getExternal("system", {"system1"}, "system", {"system1", 100}, true);
  auto system2 = cw->getExternal("system", {"system2"}, "system", {"system1", 100}, true);

  // inputs
  cw->ground("sourceNode", {"in1", "system1", "system1", "entity1", "scope1", "rep1", "none", 0, 90, 1});
  auto input1 = cw->getExternal("sourceNode", {"system1", "in1", "entity1"}, true);
  cw->ground("sourceNode", {"in2", "system1", "system1", "entity1", "scope2", "rep1", "none", 5, 90, 1});
  auto input2 = cw->getExternal("sourceNode", {"system1", "in2", "entity1"}, true);
  cw->ground("sourceNode", {"in3", "system2", "system2", "entity1", "scope2", "rep1", "none", 0, 99, 1});
  auto input3 = cw->getExternal("sourceNode", {"system2", "in3", "entity1"}, false);

  // requireds
  auto required = cw->getExternal("requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"})},
                                  "requiredStream", {"system1", Gringo::Value("information", {"entity1", "scope3",
                                                                                              "rep1", "none"}),
                                                     10000, -100},
                                  true);

  // add transfer
  auto transfer = cw->getExternal("transfer", {"system2", "system1"}, "transfer", {"system2", "system1", 1, 2}, true);

  // add node1
  cw->add("node1", {}, "#external nodeTemplate(system1,node1,any).");
  auto node1 = cw->getExternal("nodeTemplate", {"system1", "node1", "any"}, "node1", {}, true);
  cw->add("node1", {}, "input(system1,node1,scope1,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "input(system1,node1,scope2,rep1,none,1,1) :- nodeTemplate(system1,node1,any).");
  cw->add("node1", {}, "output(system1,node1,scope3,rep1,none).");
  cw->add("node1", {}, "metadataOutput(delay,system1,node1,max,1,0).");
  cw->add("node1", {}, "metadataProcessing(cost,system1,node1,10).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node1,max,0,0).");
  cw->ground("node1", {});

  // add node2
  cw->add("node2", {}, "#external nodeTemplate(system1,node2,any).");
  auto node2 = cw->getExternal("nodeTemplate", {"system1", "node2", "any"}, "node2", {}, true);
  cw->add("node2", {}, "input(system1,node2,scope1,rep1,none,1,1) :- nodeTemplate(system1,node2,any).");
  cw->add("node2", {}, "input(system1,node2,scope2,rep1,none,2,2) :- nodeTemplate(system1,node2,any).");
  cw->add("node2", {}, "output(system1,node2,scope3,rep1,none).");
  cw->add("node2", {}, "metadataOutput(delay,system1,node2,max,1,0).");
  cw->add("node2", {}, "metadataProcessing(cost,system1,node2,5).");
  cw->add("node1", {}, "metadataOutput(accuracy,system1,node2,avg,0,1).");
  cw->ground("node2", {});

  auto query1 = cw->getExternal("query", {1}, "query", {1,3,10}, true);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),2),6)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),2),90)"));

  input3->assign(true);
  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(false, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system2,in3,entity1,none),information(entity1,scope2,rep1,none),2))"));
  EXPECT_EQ(false, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),3),6)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),3),2)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),3),99)"));

  // change optimization
  cw->add("updatedOptimization", {}, "self(system1).");
  cw->add("updatedOptimization", {}, "#minimize { 1@9,NODE,ENTITY,ENTITY2 : node(1,SYSTEM,NODE,ENTITY,ENTITY2), self(SELF), SYSTEM != SELF }.");
  cw->ground("updatedOptimization", {});

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(true, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(false, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),2),6)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node1,entity1,none),information(entity1,scope3,rep1,none),2),90)"));

  // disable node 1 -> nodes from other systems are required
  node1->assign(false);

  cw->solve();
//  cw->printLastModel();

  EXPECT_EQ(false, cw->query("node(1,system1,node1,entity1,none)"));
  EXPECT_EQ(true, cw->query("node(1,system1,node2,entity1,none)"));
  EXPECT_EQ(false, cw->query("connectToNode(node(1,system1,node1,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node2,entity1,none),stream(1,system1,node(1,system1,in1,entity1,none),information(entity1,scope1,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node2,entity1,none),stream(1,system1,node(1,system1,in2,entity1,none),information(entity1,scope2,rep1,none),1))"));
  EXPECT_EQ(true, cw->query("connectToNode(node(1,system1,node2,entity1,none),stream(1,system1,node(1,system2,in3,entity1,none),information(entity1,scope2,rep1,none),2))"));
  EXPECT_EQ(true, cw->query("metadataStream(1,delay,stream(1,system1,node(1,system1,node2,entity1,none),information(entity1,scope3,rep1,none),3),6)"));
  EXPECT_EQ(true, cw->query("metadataStream(1,accuracy,stream(1,system1,node(1,system1,node2,entity1,none),information(entity1,scope3,rep1,none),3),96)"));
}
