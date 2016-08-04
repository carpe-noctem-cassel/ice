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

#include "ice/model/ProcessingModel.h"
#include "ice/ontology/OntologyInterface.h"
#include "ice/processing/Node.h"
#include "ice/EntityDirectory.h"

namespace ice
{

Entity::Entity(std::shared_ptr<EntityDirectory> const &directory, std::weak_ptr<ICEngine> engine,
		  std::shared_ptr<TimeFactory> const &factory, const std::initializer_list<Id>& ids)
      : iceIdentity(false), directory(directory), timeFactory(factory), available(false), _log(
				el::Loggers::getLogger("Entity")), index(0), timeoutDuration(2000)
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

  auto nodes = ontologyInterface->readNodesAndIROsAsASP(ownIri);

  auto &types = nodes->at(0);
  auto &names = nodes->at(1);
  auto &strings = nodes->at(2);
  auto &aspStrings = nodes->at(3);
  auto &cppStrings = nodes->at(4);

  int count = 0;

  for (int i = 0; i < names.size(); ++i)
  {
    std::string &name = names.at(i);
    std::string &elementStr = strings.at(i);
    std::string &aspStr = aspStrings.at(i);
    std::string &cppStr = cppStrings.at(i);
    std::string &typeStr = types.at(i);
    ASPElementType type;

    if (typeStr == "" || name == "" || elementStr == "")
    {
      _log->error("Empty string for element '%v': '%v' (elementStr), '%v' (typeStr), element will be skipped",
                  name, elementStr, typeStr);
      continue;
    }

    if (typeStr == "COMPUTATION_NODE")
    {
      type = ASPElementType::ASP_COMPUTATION_NODE;
    }
    else if (typeStr == "SOURCE_NODE")
    {
      type = ASPElementType::ASP_SOURCE_NODE;
    }
    else if (typeStr == "REQUIRED_STREAM")
    {
      type = ASPElementType::ASP_REQUIRED_STREAM;
    }
    else if (typeStr == "MAP_NODE")
    {
      type = ASPElementType::ASP_MAP_NODE;
    }
    else if (typeStr == "IRO_NODE")
    {
      type = ASPElementType::ASP_IRO_NODE;
    }
    else if (typeStr == "REQUIRED_MAP")
    {
      type = ASPElementType::ASP_REQUIRED_MAP;
    }
    else
    {
      _log->error("Unknown asp element type '%v' for element '%v', element will be skipped", typeStr, name);
      continue;
    }

    auto node = this->getASPElementByName(type, name);

    if (!node)
    {
      _log->info("ASP element '%v' not found, creating new element", name);
      auto element = std::make_shared<ASPElement>();
      element->aspString = aspStr;
      element->name = name;
      element->state = ASPElementState::ADDED_TO_ASP;
      element->type = type;

      if (cppStr != "")
      {
        int index = cppStr.find('\n');
        element->className = cppStr.substr(0, index);
        element->configAsString = cppStr.substr(index + 1, cppStr.length() - index - 1);
        element->config = this->readConfiguration(element->configAsString);
      }

      element->raw = elementStr;

      // ASP external stuff removed
//      auto value = supplementary::ClingWrapper::stringToValue();
//      element->external = this->asp->getExternal(*value.name(), value.args());
//
//      switch (type)
//      {
//        case ASPElementType::ASP_COMPUTATION_NODE:
//        case ASPElementType::ASP_SOURCE_NODE:
//        case ASPElementType::ASP_MAP_NODE:
//        case ASPElementType::ASP_IRO_NODE:
//          if (false == this->nodeStore->existNodeCreator(element->className))
//          {
//            _log->warn("Missing creator for node '%v' of type '%v', cpp grounding '%v', asp external set to false",
//                       element->name, ASPElementTypeNames[type],
//                       element->className == "" ? "NULL" : element->className);
//            element->external->assign(false);
//          }
//          else
//          {
//            element->external->assign(true);
//          }
//          break;
//        default:
//          element->external->assign(true);
//          break;
//      }
//
//      this->asp->add(name, {}, aspStr);
//      this->asp->ground(name, {});
//
//      this->addASPElement(element);
//      this->groundingDirty = true;
      ++count;
    }
  }

  return count;
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

void Entity::fuse(std::shared_ptr<Entity> &identity)
{
  for (auto &id : identity->ids)
  {
    this->ids[id.first] = id.second;
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

bool Entity::isActiveCooperation()
{
  return this->isTimeout() == false && this->iceIdentity && this->available
      && (this->sendSubModel != nullptr || this->receivedSubModel != nullptr);
}

bool Entity::isTimeout()
{
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
                                  std::vector<std::shared_ptr<BaseInformationStream>> &streamsReceived)
 {
   this->updateContainer(this->receivedSubModel, nodes, streamsSend, streamsReceived);
 }

 void Entity::updateSend(std::vector<std::shared_ptr<Node>> &nodes,
                                   std::vector<std::shared_ptr<BaseInformationStream>> &streamsSend,
                                   std::vector<std::shared_ptr<BaseInformationStream>> &streamsReceived)
 {
   this->updateContainer(this->sendSubModel, nodes, streamsSend, streamsReceived);
 }

 void Entity::clearReceived()
 {
   this->clearContainer(this->receivedSubModel);
 }

 void Entity::clearSend()
 {
   this->clearContainer(this->sendSubModel);
 }

 void Entity::updateContainer(SharedSubModel &container,
                                   std::vector<std::shared_ptr<Node>> &nodes,
                                   std::vector<std::shared_ptr<BaseInformationStream>> &streamsSend,
                                   std::vector<std::shared_ptr<BaseInformationStream>> &streamsReceived)
 {
   auto ths = this->shared_from_this();
   auto engine = this->engine.lock();
   auto communication = engine->getCommunication();

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
 }

 void Entity::clearContainer(SharedSubModel &container)
 {
   auto ths = this->shared_from_this();
   auto engine = this->engine.lock();
   auto communication = engine->getCommunication();

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
    case ASP_IRO_NODE:
      for (auto node : this->aspIro)
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
    case ASP_REQUIRED_MAP:
      for (auto node : this->aspRequiredMaps)
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

  for (auto node : this->aspIro)
  {
    if (node->name == name)
      return node;
  }

  for (auto node : this->aspRequiredStreams)
  {
    if (node->name == name)
      return node;
  }

  for (auto node : this->aspRequiredMaps)
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
    case ASP_IRO_NODE:
      this->aspIro.push_back(node);
      break;
    case ASP_REQUIRED_STREAM:
      this->aspRequiredStreams.push_back(node);
      break;
    case ASP_REQUIRED_MAP:
      this->aspRequiredMaps.push_back(node);
      break;
  }
}


} /* namespace ice */
