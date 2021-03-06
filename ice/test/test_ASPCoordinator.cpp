#include <gtest/gtest.h>

#include "ice/ICEngine.h"
#include "ice/information/InformationSpecification.h"
#include "ice/information/KnowledgeBase.h"
#include "ice/information/StreamStore.h"
#include "ice/information/SetStore.h"
#include "ice/model/aspModel/ASPModelGenerator.h"
#include "ice/model/updateStrategie/UpdateStrategie.h"
#include "ice/representation/GContainer.h"

#include "etc/EngineStuff.cpp"

TEST(ASPCoordinator, simpleTest)
{
  ice::Node::registerNodeCreator("TestSourceNodeGrounding", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("TestComputationalNodeGrounding", &SmothingNode::createNode);

  auto timeFactory = std::make_shared<ice::SimpleTimeFactory>();
  std::shared_ptr<ice::Configuration> config = std::make_shared<ice::Configuration>();
  config->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestSystem";
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(config);
  auto streamFactory = std::make_shared<TestFactory>(engine);
  engine->setTimeFactory(timeFactory);
  engine->setCollectionFactory(streamFactory);

  engine->init();
  engine->start();

  auto spec1 = ice::InformationSpecification("http://vs.uni-kassel.de/IceTest#TestEntity1",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation1");
  auto spec2 = ice::InformationSpecification("http://vs.uni-kassel.de/IceTest#TestEntity1",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation2");

  auto stream1 = engine->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec1, "http://vs.uni-kassel.de/IceTest#TestSourceNodeInd",
                                                                         "http://vs.uni-kassel.de/IceTest#TestSystem");
  auto stream2 = engine->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec2, "http://vs.uni-kassel.de/IceTest#TestComputationalNodeInd",
                                                                         "http://vs.uni-kassel.de/IceTest#TestSystem");

  auto selected = engine->getKnowlegeBase()->streamStore->getSelectedStream<ice::Pos3D>(&spec2);

  ASSERT_TRUE((stream1 ? true : false));
  ASSERT_TRUE((stream2 ? true : false));
  ASSERT_TRUE((selected ? true : false));

  auto position1 = std::make_shared<ice::Pos3D>(nullptr);
  position1->x = 3;
  position1->y = 2;
  position1->z = 1;

  stream1->add(position1);

  std::this_thread::sleep_for(std::chrono::milliseconds {10});

  auto position2 = stream2->getLast();

  ASSERT_TRUE((position2 ? true : false));
  EXPECT_EQ(2, position2->getInformation()->x);
  EXPECT_EQ(1, position2->getInformation()->y);
  EXPECT_EQ(0, position2->getInformation()->z);

  auto position3 = selected->getLast();

  ASSERT_TRUE((position3 ? true : false));
  EXPECT_EQ(2, position3->getInformation()->x);
  EXPECT_EQ(1, position3->getInformation()->y);
  EXPECT_EQ(0, position3->getInformation()->z);

  engine->cleanUp();
  engine.reset();
}

TEST(ASPCoordinator, transformationTest)
{
  ice::Node::clearNodeStore();
  ice::Node::registerNodeCreator("TestSourceNodeGrounding", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("TestComputationalNodeGrounding", &SmothingNode::createNode);
  ice::Node::registerNodeCreator("Pos3DSourceNode", &SimpleSourceNode::createNode);

  auto timeFactory = std::make_shared<ice::SimpleTimeFactory>();
  std::shared_ptr<ice::Configuration> config = std::make_shared<ice::Configuration>();
  config->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestTransformSystem";
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(config);
  auto streamFactory = std::make_shared<TestFactory>(engine);
  engine->setTimeFactory(timeFactory);
  engine->setCollectionFactory(streamFactory);

  engine->init();
  engine->start();

  auto specPos3D = ice::InformationSpecification("http://vs.uni-kassel.de/IceTest#TestEntity1",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/Ice#Position",
                                             "http://vs.uni-kassel.de/IceTest#Pos3D");
  auto specPos2D = ice::InformationSpecification("http://vs.uni-kassel.de/IceTest#TestEntity1",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/Ice#Position",
                                             "http://vs.uni-kassel.de/IceTest#Pos2D");

  auto stream1 = engine->getKnowlegeBase()->streamStore->getStream<ice::GContainer>(&specPos3D, "http://vs.uni-kassel.de/IceTest#Pos3DSourceNodeInd",
                                                                         "http://vs.uni-kassel.de/IceTest#TestTransformSystem");
  auto stream2 = engine->getKnowlegeBase()->streamStore->getStream<ice::GContainer>(&specPos2D, "transformation7",
                                                                         "http://vs.uni-kassel.de/IceTest#TestTransformSystem");

  ASSERT_TRUE((stream1 ? true : false));
  ASSERT_TRUE((stream2 ? true : false));


  auto p2dRep = engine->getGContainerFactory()->getRepresentation("http://vs.uni-kassel.de/IceTest#Pos2D");
  auto p3dRep = engine->getGContainerFactory()->getRepresentation("http://vs.uni-kassel.de/IceTest#Pos3D");

  ASSERT_TRUE(p2dRep != false);
  ASSERT_TRUE(p3dRep != false);

  auto p2dX = p2dRep->accessPath( {"http://vs.uni-kassel.de/Ice#XCoordinate"});
  auto p2dY = p2dRep->accessPath( {"http://vs.uni-kassel.de/Ice#YCoordinate"});

  auto p3dX = p3dRep->accessPath( {"http://vs.uni-kassel.de/Ice#XCoordinate"});
  auto p3dY = p3dRep->accessPath( {"http://vs.uni-kassel.de/Ice#YCoordinate"});
  auto p3dZ = p3dRep->accessPath( {"http://vs.uni-kassel.de/Ice#ZCoordinate"});

  auto element = engine->getGContainerFactory()->makeInstance("http://vs.uni-kassel.de/IceTest#Pos3D");

  double x = 5;
  double y = 6;
  double z = 7;

  element->set(p3dX, &x);
  element->set(p3dY, &y);
  element->set(p3dZ, &z);

  stream1->add(element);

  sleep(2);

  auto out = stream2->getLast();

  ASSERT_NE(nullptr, out);
  ASSERT_EQ(x, out->getInformation()->getValue<double>(p2dX));
  ASSERT_EQ(y, out->getInformation()->getValue<double>(p2dY));

  engine->cleanUp();
  engine.reset();
}

TEST(ASPCoordinator, twoSystemsSimple)
{
  ice::Node::clearNodeStore();
  ice::Node::registerNodeCreator("TestSourceNodeGrounding", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("TestComputationalNodeGrounding", &SmothingNode::createNode);

  // create engine 1
  auto timeFactory = std::make_shared<ice::SimpleTimeFactory>();
  std::shared_ptr<ice::Configuration> config = std::make_shared<ice::Configuration>();
  config->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestCoordination1_SystemInd1";
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(config);
  auto streamFactory = std::make_shared<TestFactory>(engine);
  engine->setTimeFactory(timeFactory);
  engine->setCollectionFactory(streamFactory);

  engine->init();
  engine->start();

  // create engine 2
  timeFactory = std::make_shared<ice::SimpleTimeFactory>();

  std::shared_ptr<ice::Configuration> config2 = std::make_shared<ice::Configuration>();
  config2->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config2->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestCoordination1_SystemInd2";
  std::shared_ptr<ice::ICEngine> engine2 = std::make_shared<ice::ICEngine>(config2);
  auto streamFactory2 = std::make_shared<TestFactory>(engine2);
  engine2->setTimeFactory(timeFactory);
  engine2->setCollectionFactory(streamFactory2);

  engine2->init();

  // adding custom creator for Pos3D
  auto creator = [](std::shared_ptr<ice::GContainerFactory> factory){
      auto rep = factory->getRepresentation("http://vs.uni-kassel.de/IceTest#TestRepresentation1");
      return std::make_shared<ice::Pos3D>(rep);
    };
  engine2->getGContainerFactory()->registerCustomCreator("http://vs.uni-kassel.de/Ice#CoordinatePositionRep", creator);

  engine2->start();

  // wait some time to enable the engines to find each other
  std::this_thread::sleep_for(std::chrono::milliseconds {2000});

  // test processing system 1
  auto spec1 = ice::InformationSpecification("http://vs.uni-kassel.de/IceTest#TestEntity1",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation1");
  auto stream1 = engine->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec1,
                                                                    "http://vs.uni-kassel.de/IceTest#TestSourceNodeInd",
                                                                    "http://vs.uni-kassel.de/IceTest#TestCoordination1_SystemInd1");
  ASSERT_TRUE((stream1 ? true : false));

  // test processing system 2
  auto spec2 = ice::InformationSpecification("http://vs.uni-kassel.de/IceTest#TestEntity1",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation1");
  auto stream2 = engine2->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec2,
                                                                     "http://vs.uni-kassel.de/IceTest#TestSourceNodeInd",
                                                                     "http://vs.uni-kassel.de/IceTest#TestCoordination1_SystemInd1");
  ASSERT_TRUE((stream2 ? true : false));

  // insert element in stream of system 2
  int x = rand();
  int y = rand();
  int z = rand();

  auto position1 = std::make_shared<ice::Pos3D>(nullptr);
  position1->x = x;
  position1->y = y;
  position1->z = z;

  stream1->add(position1);

  // wait some time
  std::this_thread::sleep_for(std::chrono::milliseconds {500});

  // check if element was send to system 2 and placed in stream2
  auto position2 = stream2->getLast();

  ASSERT_NE(nullptr, position2);
  EXPECT_EQ(x, position2->getInformation()->x);
  EXPECT_EQ(y, position2->getInformation()->y);
  EXPECT_EQ(z, position2->getInformation()->z);

  engine->cleanUp();
  engine.reset();
  engine2->cleanUp();
  engine2.reset();
}

TEST(ASPCoordinator, twoSystemsComplex)
{
  ice::Node::clearNodeStore();
  ice::Node::registerNodeCreator("TestSourceNodeGrounding", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("TestComputationalNodeGrounding", &SmothingNode::createNode);

  // create engine 1
  auto timeFactory = std::make_shared<ice::SimpleTimeFactory>();

  std::shared_ptr<ice::Configuration> config = std::make_shared<ice::Configuration>();
  config->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestCoordination2_SystemInd1";
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(config);
  engine->setTimeFactory(timeFactory);
  auto streamFactory = std::make_shared<TestFactory>(engine);
  engine->setCollectionFactory(streamFactory);

  engine->init();
  engine->start();

  // create engine 2
  timeFactory = std::make_shared<ice::SimpleTimeFactory>();

  std::shared_ptr<ice::Configuration> config2 = std::make_shared<ice::Configuration>();
  config2->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config2->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestCoordination2_SystemInd2";
  std::shared_ptr<ice::ICEngine> engine2 = std::make_shared<ice::ICEngine>(config2);
  engine2->setTimeFactory(timeFactory);
  auto streamFactory2 = std::make_shared<TestFactory>(engine2);
  engine2->setCollectionFactory(streamFactory2);

  engine2->init();

  // adding custom creator for Pos3D
  auto creator = [](std::shared_ptr<ice::GContainerFactory> factory){
      auto rep = factory->getRepresentation("http://vs.uni-kassel.de/IceTest#TestRepresentation1");
      return std::make_shared<ice::Pos3D>(rep);
    };
  engine2->getGContainerFactory()->registerCustomCreator("http://vs.uni-kassel.de/Ice#CoordinatePositionRep", creator);

  engine2->start();

  // wait some time to enable the engines to find each other
  std::this_thread::sleep_for(std::chrono::milliseconds {2000});

  auto spec1 = ice::InformationSpecification("http://vs.uni-kassel.de/IceTest#TestEntity1",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation1");
  auto spec2 = ice::InformationSpecification("http://vs.uni-kassel.de/IceTest#TestEntity1",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation2");

  // test processing system 1
  auto stream11 = engine->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec1,
                                                                     "http://vs.uni-kassel.de/IceTest#TestSourceNodeInd",
                                                                     "http://vs.uni-kassel.de/IceTest#TestCoordination2_SystemInd1");
  ASSERT_TRUE((stream11 ? true : false));

  auto stream12 = engine2->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec2,
                                                                      "http://vs.uni-kassel.de/IceTest#TestComputationalNodeInd",
                                                                      "http://vs.uni-kassel.de/IceTest#TestCoordination2_SystemInd2");
  ASSERT_TRUE((stream12 ? true : false));

  // test processing system 2
  auto stream21 = engine2->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec1,
                                                                      "http://vs.uni-kassel.de/IceTest#TestSourceNodeInd",
                                                                      "http://vs.uni-kassel.de/IceTest#TestCoordination2_SystemInd1");
  ASSERT_TRUE((stream21 ? true : false));

  auto stream22 = engine2->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec2,
                                                                      "http://vs.uni-kassel.de/IceTest#TestComputationalNodeInd",
                                                                      "http://vs.uni-kassel.de/IceTest#TestCoordination2_SystemInd2");
  ASSERT_TRUE((stream22 ? true : false));

  // insert element in stream of system 2
  int x = rand();
  int y = rand();
  int z = rand();

  auto position1 = std::make_shared<ice::Pos3D>(nullptr);
  position1->x = x;
  position1->y = y;
  position1->z = z;

  stream11->add(position1);

  // wait some time
  std::this_thread::sleep_for(std::chrono::milliseconds {500});

  // check if element was send to system 2 and placed in stream2
  auto position2 = stream12->getLast();

  ASSERT_TRUE((position2 ? true : false));
  EXPECT_EQ(x - 1, position2->getInformation()->x);
  EXPECT_EQ(y - 1, position2->getInformation()->y);
  EXPECT_EQ(z - 1, position2->getInformation()->z);

  engine->cleanUp();
  engine.reset();
  engine2->cleanUp();
  engine2.reset();
}

TEST(ASPCoordinator, simpleSetTest)
{
  ice::Node::clearNodeStore();
  ice::Node::registerNodeCreator("TestSourceNodeGrounding", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("TestComputationalNodeGrounding", &SmothingNode::createNode);
  ice::Node::registerNodeCreator("SetSourceNode", &SetSourceNode::createNode);

  auto timeFactory = std::make_shared<ice::SimpleTimeFactory>();
  std::shared_ptr<ice::Configuration> config = std::make_shared<ice::Configuration>();
  config->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestSet_SimpleInd";
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(config);
  auto streamFactory = std::make_shared<TestFactory>(engine);
  engine->setTimeFactory(timeFactory);
  engine->setCollectionFactory(streamFactory);

  engine->getConfig()->synthesizeTransformations = false;
  engine->init();
  engine->start();

  auto spec1 = ice::InformationSpecification("",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation1");

  auto set = engine->getKnowlegeBase()->setStore->getSet<ice::Pos3D>(&spec1);

  ASSERT_TRUE((set ? true : false));

  auto node = engine->getNodeStore()->getNode("http://vs.uni-kassel.de/IceTest#TestSetSourceInd", "http://vs.uni-kassel.de/IceTest#TestEntity");
  ASSERT_TRUE((node ? true : false));

  node->performTask();

  std::this_thread::sleep_for(std::chrono::milliseconds {10});

  auto position2 = set->get("muh");

  ASSERT_TRUE((position2 ? true : false));
  EXPECT_EQ(1, position2->getInformation()->x);
  EXPECT_EQ(21, position2->getInformation()->y);
  EXPECT_EQ(31, position2->getInformation()->z);

  engine->cleanUp();
  engine.reset();
}

TEST(ASPCoordinator, streamsToSetTest)
{
  ice::Node::clearNodeStore();
  ice::Node::registerNodeCreator("TestSourceNodeGrounding", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("TestComputationalNodeGrounding", &SmothingNode::createNode);
  ice::Node::registerNodeCreator("SetSourceNode", &SetSourceNode::createNode);
  ice::Node::registerNodeCreator("TestSetNode", &TestSetNode::createNode);

  // create engine 1
  auto timeFactory = std::make_shared<ice::SimpleTimeFactory>();

  std::shared_ptr<ice::Configuration> config = std::make_shared<ice::Configuration>();
  config->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestSet1_SystemInd1";
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(config);
  engine->setTimeFactory(timeFactory);
  auto streamFactory = std::make_shared<TestFactory>(engine);
  engine->setCollectionFactory(streamFactory);

  engine->getConfig()->synthesizeTransformations = false;
  engine->init();

  // adding custom creator for Pos3D
  auto creator = [](std::shared_ptr<ice::GContainerFactory> factory){
      auto rep = factory->getRepresentation("http://vs.uni-kassel.de/IceTest#TestRepresentation1");
      return std::make_shared<ice::Pos3D>(rep);
    };
  engine->getGContainerFactory()->registerCustomCreator("http://vs.uni-kassel.de/Ice#CoordinatePositionRep", creator);

  engine->start();

  // create engine 2
  std::shared_ptr<ice::Configuration> config2 = std::make_shared<ice::Configuration>();
  config2->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config2->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestSet1_SystemInd2";
  std::shared_ptr<ice::ICEngine> engine2 = std::make_shared<ice::ICEngine>(config2);
  engine2->setTimeFactory(timeFactory);
  auto streamFactory2 = std::make_shared<TestFactory>(engine2);
  engine2->setCollectionFactory(streamFactory2);

  engine2->getConfig()->synthesizeTransformations = false;
  engine2->init();

  // adding custom creator for Pos3D
  auto creator2 = [](std::shared_ptr<ice::GContainerFactory> factory){
      auto rep = factory->getRepresentation("http://vs.uni-kassel.de/IceTest#TestRepresentation1");
      return std::make_shared<ice::Pos3D>(rep);
    };
  engine2->getGContainerFactory()->registerCustomCreator("http://vs.uni-kassel.de/Ice#CoordinatePositionRep", creator2);

  engine2->start();

  // wait some time to enable the engines to find each other
  std::this_thread::sleep_for(std::chrono::milliseconds {2000});

  auto spec11 = ice::InformationSpecification("http://vs.uni-kassel.de/IceTest#TestEntity2",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation1");
  auto spec12 = ice::InformationSpecification("http://vs.uni-kassel.de/IceTest#TestEntity1",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation1");
  auto spec2 = ice::InformationSpecification("",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation2");

  // test processing system 1
  auto stream1 = engine->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec11,
                                                                     "http://vs.uni-kassel.de/IceTest#TestSourceNode2Ind",
                                                                     "http://vs.uni-kassel.de/IceTest#TestSet1_SystemInd1");
  ASSERT_TRUE((stream1 ? true : false));

  auto set = engine->getKnowlegeBase()->setStore->getSet<ice::Pos3D>(&spec2,
                                                                      "http://vs.uni-kassel.de/IceTest#TestSetNodeInd",
                                                                      "http://vs.uni-kassel.de/IceTest#TestSet1_SystemInd1");
  auto selected = engine->getKnowlegeBase()->setStore->getSelectedSet<ice::Pos3D>(&spec2);
  ASSERT_TRUE((selected ? true : false));

  // test processing system 2
  auto stream2 = engine2->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec12,
                                                                      "http://vs.uni-kassel.de/IceTest#TestSourceNodeInd",
                                                                      "http://vs.uni-kassel.de/IceTest#TestSet1_SystemInd2");
  ASSERT_TRUE((stream2 ? true : false));


  // insert element in stream of system 2
  int x = rand();
  int y = rand();
  int z = rand();

  auto position1 = std::make_shared<ice::Pos3D>(nullptr);
  position1->x = x;
  position1->y = y;
  position1->z = z;

  x = rand();
  y = rand();
  z = rand();
  auto position2 = std::make_shared<ice::Pos3D>(nullptr);
  position2->x = x;
  position2->y = y;
  position2->z = z;

  stream1->add(position1);
  stream2->add(position2);

  // wait some time
  std::this_thread::sleep_for(std::chrono::milliseconds {500});

  // check elements of streams in set
  ASSERT_EQ(2, set->getSize());

  auto pos = set->get("http://vs.uni-kassel.de/IceTest#TestEntity2");

  ASSERT_TRUE((pos ? true : false));
  EXPECT_EQ(position1->x - 1, pos->getInformation()->x);
  EXPECT_EQ(position1->y - 1, pos->getInformation()->y);
  EXPECT_EQ(position1->z - 1, pos->getInformation()->z);

  pos = set->get("http://vs.uni-kassel.de/IceTest#TestEntity1");

  ASSERT_TRUE((pos ? true : false));
  EXPECT_EQ(position2->x - 1, pos->getInformation()->x);
  EXPECT_EQ(position2->y - 1, pos->getInformation()->y);
  EXPECT_EQ(position2->z - 1, pos->getInformation()->z);

  pos = selected->get("http://vs.uni-kassel.de/IceTest#TestEntity1");

  ASSERT_TRUE((pos ? true : false));
  EXPECT_EQ(position2->x - 1, pos->getInformation()->x);
  EXPECT_EQ(position2->y - 1, pos->getInformation()->y);
  EXPECT_EQ(position2->z - 1, pos->getInformation()->z);

  engine->cleanUp();
  engine.reset();
  engine2->cleanUp();
  engine2.reset();
}

TEST(ASPCoordinator, setTransformTest)
{
  ice::Node::clearNodeStore();
  ice::Node::registerNodeCreator("SetSourceNode", &SetSourceNode::createNode);
  ice::Node::registerNodeCreator("TestSetTransformNode", &TestSetTransformNode::createNode);

  auto timeFactory = std::make_shared<ice::SimpleTimeFactory>();
  std::shared_ptr<ice::Configuration> config = std::make_shared<ice::Configuration>();
  config->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestSet_TransformInd";
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(config);
  auto streamFactory = std::make_shared<TestFactory>(engine);
  engine->setTimeFactory(timeFactory);
  engine->setCollectionFactory(streamFactory);

  engine->getConfig()->synthesizeTransformations = false;
  engine->init();
  engine->start();

  auto spec1 = ice::InformationSpecification("",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation1");
  auto spec2 = ice::InformationSpecification("",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation2");

  auto set1 = engine->getKnowlegeBase()->setStore->getSet<ice::Pos3D>(&spec1);
  ASSERT_TRUE((set1 ? true : false));
  auto set2 = engine->getKnowlegeBase()->setStore->getSet<ice::Pos3D>(&spec2);
  ASSERT_TRUE((set2 ? true : false));

  auto nodeSource = engine->getNodeStore()->getNode("http://vs.uni-kassel.de/IceTest#TestSetSourceInd", "http://vs.uni-kassel.de/IceTest#TestEntity");
  ASSERT_TRUE((nodeSource ? true : false));

  auto nodeTransform = engine->getNodeStore()->getNode("http://vs.uni-kassel.de/IceTest#TestSetTransformNodeInd", "http://vs.uni-kassel.de/IceTest#TestEntity");
  ASSERT_TRUE((nodeTransform ? true : false));

  nodeSource->performTask();

  std::this_thread::sleep_for(std::chrono::milliseconds {10});

  ASSERT_EQ(1, set1->getSize());
  ASSERT_EQ(1, set2->getSize());

  auto position2 = set2->get("muh");

  ASSERT_TRUE((position2 ? true : false));
  EXPECT_EQ(101, position2->getInformation()->x);
  EXPECT_EQ(121, position2->getInformation()->y);
  EXPECT_EQ(131, position2->getInformation()->z);

  engine->cleanUp();
  engine.reset();
}

TEST(ASPCoordinator, setTransferTest)
{
  ice::Node::clearNodeStore();
  ice::Node::registerNodeCreator("SetSourceNode", &SetSourceNode::createNode);
  ice::Node::registerNodeCreator("TestSetTransformNode", &TestSetTransformNode::createNode);

  // create engine 1
  auto timeFactory = std::make_shared<ice::SimpleTimeFactory>();

  std::shared_ptr<ice::Configuration> config = std::make_shared<ice::Configuration>();
  config->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestSet_TransferInd1";
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(config);
  engine->setTimeFactory(timeFactory);
  auto streamFactory = std::make_shared<TestFactory>(engine);
  engine->setCollectionFactory(streamFactory);

  engine->getConfig()->synthesizeTransformations = false;
  engine->init();
  engine->start();

  // create engine 2
  std::shared_ptr<ice::Configuration> config2 = std::make_shared<ice::Configuration>();
  config2->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config2->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestSet_TransferInd2";
  std::shared_ptr<ice::ICEngine> engine2 = std::make_shared<ice::ICEngine>(config2);
  engine2->setTimeFactory(timeFactory);
  auto streamFactory2 = std::make_shared<TestFactory>(engine2);
  engine2->setCollectionFactory(streamFactory2);

  engine2->getConfig()->synthesizeTransformations = false;
  engine2->init();

  // adding custom creator for Pos3D
  auto creator = [](std::shared_ptr<ice::GContainerFactory> factory){
      auto rep = factory->getRepresentation("http://vs.uni-kassel.de/IceTest#TestRepresentation1");
      return std::make_shared<ice::Pos3D>(rep);
    };
  engine2->getGContainerFactory()->registerCustomCreator("http://vs.uni-kassel.de/Ice#CoordinatePositionRep", creator);

  engine2->start();

  // wait some time to enable the engines to find each other
  std::this_thread::sleep_for(std::chrono::milliseconds {2000});

  auto spec1 = ice::InformationSpecification("",
                                               "http://vs.uni-kassel.de/IceTest#TestEntity",
                                               "http://vs.uni-kassel.de/IceTest#TestScope1",
                                               "http://vs.uni-kassel.de/IceTest#TestRepresentation1");
  auto spec2 = ice::InformationSpecification("",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation2");

  auto set1 = engine->getKnowlegeBase()->setStore->getSet<ice::Pos3D>(&spec1);
  ASSERT_TRUE((set1 ? true : false));
  auto set11 = engine2->getKnowlegeBase()->setStore->getSet<ice::Pos3D>(&spec1);
  ASSERT_TRUE((set1 ? true : false));
  auto set2 = engine2->getKnowlegeBase()->setStore->getSet<ice::Pos3D>(&spec2);
  ASSERT_TRUE((set2 ? true : false));

  auto nodeSource = engine->getNodeStore()->getNode("http://vs.uni-kassel.de/IceTest#TestSetSourceInd", "http://vs.uni-kassel.de/IceTest#TestEntity");
  ASSERT_TRUE((nodeSource ? true : false));

  auto nodeTransform = engine2->getNodeStore()->getNode("http://vs.uni-kassel.de/IceTest#TestSetTransformNodeInd", "http://vs.uni-kassel.de/IceTest#TestEntity");
  ASSERT_TRUE((nodeTransform ? true : false));

  nodeSource->performTask();

  std::this_thread::sleep_for(std::chrono::milliseconds {100});

  ASSERT_EQ(1, set1->getSize());
  ASSERT_EQ(1, set11->getSize());
  ASSERT_EQ(1, set2->getSize());

  auto position2 = set2->get("muh");

  ASSERT_TRUE((position2 ? true : false));
  EXPECT_EQ(101, position2->getInformation()->x);
  EXPECT_EQ(121, position2->getInformation()->y);
  EXPECT_EQ(131, position2->getInformation()->z);

  engine->cleanUp();
  engine.reset();
  engine2->cleanUp();
  engine2.reset();
}

TEST(ASPCoordinator, nodeFailureTest)
{
  ice::Node::registerNodeCreator("TestSourceNodeGrounding", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("TestSourceNodeAlternativeGrounding", &SimpleSourceNodeAlternative::createNode);

  auto timeFactory = std::make_shared<ice::SimpleTimeFactory>();
  std::shared_ptr<ice::Configuration> config = std::make_shared<ice::Configuration>();
  config->ontologyIri = "http://vs.uni-kassel.de/IceTest";
  config->ontologyIriOwnEntity = "http://vs.uni-kassel.de/IceTest#TestNodeFailureInd";
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(config);
  auto streamFactory = std::make_shared<TestFactory>(engine);
  engine->setTimeFactory(timeFactory);
  engine->setCollectionFactory(streamFactory);

  engine->getConfig()->synthesizeTransformations = false;
  engine->init();
  engine->start();

  auto spec1 = ice::InformationSpecification("http://vs.uni-kassel.de/IceTest#TestEntity1",
                                             "http://vs.uni-kassel.de/IceTest#TestEntity",
                                             "http://vs.uni-kassel.de/IceTest#TestScope1",
                                             "http://vs.uni-kassel.de/IceTest#TestRepresentation1");

  auto stream1 = engine->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec1, "http://vs.uni-kassel.de/IceTest#TestSourceNodeAlternativeInd",
                                                                         "http://vs.uni-kassel.de/IceTest#TestNodeFailureInd");
  auto selected = engine->getKnowlegeBase()->streamStore->getSelectedStream<ice::Pos3D>(&spec1);
  ASSERT_TRUE((stream1 ? true : false));
  ASSERT_TRUE((selected ? true : false));

  auto pos = std::make_shared<ice::Pos3D>(nullptr);
  pos->x = 1;
  pos->y = 2;
  pos->z = 3;

  stream1->add(pos);

  auto position1 = selected->getLast();

  ASSERT_TRUE((position1 ? true : false));
  EXPECT_EQ(1, position1->getInformation()->x);
  EXPECT_EQ(2, position1->getInformation()->y);
  EXPECT_EQ(3, position1->getInformation()->z);

  auto nodeSource = engine->getNodeStore()->getNode("http://vs.uni-kassel.de/IceTest#TestSourceNodeAlternativeInd", "http://vs.uni-kassel.de/IceTest#TestEntity1");
  ASSERT_TRUE((nodeSource ? true : false));

  nodeSource->performNode();

  std::this_thread::sleep_for(std::chrono::milliseconds {500});

  EXPECT_FALSE(nodeSource->isActive());

  auto stream2 = engine->getKnowlegeBase()->streamStore->getStream<ice::Pos3D>(&spec1, "http://vs.uni-kassel.de/IceTest#TestSourceNodeInd",
                                                                         "http://vs.uni-kassel.de/IceTest#TestNodeFailureInd");
  EXPECT_TRUE((stream2 ? true : false));

  auto nodeSource2 = engine->getNodeStore()->getNode("http://vs.uni-kassel.de/IceTest#TestSourceNodeInd", "http://vs.uni-kassel.de/IceTest#TestEntity1");
  EXPECT_TRUE((nodeSource2 ? true : false));

  auto pos2 = std::make_shared<ice::Pos3D>(nullptr);
  pos2->x = 4;
  pos2->y = 5;
  pos2->z = 6;

  stream2->add(pos2);

  auto position2 = selected->getLast();

  ASSERT_TRUE((position2 ? true : false));
  EXPECT_EQ(4, position2->getInformation()->x);
  EXPECT_EQ(5, position2->getInformation()->y);
  EXPECT_EQ(6, position2->getInformation()->z);

  engine->cleanUp();
  engine.reset();
}
