#include <gtest/gtest.h>

#include "ice/ICEngine.h"
#include "ice/information/InformationSpecification.h"
#include "ice/information/InformationStore.h"
#include "ice/model/aspModel/ASPModelGenerator.h"

#include "etc/EngineStuff.cpp"
#include "etc/TestTime.cpp"

TEST(ASPModelGenerator, create)
{
  ice::Node::registerNodeCreator("TestSourceNodeGrounding", &SimpleSourceNode::createNode);
  ice::Node::registerNodeCreator("TestComputationalNodeGrounding", &SmothingNode::createNode);

  auto streamFactory = std::make_shared<TestFactory>();
  auto timeFactory = std::make_shared<TestTimeFactory>();
  std::shared_ptr<ice::ICEngine> engine = std::make_shared<ice::ICEngine>(timeFactory, streamFactory, "http://vs.uni-kassel.de/IceTest#TestSystem");
  engine->init();
//  ice::ASPModelGenerator coordinator(engine, );
//  coordinator.init();

  bool result = engine->getOntologyInterface()->addOntologyIRI("http://vs.uni-kassel.de/IceTest");

  ASSERT_FALSE(engine->getOntologyInterface()->errorOccurred());
  ASSERT_TRUE(result);

  engine->getProcessingModelGenerator()->createProcessingModel();
  auto spec1 = ice::InformationSpecification("testEntity1", "testEntity", "testScope1", "testRepresentation1");
  auto spec2 = ice::InformationSpecification("testEntity1", "testEntity", "testScope1", "testRepresentation2");

  auto stream1 = engine->getInformationStore()->getStream<ice::Position>(&spec1, "testSourceNodeInd",
                                                                         "testSystem");
  auto stream2 = engine->getInformationStore()->getStream<ice::Position>(&spec2, "testComputationalNodeInd",
                                                                         "testSystem");

  ASSERT_TRUE((stream1 ? true : false));
  ASSERT_TRUE((stream2 ? true : false));

  std::unique_ptr<ice::Position> position1(new ice::Position());
  position1->x = 3;
  position1->y = 2;
  position1->z = 1;

  stream1->add(std::move(position1));

  std::this_thread::sleep_for(std::chrono::milliseconds {10});

  auto position2 = stream2->getLast();

  ASSERT_TRUE((position2 ? true : false));
  EXPECT_EQ(2, position2->getInformation().x);
  EXPECT_EQ(1, position2->getInformation().y);
  EXPECT_EQ(0, position2->getInformation().z);
}
