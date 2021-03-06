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

#ifndef TrenchBroom_BrushContentTypeEvaluator
#define TrenchBroom_BrushContentTypeEvaluator

#include "Model/ModelTypes.h"

#include <memory>

namespace TrenchBroom {
    namespace Model {
        class Brush;
        
        class BrushContentTypeEvaluator {
        public:
            virtual ~BrushContentTypeEvaluator();
            
            static std::unique_ptr<BrushContentTypeEvaluator> textureNameEvaluator(const String& pattern);
            static std::unique_ptr<BrushContentTypeEvaluator> shaderSurfaceParmsEvaluator(const String& pattern);
            static std::unique_ptr<BrushContentTypeEvaluator> contentFlagsEvaluator(int value);
            static std::unique_ptr<BrushContentTypeEvaluator> surfaceFlagsEvaluator(int value);
            static std::unique_ptr<BrushContentTypeEvaluator> entityClassnameEvaluator(const String& pattern);
            
            bool evaluate(const Brush* brush) const;
        private:
            virtual bool doEvaluate(const Brush* brush) const = 0;
        };
    }
}

#endif /* defined(TrenchBroom_BrushContentTypeEvaluator) */
