/*
 * KnowledgeBase.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: sni
 */

#include "ice/information/KnowledgeBase.h"

#include "ice/ICEngine.h"
#include "ice/information/InformationStore.h"
#include "ice/information/StreamStore.h"
#include "ice/information/SetStore.h"
#include "ice/ontology/OntologyInterface.h"
#include "ice/representation/GContainer.h"

namespace ice
{

KnowledgeBase::KnowledgeBase(std::weak_ptr<ICEngine> engine) : engine(engine)
{
  this->_log = el::Loggers::getLogger("KnowledgeBase");

  this->informationStore = std::make_shared<InformationStore>(engine);
  this->setStore = std::make_shared<SetStore>(engine);
  this->streamStore = std::make_shared<StreamStore>(engine);
}

KnowledgeBase::~KnowledgeBase()
{
  this->cleanUp();
}

void KnowledgeBase::init()
{
  if (this->engine.expired())
    return;
  auto engineObject = engine.lock();

  this->ontology = engineObject->getOntologyInterface();
  // reading information structure from ontology
  this->readEntitiesFromOntology();

  this->informationStore->init();
  this->setStore->init();
  this->streamStore->init();
}

void KnowledgeBase::cleanUp()
{
  this->ontology.reset();

  if (this->informationStore)
    this->informationStore->cleanUp();

  if (this->setStore)
    this->setStore->cleanUp();

  if (this->streamStore)
    this->streamStore->cleanUp();

  this->informationStore.reset();
  this->setStore.reset();
  this->streamStore.reset();
}

void KnowledgeBase::cleanUpStores()
{
  this->setStore->cleanUpUnused();
  this->streamStore->cleanUpUnused();
}


int KnowledgeBase::getInformation(std::shared_ptr<InformationSpecification> request,
                   std::vector<std::shared_ptr<InformationElement<GContainer>>> &outInfo,
                   bool useTransfromation)
{
  // request from knowledge base
  int count = this->informationStore->getInformation(request, outInfo, useTransfromation);

  // stream store
  count += this->streamStore->getInformation(request, outInfo, useTransfromation);

  // stream store
  count +=this->streamStore->getInformation(request, outInfo, useTransfromation);

  return count;
}

ont::entityType KnowledgeBase::getEntityType(ont::entity entity)
{
  auto it = this->entityTypeMap.find(entity);
  if (it != this->entityTypeMap.end())
    return it->second;

  return "";
}

void KnowledgeBase::readEntitiesFromOntology()
{
  _log->verbose(1, "Read entities and types from ontology");

  if (this->ontology->isLoadDirty())
    this->ontology->loadOntologies();

  std::stringstream ss;
  ss.str(this->ontology->readInformationStructureAsASP());

  _log->debug("Extracted entities from ontology");
  _log->verbose(1, ss.str());

  this->entityTypeMap.clear();

  std::string item;

  while (std::getline(ss, item, '\n'))
  {
    if (item.find("entity(") == 0)
    {
      int index1 = item.find(",");
      int index2 = item.find(")");
      auto entity = item.substr(7, index1 - 7);
      auto entityType = item.substr(index1 + 1, index2 - index1 - 1);

      this->entityTypeMap[entity] = entityType;
    }
  }
}

std::string KnowledgeBase::dataTypeForRepresentation(std::string representation)
{
  // TODO
  return representation;
}

} /* namespace ice */
