#pragma once

// don't add entites immediately. create a temp var then add to toadd vector

#include "Entity.h"
#include <vector>
#include <map>

typedef std::vector<std::shared_ptr<Entity>>          EntityVector;


class EntityManager {
    EntityVector                          m_entities;
    std::map<std::string, EntityVector>   m_entityMap;      // map contains the entities grouped by tags
    EntityVector                          m_entitiesToAdd;            // wait line to render next frame
    size_t                                m_totalEntities = 0;
    
    void removeDeadEntities(EntityVector& vec) {
        auto itr = std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity>& e) {
            bool v = e->isActive();
            if (v) {
                return false;
                
            }
            return true;
            });

        vec.erase(itr, vec.end());

    }

public:
    EntityManager() {
    }
    void init() {

    }
    void update() {
        // will add the waiting room entites (toAdd) to the entities list
        // add to m_entites and m_entityMap
        // m_entityMap[e->tag()].push_back(e)
            // will create a new field if tag doesn't exist 
        // clear toAdd
        // removeDeadEntities(m_entities)
            // status active
        // remove from map
            // for(auto& [tag, entityVel] : m_entityMap)
                // remove dead entity
        for (auto& e : m_entitiesToAdd) {
            m_entities.push_back(e);
            m_entityMap[e->tag()].push_back(e);
        }
    
        removeDeadEntities(m_entities);
    
        for (auto& em : m_entityMap) {
            removeDeadEntities(em.second);
        }
        m_entitiesToAdd.clear();
    }
    std::shared_ptr<Entity> addEntity(std::string tag) {
        // only way of adding an entity  // delayed
 
        auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
        
        m_entitiesToAdd.push_back(entity);
        return entity;
    }
    EntityVector getEntities() {
        return m_entities;
    }
    EntityVector getEntities(std::string tag) {
        return m_entityMap[tag];
    }
};


// factor class producing and handling entity creation and deletion
//class EntityManager1 {
//    EntityVector                          m_entities;
//    std::map<std::string, EntityVector>   m_entityMap;      // map contains the entities grouped by tags
//    EntityVector                          m_entitiesToAdd;            // wait line to render next frame
//    int                                   m_totalEntities = 1;
//
//    // not the prefered way, but used if entity has a private constructor 
//public:
//    void init();
//    std::shared_ptr<Entity> addEntity(std::string tag) {
//        // only way of adding an entity  // delayed
//        auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
//
//        m_entitiesToAdd.push_back(entity);
//        return entity;
//    }
//    EntityVector getEntities() {
//        return m_entities;
//    }
//    EntityVector getEntities(std::string tag) {
//        return m_entityMap[tag];
//    }
//    EntityVector testReturnEnt() {
//        return m_entities;
//    }
// 
//    void removeDeadEntities(EntityVector& vec) {
//        auto itr = std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity>& e) {
//            bool v = e->isActive();
//            if (v) {
//                return false;
//
//            }
//            return true;
//            });
//
//        vec.erase(itr, vec.end());
//
//    }
//
//    void update() {
//        // will add the waiting room entites (toAdd) to the entities list
//        // add to m_entites and m_entityMap
//        // m_entityMap[e->tag()].push_back(e)
//            // will create a new field if tag doesn't exist 
//        // clear toAdd
//        // removeDeadEntities(m_entities)
//            // status active
//        // remove from map
//            // for(auto& [tag, entityVel] : m_entityMap)
//                // remove dead entity
//
//        for (auto& e : m_entitiesToAdd) {
//            m_entities.push_back(e);
//            m_entityMap[e->tag()].push_back(e);
//        }
//
//        removeDeadEntities(m_entities);
//
//        for (auto& em : m_entityMap) {
//            removeDeadEntities(em.second);
//        }
//        m_entitiesToAdd.clear();
//    }
//};