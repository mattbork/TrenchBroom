/*
 Copyright (C) 2010-2014 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ModelUtils.h"

#include "Assets/EntityDefinition.h"
#include "Assets/PropertyDefinition.h"
#include "Model/Map.h"
#include "Model/Entity.h"
#include "Model/BrushFace.h"
#include "Model/Brush.h"

namespace TrenchBroom {
    namespace Model {
        const Assets::PropertyDefinition* safeGetPropertyDefinition(const Model::PropertyKey& key, const Model::Entity* entity);

        Assets::EntityDefinition* selectEntityDefinition(const Model::EntityList& entities) {
            Assets::EntityDefinition* definition = NULL;
            
            Model::EntityList::const_iterator entityIt, entityEnd;
            for (entityIt = entities.begin(), entityEnd = entities.end(); entityIt != entityEnd; ++entityIt) {
                Model::Entity* entity = *entityIt;
                if (definition == NULL) {
                    definition = entity->definition();
                } else if (definition != entity->definition()) {
                    definition = NULL;
                    break;
                }
            }
            
            return definition;
        }

        const Assets::PropertyDefinition* safeGetPropertyDefinition(const Model::PropertyKey& key, const Model::Entity* entity) {
            const Assets::EntityDefinition* definition = entity->definition();
            if (definition == NULL)
                return NULL;
            
            const Assets::PropertyDefinition* propDefinition = definition->propertyDefinition(key);
            if (propDefinition == NULL)
                return NULL;

            return propDefinition;
        }
        
        const Assets::PropertyDefinition* selectPropertyDefinition(const Model::PropertyKey& key, const Model::EntityList& entities) {
            Model::EntityList::const_iterator it = entities.begin();
            Model::EntityList::const_iterator end = entities.end();
            if (it == end)
                return NULL;
            
            const Model::Entity* entity = *it;
            const Assets::PropertyDefinition* definition = safeGetPropertyDefinition(key, entity);
            if (definition == NULL)
                return NULL;
            
            while (++it != end) {
                entity = *it;
                const Assets::PropertyDefinition* currentDefinition = safeGetPropertyDefinition(key, entity);
                if (currentDefinition == NULL)
                    return NULL;
                
                if (!definition->equals(currentDefinition))
                    return NULL;
            }
            
            return definition;
        }

        Model::PropertyValue selectPropertyValue(const Model::PropertyKey& key, const Model::EntityList& entities) {
            Model::EntityList::const_iterator it = entities.begin();
            Model::EntityList::const_iterator end = entities.end();
            if (it == end)
                return "";
            
            const Model::Entity* entity = *it;
            if (!entity->hasProperty(key))
                return "";
            
            const Model::PropertyValue& value = entity->property(key);
            while (++it != end) {
                entity = *it;
                if (!entity->hasProperty(key))
                    return "";
                if (value != entity->property(key))
                    return "";
            }
            return value;
        }

        Brush* createBrushFromBounds(const Map& map, const BBox3& worldBounds, const BBox3& brushBounds, const String& textureName) {
            const Vec3 size = brushBounds.size();
            const Vec3 x = Vec3(size.x(), 0.0, 0.0);
            const Vec3 y = Vec3(0.0, size.y(), 0.0);
            const Vec3 z = Vec3(0.0, 0.0, size.z());
            
            // east, west, front, back, top, bottom
            BrushFaceList faces(6);
            faces[0] = map.createFace(brushBounds.min, brushBounds.min + y, brushBounds.min + z, textureName);
            faces[1] = map.createFace(brushBounds.max, brushBounds.max - z, brushBounds.max - y, textureName);
            faces[2] = map.createFace(brushBounds.min, brushBounds.min + z, brushBounds.min + x, textureName);
            faces[3] = map.createFace(brushBounds.max, brushBounds.max - x, brushBounds.max - z, textureName);
            faces[4] = map.createFace(brushBounds.max, brushBounds.max - y, brushBounds.max - x, textureName);
            faces[5] = map.createFace(brushBounds.min, brushBounds.min + x, brushBounds.min + y, textureName);
            
            return map.createBrush(worldBounds, faces);
        }

        BrushList mergeEntityBrushesMap(const EntityBrushesMap& map) {
            BrushList result;
            EntityBrushesMap::const_iterator it, end;
            for (it = map.begin(), end = map.end(); it != end; ++it) {
                const BrushList& entityBrushes = it->second;
                result.insert(result.end(), entityBrushes.begin(), entityBrushes.end());
            }
            return result;
        }

        ObjectParentList makeObjectParentList(const EntityBrushesMap& map) {
            Model::ObjectParentList result;
            
            Model::EntityBrushesMap::const_iterator eIt, eEnd;
            for (eIt = map.begin(), eEnd = map.end(); eIt != eEnd; ++eIt) {
                Model::Entity* entity = eIt->first;
                const Model::BrushList& brushes = eIt->second;
                
                Model::BrushList::const_iterator bIt, bEnd;
                for (bIt = brushes.begin(), bEnd = brushes.end(); bIt != bEnd; ++bIt) {
                    Model::Brush* brush = *bIt;
                    result.push_back(Model::ObjectParentPair(brush, entity));
                }
            }
            
            return result;
        }

        ObjectParentList makeObjectParentList(const ObjectList& list) {
            ObjectParentList result;
            result.reserve(list.size());
            
            ObjectList::const_iterator it, end;
            for (it = list.begin(), end = list.end(); it != end; ++it)
                result.push_back(ObjectParentPair(*it));
            return result;
        }

        ObjectParentList makeObjectParentList(const EntityList& list) {
            ObjectParentList result;
            result.reserve(list.size());
            
            EntityList::const_iterator it, end;
            for (it = list.begin(), end = list.end(); it != end; ++it)
                result.push_back(ObjectParentPair(*it));
            return result;
        }
        
        ObjectParentList makeObjectParentList(const BrushList& list) {
            ObjectParentList result;
            result.reserve(list.size());
            
            BrushList::const_iterator it, end;
            for (it = list.begin(), end = list.end(); it != end; ++it)
                result.push_back(ObjectParentPair(*it));
            return result;
        }

        ObjectParentList makeObjectParentList(const BrushList& list, Entity* parent) {
            ObjectParentList result;
            result.reserve(list.size());
            
            BrushList::const_iterator it, end;
            for (it = list.begin(), end = list.end(); it != end; ++it)
                result.push_back(ObjectParentPair(*it, parent));
            return result;
        }

        ObjectList makeObjectList(const ObjectParentList& list) {
            ObjectList result;
            result.reserve(list.size());
            
            ObjectParentList::const_iterator it, end;
            for (it = list.begin(), end = list.end(); it != end; ++it)
                result.push_back(it->object);
            return result;
        }

        bool MatchAll::operator()(const ObjectParentPair& pair) const {
            return true;
        }

        bool MatchAll::operator()(const Object* object) const {
            return true;
        }
        
        bool MatchAll::operator()(const Entity* entity) const {
            return true;
        }
        
        bool MatchAll::operator()(const Brush* brush) const {
            return true;
        }
        
        bool MatchAll::operator()(const BrushFace* face) const {
            return true;
        }

        bool MatchAll::operator()(const BrushEdge* edge) const {
            return true;
        }
        
        bool MatchAll::operator()(const BrushVertex* vertex) const {
            return true;
        }

        MatchObjectByType::MatchObjectByType(const Object::Type type) :
        m_type(type) {}
        
        bool MatchObjectByType::operator()(const Object* object) const {
            return object->type() == m_type;
        }

        MatchObjectByFilePosition::MatchObjectByFilePosition(const size_t position) :
        m_position(position) {}
        
        bool MatchObjectByFilePosition::operator()(const Object* object) const {
            return object->containsLine(m_position);
        }

        Transform::Transform(const Mat4x4& transformation, const bool lockTextures, const BBox3& worldBounds) :
        m_transformation(transformation),
        m_lockTextures(lockTextures),
        m_worldBounds(worldBounds) {}
        
        void Transform::operator()(Model::Object* object) const {
            object->transform(m_transformation, m_lockTextures, m_worldBounds);
        }
        
        void Transform::operator()(Model::BrushFace* face) const {
            face->transform(m_transformation, m_lockTextures);
        }

        CheckBounds::CheckBounds(const BBox3& bounds) :
        m_bounds(bounds) {}
        
        bool CheckBounds::operator()(const Model::Pickable* object) const {
            return m_bounds.contains(object->bounds());
        }

        NotifyParent::NotifyParent(Notifier1<Object*>& notifier) :
        m_notifier(notifier) {}
        
        void NotifyParent::operator()(const ObjectParentPair& pair) {
            if (pair.parent != NULL && m_notified.insert(pair.parent).second)
                m_notifier(pair.parent);
        }

        void NotifyParent::operator()(Object* object) {
            if (object->type() == Object::Type_Brush) {
                Model::Object* parent = static_cast<Brush*>(object)->parent();
                if (parent != NULL && m_notified.insert(parent).second)
                    m_notifier(parent);
            }
        }

        NotifyObject::NotifyObject(Notifier1<Object*>& notifier) :
        m_notifier(notifier) {}
        
        void NotifyObject::operator()(Object* object) {
            m_notifier(object);
        }
    }
}
