/*
 Copyright (C) 2010-2012 Kristian Duske
 
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
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TrenchBroom__BoxInfoRenderer__
#define __TrenchBroom__BoxInfoRenderer__

#include "Renderer/Text/TextRenderer.h"
#include "Utility/VecMath.h"

using namespace TrenchBroom::Math;

namespace TrenchBroom {
    namespace Renderer {
        namespace Text {
            class FontManager;
        }
        
        class Camera;
        class RenderContext;
        class Vbo;
        
        class BoxInfoSizeTextAnchor : public Text::TextAnchor {
        private:
            BBox m_bounds;
            Axis::Type m_axis;
            Renderer::Camera& m_camera;
        protected:
            const Vec3f basePosition() const;
            const Text::Alignment::Type alignment() const;
        public:
            BoxInfoSizeTextAnchor(const BBox& bounds, Axis::Type axis, Renderer::Camera& camera);
        };
        
        class BoxInfoMinMaxTextAnchor : public Text::TextAnchor {
        public:
            typedef enum {
                BoxMin,
                BoxMax
            } EMinMax;
        private:
            BBox m_bounds;
            EMinMax m_minMax;
            Renderer::Camera& m_camera;
        protected:
            const Vec3f basePosition() const;
            const Text::Alignment::Type alignment() const;
        public:
            BoxInfoMinMaxTextAnchor(const BBox& bounds, EMinMax minMax, Renderer::Camera& camera);
        };

        class BoxInfoRenderer {
        private:
            BBox m_bounds;
            Text::TextRenderer<unsigned int>* m_textRenderer;
            Text::TextRenderer<unsigned int>::SimpleTextRendererFilter m_textFilter;
            bool m_initialized;
        public:
            BoxInfoRenderer(const BBox& bounds, Text::FontManager& fontManager);
            ~BoxInfoRenderer();
            
            void render(Vbo& vbo, RenderContext& context);
        };
    }
}

#endif /* defined(__TrenchBroom__BoxInfoRenderer__) */
