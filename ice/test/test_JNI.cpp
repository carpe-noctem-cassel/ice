#include <iostream>
#include <jni.h>
#include <vector>
#include <string>

#include <ros/package.h>

#include "ice/ontology/OntologyInterface.h"
//#include "ice/representation/RepresentationFactory.h"
#include "ice/representation/Representation.h"

#include "gtest/gtest.h"

namespace
{

TEST(JNITest, create)
{
  std::string path = ros::package::getPath("ice");
    bool result;

    ice::OntologyInterface oi(path + "/java/lib/");

    ASSERT_FALSE(oi.errorOccurred());
}

TEST(JNITest, loadOntology)
{
  std::string path = ros::package::getPath("ice");
  bool result;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");

  ASSERT_FALSE(oi.errorOccurred());

  result = oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.loadOntologies();

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.isConsistent();

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);
}

TEST(JNITest, getOntologyIDs)
{
  std::string path = ros::package::getPath("ice");
  bool result;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");

  ASSERT_FALSE(oi.errorOccurred());

  result = oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.loadOntologies();

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.isConsistent();

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  std::vector<std::pair<std::string,std::string>> ids;
  oi.getOntologyIDs(ids);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);
}

TEST(JNITest, addSystem)
{
  std::string path = ros::package::getPath("ice");
  bool result;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  result = oi.addSystem("TestSystem");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);
}

TEST(JNITest, addInfoStructure)
{
  std::string path = ros::package::getPath("ice");
  bool result;
  std::vector<std::string> metadatas;
  std::vector<int> metadataValues;
  std::vector<int> metadataValues2;
  std::vector<std::string> metadataGroundings;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  result = oi.addValueScope("TestValueScope", "TestValueScope1", "DoubleRep");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.addValueScope("TestValueScope", "TestValueScope2", "DoubleRep");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  std::vector<std::string> vec;
  vec.push_back("TestValueScope1");
  vec.push_back("TestValueScope2");
  result = oi.addRepresentation("TestRep", "TestRep1", vec);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  std::vector<std::string> reps;
  reps.push_back("TestRep1");
  result = oi.addEntityScope("TestScope", reps);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  std::vector<std::string> scopes;
  scopes.push_back("TestScope");
  result = oi.addEntityType("TestEntity", scopes);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  std::string str = oi.readInformationStructureAsASP();

  std::vector<std::string> toSearch;

  EXPECT_TRUE(str.find("entityType(http://vs.uni-kassel.de/Ice#TestEntity).") != std::string::npos);
  EXPECT_TRUE(str.find("scope(http://vs.uni-kassel.de/Ice#TestScope).") != std::string::npos);
  EXPECT_TRUE(str.find("valueScope(http://vs.uni-kassel.de/Ice#TestValueScope1).") != std::string::npos);
  EXPECT_TRUE(str.find("valueScope(http://vs.uni-kassel.de/Ice#TestValueScope2).") != std::string::npos);
  EXPECT_TRUE(str.find("representation(http://vs.uni-kassel.de/Ice#TestRep1).") != std::string::npos);

  EXPECT_TRUE(str.find("hasScope(http://vs.uni-kassel.de/Ice#TestEntity,http://vs.uni-kassel.de/Ice#TestScope).") != std::string::npos);
  EXPECT_TRUE(str.find("hasRepresentation(http://vs.uni-kassel.de/Ice#TestScope,http://vs.uni-kassel.de/Ice#TestRep1).") != std::string::npos);
  EXPECT_TRUE(str.find("hasDimension(http://vs.uni-kassel.de/Ice#TestRep1,http://vs.uni-kassel.de/Ice#TestValueScope1,http://vs.uni-kassel.de/Ice#DoubleRep).") != std::string::npos);
  EXPECT_TRUE(str.find("hasDimension(http://vs.uni-kassel.de/Ice#TestRep1,http://vs.uni-kassel.de/Ice#TestValueScope2,http://vs.uni-kassel.de/Ice#DoubleRep).") != std::string::npos);
}

TEST(JNITest, addDimension)
{
  std::string path = ros::package::getPath("ice");
  bool result;
  std::vector<std::string> metadatas;
  std::vector<int> metadataValues;
  std::vector<int> metadataValues2;
  std::vector<std::string> metadataGroundings;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  result = oi.addValueScope("TestValueScope", "TestValueScope1", "DoubleRep");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.addValueScope("TestValueScope", "TestValueScope2", "DoubleRep");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.addValueScope("TestValueScope", "TestValueScope3", "DoubleRep");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  std::vector<std::string> vec;
  vec.push_back("TestValueScope1");
  vec.push_back("TestValueScope2");
  result = oi.addRepresentation("TestRep", "TestRep1", vec);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  std::vector<std::string> reps;
  reps.push_back("TestRep1");
  result = oi.addEntityScope("TestScope", reps);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  std::vector<std::string> scopes;
  scopes.push_back("TestScope");
  result = oi.addEntityType("TestEntity", scopes);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.addDimensionToRep("TestRep1", "EulerAnglesRep", "Orientation");
  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.addDimensionToRep("TestRep1", "TestValueScope3");
  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.addDimensionToRep("TestRep1", "XCoordinate");
  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  oi.saveOntology("/tmp/text.owl");

  std::string str = oi.readInformationStructureAsASP();

  std::vector<std::string> toSearch;

  std::cout << str << std::endl;

  EXPECT_TRUE(str.find("entityType(http://vs.uni-kassel.de/Ice#TestEntity).") != std::string::npos);
  EXPECT_TRUE(str.find("scope(http://vs.uni-kassel.de/Ice#TestScope).") != std::string::npos);
  EXPECT_TRUE(str.find("valueScope(http://vs.uni-kassel.de/Ice#TestValueScope1).") != std::string::npos);
  EXPECT_TRUE(str.find("valueScope(http://vs.uni-kassel.de/Ice#TestValueScope2).") != std::string::npos);
  EXPECT_TRUE(str.find("representation(http://vs.uni-kassel.de/Ice#TestRep1).") != std::string::npos);

  EXPECT_TRUE(str.find("hasScope(http://vs.uni-kassel.de/Ice#TestEntity,http://vs.uni-kassel.de/Ice#TestScope).") != std::string::npos);
  EXPECT_TRUE(str.find("hasRepresentation(http://vs.uni-kassel.de/Ice#TestScope,http://vs.uni-kassel.de/Ice#TestRep1).") != std::string::npos);
  EXPECT_TRUE(str.find("hasDimension(http://vs.uni-kassel.de/Ice#TestRep1,http://vs.uni-kassel.de/Ice#TestValueScope1,http://vs.uni-kassel.de/Ice#DoubleRep).") != std::string::npos);
  EXPECT_TRUE(str.find("hasDimension(http://vs.uni-kassel.de/Ice#TestRep1,http://vs.uni-kassel.de/Ice#TestValueScope2,http://vs.uni-kassel.de/Ice#DoubleRep).") != std::string::npos);
  EXPECT_TRUE(str.find("hasDimension(http://vs.uni-kassel.de/Ice#TestRep1,http://vs.uni-kassel.de/Ice#Orientation,http://vs.uni-kassel.de/Ice#EulerAnglesRep).") != std::string::npos);
  EXPECT_TRUE(str.find("hasDimension(http://vs.uni-kassel.de/Ice#TestRep1,http://vs.uni-kassel.de/Ice#TestValueScope3,http://vs.uni-kassel.de/Ice#DoubleRep).") != std::string::npos);
  EXPECT_TRUE(str.find("hasDimension(http://vs.uni-kassel.de/Ice#TestRep1,http://vs.uni-kassel.de/Ice#XCoordinate,http://vs.uni-kassel.de/Ice#DoubleRep).") != std::string::npos);
}

TEST(JNITest, addStreamClass)
{
  std::string path = ros::package::getPath("ice");
  bool result;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  oi.addSystem("TestSystem");

  result = oi.addNamedStream("TestStream","Position","CoordinatePositionRep");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.addNamedStream("TestStream","Positionasdf","CoordinatePositionRep");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_FALSE(result);
}

TEST(JNITest, addRequiredStream)
{
  std::string path = ros::package::getPath("ice");
  bool result;

  ice::OntologyInterface oi(path + "/java/lib/");
  oi.setLogLevel(ice::Info);

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  oi.addSystem("TestSystem");

  result = oi.addNamedStream("TestStream","Position","CoordinatePositionRep");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.addRequiredStream("TestReqStream", "TestStream", "TestSystem", "", "");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_FALSE(result);

  result = oi.addIndividual("TestEntity", "Robot");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.addRequiredStream("TestReqStream", "TestStream", "TestSystem", "TestEntity", "");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  auto returnValues = oi.readNodesAsASP("TestSystem");

    ASSERT_FALSE(oi.errorOccurred());
    ASSERT_TRUE(result);

    ASSERT_EQ(returnValues->size(), 5);

    ASSERT_EQ(returnValues->at(0).size(), 1);
    ASSERT_EQ(returnValues->at(1).size(), 1);
    ASSERT_EQ(returnValues->at(2).size(), 1);
    ASSERT_EQ(returnValues->at(3).size(), 1);
    ASSERT_EQ(returnValues->at(4).size(), 1);

    EXPECT_EQ("REQUIRED_STREAM", returnValues->at(0).at(0));
    EXPECT_EQ("http://vs.uni-kassel.de/Ice#TestReqStream", returnValues->at(1).at(0));
    EXPECT_EQ("requiredStream(http://vs.uni-kassel.de/Ice#TestSystem,information(http://vs.uni-kassel.de/Ice#TestEntity,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none)).\n", returnValues->at(2).at(0));

    EXPECT_EQ(0, returnValues->at(4).at(0).length());
}

TEST(JNITest, addSourceNode)
{
  std::string path = ros::package::getPath("ice");
  bool result;
  std::vector<std::string> inputs;
  std::vector<int> inputsMin;
  std::vector<int> inputsMax;
  std::vector<std::string> outputs;
  std::vector<int> outputsMin;
  std::vector<int> outputsMax;
  std::vector<std::string> metadatas;
  std::vector<int> metadataValues;
  std::vector<int> metadataValues2;
  std::vector<std::string> metadataGroundings;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  oi.addSystem("TestSystem");

  result = oi.addNamedStream("TestStream","Position","CoordinatePositionRep");

  inputs.push_back("TestStream");
  inputsMin.push_back(1);
  inputsMax.push_back(1);

  result = oi.addSourceNodeClass("TestSourceNode", inputs, inputsMin, inputsMax);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.addIndividual("TestEntity", "Robot");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  metadatas.push_back("Delay");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeDelayFixASPGrounding");
  metadatas.push_back("Cost");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeCostASPGrounding");
  metadatas.push_back("Accuracy");
  metadataValues.push_back(-1);
  metadataValues2.push_back(-1);
  metadataGroundings.push_back("NodeAccuracyFixASPGrounding");

  result = oi.addNodeIndividual("TestSourceNodeInd", "TestSourceNode", "TestSystem", "TestEntity", "", metadatas, metadataValues, metadataValues2,
                                metadataGroundings);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  auto returnValues = oi.readNodesAsASP("TestSystem");

  ASSERT_EQ(returnValues->size(), 5);

  ASSERT_EQ(returnValues->at(0).size(), 1);
  ASSERT_EQ(returnValues->at(1).size(), 1);
  ASSERT_EQ(returnValues->at(2).size(), 1);
  ASSERT_EQ(returnValues->at(3).size(), 1);
  ASSERT_EQ(returnValues->at(4).size(), 1);

  EXPECT_EQ("SOURCE_NODE", returnValues->at(0).at(0));
  EXPECT_EQ("http://vs.uni-kassel.de/Ice#TestSourceNodeInd", returnValues->at(1).at(0));
  EXPECT_EQ("sourceNode(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestSourceNodeInd,http://vs.uni-kassel.de/Ice#TestEntity).\n", returnValues->at(2).at(0));

  std::string toTest = returnValues->at(3).at(0);
  EXPECT_TRUE(toTest.find("#external sourceNode(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestSourceNodeInd,http://vs.uni-kassel.de/Ice#TestEntity).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataProcessing(cost,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestSourceNodeInd,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(delay,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestSourceNodeInd,fix,5,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(accuracy,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestSourceNodeInd,fix,-1,-1).") != std::string::npos);
  EXPECT_TRUE(toTest.find("output(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestSourceNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none).") != std::string::npos);

  EXPECT_EQ(0, returnValues->at(4).at(0).length());
}

TEST(JNITest, addComputationNode)
{
  std::string path = ros::package::getPath("ice");
  bool result;
  std::vector<std::string> inputs;
  std::vector<int> inputsMin;
  std::vector<int> inputsMax;
  std::vector<std::string> outputs;
  std::vector<int> outputsMin;
  std::vector<int> outputsMax;
  std::vector<std::string> metadatas;
  std::vector<int> metadataValues;
  std::vector<int> metadataValues2;
  std::vector<std::string> metadataGroundings;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  oi.addSystem("TestSystem");

  result = oi.addNamedStream("TestStream","Position","CoordinatePositionRep");

  inputs.push_back("TestStream");
  inputsMin.push_back(1);
  inputsMax.push_back(2);
  outputs.push_back("TestStream");
  outputsMin.push_back(1);
  outputsMax.push_back(1);

  result = oi.addComputationNodeClass("TestNode", inputs, inputsMin, inputsMax, outputs, outputsMin, outputsMax);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  metadatas.push_back("Delay");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeDelayASPGrounding");
  metadatas.push_back("Cost");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeCostASPGrounding");
  metadatas.push_back("Accuracy");
  metadataValues.push_back(-1);
  metadataValues2.push_back(-1);
  metadataGroundings.push_back("NodeAccuracyMaxASPGrounding");

  result = oi.addNodeIndividual("TestNodeInd", "TestNode", "TestSystem", "", "", metadatas, metadataValues, metadataValues2,
                                metadataGroundings);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  auto returnValues = oi.readNodesAsASP("TestSystem");

  ASSERT_EQ(returnValues->size(), 5);

  ASSERT_EQ(returnValues->at(0).size(), 1);
  ASSERT_EQ(returnValues->at(1).size(), 1);
  ASSERT_EQ(returnValues->at(2).size(), 1);
  ASSERT_EQ(returnValues->at(3).size(), 1);
  ASSERT_EQ(returnValues->at(4).size(), 1);

  EXPECT_EQ("COMPUTATION_NODE", returnValues->at(0).at(0));
  EXPECT_EQ("http://vs.uni-kassel.de/Ice#TestNodeInd", returnValues->at(1).at(0));
  EXPECT_EQ("nodeTemplate(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,any).\n", returnValues->at(2).at(0));


  std::string toTest = returnValues->at(3).at(0);
  EXPECT_TRUE(toTest.find("#external nodeTemplate(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,any).") != std::string::npos);
  EXPECT_TRUE(toTest.find("output(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none).") != std::string::npos);
  EXPECT_TRUE(toTest.find("input(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none,1,2).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(delay,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,max,5,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataProcessing(cost,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(accuracy,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,max,-1,-1).") != std::string::npos);
  EXPECT_TRUE(toTest.find("output(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none).") != std::string::npos);

  EXPECT_EQ(0, returnValues->at(4).at(0).length());
}


TEST(JNITest, addIroNode)
{
  std::string path = ros::package::getPath("ice");
  bool result;
  std::vector<std::string> inputs;
  std::vector<int> inputsMin;
  std::vector<int> inputsMax;
  std::vector<std::string> inputsRelated;
  std::vector<int> inputsRelatedMin;
  std::vector<int> inputsRelatedMax;
  std::vector<std::string> outputs;
  std::vector<int> outputsMin;
  std::vector<int> outputsMax;
  std::vector<std::string> metadatas;
  std::vector<int> metadataValues;
  std::vector<int> metadataValues2;
  std::vector<std::string> metadataGroundings;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  oi.addSystem("TestSystem");

  result = oi.addNamedStream("TestStream","Position","CoordinatePositionRep");

  inputs.push_back("TestStream");
  inputsMin.push_back(1);
  inputsMax.push_back(2);
  inputsRelated.push_back("TestStream");
  inputsRelatedMin.push_back(1);
  inputsRelatedMax.push_back(1);
  outputs.push_back("TestStream");
  outputsMin.push_back(1);
  outputsMax.push_back(1);

  result = oi.addTransformationNodeClass("TestNode", inputs, inputsMin, inputsMax, inputsRelated, inputsRelatedMin,
                              inputsRelatedMax, outputs, outputsMin, outputsMax);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  metadatas.push_back("Delay");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeDelayASPGrounding");
  metadatas.push_back("Cost");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeCostASPGrounding");
  metadatas.push_back("Accuracy");
  metadataValues.push_back(-1);
  metadataValues2.push_back(-1);
  metadataGroundings.push_back("NodeAccuracyMaxASPGrounding");

  result = oi.addNodeIndividual("TestNodeInd", "TestNode", "TestSystem", "", "", metadatas, metadataValues, metadataValues2,
                                metadataGroundings);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  auto returnValues = oi.readNodesAsASP("TestSystem");

  ASSERT_EQ(returnValues->size(), 5);

  ASSERT_EQ(returnValues->at(0).size(), 1);
  ASSERT_EQ(returnValues->at(1).size(), 1);
  ASSERT_EQ(returnValues->at(2).size(), 1);
  ASSERT_EQ(returnValues->at(3).size(), 1);
  ASSERT_EQ(returnValues->at(4).size(), 1);

  EXPECT_EQ("TRANSFORMATION_NODE", returnValues->at(0).at(0));
  EXPECT_EQ("http://vs.uni-kassel.de/Ice#TestNodeInd", returnValues->at(1).at(0));
  EXPECT_EQ("transformation(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,any,none).\n", returnValues->at(2).at(0));

  std::string toTest = returnValues->at(3).at(0);
  EXPECT_TRUE(toTest.find("#external transformation(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,any,none).") != std::string::npos);
  EXPECT_TRUE(toTest.find("input(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none,1,2).") != std::string::npos);
  EXPECT_TRUE(toTest.find("input2(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none,1,1).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(delay,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,max,5,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataProcessing(cost,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(accuracy,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,max,-1,-1).") != std::string::npos);
  EXPECT_TRUE(toTest.find("output(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none).") != std::string::npos);

  EXPECT_EQ(0, returnValues->at(4).at(0).length());
}

TEST(JNITest, addNodeToSystem)
{
  std::string path = ros::package::getPath("ice");
  bool result;
  std::vector<std::string> toAdd;
  std::vector<std::string> inputs;
  std::vector<int> inputsMin;
  std::vector<int> inputsMax;
  std::vector<std::string> outputs;
  std::vector<int> outputsMin;
  std::vector<int> outputsMax;
  std::vector<std::string> metadatas;
  std::vector<int> metadataValues;
  std::vector<int> metadataValues2;
  std::vector<std::string> metadataGroundings;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  oi.addSystem("TestSystem");

  result = oi.addNamedStream("TestStream","Position","CoordinatePositionRep");

  inputs.push_back("TestStream");
  inputsMin.push_back(1);
  inputsMax.push_back(1);
  result = oi.addSourceNodeClass("TestSourceNode", outputs, outputsMin, outputsMax);

  result = oi.addIndividual("TestEntity", "Robot");

  metadatas.push_back("Delay");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeDelayFixASPGrounding");
  metadatas.push_back("Cost");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeCostASPGrounding");
  metadatas.push_back("Accuracy");
  metadataValues.push_back(-1);
  metadataValues2.push_back(-1);
  metadataGroundings.push_back("NodeAccuracyFixASPGrounding");

  result = oi.addNodeIndividual("TestSourceNodeInd", "TestSourceNode", "TestSystem", "TestEntity", "", metadatas, metadataValues, metadataValues2,
                                metadataGroundings);

  result = oi.addSystem("TestSystem2");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  toAdd.push_back("TestSourceNodeInd");

  result = oi.addNodesToSystem("TestSystem2", toAdd);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  auto returnValues = oi.readNodesAsASP("TestSystem2");

  ASSERT_EQ(returnValues->size(), 5);

  ASSERT_EQ(returnValues->at(0).size(), 1);
  ASSERT_EQ(returnValues->at(1).size(), 1);
  ASSERT_EQ(returnValues->at(2).size(), 1);
  ASSERT_EQ(returnValues->at(3).size(), 1);
  ASSERT_EQ(returnValues->at(4).size(), 1);

  EXPECT_EQ("SOURCE_NODE", returnValues->at(0).at(0));
  EXPECT_EQ("http://vs.uni-kassel.de/Ice#TestSourceNodeInd", returnValues->at(1).at(0));
  EXPECT_EQ("sourceNode(http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestSourceNodeInd,http://vs.uni-kassel.de/Ice#TestEntity).\n", returnValues->at(2).at(0));

  std::string toTest = returnValues->at(3).at(0);
  EXPECT_TRUE(toTest.find("#external sourceNode(http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestSourceNodeInd,http://vs.uni-kassel.de/Ice#TestEntity).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataProcessing(cost,http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestSourceNodeInd,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(delay,http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestSourceNodeInd,fix,5,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(accuracy,http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestSourceNodeInd,fix,-1,-1).") != std::string::npos);

  EXPECT_EQ(0, returnValues->at(4).at(0).length());
}


TEST(JNITest, addMapNodeToSystem1)
{
  std::string path = ros::package::getPath("ice");
  bool result;
  std::vector<std::string> toAdd;
  std::vector<std::string> inputs;
  std::vector<int> inputsMin;
  std::vector<int> inputsMax;
  std::vector<std::string> inputsRelated;
  std::vector<int> inputsRelatedMin;
  std::vector<int> inputsRelatedMax;
  std::vector<std::string> inputMaps;
  std::vector<int> inputMapsMin;
  std::vector<int> inputMapsMax;
  std::vector<std::string> outputs;
  std::vector<int> outputsMin;
  std::vector<int> outputsMax;
  std::vector<std::string> metadatas;
  std::vector<int> metadataValues;
  std::vector<int> metadataValues2;
  std::vector<std::string> metadataGroundings;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  oi.addSystem("TestSystem");

  result = oi.addNamedSet("TestMap", "Robot", "Position", "CoordinatePositionRep");

  result = oi.addNamedStream("TestStream","Position","CoordinatePositionRep");

  inputs.push_back("TestStream");
  inputsMin.push_back(1);
  inputsMax.push_back(1);

  outputs.push_back("TestMap");
  outputsMin.push_back(1);
  outputsMax.push_back(1);

  result = oi.addSetNodeClass("TestMapNode", inputs, inputsMin, inputsMax, inputsRelated, inputsRelatedMin,
                              inputsRelatedMax, inputMaps, inputMapsMin, inputMapsMax, outputs, outputsMin, outputsMax);

  result = oi.addIndividual("TestEntity1", "Robot");
  result = oi.addIndividual("TestEntity2", "Robot");

  metadatas.push_back("Delay");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeDelayFixASPGrounding");
  metadatas.push_back("Cost");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeCostASPGrounding");
  metadatas.push_back("Accuracy");
  metadataValues.push_back(-1);
  metadataValues2.push_back(-1);
  metadataGroundings.push_back("NodeAccuracyFixASPGrounding");

  result = oi.addNodeIndividual("TestMapNodeInd", "TestMapNode", "TestSystem", "", "", metadatas, metadataValues, metadataValues2,
                                metadataGroundings);


  result = oi.addSystem("TestSystem2");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  toAdd.push_back("TestMapNodeInd");

  result = oi.addNodesToSystem("TestSystem2", toAdd);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  auto returnValues = oi.readNodesAsASP("TestSystem2");

  ASSERT_EQ(returnValues->size(), 5);

  ASSERT_EQ(returnValues->at(0).size(), 1);
  ASSERT_EQ(returnValues->at(1).size(), 1);
  ASSERT_EQ(returnValues->at(2).size(), 1);
  ASSERT_EQ(returnValues->at(3).size(), 1);
  ASSERT_EQ(returnValues->at(4).size(), 1);

  EXPECT_EQ("SET_NODE", returnValues->at(0).at(0));
  EXPECT_EQ("http://vs.uni-kassel.de/Ice#TestMapNodeInd", returnValues->at(1).at(0));
  EXPECT_EQ("setNodeTemplate(http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,any).\n", returnValues->at(2).at(0));

  std::string toTest = returnValues->at(3).at(0);

  EXPECT_TRUE(toTest.find("#external setNodeTemplate(http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,any)") != std::string::npos);
  EXPECT_TRUE(toTest.find("input(http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none,1,1).") != std::string::npos);
  EXPECT_TRUE(toTest.find("outputSet(http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,http://vs.uni-kassel.de/Ice#Robot,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataProcessing(cost,http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(delay,http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,fix,5,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(accuracy,http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,fix,-1,-1).") != std::string::npos);

  EXPECT_EQ(0, returnValues->at(4).at(0).length());
}

TEST(JNITest, addMapNodeToSystem2)
{
  std::string path = ros::package::getPath("ice");
  bool result;
  std::vector<std::string> toAdd;
  std::vector<std::string> inputs;
  std::vector<int> inputsMin;
  std::vector<int> inputsMax;
  std::vector<std::string> inputsRelated;
  std::vector<int> inputsRelatedMin;
  std::vector<int> inputsRelatedMax;
  std::vector<std::string> inputMaps;
  std::vector<int> inputMapsMin;
  std::vector<int> inputMapsMax;
  std::vector<std::string> outputs;
  std::vector<int> outputsMin;
  std::vector<int> outputsMax;
  std::vector<std::string> metadatas;
  std::vector<int> metadataValues;
  std::vector<int> metadataValues2;
  std::vector<std::string> metadataGroundings;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  oi.addSystem("TestSystem");

  result = oi.addNamedSet("TestMap", "Robot", "Position", "CoordinatePositionRep");

  result = oi.addNamedStream("TestStream","Position","CoordinatePositionRep");

  inputs.push_back("TestStream");
  inputsMin.push_back(1);
  inputsMax.push_back(1);

  outputs.push_back("TestMap");
  outputsMin.push_back(1);
  outputsMax.push_back(1);


  inputMaps.push_back("TestMap");
  inputMapsMin.push_back(1);
  inputMapsMax.push_back(2);

  result = oi.addSetNodeClass("TestMapNode", inputs, inputsMin, inputsMax, inputsRelated, inputsRelatedMin,
                              inputsRelatedMax, inputMaps, inputMapsMin, inputMapsMax, outputs, outputsMin, outputsMax);

  result = oi.addIndividual("TestEntity1", "Robot");
  result = oi.addIndividual("TestEntity2", "Robot");

  metadatas.push_back("Delay");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeDelayFixASPGrounding");
  metadatas.push_back("Cost");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeCostASPGrounding");
  metadatas.push_back("Accuracy");
  metadataValues.push_back(-1);
  metadataValues2.push_back(-1);
  metadataGroundings.push_back("NodeAccuracyFixASPGrounding");

  result = oi.addNodeIndividual("TestMapNodeInd", "TestMapNode", "TestSystem", "", "", metadatas, metadataValues, metadataValues2,
                                metadataGroundings);


  result = oi.addSystem("TestSystem2");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  toAdd.push_back("TestMapNodeInd");

  result = oi.addNodesToSystem("TestSystem2", toAdd);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  auto returnValues = oi.readNodesAsASP("TestSystem2");

  ASSERT_EQ(returnValues->size(), 5);

  ASSERT_EQ(returnValues->at(0).size(), 1);
  ASSERT_EQ(returnValues->at(1).size(), 1);
  ASSERT_EQ(returnValues->at(2).size(), 1);
  ASSERT_EQ(returnValues->at(3).size(), 1);
  ASSERT_EQ(returnValues->at(4).size(), 1);

  EXPECT_EQ("SET_NODE", returnValues->at(0).at(0));
  EXPECT_EQ("http://vs.uni-kassel.de/Ice#TestMapNodeInd", returnValues->at(1).at(0));
  EXPECT_EQ("setNodeTemplate(http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,any).\n", returnValues->at(2).at(0));

  std::string toTest = returnValues->at(3).at(0);

  EXPECT_TRUE(toTest.find("#external setNodeTemplate(http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,any)") != std::string::npos);
  EXPECT_TRUE(toTest.find("input(http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none,1,1).") != std::string::npos);
  EXPECT_TRUE(toTest.find("inputSet(http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,http://vs.uni-kassel.de/Ice#Robot,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none,1,2).") != std::string::npos);
  EXPECT_TRUE(toTest.find("outputSet(http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,http://vs.uni-kassel.de/Ice#Robot,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataProcessing(cost,http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(delay,http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,fix,5,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(accuracy,http://vs.uni-kassel.de/Ice#TestSystem2,http://vs.uni-kassel.de/Ice#TestMapNodeInd,fix,-1,-1).") != std::string::npos);

  EXPECT_EQ(0, returnValues->at(4).at(0).length());
}

TEST(JNITest, addRequiredSet)
{
  std::string path = ros::package::getPath("ice");
  bool result;
  std::vector<std::string> toAdd;
  std::vector<std::string> inputs;
  std::vector<int> inputsMin;
  std::vector<int> inputsMax;
  std::vector<std::string> inputsRelated;
  std::vector<int> inputsRelatedMin;
  std::vector<int> inputsRelatedMax;
  std::vector<std::string> inputMaps;
  std::vector<int> inputMapsMin;
  std::vector<int> inputMapsMax;
  std::vector<std::string> outputs;
  std::vector<int> outputsMin;
  std::vector<int> outputsMax;
  std::vector<std::string> metadatas;
  std::vector<int> metadataValues;
  std::vector<int> metadataValues2;
  std::vector<std::string> metadataGroundings;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  oi.addSystem("TestSystem");

  result = oi.addNamedSet("TestMap", "Robot", "Position", "CoordinatePositionRep");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.addRequiredSet("ReqMap", "TestMap", "TestSystem", "");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  auto returnValues = oi.readNodesAsASP("TestSystem");

  ASSERT_EQ(returnValues->size(), 5);

  ASSERT_EQ(returnValues->at(0).size(), 1);
  ASSERT_EQ(returnValues->at(1).size(), 1);
  ASSERT_EQ(returnValues->at(2).size(), 1);
  ASSERT_EQ(returnValues->at(3).size(), 1);
  ASSERT_EQ(returnValues->at(4).size(), 1);

  EXPECT_EQ("REQUIRED_SET", returnValues->at(0).at(0));
  EXPECT_EQ("http://vs.uni-kassel.de/Ice#ReqMap", returnValues->at(1).at(0));
  EXPECT_EQ("requiredSet(http://vs.uni-kassel.de/Ice#TestSystem,informationType(http://vs.uni-kassel.de/Ice#Robot,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none)).\n", returnValues->at(2).at(0));

  std::string toTest = returnValues->at(3).at(0);

  EXPECT_TRUE(toTest.find("#external requiredSet(http://vs.uni-kassel.de/Ice#TestSystem,informationType(http://vs.uni-kassel.de/Ice#Robot,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none))") != std::string::npos);

  EXPECT_EQ(0, returnValues->at(4).at(0).length());
}

TEST(JNITest, save)
{
  std::string path = ros::package::getPath("ice");
  bool result;
  std::vector<std::string> inputs;
  std::vector<int> inputsMin;
  std::vector<int> inputsMax;
  std::vector<std::string> outputs;
  std::vector<int> outputsMin;
  std::vector<int> outputsMax;
  std::vector<std::string> metadatas;
  std::vector<int> metadataValues;
  std::vector<int> metadataValues2;
  std::vector<std::string> metadataGroundings;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");
  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
  oi.loadOntologies();

  oi.addSystem("TestSystem");

  result = oi.addNamedStream("TestStream","Position","CoordinatePositionRep");

  inputs.push_back("TestStream");
  inputsMin.push_back(1);
  inputsMax.push_back(2);
  outputs.push_back("TestStream");
  outputsMin.push_back(1);
  outputsMax.push_back(1);

  result = oi.addComputationNodeClass("TestNode", inputs, inputsMin, inputsMax, outputs, outputsMin, outputsMax);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  metadatas.push_back("Delay");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeDelayASPGrounding");
  metadatas.push_back("Cost");
  metadataValues.push_back(5);
  metadataValues2.push_back(5);
  metadataGroundings.push_back("NodeCostASPGrounding");
  metadatas.push_back("Accuracy");
  metadataValues.push_back(-1);
  metadataValues2.push_back(-1);
  metadataGroundings.push_back("NodeAccuracyMaxASPGrounding");

  result = oi.addNodeIndividual("TestNodeInd", "TestNode", "TestSystem", "", "", metadatas, metadataValues, metadataValues2,
                                metadataGroundings);

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.saveOntology("/tmp/testSaveOnt.owl");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  ice::OntologyInterface oi2("/java/lib/");

  oi2.addIRIMapper(path + "/ontology/");
  oi2.loadOntology("/tmp/testSaveOnt.owl");

  //oi2.getSystems();
  ASSERT_FALSE(oi.errorOccurred());

  oi2.readInformationStructureAsASP();
  ASSERT_FALSE(oi.errorOccurred());

  auto returnValues = oi2.readNodesAsASP("TestSystem");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  ASSERT_EQ(returnValues->size(), 5);

  ASSERT_EQ(1, returnValues->at(0).size());
  ASSERT_EQ(1, returnValues->at(1).size());
  ASSERT_EQ(1, returnValues->at(2).size());
  ASSERT_EQ(1, returnValues->at(3).size());
  ASSERT_EQ(1, returnValues->at(4).size());

  EXPECT_EQ("COMPUTATION_NODE", returnValues->at(0).at(0));
  EXPECT_EQ("http://vs.uni-kassel.de/Ice#TestNodeInd", returnValues->at(1).at(0));
  EXPECT_EQ("nodeTemplate(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,any).\n", returnValues->at(2).at(0));

  std::string toTest = returnValues->at(3).at(0);
  EXPECT_TRUE(toTest.find("#external nodeTemplate(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,any).") != std::string::npos);
  EXPECT_TRUE(toTest.find("output(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none).") != std::string::npos);
  EXPECT_TRUE(toTest.find("input(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none,1,2).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(delay,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,max,5,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataProcessing(cost,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,5).") != std::string::npos);
  EXPECT_TRUE(toTest.find("metadataOutput(accuracy,http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,max,-1,-1).") != std::string::npos);
  EXPECT_TRUE(toTest.find("output(http://vs.uni-kassel.de/Ice#TestSystem,http://vs.uni-kassel.de/Ice#TestNodeInd,http://vs.uni-kassel.de/Ice#Position,http://vs.uni-kassel.de/Ice#CoordinatePositionRep,none).") != std::string::npos);

  EXPECT_EQ(0, returnValues->at(4).at(0).length());
}

//TEST(JNITest, create)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  ASSERT_FALSE(oi.errorOccurred());
//}
//
//TEST(JNITest, loadOntology)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//
//  ASSERT_FALSE(oi.errorOccurred());
//
//  result = oi.addOntologyIRI(
//      "http://vs.uni-kassel.de/Ice");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.loadOntologies();
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.isConsistent();
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//}
//
//TEST(JNITest, addSystem)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  result = oi.addSystem("TestSystem");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//}
//
//TEST(JNITest, addInfoStructure)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//  std::vector<std::string> metadatas;
//  std::vector<int> metadataValues;
//  std::vector<int> metadataValues2;
//  std::vector<std::string> metadataGroundings;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  result = oi.addValueScope("TestValueScope", "TestValueScope1");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.addValueScope("TestValueScope", "TestValueScope2");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  std::vector<std::string> vec;
//  vec.push_back("TestValueScope1");
//  vec.push_back("TestValueScope2");
//  result = oi.addRepresentation("TestRep", "TestRep1", vec);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  std::vector<std::string> reps;
//  reps.push_back("TestRep1");
//  result = oi.addEntityScope("TestScope", reps);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  std::vector<std::string> scopes;
//  scopes.push_back("TestScope");
//  result = oi.addEntityType("TestEntity", scopes);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  std::string str = oi.readInformationStructureAsASP();
//
//  std::vector<std::string> toSearch;
//
//  EXPECT_TRUE(str.find("entityType(testEntity).") >= 0);
//  EXPECT_TRUE(str.find("scope(testScope).") >= 0);
//  EXPECT_TRUE(str.find("valueScope(testValueScope1).") >= 0);
//  EXPECT_TRUE(str.find("valueScope(testValueScope2).") >= 0);
//  EXPECT_TRUE(str.find("representation(testRep1).") >= 0);
//
//  EXPECT_TRUE(str.find("hasScope(testEntity,testScope).") >= 0);
//  EXPECT_TRUE(str.find("hasRepresentation(testScope,testRep1).") >= 0);
//  EXPECT_TRUE(str.find("hasDimension(testRep1,testValueScope1).") >= 0);
//  EXPECT_TRUE(str.find("hasDimension(testRep1,testValueScope2).") >= 0);
//}
//
//TEST(JNITest, addStreamClass)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  oi.addSystem("TestSystem");
//
//  result = oi.addNamedStream("TestStream", "Position", "CoordinatePositionRep");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.addNamedStream("TestStream", "Positionasdf",
//      "CoordinatePositionRep");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_FALSE(result);
//}
//
//TEST(JNITest, addRequiredStream)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  oi.addSystem("TestSystem");
//
//  result = oi.addNamedStream("TestStream", "Position", "CoordinatePositionRep");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.addRequiredStream("TestReqStream", "TestStream", "TestSystem", "",
//      "");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_FALSE(result);
//
//  result = oi.addIndividual("TestEntity", "Robot");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.addRequiredStream("TestReqStream", "TestStream", "TestSystem",
//      "TestEntity", "");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  auto returnValues = oi.readNodesAndIROsAsASP("TestSystem");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  ASSERT_EQ(returnValues->size(), 5);
//
//  ASSERT_EQ(returnValues->at(0)->size(), 1);
//  ASSERT_EQ(returnValues->at(1)->size(), 1);
//  ASSERT_EQ(returnValues->at(2)->size(), 1);
//  ASSERT_EQ(returnValues->at(3)->size(), 1);
//  ASSERT_EQ(returnValues->at(4)->size(), 1);
//
//  EXPECT_EQ("REQUIRED_STREAM", std::string(returnValues->at(0)->at(0)));
//  EXPECT_EQ("testReqStream", std::string(returnValues->at(1)->at(0)));
//  EXPECT_EQ(
//      "requiredStream(testSystem,information(testEntity,position,coordinatePositionRep,none)).\n",
//      std::string(returnValues->at(2)->at(0)));
//
//  EXPECT_TRUE(strlen(returnValues->at(4)->at(0)) == 0);
//}
//
//TEST(JNITest, addSourceNode)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//  std::vector<std::string> inputs;
//  std::vector<int> inputsMin;
//  std::vector<int> inputsMax;
//  std::vector<std::string> outputs;
//  std::vector<int> outputsMin;
//  std::vector<int> outputsMax;
//  std::vector<std::string> metadatas;
//  std::vector<int> metadataValues;
//  std::vector<int> metadataValues2;
//  std::vector<std::string> metadataGroundings;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  oi.addSystem("TestSystem");
//
//  result = oi.addNamedStream("TestStream", "Position", "CoordinatePositionRep");
//
//  inputs.push_back("TestStream");
//  inputsMin.push_back(1);
//  inputsMax.push_back(1);
//
//  result = oi.addSourceNodeClass("TestSourceNode", inputs, inputsMin,
//      inputsMax);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.addIndividual("TestEntity", "Robot");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  metadatas.push_back("Delay");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeDelayFixASPGrounding");
//  metadatas.push_back("Cost");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeCostASPGrounding");
//  metadatas.push_back("Accuracy");
//  metadataValues.push_back(-1);
//  metadataValues2.push_back(-1);
//  metadataGroundings.push_back("NodeAccuracyFixASPGrounding");
//
//  result = oi.addNodeIndividual("TestSourceNodeInd", "TestSourceNode",
//      "TestSystem", "TestEntity", "", metadatas, metadataValues,
//      metadataValues2, metadataGroundings);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  auto returnValues = oi.readNodesAndIROsAsASP("TestSystem");
//
//  ASSERT_EQ(returnValues->size(), 5);
//
//  ASSERT_EQ(returnValues->at(0)->size(), 1);
//  ASSERT_EQ(returnValues->at(1)->size(), 1);
//  ASSERT_EQ(returnValues->at(2)->size(), 1);
//  ASSERT_EQ(returnValues->at(3)->size(), 1);
//  ASSERT_EQ(returnValues->at(4)->size(), 1);
//
//  EXPECT_EQ("SOURCE_NODE", std::string(returnValues->at(0)->at(0)));
//  EXPECT_EQ("testSourceNodeInd", std::string(returnValues->at(1)->at(0)));
//  EXPECT_EQ("sourceNode(testSystem,testSourceNodeInd,testEntity).\n",
//      std::string(returnValues->at(2)->at(0)));
//
//  std::string toTest = std::string(returnValues->at(3)->at(0));
//  EXPECT_TRUE(
//      toTest.find(
//          "#external sourceNode(testSystem,testSourceNodeInd,testEntity).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataProcessing(cost,testSystem,testSourceNodeInd,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataOutput(delay,testSystem,testSourceNodeInd,fix,5,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "metadataOutput(accuracy,testSystem,testSourceNodeInd,fix,-1,-1).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "output(testSystem,testSourceNodeInd,position,coordinatePositionRep,none).")
//          != std::string::npos);
//
//  EXPECT_TRUE(strlen(returnValues->at(4)->at(0)) == 0);
//}
//
//TEST(JNITest, addComputationNode)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//  std::vector<std::string> inputs;
//  std::vector<int> inputsMin;
//  std::vector<int> inputsMax;
//  std::vector<std::string> outputs;
//  std::vector<int> outputsMin;
//  std::vector<int> outputsMax;
//  std::vector<std::string> metadatas;
//  std::vector<int> metadataValues;
//  std::vector<int> metadataValues2;
//  std::vector<std::string> metadataGroundings;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  oi.addSystem("TestSystem");
//
//  result = oi.addNamedStream("TestStream", "Position", "CoordinatePositionRep");
//
//  inputs.push_back("TestStream");
//  inputsMin.push_back(1);
//  inputsMax.push_back(2);
//  outputs.push_back("TestStream");
//  outputsMin.push_back(1);
//  outputsMax.push_back(1);
//
//  result = oi.addComputationNodeClass("TestNode", inputs, inputsMin, inputsMax,
//      outputs, outputsMin, outputsMax);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  metadatas.push_back("Delay");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeDelayASPGrounding");
//  metadatas.push_back("Cost");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeCostASPGrounding");
//  metadatas.push_back("Accuracy");
//  metadataValues.push_back(-1);
//  metadataValues2.push_back(-1);
//  metadataGroundings.push_back("NodeAccuracyMaxASPGrounding");
//
//  result = oi.addNodeIndividual("TestNodeInd", "TestNode", "TestSystem", "", "",
//      metadatas, metadataValues, metadataValues2, metadataGroundings);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  auto returnValues = oi.readNodesAndIROsAsASP("TestSystem");
//
//  ASSERT_EQ(returnValues->size(), 5);
//
//  ASSERT_EQ(returnValues->at(0)->size(), 1);
//  ASSERT_EQ(returnValues->at(1)->size(), 1);
//  ASSERT_EQ(returnValues->at(2)->size(), 1);
//  ASSERT_EQ(returnValues->at(3)->size(), 1);
//  ASSERT_EQ(returnValues->at(4)->size(), 1);
//
//  EXPECT_EQ("COMPUTATION_NODE", std::string(returnValues->at(0)->at(0)));
//  EXPECT_EQ("testNodeInd", std::string(returnValues->at(1)->at(0)));
//  EXPECT_EQ("nodeTemplate(testSystem,testNodeInd,any).\n",
//      std::string(returnValues->at(2)->at(0)));
//
//  std::string toTest = std::string(returnValues->at(3)->at(0));
//  EXPECT_TRUE(
//      toTest.find("#external nodeTemplate(testSystem,testNodeInd,any).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "output(testSystem,testNodeInd,position,coordinatePositionRep,none).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "input(testSystem,testNodeInd,position,coordinatePositionRep,none,1,2).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataOutput(delay,testSystem,testNodeInd,max,5,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataProcessing(cost,testSystem,testNodeInd,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataOutput(accuracy,testSystem,testNodeInd,max,-1,-1).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "output(testSystem,testNodeInd,position,coordinatePositionRep,none).")
//          != std::string::npos);
//
//  EXPECT_TRUE(strlen(returnValues->at(4)->at(0)) == 0);
//}
//
//TEST(JNITest, addIroNode)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//  std::vector<std::string> inputs;
//  std::vector<int> inputsMin;
//  std::vector<int> inputsMax;
//  std::vector<std::string> inputsRelated;
//  std::vector<int> inputsRelatedMin;
//  std::vector<int> inputsRelatedMax;
//  std::vector<std::string> outputs;
//  std::vector<int> outputsMin;
//  std::vector<int> outputsMax;
//  std::vector<std::string> metadatas;
//  std::vector<int> metadataValues;
//  std::vector<int> metadataValues2;
//  std::vector<std::string> metadataGroundings;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  oi.addSystem("TestSystem");
//
//  result = oi.addNamedStream("TestStream", "Position", "CoordinatePositionRep");
//
//  inputs.push_back("TestStream");
//  inputsMin.push_back(1);
//  inputsMax.push_back(2);
//  inputsRelated.push_back("TestStream");
//  inputsRelatedMin.push_back(1);
//  inputsRelatedMax.push_back(1);
//  outputs.push_back("TestStream");
//  outputsMin.push_back(1);
//  outputsMax.push_back(1);
//
//  result = oi.addIroNodeClass("TestNode", inputs, inputsMin, inputsMax,
//      inputsRelated, inputsRelatedMin, inputsRelatedMax, outputs, outputsMin,
//      outputsMax);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  metadatas.push_back("Delay");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeDelayASPGrounding");
//  metadatas.push_back("Cost");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeCostASPGrounding");
//  metadatas.push_back("Accuracy");
//  metadataValues.push_back(-1);
//  metadataValues2.push_back(-1);
//  metadataGroundings.push_back("NodeAccuracyMaxASPGrounding");
//
//  result = oi.addNodeIndividual("TestNodeInd", "TestNode", "TestSystem", "", "",
//      metadatas, metadataValues, metadataValues2, metadataGroundings);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  auto returnValues = oi.readNodesAndIROsAsASP("TestSystem");
//
//  ASSERT_EQ(returnValues->size(), 5);
//
//  ASSERT_EQ(returnValues->at(0)->size(), 1);
//  ASSERT_EQ(returnValues->at(1)->size(), 1);
//  ASSERT_EQ(returnValues->at(2)->size(), 1);
//  ASSERT_EQ(returnValues->at(3)->size(), 1);
//  ASSERT_EQ(returnValues->at(4)->size(), 1);
//
//  EXPECT_EQ("IRO_NODE", std::string(returnValues->at(0)->at(0)));
//  EXPECT_EQ("testNodeInd", std::string(returnValues->at(1)->at(0)));
//  EXPECT_EQ("iro(testSystem,testNodeInd,any,none).\n",
//      std::string(returnValues->at(2)->at(0)));
//
//  std::string toTest = std::string(returnValues->at(3)->at(0));
//  EXPECT_TRUE(
//      toTest.find("#external iro(testSystem,testNodeInd,any,none).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "input(testSystem,testNodeInd,position,coordinatePositionRep,none,1,2).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "input2(testSystem,testNodeInd,position,coordinatePositionRep,none,1,1).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataOutput(delay,testSystem,testNodeInd,max,5,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataProcessing(cost,testSystem,testNodeInd,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataOutput(accuracy,testSystem,testNodeInd,max,-1,-1).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "output(testSystem,testNodeInd,position,coordinatePositionRep,none).")
//          != std::string::npos);
//
//  EXPECT_TRUE(strlen(returnValues->at(4)->at(0)) == 0);
//}
//
//TEST(JNITest, addNodeToSystem)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//  std::vector<std::string> toAdd;
//  std::vector<std::string> inputs;
//  std::vector<int> inputsMin;
//  std::vector<int> inputsMax;
//  std::vector<std::string> outputs;
//  std::vector<int> outputsMin;
//  std::vector<int> outputsMax;
//  std::vector<std::string> metadatas;
//  std::vector<int> metadataValues;
//  std::vector<int> metadataValues2;
//  std::vector<std::string> metadataGroundings;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  oi.addSystem("TestSystem");
//
//  result = oi.addNamedStream("TestStream", "Position", "CoordinatePositionRep");
//
//  inputs.push_back("TestStream");
//  inputsMin.push_back(1);
//  inputsMax.push_back(1);
//  result = oi.addSourceNodeClass("TestSourceNode", outputs, outputsMin,
//      outputsMax);
//
//  result = oi.addIndividual("TestEntity", "Robot");
//
//  metadatas.push_back("Delay");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeDelayFixASPGrounding");
//  metadatas.push_back("Cost");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeCostASPGrounding");
//  metadatas.push_back("Accuracy");
//  metadataValues.push_back(-1);
//  metadataValues2.push_back(-1);
//  metadataGroundings.push_back("NodeAccuracyFixASPGrounding");
//
//  result = oi.addNodeIndividual("TestSourceNodeInd", "TestSourceNode",
//      "TestSystem", "TestEntity", "", metadatas, metadataValues,
//      metadataValues2, metadataGroundings);
//
//  result = oi.addSystem("TestSystem2");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  toAdd.push_back("TestSourceNodeInd");
//
//  result = oi.addNodesToSystem("TestSystem2", toAdd);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  auto returnValues = oi.readNodesAndIROsAsASP("TestSystem2");
//
//  ASSERT_EQ(returnValues->size(), 5);
//
//  ASSERT_EQ(returnValues->at(0)->size(), 1);
//  ASSERT_EQ(returnValues->at(1)->size(), 1);
//  ASSERT_EQ(returnValues->at(2)->size(), 1);
//  ASSERT_EQ(returnValues->at(3)->size(), 1);
//  ASSERT_EQ(returnValues->at(4)->size(), 1);
//
//  EXPECT_EQ("SOURCE_NODE", std::string(returnValues->at(0)->at(0)));
//  EXPECT_EQ("testSourceNodeInd", std::string(returnValues->at(1)->at(0)));
//  EXPECT_EQ("sourceNode(testSystem2,testSourceNodeInd,testEntity).\n",
//      std::string(returnValues->at(2)->at(0)));
//
//  std::string toTest = std::string(returnValues->at(3)->at(0));
//  EXPECT_TRUE(
//      toTest.find(
//          "#external sourceNode(testSystem2,testSourceNodeInd,testEntity).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataProcessing(cost,testSystem2,testSourceNodeInd,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "metadataOutput(delay,testSystem2,testSourceNodeInd,fix,5,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "metadataOutput(accuracy,testSystem2,testSourceNodeInd,fix,-1,-1).")
//          != std::string::npos);
//
//  EXPECT_TRUE(strlen(returnValues->at(4)->at(0)) == 0);
//}
//
//TEST(JNITest, addMapNodeToSystem1)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//  std::vector<std::string> toAdd;
//  std::vector<std::string> inputs;
//  std::vector<int> inputsMin;
//  std::vector<int> inputsMax;
//  std::vector<std::string> inputsRelated;
//  std::vector<int> inputsRelatedMin;
//  std::vector<int> inputsRelatedMax;
//  std::vector<std::string> inputMaps;
//  std::vector<int> inputMapsMin;
//  std::vector<int> inputMapsMax;
//  std::vector<std::string> outputs;
//  std::vector<int> outputsMin;
//  std::vector<int> outputsMax;
//  std::vector<std::string> metadatas;
//  std::vector<int> metadataValues;
//  std::vector<int> metadataValues2;
//  std::vector<std::string> metadataGroundings;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  oi.addSystem("TestSystem");
//
//  result = oi.addNamedMap("TestMap", "Robot", "Position",
//      "CoordinatePositionRep");
//
//  result = oi.addNamedStream("TestStream", "Position", "CoordinatePositionRep");
//
//  inputs.push_back("TestStream");
//  inputsMin.push_back(1);
//  inputsMax.push_back(1);
//
//  outputs.push_back("TestMap");
//  outputsMin.push_back(1);
//  outputsMax.push_back(1);
//
//  result = oi.addMapNodeClass("TestMapNode", inputs, inputsMin, inputsMax,
//      inputsRelated, inputsRelatedMin, inputsRelatedMax, inputMaps,
//      inputMapsMin, inputMapsMax, outputs, outputsMin, outputsMax);
//
//  result = oi.addIndividual("TestEntity1", "Robot");
//  result = oi.addIndividual("TestEntity2", "Robot");
//
//  metadatas.push_back("Delay");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeDelayFixASPGrounding");
//  metadatas.push_back("Cost");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeCostASPGrounding");
//  metadatas.push_back("Accuracy");
//  metadataValues.push_back(-1);
//  metadataValues2.push_back(-1);
//  metadataGroundings.push_back("NodeAccuracyFixASPGrounding");
//
//  result = oi.addNodeIndividual("TestMapNodeInd", "TestMapNode", "TestSystem",
//      "", "", metadatas, metadataValues, metadataValues2, metadataGroundings);
//
//  result = oi.addSystem("TestSystem2");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  toAdd.push_back("TestMapNodeInd");
//
//  result = oi.addNodesToSystem("TestSystem2", toAdd);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  auto returnValues = oi.readNodesAndIROsAsASP("TestSystem2");
//
//  ASSERT_EQ(returnValues->size(), 5);
//
//  ASSERT_EQ(returnValues->at(0)->size(), 1);
//  ASSERT_EQ(returnValues->at(1)->size(), 1);
//  ASSERT_EQ(returnValues->at(2)->size(), 1);
//  ASSERT_EQ(returnValues->at(3)->size(), 1);
//  ASSERT_EQ(returnValues->at(4)->size(), 1);
//
//  EXPECT_EQ("MAP_NODE", std::string(returnValues->at(0)->at(0)));
//  EXPECT_EQ("testMapNodeInd", std::string(returnValues->at(1)->at(0)));
//  EXPECT_EQ("mapNodeTemplate(testSystem2,testMapNodeInd,any).\n",
//      std::string(returnValues->at(2)->at(0)));
//
//  std::string toTest = std::string(returnValues->at(3)->at(0));
//
//  EXPECT_TRUE(
//      toTest.find("#external mapNodeTemplate(testSystem2,testMapNodeInd,any)")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "input(testSystem2,testMapNodeInd,position,coordinatePositionRep,none,1,1).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "outputMap(testSystem2,testMapNodeInd,robot,position,coordinatePositionRep,none).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataProcessing(cost,testSystem2,testMapNodeInd,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataOutput(delay,testSystem2,testMapNodeInd,fix,5,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "metadataOutput(accuracy,testSystem2,testMapNodeInd,fix,-1,-1).")
//          != std::string::npos);
//
//  EXPECT_TRUE(strlen(returnValues->at(4)->at(0)) == 0);
//}
//
//TEST(JNITest, addMapNodeToSystem2)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//  std::vector<std::string> toAdd;
//  std::vector<std::string> inputs;
//  std::vector<int> inputsMin;
//  std::vector<int> inputsMax;
//  std::vector<std::string> inputsRelated;
//  std::vector<int> inputsRelatedMin;
//  std::vector<int> inputsRelatedMax;
//  std::vector<std::string> inputMaps;
//  std::vector<int> inputMapsMin;
//  std::vector<int> inputMapsMax;
//  std::vector<std::string> outputs;
//  std::vector<int> outputsMin;
//  std::vector<int> outputsMax;
//  std::vector<std::string> metadatas;
//  std::vector<int> metadataValues;
//  std::vector<int> metadataValues2;
//  std::vector<std::string> metadataGroundings;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  oi.addSystem("TestSystem");
//
//  result = oi.addNamedMap("TestMap", "Robot", "Position",
//      "CoordinatePositionRep");
//
//  result = oi.addNamedStream("TestStream", "Position", "CoordinatePositionRep");
//
//  inputs.push_back("TestStream");
//  inputsMin.push_back(1);
//  inputsMax.push_back(1);
//
//  outputs.push_back("TestMap");
//  outputsMin.push_back(1);
//  outputsMax.push_back(1);
//
//  inputMaps.push_back("TestMap");
//  inputMapsMin.push_back(1);
//  inputMapsMax.push_back(2);
//
//  result = oi.addMapNodeClass("TestMapNode", inputs, inputsMin, inputsMax,
//      inputsRelated, inputsRelatedMin, inputsRelatedMax, inputMaps,
//      inputMapsMin, inputMapsMax, outputs, outputsMin, outputsMax);
//
//  result = oi.addIndividual("TestEntity1", "Robot");
//  result = oi.addIndividual("TestEntity2", "Robot");
//
//  metadatas.push_back("Delay");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeDelayFixASPGrounding");
//  metadatas.push_back("Cost");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeCostASPGrounding");
//  metadatas.push_back("Accuracy");
//  metadataValues.push_back(-1);
//  metadataValues2.push_back(-1);
//  metadataGroundings.push_back("NodeAccuracyFixASPGrounding");
//
//  result = oi.addNodeIndividual("TestMapNodeInd", "TestMapNode", "TestSystem",
//      "", "", metadatas, metadataValues, metadataValues2, metadataGroundings);
//
//  result = oi.addSystem("TestSystem2");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  toAdd.push_back("TestMapNodeInd");
//
//  result = oi.addNodesToSystem("TestSystem2", toAdd);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  auto returnValues = oi.readNodesAndIROsAsASP("TestSystem2");
//
//  ASSERT_EQ(returnValues->size(), 5);
//
//  ASSERT_EQ(returnValues->at(0)->size(), 1);
//  ASSERT_EQ(returnValues->at(1)->size(), 1);
//  ASSERT_EQ(returnValues->at(2)->size(), 1);
//  ASSERT_EQ(returnValues->at(3)->size(), 1);
//  ASSERT_EQ(returnValues->at(4)->size(), 1);
//
//  EXPECT_EQ("MAP_NODE", std::string(returnValues->at(0)->at(0)));
//  EXPECT_EQ("testMapNodeInd", std::string(returnValues->at(1)->at(0)));
//  EXPECT_EQ("mapNodeTemplate(testSystem2,testMapNodeInd,any).\n",
//      std::string(returnValues->at(2)->at(0)));
//
//  std::string toTest = std::string(returnValues->at(3)->at(0));
//
//  EXPECT_TRUE(
//      toTest.find("#external mapNodeTemplate(testSystem2,testMapNodeInd,any)")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "input(testSystem2,testMapNodeInd,position,coordinatePositionRep,none,1,1).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "inputMap(testSystem2,testMapNodeInd,robot,position,coordinatePositionRep,none,1,2).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "outputMap(testSystem2,testMapNodeInd,robot,position,coordinatePositionRep,none).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataProcessing(cost,testSystem2,testMapNodeInd,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataOutput(delay,testSystem2,testMapNodeInd,fix,5,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "metadataOutput(accuracy,testSystem2,testMapNodeInd,fix,-1,-1).")
//          != std::string::npos);
//
//  EXPECT_TRUE(strlen(returnValues->at(4)->at(0)) == 0);
//}
//
//TEST(JNITest, addRequiredMap)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//  std::vector<std::string> toAdd;
//  std::vector<std::string> inputs;
//  std::vector<int> inputsMin;
//  std::vector<int> inputsMax;
//  std::vector<std::string> inputsRelated;
//  std::vector<int> inputsRelatedMin;
//  std::vector<int> inputsRelatedMax;
//  std::vector<std::string> inputMaps;
//  std::vector<int> inputMapsMin;
//  std::vector<int> inputMapsMax;
//  std::vector<std::string> outputs;
//  std::vector<int> outputsMin;
//  std::vector<int> outputsMax;
//  std::vector<std::string> metadatas;
//  std::vector<int> metadataValues;
//  std::vector<int> metadataValues2;
//  std::vector<std::string> metadataGroundings;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  oi.addSystem("TestSystem");
//
//  result = oi.addNamedMap("TestMap", "Robot", "Position",
//      "CoordinatePositionRep");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.addRequiredMap("ReqMap", "TestMap", "TestSystem", "");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  auto returnValues = oi.readNodesAndIROsAsASP("TestSystem");
//
//  ASSERT_EQ(returnValues->size(), 5);
//
//  ASSERT_EQ(returnValues->at(0)->size(), 1);
//  ASSERT_EQ(returnValues->at(1)->size(), 1);
//  ASSERT_EQ(returnValues->at(2)->size(), 1);
//  ASSERT_EQ(returnValues->at(3)->size(), 1);
//  ASSERT_EQ(returnValues->at(4)->size(), 1);
//
//  EXPECT_EQ("REQUIRED_MAP", std::string(returnValues->at(0)->at(0)));
//  EXPECT_EQ("reqMap", std::string(returnValues->at(1)->at(0)));
//  EXPECT_EQ(
//      "requiredMap(testSystem,informationType(robot,position,coordinatePositionRep,none)).\n",
//      std::string(returnValues->at(2)->at(0)));
//
//  std::string toTest = std::string(returnValues->at(3)->at(0));
//
//  EXPECT_TRUE(
//      toTest.find(
//          "#external requiredMap(testSystem,informationType(robot,position,coordinatePositionRep,none))")
//          != std::string::npos);
//
//  EXPECT_TRUE(strlen(returnValues->at(4)->at(0)) == 0);
//}
//
//TEST(JNITest, save)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//  std::vector<std::string> inputs;
//  std::vector<int> inputsMin;
//  std::vector<int> inputsMax;
//  std::vector<std::string> outputs;
//  std::vector<int> outputsMin;
//  std::vector<int> outputsMax;
//  std::vector<std::string> metadatas;
//  std::vector<int> metadataValues;
//  std::vector<int> metadataValues2;
//  std::vector<std::string> metadataGroundings;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  oi.addSystem("TestSystem");
//
//  result = oi.addNamedStream("TestStream", "Position", "CoordinatePositionRep");
//
//  inputs.push_back("TestStream");
//  inputsMin.push_back(1);
//  inputsMax.push_back(2);
//  outputs.push_back("TestStream");
//  outputsMin.push_back(1);
//  outputsMax.push_back(1);
//
//  result = oi.addComputationNodeClass("TestNode", inputs, inputsMin, inputsMax,
//      outputs, outputsMin, outputsMax);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  metadatas.push_back("Delay");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeDelayASPGrounding");
//  metadatas.push_back("Cost");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeCostASPGrounding");
//  metadatas.push_back("Accuracy");
//  metadataValues.push_back(-1);
//  metadataValues2.push_back(-1);
//  metadataGroundings.push_back("NodeAccuracyMaxASPGrounding");
//
//  result = oi.addNodeIndividual("TestNodeInd", "TestNode", "TestSystem", "", "",
//      metadatas, metadataValues, metadataValues2, metadataGroundings);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.saveOntology("/tmp/testSaveOnt.owl");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  ice::OntologyInterface oi2("/java/lib/");
//
//  oi2.addIRIMapper(path + "/ontology/");
//  oi2.loadOntology("/tmp/testSaveOnt.owl");
//
//  //oi2.getSystems();
//  ASSERT_FALSE(oi.errorOccurred());
//
//  oi2.readInformationStructureAsASP();
//  ASSERT_FALSE(oi.errorOccurred());
//
//  auto returnValues = oi2.readNodesAndIROsAsASP("TestSystem");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  ASSERT_EQ(returnValues->size(), 5);
//
//  ASSERT_EQ(1, returnValues->at(0)->size());
//  ASSERT_EQ(1, returnValues->at(1)->size());
//  ASSERT_EQ(1, returnValues->at(2)->size());
//  ASSERT_EQ(1, returnValues->at(3)->size());
//  ASSERT_EQ(1, returnValues->at(4)->size());
//
//  EXPECT_EQ("COMPUTATION_NODE", std::string(returnValues->at(0)->at(0)));
//  EXPECT_EQ("testNodeInd", std::string(returnValues->at(1)->at(0)));
//  EXPECT_EQ("nodeTemplate(testSystem,testNodeInd,any).\n",
//      std::string(returnValues->at(2)->at(0)));
//
//  std::string toTest = std::string(returnValues->at(3)->at(0));
//  EXPECT_TRUE(
//      toTest.find("#external nodeTemplate(testSystem,testNodeInd,any).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "output(testSystem,testNodeInd,position,coordinatePositionRep,none).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "input(testSystem,testNodeInd,position,coordinatePositionRep,none,1,2).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataOutput(delay,testSystem,testNodeInd,max,5,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataProcessing(cost,testSystem,testNodeInd,5).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find("metadataOutput(accuracy,testSystem,testNodeInd,max,-1,-1).")
//          != std::string::npos);
//  EXPECT_TRUE(
//      toTest.find(
//          "output(testSystem,testNodeInd,position,coordinatePositionRep,none).")
//          != std::string::npos);
//
//  EXPECT_TRUE(strlen(returnValues->at(4)->at(0)) == 0);
//}

//TEST(JNITest, addNode)
//{
//  std::string path = ros::package::getPath("ice");
//  bool result;
//  std::vector<std::string> metadatas;
//  std::vector<int> metadataValues;
//  std::vector<int> metadataValues2;
//  std::vector<std::string> metadataGroundings;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  oi.addIRIMapper(path + "/ontology/");
//  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  oi.loadOntologies();
//
//  result = oi.addSystem("TestSystem");
//
//  metadatas.push_back("Delay");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeDelayASPGrounding");
//  metadatas.push_back("Cost");
//  metadataValues.push_back(5);
//  metadataValues2.push_back(5);
//  metadataGroundings.push_back("NodeCostASPGrounding");
//  metadatas.push_back("Accuracy");
//  metadataValues.push_back(-1);
//  metadataValues2.push_back(-1);
//  metadataGroundings.push_back("NodeAccuracyMaxASPGrounding");
//
//  result = oi.addNodeIndividual("TestNode", "ComputationNode", "TestSystem", metadatas, metadataValues, metadataValues2,
//                                metadataGroundings);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  std::unique_ptr<std::vector<std::vector<std::string>>>vec= oi.readNodesAndIROsAsASP("TestSystem");
//
//  ASSERT_FALSE(oi.errorOccurred());
//
//      for (std::vector<std::string> v : *vec)
//      {
//        for (std::string s : v)
//        {
//          std::cout << "#" << s << std::endl;
//        }
//      }
//}

//  TEST(JNITest, old)
//  {
//  std::string path = ros::package::getPath("ice");
//  bool result;
//
//  ice::OntologyInterface oi(path + "/java/lib/");
//
//  ASSERT_FALSE(oi.errorOccurred());
//
//  oi.addIRIMapper(path + "/ontology/");
//
//  ASSERT_FALSE(oi.errorOccurred());
//
////  result =  oi.addOntologyIRI("http://vs.uni-kassel.de/Ice");
//  result =  oi.addOntologyIRI("http://vs.uni-kassel.de/IceTest");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.loadOntologies();
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.isConsistent();
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  result = oi.addSystem("TestSystem");
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_FALSE(result);
//
//  std::unique_ptr<std::vector<std::string>> systems = oi.getSystems();
//
//  ASSERT_FALSE(oi.errorOccurred());
//
////  for (std::string sys : *systems)
////    std::cout << sys << std::endl;
//
//  std::string infoStructure = oi.readInformationStructureAsASP();
//
//  ASSERT_FALSE(oi.errorOccurred());
////  std::cout << infoStructure << std::endl;
//
//  std::vector<std::string> metadatas;
//  std::vector<int> metadataValues;
//  std::vector<std::string> metadataGroundings;
//
//  metadatas.push_back("Delay");
//  metadataValues.push_back(5);
//  metadataGroundings.push_back("NodeDelayASPGrounding");
//  metadatas.push_back("Cost");
//  metadataValues.push_back(5);
//  metadataGroundings.push_back("NodeCostASPGrounding");
//  metadatas.push_back("Accuracy");
//  metadataValues.push_back(-1);
//  metadataGroundings.push_back("NodeAccuracyMaxASPGrounding");
//
//  result = oi.addNodeIndividual("TestNode", "KalmanFilter", "TestSystem", metadatas, metadataValues, metadataGroundings);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//
//  metadatas.clear();
//  metadataValues.clear();
//  metadataGroundings.clear();
//
//  metadatas.push_back("Delay");
//  metadataValues.push_back(3);
//  metadataGroundings.push_back("IRODelayASPGrounding");
//  metadatas.push_back("Cost");
//  metadataValues.push_back(4);
//  metadataGroundings.push_back("IROCostASPGrounding");
//  metadatas.push_back("Accuracy");
//  metadataValues.push_back(-5);
//  metadataGroundings.push_back("IROAccuracyMaxASPGrounding");
//
//  result = oi.addIROIndividual("TestIRO", "Ego2AlloIRO", "TestSystem", metadatas, metadataValues, metadataGroundings);
//
//  ASSERT_FALSE(oi.errorOccurred());
//  ASSERT_TRUE(result);
//  for (std::string sys : *systems)
//  {
//    std::unique_ptr<std::vector<std::vector<std::string>>>vec= oi.readNodesAndIROsAsASP(sys);
//
//    ASSERT_FALSE(oi.errorOccurred());
//
//    for (std::vector<std::string> v : *vec)
//    {
//      for (std::string s : v)
//      {
//        std::cout << "#" << s << std::endl;
//      }
//    }
//  }
//}

TEST(JNITest, readBaseRepresentationsAsCSV)
{
  // Given a valid empty ice ontology
  std::string path = ros::package::getPath("ice");
  bool result;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");

  ASSERT_FALSE(oi.errorOccurred());

  result = oi.addOntologyIRI(
      "http://vs.uni-kassel.de/Ice");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.loadOntologies();

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.isConsistent();

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  // The csv string with the right reps is returned
  std::string read_csv_res = oi.readRepresentationsAsCSV();

  // Sample testing of integer representation
  int index =
      read_csv_res.find(
          "http://vs.uni-kassel.de/Ice#DefaultMovementRep;http://vs.uni-kassel.de/Ice#Orientation;http://vs.uni-kassel.de/Ice#EulerAnglesRep");
  ASSERT_FALSE(index == std::string::npos);
}


TEST(JNITest, iriMapping)
{
  // Given a valid empty ice ontology
  std::string path = ros::package::getPath("ice");
  bool result;

  ice::OntologyInterface oi(path + "/java/lib/");

  oi.addIRIMapper(path + "/ontology/");

  ASSERT_FALSE(oi.errorOccurred());

  result = oi.addOntologyIRI(
      "http://vs.uni-kassel.de/Ice");

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.loadOntologies();

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  result = oi.isConsistent();

  ASSERT_FALSE(oi.errorOccurred());
  ASSERT_TRUE(result);

  // The csv string with the right reps is returned
  const std::string longIri = "http://vs.uni-kassel.de/Ice#TestEntity";
  const std::string shortIri = "o0_TestEntity";

  std::string convertedLongIri = oi.toShortIri(longIri);
  std::string convertedShortIri = oi.toLongIri(shortIri);

  EXPECT_EQ(longIri, convertedShortIri);
  EXPECT_EQ(shortIri, convertedLongIri);
}

}
