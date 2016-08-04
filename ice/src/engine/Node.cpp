/*
 * Node.cpp
 *
 *  Created on: Jun 2, 2014
 *      Author: sni
 */

#include "ice/processing/Node.h"

#include "ice/Entity.h"
#include "ice/information/BaseInformationStream.h"

#include <sstream>
#include <set>

namespace ice
{
// static part
std::map<std::string, Node::creatorFunc> Node::creators;

int Node::registerNodeCreator(const std::string& className, const creatorFunc& creator)
{
  if (Node::creators.find(className) != Node::creators.end())
    return 1;

  auto p = std::pair<std::string, const creatorFunc&>(className, creator);

  Node::creators.insert(p);

  return 0;
}

std::shared_ptr<Node> Node::createNode(const std::string& className)
{
  std::shared_ptr<Node> node;

  if (Node::creators.find(className) == Node::creators.end())
    return node;

  return (*Node::creators[className])();
}

bool Node::existNodeCreator(const std::string &className)
{
  return Node::creators.find(className) != Node::creators.end();
}

// object part
Node::Node()
{
  this->eventHandler = eventHandler;
  this->active = false;
  this->cyclicTriggerTime = cyclicTriggerTime;
}

Node::~Node()
{
  //
}

std::shared_ptr<NodeDescription>& Node::getNodeDescription()
{
  return this->nodeDescription;
}

void Node::setNodeDescription(std::shared_ptr<NodeDescription> &desc)
{
  this->nodeDescription = desc;
}

int Node::performTask()
{
  // Node is not active
  if (false == this->active)
    return 1;

  // Node is not valid
  if (false == this->isValid())
    return 1;

  // No cyclic execution
  if (this->cyclicTriggerTime < 0)
  {
    return this->performNode();
    return 0;
  }

  // Cyclic execution
  timeval start, stop;
  gettimeofday(&start, NULL);

  return this->performNode();

  gettimeofday(&stop, NULL);

  long diff = (stop.tv_usec - start.tv_usec) / 1000;
  long next = this->cyclicTriggerTime - diff;

  this->eventHandler->addTimerTask(this->shared_from_this(), next);

  return 0;
}

int Node::addInput(std::shared_ptr<BaseInformationStream> stream, bool trigger)
{
  std::lock_guard<std::mutex> guard(this->mtx_);

  for (auto streamItr : this->inputs)
  {
    if (streamItr == stream)
      return 1;
  }

  this->inputs.push_back(stream);

  if (trigger)
  {
    this->triggeredByInputs.push_back(stream);
    stream->registerTaskAsync(this->shared_from_this());
  }

//  if (base)
//  {
//    this->baseInputs.push_back(stream);
//  }

  return 0;
}

int Node::removeInput(std::shared_ptr<BaseInformationStream> stream)
{
  std::lock_guard<std::mutex> guard(this->mtx_);
  int returnVel = 1;

  for (int i = 0; i < this->inputs.size(); ++i)
  {
    auto streamItr = this->inputs[i];
    if (streamItr == stream)
    {
      this->inputs.erase(this->inputs.begin() + i);
      returnVel = 0;
    }
  }

  if (returnVel != 0)
    return returnVel;

//  for (int i = 0; i < this->baseInputs.size(); ++i)
//  {
//    auto streamItr = this->baseInputs[i];
//    if (streamItr == stream)
//    {
//      this->baseInputs.erase(this->baseInputs.begin() + i);
//    }
//  }

  for (int i = 0; i < this->triggeredByInputs.size(); ++i)
  {
    auto streamItr = this->triggeredByInputs[i];
    if (streamItr == stream)
    {
      this->triggeredByInputs.erase(this->triggeredByInputs.begin() + i);
    }
  }

  return returnVel;
}

int Node::addOutput(std::shared_ptr<BaseInformationStream> stream)
{
  std::lock_guard<std::mutex> guard(this->mtx_);

  for (auto streamItr : this->outputs)
  {
    if (streamItr == stream)
      return 1;
  }

  this->outputs.push_back(stream);

  return 0;
}

int Node::removeOutput(std::shared_ptr<BaseInformationStream> stream)
{
  std::lock_guard<std::mutex> guard(this->mtx_);

  for (int i = 0; i < this->outputs.size(); ++i)
  {
    auto streamItr = this->outputs[i];
    if (streamItr == stream)
    {
      this->outputs.erase(this->outputs.begin() + i);
      return 0;
    }
  }

  return 1;
}

int Node::init()
{
  //
  return 0;
}

int Node::cleanUp()
{
  //
  return 0;
}

int Node::destroy()
{
  this->inputs.clear();
  this->outputs.clear();

  for (auto stream : this->triggeredByInputs)
  {
    stream->registerTaskAsync(this->shared_from_this());
  }

  this->triggeredByInputs.clear();

  return 0;
}

bool Node::isActive() const
{
  return this->active;
}

void Node::activate()
{
  if (this->active)
    return;

  this->active = true;
  this->init();
}

void Node::deactivate()
{
  if (false == this->active)
    return;

  this->active = false;
  this->cleanUp();
}

bool Node::isValid()
{
  return true;
}

long Node::getCyclicTriggerTime() const
{
  return this->cyclicTriggerTime;
}

void Node::setCyclicTriggerTime(long cyclicTriggerTime)
{
  this->cyclicTriggerTime = cyclicTriggerTime;
}

std::shared_ptr<EventHandler> Node::getEventHandler() const
{
  return this->eventHandler;
}

void Node::setEventHandler(std::shared_ptr<EventHandler> eventHandler)
{
  this->eventHandler = eventHandler;
}

std::map<std::string, std::string> Node::getConfiguration() const
{
  return this->configuration;
}

void Node::setConfiguration(std::map<std::string, std::string> configuration)
{
  this->configuration = configuration;
}

const std::vector<std::shared_ptr<BaseInformationStream>>* Node::getInputs() const
{
  return &this->inputs;
}

const std::vector<std::shared_ptr<BaseInformationStream>>* Node::getTriggeredByInputs() const
{
  return &this->triggeredByInputs;
}

const std::vector<std::shared_ptr<BaseInformationStream>>* Node::getOutputs() const
{
  return &this->outputs;
}

std::string Node::toString()
{
  std::stringstream ss;

  ss << "node(" << this->nodeDescription->toString() << "," << this->active << ")";

  return ss.str();
}

void Node::registerEntity(std::shared_ptr<Entity> &entity)
{
  this->registeredEngines.insert(entity);
  entity->getNodes().insert(this->shared_from_this());
}

void Node::unregisterEntity(std::shared_ptr<Entity> &entity)
{
  this->registeredEngines.erase(entity);
  entity->getNodes().erase(this->shared_from_this());
}

int Node::getRegisteredEngineCount()
{
  return this->registeredEngines.size();
}

} /* namespace ice */
