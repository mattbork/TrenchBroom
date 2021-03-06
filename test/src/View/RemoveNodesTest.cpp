/*
 Copyright (C) 2010-2017 Kristian Duske
 
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

#include <stdio.h>

#include <gtest/gtest.h>

#include "Model/Brush.h"
#include "Model/BrushBuilder.h"
#include "Model/Entity.h"
#include "Model/Group.h"
#include "Model/Layer.h"
#include "Model/World.h"
#include "View/MapDocumentTest.h"
#include "View/MapDocument.h"

namespace TrenchBroom {
    namespace View {
        class RemoveNodesTest : public MapDocumentTest {};

        TEST_F(RemoveNodesTest, removeLayer) {
            Model::Layer* layer = new Model::Layer("Layer 1", document->worldBounds());
            document->addNode(layer, document->world());

            document->removeNode(layer);
            ASSERT_TRUE(layer->parent() == nullptr);

            document->undoLastCommand();
            ASSERT_EQ(document->world(), layer->parent());
        }

        TEST_F(RemoveNodesTest, removeEmptyBrushEntity) {
            Model::Layer* layer = new Model::Layer("Layer 1", document->worldBounds());
            document->addNode(layer, document->world());
            
            Model::Entity* entity = new Model::Entity();
            document->addNode(entity, layer);
            
            Model::Brush* brush = createBrush();
            document->addNode(brush, entity);
            
            document->removeNode(brush);
            ASSERT_TRUE(brush->parent() == nullptr);
            ASSERT_TRUE(entity->parent() == nullptr);
            
            document->undoLastCommand();
            ASSERT_EQ(entity, brush->parent());
            ASSERT_EQ(layer, entity->parent());
        }

        TEST_F(RemoveNodesTest, removeEmptyGroup) {
            Model::Group* group = new Model::Group("group");
            document->addNode(group, document->currentParent());
            
            document->openGroup(group);
            
            Model::Brush* brush = createBrush();
            document->addNode(brush, document->currentParent());
            
            document->removeNode(brush);
            ASSERT_TRUE(document->currentGroup() == nullptr);
            ASSERT_TRUE(brush->parent() == nullptr);
            ASSERT_TRUE(group->parent() == nullptr);
            
            document->undoLastCommand();
            ASSERT_EQ(group, document->currentGroup());
            ASSERT_EQ(group, brush->parent());
            ASSERT_EQ(document->world()->defaultLayer(), group->parent());
        }
        
        TEST_F(RemoveNodesTest, recursivelyRemoveEmptyGroups) {
            Model::Group* outer = new Model::Group("outer");
            document->addNode(outer, document->currentParent());
            
            document->openGroup(outer);
            
            Model::Group* inner = new Model::Group("inner");
            document->addNode(inner, document->currentParent());
            
            document->openGroup(inner);
            
            Model::Brush* brush = createBrush();
            document->addNode(brush, document->currentParent());
            
            document->removeNode(brush);
            ASSERT_TRUE(document->currentGroup() == nullptr);
            ASSERT_TRUE(brush->parent() == nullptr);
            ASSERT_TRUE(inner->parent() == nullptr);
            ASSERT_TRUE(outer->parent() == nullptr);
            
            document->undoLastCommand();
            ASSERT_EQ(inner, document->currentGroup());
            ASSERT_EQ(inner, brush->parent());
            ASSERT_EQ(outer, inner->parent());
            ASSERT_EQ(document->world()->defaultLayer(), outer->parent());
        }
    }
}
