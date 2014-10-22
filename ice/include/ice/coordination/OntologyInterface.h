/*
 * OntologyInterface.h
 *
 *  Created on: Oct 16, 2014
 *      Author: sni
 */

#ifndef ONTOLOGYINTERFACE_H_
#define ONTOLOGYINTERFACE_H_

#include <jni.h>
#include <string>
#include <vector>

// Forward declaration
namespace ice
{
class Logger;
}

namespace ice
{

class OntologyInterface
{
public:
  OntologyInterface(std::string const p_jarPath);
  virtual ~OntologyInterface();
  bool errorOccurred();
  void addIRIMapper(std::string const p_mapper);
  bool loadOntologies();
  bool isConsistent();
  bool addSystem(std::string const p_system);
  bool addOntologyIRI(std::string const p_iri);
  bool removeOntologyIRI(std::string const p_iri);
  std::string readInformationStructureAsASP();
  std::string readNodesAndIROsAsASP();
  bool addNodeIndividual(std::string const p_node, std::string const p_nodeClass, std::string const p_system, std::vector<std::string> p_metadatas,
               std::vector<int> p_metadataValues, std::vector<std::string> p_metadataGroundings);
  int getSomeMinCardinality();
  bool setSomeMinCardinality(int p_value);
  int getSomeMaxCardinality();
  bool setSomeMaxCardinality(int p_value);

private:
  bool checkError(std::string p_method, std::string p_error);

private:
  Logger* _log; /**< Logger */
  JavaVM *jvm; /*< denotes a Java VM */
  JNIEnv *env; /*< pointer to native method interface */
  bool error; /*< an error has occurred */
  jclass javaOntologyInterface; /*< java class to access the ontology */
  jobject javaInterface; /*< java interface object */
  jmethodID addIRIMapperMethod; /*< Method id */
  jmethodID loadOntologiesMethod; /*< Method id */
  jmethodID isConsistentMethod; /*< Method id */
  jmethodID addSystemMethod; /*< Method id */
  jmethodID addOntologyIRIMethod; /*< Method id */
  jmethodID removeOntologyIRIMethod; /*< Method id */
  jmethodID readInformationStructureAsASPMethod; /* Method id */
  jmethodID readNodesAndIROsAsASPMethod; /*< Method id */
  jmethodID addNodeIndividualMethod; /* Method id */
  jmethodID getSomeMinCardinalityMethod; /*< Method id */
  jmethodID setSomeMinCardinalityMethod; /*< Method id */
  jmethodID getSomeMaxCardinalityMethod; /*< Method id */
  jmethodID setSomeMaxCardinalityMethod; /*< Method id */
};

} /* namespace ice */

#endif /* ONTOLOGYINTERFACE_H_ */
