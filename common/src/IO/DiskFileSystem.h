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

#ifndef TrenchBroom_DiskFileSystem
#define TrenchBroom_DiskFileSystem

#include "IO/MappedFile.h"
#include "IO/FileSystem.h"
#include "IO/Path.h"

#include <memory>

namespace TrenchBroom {
    namespace IO {
        class DiskFileSystem : public FileSystem {
        protected:
            Path m_root;
        public:
            explicit DiskFileSystem(const Path& root, bool ensureExists = true);
            DiskFileSystem(std::unique_ptr<FileSystem> next, const Path& root, bool ensureExists = true);

            const Path& root() const;
        protected:
            bool doCanMakeAbsolute(const Path& path) const override;
            Path doMakeAbsolute(const Path& path) const override;

            bool doDirectoryExists(const Path& path) const override;
            bool doFileExists(const Path& path) const override;
            
            Path::List doGetDirectoryContents(const Path& path) const override;
            const MappedFile::Ptr doOpenFile(const Path& path) const override;
        };
        
#ifdef _MSC_VER
// MSVC complains about the fact that this class inherits some (pure virtual) method declarations several times from different base classes, even though there is only one definition.
#pragma warning(push)
#pragma warning(disable : 4250)
#endif
        class WritableDiskFileSystem : public DiskFileSystem, public WritableFileSystem {
        public:
            WritableDiskFileSystem(const Path& root, bool create);
            WritableDiskFileSystem(std::unique_ptr<FileSystem> next, const Path& root, bool create);
        private:
            void doCreateFile(const Path& path, const String& contents) override;
            void doCreateDirectory(const Path& path) override;
            void doDeleteFile(const Path& path) override;
            void doCopyFile(const Path& sourcePath, const Path& destPath, bool overwrite) override;
            void doMoveFile(const Path& sourcePath, const Path& destPath, bool overwrite) override;
        };
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    }
}

#endif /* defined(TrenchBroom_DiskFileSystem) */
