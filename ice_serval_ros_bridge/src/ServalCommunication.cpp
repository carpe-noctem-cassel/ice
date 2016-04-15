/*
 * ServalCommunication.cpp
 *
 *  Created on: Apr 15, 2016
 *      Author: sni
 */

#include "ServalCommunication.h"

#include <chrono>
#include <serval_interface.h>

namespace ice
{

ServalCommunication::ServalCommunication(IdentityDirectory const &directory, std::string configPath, std::string const host,
                                         int const port, std::string const authName, std::string const authPass) :
    CommunicationInterface(), configPath(configPath), host(host),
    port(port), authName(authName), authPass(authPass), serval(nullptr), running(false)
{
  this->directory = directory;
}

ServalCommunication::~ServalCommunication()
{
  if (this->serval != nullptr)
    delete this->serval;
}

void ServalCommunication::init()
{
  // create interface
  this->serval = new serval_interface(this->configPath, this->host, this->port, this->authName, this->authPass);

  // get own id
  auto id = this->serval->keyring.getSelf();
  if (id->size() == 0)
  {
    // error case, own id could not be determined
    throw (std::runtime_error("Own serval id could not be determined"));
  }

  this->self = this->directory.lookup(IdentityDirectory::ID_SERVAL, id->at(0).sid);
  this->running = true;
  this->worker = std::thread(&ServalCommunication::checkServal, this);
}

void ServalCommunication::cleanUp()
{
  this->running = false;
  this->worker.join();

  if (this->serval != nullptr)
  {
    delete this->serval;
    this->serval = nullptr;
  }
}

bool ServalCommunication::requestId(std::shared_ptr<Identity> const &identity, std::string const &id)
{
  // TODO
}

bool ServalCommunication::requestIds(std::shared_ptr<Identity> const &identity)
{
  // TODO
}

void ServalCommunication::checkServal()
{
  int counter = 0;

  while (this->running)
  {
    // check all 10 iterations for new peers
    if (counter >= 9)
    {
      auto sids = this->serval->keyring.getPeerIdentities();

      // TODO update timestamp
      for (auto &sid : *sids)
      {
        auto identity = this->directory.lookup(IdentityDirectory::ID_SERVAL, sid.sid);

        if (identity == nullptr)
        {
          // Create new instance and request ids
          identity = this->directory.create(IdentityDirectory::ID_SERVAL, sid.sid);
          this->requestIds(identity);
        }
      }
      counter = 0;
    }

    // check for new messages
    // TODO

    // send messages
    // TODO

    ++counter;
    // sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

} /* namespace ice */
