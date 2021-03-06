/*
 *  Created on: 10.06.2015
 *      Author: paspartout
 */

#include <ros/package.h>

#include "ice/ontology/OntologyInterface.h"
#include "ice/representation/GContainer.h"
#include "ice/representation/GContainerFactory.h"
#include "ice/representation/Representation.h"

#include "gtest/gtest.h"


namespace
{

TEST(GContainerTest, simpleTest)
{
  // Given a valid empty ice ontology
  std::string path = ros::package::getPath("ice");
  bool result;

  auto oi = std::make_shared<ice::OntologyInterface>(path + "/java/lib/");
  oi->addIRIMapper(path + "/ontology/");

  ASSERT_FALSE(oi->errorOccurred());

  result = oi->addOntologyIRI("http://vs.uni-kassel.de/Ice");

  ASSERT_FALSE(oi->errorOccurred());
  ASSERT_TRUE(result);

  result = oi->loadOntologies();

  ASSERT_FALSE(oi->errorOccurred());
  ASSERT_TRUE(result);

  result = oi->isConsistent();

  ASSERT_FALSE(oi->errorOccurred());
  ASSERT_TRUE(result);

  ice::GContainerFactory fac;
  fac.setOntologyInterface(oi);
  fac.init();

  auto rep = fac.getRepresentation("http://vs.uni-kassel.de/Ice#DefaultMovementRep");

  ASSERT_TRUE(rep != false);

  auto movement = fac.makeInstance(rep);

  const double testVal = 4.2f;
  auto pos = rep->accessPath({"http://vs.uni-kassel.de/Ice#Translation"});

  ASSERT_TRUE(pos != nullptr);

  movement->set(pos, &testVal);

  double val = movement->getValue<double>(pos);

  ASSERT_EQ(testVal, val);
}


TEST(GContainerTest, serialize)
{
  // Given a valid empty ice ontology
  std::string path = ros::package::getPath("ice");
  bool result;

  auto oi = std::make_shared<ice::OntologyInterface>(path + "/java/lib/");
  oi->addIRIMapper(path + "/ontology/");

  ASSERT_FALSE(oi->errorOccurred());

  result = oi->addOntologyIRI("http://vs.uni-kassel.de/Ice");

  ASSERT_FALSE(oi->errorOccurred());
  ASSERT_TRUE(result);

  result = oi->loadOntologies();

  ASSERT_FALSE(oi->errorOccurred());
  ASSERT_TRUE(result);

  result = oi->isConsistent();

  ASSERT_FALSE(oi->errorOccurred());
  ASSERT_TRUE(result);

  ice::GContainerFactory fac;
  fac.setOntologyInterface(oi);
  fac.init();

  auto rep = fac.getRepresentation("http://vs.uni-kassel.de/Ice#DefaultMovementRep");

  ASSERT_TRUE(rep != false);

  auto movement = fac.makeInstance(rep);
  auto movement2 = fac.makeInstance(rep);

  const double testVal = 4.2f;
  auto pos = rep->accessPath({"http://vs.uni-kassel.de/Ice#Translation"});

  ASSERT_TRUE(pos != nullptr);

  movement->set(pos, &testVal);

  std::vector<std::vector<uint8_t>> bytes;

//  movement->toByte(bytes);
//  movement2->fromByte(bytes);

  double val = movement2->getValue<double>(pos);

//  ASSERT_EQ(testVal, val);
}

}



