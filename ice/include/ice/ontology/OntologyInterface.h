/*
 * OntologyInterface.h
 *
 *  Created on: Oct 16, 2014
 *      Author: sni
 */

#ifndef ONTOLOGYINTERFACE_H_
#define ONTOLOGYINTERFACE_H_

#include <iostream>
#include <jni.h>
#include <memory>
#include <mutex>
#include <string>
#include <vector>


// Forward declaration
namespace el
{
class Logger;
}

namespace ice
{

enum LogLevel { 
	Disabled, Error, Warning, Info, Debug
};

class OntologyInterface
{
public:
  static void callJniGc();

public:
  static const std::string ICE_IRI;

public:
  OntologyInterface(std::string const p_jarPath);
  virtual ~OntologyInterface();
  bool errorOccurred();
  void addIRIMapper(std::string const p_mapper);
  bool loadOntologies();
  bool loadOntology(std::string const p_path);
  bool saveOntology(std::string const p_path);
  int getOntologyIDs(std::vector<std::pair<std::string,std::string>> &ids);
  void compareOntologyIDs(std::vector<std::pair<std::string,std::string>> &ids, std::vector<std::pair<std::string,std::string>> &outDiff);
  bool initReasoner(bool const p_force);
  bool isConsistent();
  std::unique_ptr<std::vector<std::string>> getSystems();
  bool isSystemKnown(std::string const p_system);
  bool addSystem(std::string const p_system);
  bool addNodesToSystem(std::string const p_system, std::vector<std::string> p_toAdd);
  bool addIndividual(std::string const p_individual, std::string const p_class);
  bool addEntityType(std::string const p_entityType, std::vector<std::string> p_entityScopes);
  bool addScopesToEntityType(std::string const p_entityType, std::vector<std::string> p_entityScopes);
  bool addEntityScope(std::string const p_entityScope, std::vector<std::string> p_representations);
  bool addValueScope(std::string const p_superValueScope, std::string const p_valueScope, std::string const p_representation);
  bool addRepresentation(std::string const p_superRepresentation, std::string const p_representation,
                         std::vector<std::string> p_dimensions);
  bool addDimensionToRep(std::string const p_representation, std::string p_dimensions, std::string const p_entityScope);
  bool addDimensionToRep(std::string const p_representation, std::string p_dimensions);
  bool addNamedStream(std::string const p_stream, std::string const p_entityScope, std::string const p_representation);
  bool addNamedSet(std::string const p_set, std::string const p_entityType, std::string const p_entityScope, std::string const p_representation);
  bool addRequiredStream(std::string const p_namedStream, std::string const p_namedStreamClass, std::string const p_system, std::string const p_entity, std::string const p_entityRelated);
  bool addRequiredSet(std::string const p_namedSet, std::string const p_namedSetClass, std::string const p_system, std::string const p_entityRelated);
  bool addSourceNodeClass(std::string const p_node, std::vector<std::string> p_outputs,
                          std::vector<int> p_outputsMinSize, std::vector<int> p_outputsMaxSize);
  bool addComputationNodeClass(std::string const p_node, std::vector<std::string> p_inputs,
                               std::vector<int> p_inputsMinSize, std::vector<int> p_inputsMaxSize,
                               std::vector<std::string> p_outputs, std::vector<int> p_outputsMinSize,
                               std::vector<int> p_outputsMaxSize);
  bool addTransformationNodeClass(std::string const p_node, std::vector<std::string> p_inputs, std::vector<int> p_inputsMinSize,
                       std::vector<int> p_inputsMaxSize, std::vector<std::string> p_inputsRelated,
                       std::vector<int> p_inputsRelatedMinSize, std::vector<int> p_inputsRelatedMaxSize,
                       std::vector<std::string> p_outputs, std::vector<int> p_outputsMinSize,
                       std::vector<int> p_outputsMaxSize);
  bool addSetNodeClass(std::string const p_node, std::vector<std::string> p_inputs, std::vector<int> p_inputsMinSize,
                       std::vector<int> p_inputsMaxSize, std::vector<std::string> p_inputsRelated,
                       std::vector<int> p_inputsRelatedMinSize, std::vector<int> p_inputsRelatedMaxSize,
                       std::vector<std::string> p_inputSets, std::vector<int> p_inputSetsMinSize,
                       std::vector<int> p_inputSetsMaxSize, std::vector<std::string> p_outputSets,
                       std::vector<int> p_outputSetsMinSize, std::vector<int> p_outputSetsMaxSize);
  std::string toLongIri(std::string p_shortIri);
  std::string toShortIri(std::string p_longIri);
  std::string toShortIriAll(std::string p_string);

  bool addOntologyIRI(std::string const p_iri);
  bool removeOntologyIRI(std::string const p_iri);
  std::unique_ptr<std::vector<std::string>> getOntologyIriMapping();
  void readOntologyIriMappingFromOntology();
  std::string readInformationStructureAsASP();
  std::string readRepresentationsAsCSV();
  std::unique_ptr<std::vector<std::string>> readRepresentations();
  std::unique_ptr<std::vector<std::vector<std::string>>> readNodesAsASP(std::string const p_system);
  bool addNodeIndividual(std::string const p_node, std::string const p_nodeClass, std::string const p_system, std::string const p_aboutEntity, std::string const p_aboutRelatedEntity, std::vector<std::string> p_metadatas,
      std::vector<int> p_metadataValues, std::vector<int> p_metadataValues2, std::vector<std::string> p_metadataGroundings);
  bool addTransformationIndividual(std::string const p_transformation, std::string const p_transformationClass, std::string const p_system, std::vector<std::string> p_metadatas,
      std::vector<int> p_metadataValues, std::vector<std::string> p_metadataGroundings);
  int getSomeMinCardinality();
  bool setSomeMinCardinality(int p_value);
  int getSomeMaxCardinality();
  bool setSomeMaxCardinality(int p_value);
  LogLevel getLogLevel();
  void setLogLevel(LogLevel level);
  void getMemoryUsage(double &total, double &max, double &free);
  void startMemoryMonitor();
  void stopMemoryMonitor();
  void resetMemoryMonitor();

  bool isInformationDirty();
  bool isSystemDirty();
  bool isLoadDirty();

  void attachCurrentThread();
  void detachCurrentThread();

private:
  bool checkError(std::string p_method, std::string p_error);
  void readSystemsFromOntology();
  void readOntologyIDsFromOntology();

private:
  el::Logger*                                           _log;                   /**< Logger */
  static JavaVM                                         *jvm;                   /**< a Java VM */
  JNIEnv                                                *env;                   /**< pointer to native method interface */
  bool                                                  error;                  /**< an error has occurred */
  bool                                                  informationDirty;       /**< Flag to check if the information model was changed */
  bool                                                  systemDirty;            /**< Flag to check if the system model was changed */
  bool                                                  loadDirty;              /**< Flag to check if the ontology needs to be loaded again */
  bool                                                  mappingDirty;           /**< Flag to check if the ontology iri mapping needs to be loaded again */
  std::vector<std::string>                              knownSystem;            /**< List of known systems */
  std::vector<std::pair<std::string, std::string>>      ontologyIds;            /**< List of ontology iris and version iris */
  std::vector<std::string>                              ontologyIriMapping;     /**< List of ontology iris for Mapping */
  std::string                                           informationStructure;   /**< The information structure */
  std::mutex                                            mtx_;                   /**< Mutex */

  jclass                                                javaOntologyInterface;  /**< java class to access the ontology */
  jstring                                               empty;                  /**< Empty string */
  jobject                                               javaInterface;          /**< java interface object */

  jmethodID                                             addIRIMapperMethod; /**< Method id */
  jmethodID                                             loadOntologiesMethod; /**< Method id */
  jmethodID                                             loadOntologyMethod; /**< Method id */
  jmethodID                                             saveOntologyMethod; /**< Method id */
  jmethodID                                             getOntologyIDsMethod; /**< Method id */
  jmethodID                                             initReasonerMethod; /**< Method id */
  jmethodID                                             isConsistentMethod; /**< Method id */
  jmethodID                                             getSystemsMethod; /**< Method id */
  jmethodID                                             addSystemMethod; /**< Method id */
  jmethodID                                             addNodesToSystemMethod; /**< Method id */
  jmethodID                                             addIndividualMethod; /**< Method id */
  jmethodID                                             addEntityTypeMethod; /**< Method id */
  jmethodID                                             addEntityScopeMethod; /**< Method id */
  jmethodID                                             addScopesToEntityTypeMethod; /**< Method id */
  jmethodID                                             addValueScopeMethod; /**< Method id */
  jmethodID                                             addRepresentationMethod; /**< Method id */
  jmethodID                                             addDimensionToRep2Method; /**< Method id */
  jmethodID                                             addDimensionToRep3Method; /**< Method id */
  jmethodID                                             addNamedStreamMethod; /**< Method id */
  jmethodID                                             addNamedSetMethod; /**< Method id */
  jmethodID                                             addRequiredStreamMethod; /**< Method id */
  jmethodID                                             addRequiredSetMethod; /**< Method id */
  jmethodID                                             addSourceNodeClassMethod; /**< Method id */
  jmethodID                                             addComputationNodeClassMethod; /**< Method id */
  jmethodID                                             addTransformationNodeClassMethod; /**< Method id */
  jmethodID                                             addSetNodeClassMethod; /**< Method id */

  jmethodID                                             addOntologyIRIMethod; /**< Method id */
  jmethodID                                             removeOntologyIRIMethod; /**< Method id */
  jmethodID                                             getOntologyIriMappingMethod; /**< Method id */
  jmethodID                                             readInformationStructureAsASPMethod; /**< Method id */
  jmethodID                                             readRepresentationsAsCSVMethod; /**< Method id */
  jmethodID                                             readNodesAsASPMethod; /**< Method id */
  jmethodID                                             addNodeIndividualMethod; /**< Method id */
  jmethodID                                             addTransformationIndividualMethod; /**< Method id */
  jmethodID                                             getSomeMinCardinalityMethod; /**< Method id */
  jmethodID                                             setSomeMinCardinalityMethod; /**< Method id */
  jmethodID                                             getSomeMaxCardinalityMethod; /**< Method id */
  jmethodID                                             setSomeMaxCardinalityMethod; /**< Method id */

  jmethodID                                             getLogLevelMethod; /**< Method id */
  jmethodID                                             setLogLevelMethod; /**< Method id */

  jmethodID                                             getMemoryUsageMethod; /**< Method id */
  jmethodID                                             startMemoryMonitorMethod; /**< Method id */
  jmethodID                                             stopMemoryMonitorMethod; /**< Method id */
  jmethodID                                             resetMemoryMonitorMethod; /**< Method id */

};

}
/* namespace ice */

#endif /* ONTOLOGYINTERFACE_H_ */
