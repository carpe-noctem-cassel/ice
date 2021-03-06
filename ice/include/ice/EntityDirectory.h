/*
 * identities.h
 *
 *  Created on: Apr 11, 2016
 *      Author: sni
 */

#ifndef IDENTITIES_H_
#define IDENTITIES_H_

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "ice/processing/CallbackList.h"
#include "ice/Entity.h"

namespace ice
{

class OntologyInterface;
class TimeFactory;

typedef std::string serval_id;
typedef std::string ontology_iri;

class EntityDirectory : public std::enable_shared_from_this<EntityDirectory>
{
public:
  static const std::string ID_ICE;
  static const std::string ID_SERVAL;
  static const std::string ID_ONTOLOGY;
  static const std::string ID_ONTOLOGY_SHORT;

public:
  EntityDirectory(std::weak_ptr<ICEngine> const &engine);
  virtual ~EntityDirectory();

  void init();
  void cleanUp();

  int initializeFromOntology(std::shared_ptr<OntologyInterface> const &ontologyInterface);

  std::shared_ptr<Entity> lookup(std::string const &key, std::string const &value, bool create = false);
  std::shared_ptr<Entity> lookup(const std::initializer_list<Id>& ids, bool create = false);
  int fuse(std::shared_ptr<Entity> entity);

  std::shared_ptr<Entity> create(std::string const &key, std::string const &value);
  std::shared_ptr<Entity> create(const std::initializer_list<Id>& ids);

  std::unique_ptr<std::vector<std::shared_ptr<Entity>>> allEntities();
  std::unique_ptr<std::vector<std::shared_ptr<Entity>>> availableEntities();
  std::unique_ptr<std::vector<std::shared_ptr<Entity>>> activeCooperationEntities();
  void checkTimeout();

  int count();
  void print();

private:
  void removeEntity(std::shared_ptr<Entity> &entity);

public:
  std::weak_ptr<ICEngine> 			engine;
  std::shared_ptr<TimeFactory>			timeFactory;
  std::shared_ptr<Entity> 			self;
  CallbackList<std::shared_ptr<Entity>> 	disvoeredIceIdentity;
  CallbackList<std::shared_ptr<Entity>> 	vanishedIceIdentity;
  CallbackList<std::shared_ptr<Entity>> 	offeredInformation;

private:
  std::vector<std::shared_ptr<Entity>>          entities;
  std::mutex                                    _mtx;
};

} /* namespace ice */

#endif /* IDENTITIES_H_ */
