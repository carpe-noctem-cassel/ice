/*
 * TransformationSynthese.h
 *
 *  Created on: Dec 1, 2015
 *      Author: sni
 */

#ifndef TRANSFORMATIONSYNTHESIS_H_
#define TRANSFORMATIONSYNTHESIS_H_

#include <memory>
#include <tuple>
#include <vector>
#include <gringo/value.hh>

#include "ClingWrapper.h"

#include "easylogging++.h"

//Forward declarations
namespace ice
{
class GContainerFactory;
class ICEngine;
class OntologyInterface;
class Representation;
class Transformation;
} /* namespace ice */

//Forward declarations
namespace supplementary
{
class ClingWrapper;
} /* namespace ice */

namespace ice
{

class TransformationSynthesis
{
public:
  TransformationSynthesis();
  TransformationSynthesis(std::weak_ptr<ICEngine> engine);
  virtual ~TransformationSynthesis();

  void init();
  void cleanUp();
  void synthesizeTransformations();

  void setOntology(std::shared_ptr<OntologyInterface> ontology);
  void setGContainerFactory(std::shared_ptr<GContainerFactory> factory);

private:
  void readInfoStructureFromOntology();
  bool extractOperations(supplementary::ClingWrapper &asp, std::shared_ptr<Transformation> transformation,
                      Gringo::Value simRep, std::shared_ptr<Representation> rep1, std::shared_ptr<Representation> rep2, std::vector<std::string> &path);
  void extractDeviation(supplementary::ClingWrapper &asp, std::vector<std::string> &deviations,
                        Gringo::Value &simRep, std::string type);

private:
  std::weak_ptr<ICEngine> engine;
  std::shared_ptr<OntologyInterface> ontology;
  std::shared_ptr<GContainerFactory> containerFactory;
  std::vector<std::string> entities; /**< The entites as strings */
  el::Logger* _log; /**< Logger */
};

} /* namespace ice */

#endif /* ASPTRANSFORMATIONGENERATION_H_ */
