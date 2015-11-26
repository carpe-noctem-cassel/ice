/*
 * InformationStore.cpp
 *
 *  Created on: May 19, 2014
 *      Author: sni
 */

#include "ice/information/InformationStore.h"

#include "ice/ICEngine.h"
#include "ice/information/StreamFactory.h"
#include "ice/ontology/OntologyInterface.h"

#include "easylogging++.h"

namespace ice
{

InformationStore::InformationStore(std::weak_ptr<ICEngine> engine)
{
  this->engine = engine;
  this->_log = el::Loggers::getLogger("InformationStore");
}

void InformationStore::init()
{
  auto engineObject = engine.lock();

  this->eventHandler = engineObject->getEventHandler();
  this->config = engineObject->getConfig();
  this->streamFactory = engineObject->getStreamFactory();
  this->ontology = engineObject->getOntologyInterface();

//  this->readEntitiesFromOntology();
}

void InformationStore::cleanUp()
{
  this->eventHandler.reset();
  this->config.reset();
  this->streamFactory.reset();
  this->ontology.reset();

  for(auto stream : this->streams)
  {
    stream->destroy();
  }

  this->streams.clear();
}

InformationStore::InformationStore(std::shared_ptr<EventHandler> eventHandler)
{
  this->eventHandler = eventHandler;
  this->_log = el::Loggers::getLogger("InformationStore");
}

InformationStore::~InformationStore()
{
  // TODO Auto-generated destructor stub
}

std::shared_ptr<BaseInformationStream> InformationStore::registerBaseStream(
    std::string dataType, std::shared_ptr<InformationSpecification> specification, const std::string name,
    const int streamSize, std::map<std::string, int> metadata, std::string provider, std::string sourceSystem)
{
  auto ptr = this->getBaseStream(specification.get(), provider, sourceSystem);

  //stream already registered
  if (ptr)
  {
    _log->warn("InformationStore: Duplicated Stream with '%v', '%v', '%v'",
                  specification->toString(), provider, sourceSystem);
    return ptr;
  }

  auto desc = std::make_shared<StreamDescription>(specification, name, provider, sourceSystem, metadata);
  std::string type = this->ontology->toLongIri(dataType);
  auto stream = this->streamFactory->createStream(type, desc, this->eventHandler, streamSize);

  if (stream)
  {
    _log->debug("Created stream with '%v', '%v', '%v', '%v'", specification->toString(),
                provider, sourceSystem, type);
    this->streams.push_back(stream);
  }
  else
  {
    _log->error("Stream with '%v', '%v', '%v', '%v' could not be created", specification->toString(),
                provider, sourceSystem, type);
  }
  return stream;
}

std::shared_ptr<Configuration> InformationStore::getConfig() const
{
  return this->config;
}

std::shared_ptr<EventHandler> InformationStore::getEventHandler() const
{
  return this->eventHandler;
}

std::shared_ptr<BaseInformationStream> InformationStore::getBaseStream(InformationSpecification * specification)
{
  return this->getBaseStream(specification, "", "");
}

std::shared_ptr<BaseInformationStream> InformationStore::getBaseStream(InformationSpecification *specification,
                                                                       std::string provider, std::string sourceSystem)
{
  _log->debug("Get stream by '%v', '%v', '%v'", specification->toString(), provider,
              sourceSystem);

  std::vector<std::shared_ptr<BaseInformationStream>> selected;

  for (auto stream : this->streams)
  {
    auto spec = stream->getStreamDescription();
    if (*spec->getInformationSpecification() == *specification)
    {
      if (provider != "")
      {
        if (provider == spec->getProvider() && sourceSystem == spec->getSourceSystem())
        {
          selected.push_back(stream);
        }
      }
      else
      {
        selected.push_back(stream);
      }
    }
  }

  if (selected.size() == 0)
  {
    return std::shared_ptr<BaseInformationStream>();
  }

  return selectBestStream(&selected);
}

std::shared_ptr<BaseInformationStream> InformationStore::selectBestStream(
    std::vector<std::shared_ptr<BaseInformationStream>> *streams)
{
  if (streams->size() == 0)
  {
    return std::shared_ptr<BaseInformationStream>();
  }

  auto best = streams->at(0);

  for (int i = 0; i < streams->size(); ++i)
  {
    // TODO
  }

  return best;
}

std::shared_ptr<BaseInformationStream> InformationStore::getBaseStream(
    const std::shared_ptr<StreamDescription> streamDescription)
{
  return this->getBaseStream(streamDescription->getInformationSpecification().get(), streamDescription->getProvider(),
                             streamDescription->getSourceSystem());
}

bool ice::InformationStore::addDescriptionsToInformationModel(std::shared_ptr<InformationModel> informationModel)
{
  std::lock_guard<std::mutex> guard(this->_mtx);
  bool returnValue = false;

  return returnValue;
}

void InformationStore::cleanUpStreams()
{
  std::lock_guard<std::mutex> guard(this->_mtx);
  _log->verbose(1, "Start removing unused streams");
  int counter = 0;

  for (int i=0; i < this->streams.size(); ++i)
  {
    auto stream = this->streams.at(i);


    _log->info("Checking stream '%v', reference count %v", stream->toString(), stream.use_count());

    if (stream.use_count() == 2)
    {
      _log->info("Remove unused stream '%v'", stream->toString());

      ++counter;
      this->streams.erase(this->streams.begin() + i);
      --i;
    }
  }

  _log->info("Clean up information store: '%v' streams are removed", counter);
}

ont::entityType InformationStore::getEntityType(ont::entity entity)
{
  auto it = this->entityTypeMap.find(entity);
  if (it != this->entityTypeMap.end())
    return it->second;

  return "";
}

void InformationStore::readEntitiesFromOntology()
{
  _log->verbose(1, "Read entities and types from ontology");

  if (this->ontology->isLoadDirty())
    this->ontology->loadOntologies();

  const char* infoStructure = this->ontology->readInformationStructureAsASP();

  _log->debug("Extracted entities from ontology");
  _log->verbose(1, infoStructure);

  this->entityTypeMap.clear();

  std::stringstream ss;
  std::string item;

  ss << infoStructure;
//  delete infoStructure;

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

} /* namespace ice */

