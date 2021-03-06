/*
 * Identity.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: sni
 */

#include "ice/Entity.h"

#include <iostream>
#include <tuple>
#include <sstream>

#include "External.h"

#include "ice/model/ProcessingModel.h"
#include "ice/ontology/OntologyInterface.h"
#include "ice/processing/Node.h"
#include "ice/information/BaseInformationStream.h"
#include "ice/information/BaseInformationSet.h"
#include "ice/ICEngine.h"
#include "ice/EntityDirectory.h"

namespace ice
{

Entity::Entity(const std::initializer_list<Id> ids) :
    iceIdentity(false), directory(nullptr), timeFactory(nullptr), available(false), _log(
        el::Loggers::getLogger("Entity")), index(0), timeoutDuration(2000), timestamp(0), superEngine(false),
        nodesExtracted(false)
{
  if (ids.size() == 0)
  {
    throw std::runtime_error(std::string("Entity ids can not be empty"));
  }

  for (const auto& id : ids) {
          this->ids[id.key] = id.value;
  }
}

Entity::Entity(std::shared_ptr<EntityDirectory> const &directory, std::weak_ptr<ICEngine> engine,
               std::shared_ptr<TimeFactory> const &factory, const std::initializer_list<Id> ids) :
    iceIdentity(false), directory(directory), engine(engine), timeFactory(factory), available(false), _log(
        el::Loggers::getLogger("Entity")), index(0), timeoutDuration(2000), superEngine(false),
        nodesExtracted(false)
{
  if (ids.size() == 0)
  {
    throw std::runtime_error(std::string("Entity ids can not be empty"));
  }

  for (const auto& id : ids) {
          this->ids[id.key] = id.value;
  }

  this->timestamp = this->timeFactory->createTime();
}

Entity::~Entity()
{
  //
}

uint8_t Entity::getNextIndex()
{
  return ++this->index % 256;
}

int Entity::initializeFromOntology(std::shared_ptr<OntologyInterface> const &ontologyInterface)
{

  _log->debug("Extract asp information from ontology for entity '%v'", this->toString());
  std::string ownIri = "";

  if (false == this->getId(EntityDirectory::ID_ONTOLOGY, ownIri) || ontologyInterface->isSystemKnown(ownIri))
    return -1;

  return 1;
}

std::map<std::string, std::string> Entity::readConfiguration(std::string const config)
{
  std::map<std::string, std::string> configuration;
  std::stringstream ss(config);
  std::string item;

  while (std::getline(ss, item, ';'))
  {
    int index = item.find("=");

    if (index == std::string::npos)
    {
      _log->warn("Broken configuration '%v', skipped", item);
      continue;
    }

    configuration[item.substr(0, index)] = item.substr(index + 1, item.size());
  }

  return configuration;
}

entity_match Entity::checkMatching(std::shared_ptr<Entity> &identity)
{
  int matchCount = 0;
  bool conflicting = false;

  for (auto &id : this->ids)
  {
    auto rid = identity->ids.find(id.first);

    if (rid == identity->ids.end())
    {
      continue;
    }

    if (rid->second == id.second)
    {
      ++matchCount;
    }
    else
    {
      conflicting = true;
    }
  }

  if (matchCount == 0)
  {
    return entity_match::NO_MATCH;
  }

  if (conflicting)
  {
    return entity_match::CONFLICTING;
  }

  if (matchCount == this->ids.size() && matchCount == identity->ids.size())
  {
    return entity_match::FULL_MATCH;
  }

  if (matchCount == this->ids.size())
  {
    return entity_match::INCLUDED;
  }

  if (matchCount == identity->ids.size())
  {
    return entity_match::INCLUDING;
  }

  return entity_match::PARTIAL_MATCH;
}

entity_match Entity::checkMatching(const std::initializer_list<Id>& ids)
{
  int matchCount = 0;
  bool conflicting = false;

  for (auto &id : ids)
  {
    auto rid = this->ids.find(id.key);

    if (rid == this->ids.end())
    {
      continue;
    }

    if (rid->second == id.value)
    {
      ++matchCount;
    }
    else
    {
      conflicting = true;
    }
  }

  if (matchCount == 0)
  {
    return entity_match::NO_MATCH;
  }

  if (conflicting)
  {
    return entity_match::CONFLICTING;
  }

  if (matchCount == this->ids.size() && matchCount == ids.size())
  {
    return entity_match::FULL_MATCH;
  }

  if (matchCount == this->ids.size())
  {
    return entity_match::INCLUDED;
  }

  if (matchCount == ids.size())
  {
    return entity_match::INCLUDING;
  }

  return entity_match::PARTIAL_MATCH;
}

entity_match Entity::checkMatching(std::string &key, std::string &value)
{
  auto id = this->ids.find(key);

  if (id == this->ids.end())
  {
    return entity_match::NO_MATCH;
  }

  if (id->second == value)
    return entity_match::FULL_MATCH;

  return entity_match::CONFLICTING;
}

void Entity::fuse(std::shared_ptr<Entity> &entity)
{
  for (auto &id : entity->ids)
  {
    this->ids[id.first] = id.second;
  }

  this->iceIdentity |= entity->iceIdentity;
  this->available |= entity->available;
  this->index = std::max(this->index, entity->index);
  this->timestamp = std::max(this->timestamp, entity->timestamp);

  for (auto &meta : entity->metadata)
  {
    this->metadata[meta.first] = meta.second;
  }

  for (auto &con : entity->connectionQuality)
  {
    this->connectionQuality[con.first] = con.second;
  }

  for (auto &spec : entity->offeredInformation)
  {
    this->offeredInformation.push_back(spec);
  }

  for (auto &oid : entity->ontologyIds)
  {
    this->ontologyIds.push_back(oid);
  }

  // Sharing Stuff
//    SharedSubModel                                        sendSubModel;
//      SharedSubModel                                        receivedSubModel;
//      std::set<std::shared_ptr<Node>>                       nodes;                  /**< Nodes activated by this engine */
//      std::vector<std::pair<std::string,std::string>>       ontologyIriDiff;

  // ASP Stuff
  if (this->external == nullptr)
  {
    this->external = entity->external;
  }
  else
  {
    if (entity->external != nullptr)
    {
      entity->external->assign(false);
      entity->external->release();
    }
  }

  for (auto &element : entity->aspNodes)
  {
    this->addASPElement(element);
  }

  for (auto &element : entity->aspSourceNodes)
  {
    this->addASPElement(element);
  }

  for (auto &element : entity->aspTransformation)
  {
    this->addASPElement(element);
  }

  for (auto &element : entity->aspRequiredStreams)
  {
    this->addASPElement(element);
  }

  for (auto &element : entity->aspRequiredSets)
  {
    this->addASPElement(element);
  }
}


void Entity::fuse(const std::initializer_list<Id>& ids)
{
  for (auto &id : ids)
  {
    this->ids[id.key] = id.value;
  }
}

void Entity::fuse(std::vector<std::tuple<std::string, std::string>>& vector)
{
  for (auto &id : vector)
  {
    this->ids[std::get<0>(id)] = std::get<1>(id);
  }
}

void Entity::pushIds(std::vector<std::tuple<std::string, std::string>>& vector)
{
  for (auto &id : this->ids)
  {
    vector.push_back(std::make_tuple(id.first, id.second));
  }
}

void Entity::checkDirectory()
{
  this->directory->fuse(this->shared_from_this());
}

void Entity::checkIce()
{
  if (this->iceIdentity)
    return;

  this->iceIdentity = (this->ids.find(EntityDirectory::ID_ONTOLOGY) != this->ids.end());

  if (this->iceIdentity && this->available)
  {
    this->directory->disvoeredIceIdentity.trigger(this->shared_from_this());
  }
}

bool Entity::isIceIdentity()
{
  return this->iceIdentity;
}

void Entity::setIceIdentity(bool value)
{
  this->iceIdentity = value;
}

time Entity::getActiveTimestamp()
{
  return this->timestamp;
}

void Entity::setActiveTimestamp(time value)
{
  this->timestamp = value;
}

void Entity::setActiveTimestamp()
{
  this->timestamp = this->timeFactory->createTime();

  if (false == this->available && this->isTimeout() == false)
  {
    this->setAvailable(true);
  }
}

bool Entity::isAvailable()
{
  return this->available;
}

void Entity::setAvailable(bool const &value)
{
  if (value)
  {
    if (this->iceIdentity && false == this->available)
    {
      this->directory->disvoeredIceIdentity.trigger(this->shared_from_this());
    }
  }
  else
  {
    if (this->iceIdentity)
    {
      this->directory->vanishedIceIdentity.trigger(this->shared_from_this());
    }
  }

  this->available = value;
}

bool Entity::isCooperationPossible()
{
  return this->isTimeout() == false && this->isAvailable() && this->isIceIdentity();
}

bool Entity::isActiveCooperation()
{
  return this->isTimeout() == false && this->iceIdentity && this->available
      && (this->sendSubModel.subModel != nullptr || this->receivedSubModel.subModel != nullptr);
}

bool Entity::isTimeout()
{
  if (this->superEngine)
    return false;

  return this->timeFactory->checkTimeout(this->timestamp, this->timeoutDuration);
}

void Entity::addId(std::string const &key, std::string const &value)
{
  this->ids[key] = value;
}

bool Entity::getId(std::string const &key, std::string &outValue)
{
  auto cq = this->ids.find(key);

  if (cq == this->ids.end())
    return false;

  outValue = cq->second;

  return true;
}

void Entity::addMetadata(std::string const &key, std::string const &value)
{
  this->metadata[key] = value;
}

bool Entity::getMetadata(std::string const &key, std::string &outValue)
{
  auto cq = this->metadata.find(key);

  if (cq == this->metadata.end())
    return false;

  outValue = cq->second;

  return true;
}

void Entity::addConnectionQuality(std::string const &key, double const &value)
{
  this->connectionQuality[key] = value;
}

bool Entity::getConnectionQuality(std::string const &key, double &outValue)
{
  auto cq = this->connectionQuality.find(key);

  if (cq == this->connectionQuality.end())
    return false;

  outValue = cq->second;

  return true;
}

std::string Entity::toString()
{
  std::stringstream ss;
  ss << "Identity(";

  for (auto &id : this->ids)
  {
    ss << id.first << "='" << id.second << "',";
  }

  ss << ")";
  return ss.str();
}

std::vector<InformationSpecification>& Entity::getOfferedInformation()
{
  return this->offeredInformation;
}

void Entity::addOfferedInformation(std::vector<InformationSpecification> const &offeres)
{
  for (auto &info : offeres)
  {
    this->offeredInformation.push_back(info);
  }

  this->directory->offeredInformation.trigger(this->shared_from_this());
}

std::vector<std::pair<std::string, std::string>>& Entity::getOntologyIds()
{
  return this->ontologyIds;
}

void Entity::setSuperEngine(bool value)
{
  this->superEngine = value;

  if (value)
  {
    this->iceIdentity = true;
    this->available = true;
  }
}

// -----------------------------------------------------------------------------------
// ----------------------------------- Sharing Stuff ---------------------------------
// -----------------------------------------------------------------------------------

SharedSubModel& Entity::getSendSubModel()
{
  return this->sendSubModel;
}

SharedSubModel& Entity::getReceivedSubModel()
{
  return this->receivedSubModel;
}

std::set<std::shared_ptr<Node>>& Entity::getNodes()
{
  return this->nodes;
}

std::vector<std::pair<std::string,std::string>>& Entity::getOntologyIriDiff()
{
  return this->ontologyIriDiff;
}

void Entity::updateReceived(std::vector<std::shared_ptr<Node>> &nodes,
                                  std::vector<std::shared_ptr<BaseInformationStream>> &streamsSend,
                                  std::vector<std::shared_ptr<BaseInformationStream>> &streamsReceived,
                                  std::vector<std::shared_ptr<BaseInformationSet>> &setsSend,
                                  std::vector<std::shared_ptr<BaseInformationSet>> &setsReceived)
 {
   this->updateContainer(this->receivedSubModel, nodes, streamsSend, streamsReceived, setsSend, setsReceived);
 }

 void Entity::updateSend(std::vector<std::shared_ptr<Node>> &nodes,
                                   std::vector<std::shared_ptr<BaseInformationStream>> &streamsSend,
                                   std::vector<std::shared_ptr<BaseInformationStream>> &streamsReceived,
                                   std::vector<std::shared_ptr<BaseInformationSet>> &setsSend,
                                   std::vector<std::shared_ptr<BaseInformationSet>> &setsReceived)
 {
   this->updateContainer(this->sendSubModel, nodes, streamsSend, streamsReceived, setsSend, setsReceived);
 }

 void Entity::clearReceived()
 {
   this->clearContainer(this->receivedSubModel);
 }

 void Entity::clearSend()
 {
   this->clearContainer(this->sendSubModel);
 }

 void Entity::toShortIris(std::string &str)
 {
   for (int i = 0; i < this->ontologyIds.size(); ++i)
    {
     auto &iri = this->ontologyIds[i].first;
      size_t start_pos = 0;
      std::string from = iri + "#";
      std::string to = "o" + std::to_string(i) + "_";
      while((start_pos = str.find(from, start_pos)) != std::string::npos)
      {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
      }
    }
 }
 void Entity::toLongIris(std::string &str)
 {
   for (int i = 0; i < this->ontologyIds.size(); ++i)
   {
    auto &iri = this->ontologyIds[i].first;
     size_t start_pos = 0;
     std::string to = iri + "#";
     std::string from = "o" + std::to_string(i) + "_";
     while((start_pos = str.find(from, start_pos)) != std::string::npos)
     {
       str.replace(start_pos, from.length(), to);
       start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
     }
   }
 }

 void Entity::updateContainer(SharedSubModel &container,
                                   std::vector<std::shared_ptr<Node>> &nodes,
                                   std::vector<std::shared_ptr<BaseInformationStream>> &streamsSend,
                                   std::vector<std::shared_ptr<BaseInformationStream>> &streamsReceived,
                                   std::vector<std::shared_ptr<BaseInformationSet>> &setsSend,
                                   std::vector<std::shared_ptr<BaseInformationSet>> &setsReceived)
 {
   auto ths = this->shared_from_this();
   auto engine = this->engine.lock();
   auto communication = engine->getCommunicationInterface();

   // update nodes
   for (auto node : this->nodes)
   {
     if (std::find(nodes.begin(), nodes.end(), node) == nodes.end())
     {
       node->unregisterEntity(ths);
     }
   }

   for (auto node : nodes)
   {
     node->registerEntity(ths);
   }

   // update streams send
   for (int i = 0; i < container.streamsSend.size(); ++i)
   {
     auto stream = container.streamsSend[i];
     if (std::find(streamsSend.begin(), streamsSend.end(), stream) == streamsSend.end())
     {
       container.streamsSend.erase(container.streamsSend.begin() + i);
       --i;
       stream->unregisterRemoteListener(ths);
     }
   }

   for (auto stream : streamsSend)
   {
     if (stream->registerRemoteListener(ths, communication) == 0)
     {
       container.streamsSend.push_back(stream);
     }
   }

   // update streams received
   for (int i = 0; i < container.streamsReceived.size(); ++i)
   {
     auto stream = container.streamsReceived[i];
     if (std::find(streamsReceived.begin(), streamsReceived.end(), stream) == streamsReceived.end())
     {
       container.streamsReceived.erase(container.streamsReceived.begin() + i);
       --i;
       stream->setRemoteSource(nullptr, communication);
     }
   }

   for (auto stream : streamsReceived)
   {
     if (stream->setRemoteSource(ths, communication) == 0)
     {
       container.streamsReceived.push_back(stream);
     }
   }

   // update sets send
   for (int i = 0; i < container.setsSend.size(); ++i)
   {
     auto set = container.setsSend[i];
     if (std::find(setsSend.begin(), setsSend.end(), set) == setsSend.end())
     {
       container.setsSend.erase(container.setsSend.begin() + i);
       --i;
       set->unregisterRemoteListener(ths);
     }
   }

   for (auto set : setsSend)
   {
     if (set->registerRemoteListener(ths, communication) == 0)
     {
       container.setsSend.push_back(set);
     }
   }

   // update sets received
   for (int i = 0; i < container.setsReceived.size(); ++i)
   {
     auto set = container.setsReceived[i];
     if (std::find(setsReceived.begin(), setsReceived.end(), set) == setsReceived.end())
     {
       container.setsReceived.erase(container.setsReceived.begin() + i);
       --i;
       set->setRemoteSource(nullptr, communication);
     }
   }

   for (auto set : setsReceived)
   {
     if (set->setRemoteSource(ths, communication) == 0)
     {
       container.setsReceived.push_back(set);
     }
   }
 }

 void Entity::clearContainer(SharedSubModel &container)
 {
   auto ths = this->shared_from_this();
   auto engine = this->engine.lock();
   auto communication = engine->getCommunicationInterface();

   // clear sub model
   container.subModel.reset();

   // clear nodes
   for (auto node : this->nodes)
   {
     node->unregisterEntity(ths);
   }

   // clear streams send
   for (int i = 0; i < container.streamsSend.size(); ++i)
   {
     auto stream = container.streamsSend[i];

     stream->unregisterRemoteListener(ths);
   }
   container.streamsSend.clear();

   // clear streams received
   for (int i = 0; i < container.streamsReceived.size(); ++i)
   {
     auto stream = container.streamsReceived[i];

     stream->setRemoteSource(nullptr, communication);
   }
   container.streamsReceived.clear();
 }

// -----------------------------------------------------------------------------------
// ----------------------------------- ASP Stuff -------------------------------------
// -----------------------------------------------------------------------------------

bool Entity::isNodesExtracted()
{
  return this->nodesExtracted;
}

void Entity::setNodesExtracted(bool value)
{
  this->nodesExtracted = value;
}

std::shared_ptr<ASPElement> Entity::getASPElementByName(ASPElementType type, std::string const name)
{
  switch (type)
  {
    case ASP_COMPUTATION_NODE:
      for (auto node : this->aspNodes)
      {
        if (node->name == name)
          return node;
      }
      break;
    case ASP_SOURCE_NODE:
      for (auto node : this->aspSourceNodes)
      {
        if (node->name == name)
          return node;
      }
      break;
    case ASP_SET_NODE:
      for (auto node : this->aspSetNodes)
      {
        if (node->name == name)
          return node;
      }
      break;
    case ASP_TRANSFORMATION_NODE:
      for (auto node : this->aspTransformation)
      {
        if (node->name == name)
          return node;
      }
      break;
    case ASP_REQUIRED_STREAM:
      for (auto node : this->aspRequiredStreams)
      {
        if (node->name == name)
          return node;
      }
      break;
    case ASP_REQUIRED_SET:
      for (auto node : this->aspRequiredSets)
      {
        if (node->name == name)
          return node;
      }
      break;
  }

  return nullptr;
}

std::shared_ptr<ASPElement> Entity::getASPElementByName(std::string const name)
{
  for (auto node : this->aspNodes)
  {
    if (node->name == name)
      return node;
  }

  for (auto node : this->aspSourceNodes)
  {
    if (node->name == name)
      return node;
  }

  for (auto node : this->aspSetNodes)
  {
    if (node->name == name)
      return node;
  }

  for (auto node : this->aspTransformation)
  {
    if (node->name == name)
      return node;
  }

  for (auto node : this->aspRequiredStreams)
  {
    if (node->name == name)
      return node;
  }

  for (auto node : this->aspRequiredSets)
  {
    if (node->name == name)
      return node;
  }

  return nullptr;
}

void Entity::addASPElement(std::shared_ptr<ASPElement> node)
{
  switch (node->type)
  {
    case ASP_COMPUTATION_NODE:
      this->aspNodes.push_back(node);
      break;
    case ASP_SOURCE_NODE:
      this->aspSourceNodes.push_back(node);
      break;
    case ASP_SET_NODE:
      this->aspSetNodes.push_back(node);
      break;
    case ASP_TRANSFORMATION_NODE:
      this->aspTransformation.push_back(node);
      break;
    case ASP_REQUIRED_STREAM:
      this->aspRequiredStreams.push_back(node);
      break;
    case ASP_REQUIRED_SET:
      this->aspRequiredSets.push_back(node);
      break;
  }
}

void Entity::setExternal(std::shared_ptr<supplementary::External> &external)
{
  this->external = external;
}

std::shared_ptr<supplementary::External>& Entity::getExternal()
{
  return this->external;
}

bool Entity::updateExternals(bool activateRequired, bool activateRequirements)
{
  bool active = activateRequired || this->isCooperationPossible();

  if (this->external == nullptr)
    return false;

  this->external->assign(active);

  for (auto element : this->aspTransformation)
  {
    element->external->assign((element->defect == false && active));
  }

  for (auto element : this->aspNodes)
  {
    element->external->assign((element->defect == false && active));
  }

  for (auto element : this->aspSourceNodes)
  {
    element->external->assign((element->defect == false && active));
  }

  for (auto element : this->aspSetNodes)
  {
    element->external->assign((element->defect == false && active));
  }

  for (auto element : this->aspRequiredStreams)
  {
    element->external->assign(activateRequired && (element->defect == false && active && activateRequired));
  }

  for (auto element : this->aspRequiredSets)
  {
    element->external->assign(activateRequired && (element->defect == false && active));
  }

  return active;
}

int Entity::getNodeForClass(std::string className, std::vector<std::shared_ptr<ASPElement>> &result)
{
  int count = 0;

  for (auto element : this->aspTransformation)
  {
    if (element->className == className)
    {
      result.push_back(element);
      ++count;
    }
  }

  for (auto element : this->aspNodes)
  {
    if (element->className == className)
    {
      result.push_back(element);
      ++count;
    }
  }

  for (auto element : this->aspSourceNodes)
  {
    if (element->className == className)
    {
      result.push_back(element);
      ++count;
    }
  }

  for (auto element : this->aspSetNodes)
  {
    if (element->className == className)
    {
      result.push_back(element);
      ++count;
    }
  }

  return count;
}

} /* namespace ice */
