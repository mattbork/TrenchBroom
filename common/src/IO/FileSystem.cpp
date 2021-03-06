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

#include "FileSystem.h"

#include "Exceptions.h"
#include "CollectionUtils.h"
#include "IO/FileMatcher.h"

namespace TrenchBroom {
    namespace IO {
        FileSystem::FileSystem(std::unique_ptr<FileSystem> next) :
        m_next(std::move(next)) {}

        FileSystem::~FileSystem() {}

        bool FileSystem::hasNext() const {
            return m_next != nullptr;
        }

        const FileSystem& FileSystem::next() const {
            if (!m_next) {
                throw FileSystemException("File system chain ends here");
            }
            return *m_next;
        }

        std::unique_ptr<FileSystem> FileSystem::releaseNext() {
            return std::move(m_next);
        }

        bool FileSystem::canMakeAbsolute(const Path& path) const {
            if (path.isAbsolute()) {
                return false;
            } else {
                return _canMakeAbsolute(path);
            }
        }

        Path FileSystem::makeAbsolute(const Path& path) const {
            try {
                if (!canMakeAbsolute(path)) {
                    throw FileSystemException("Cannot make absolute path of '" + path.asString() + "'");
                }

                return _makeAbsolute(path);
            } catch (const PathException& e) {
                throw FileSystemException("Invalid path: '" + path.asString() + "'", e);
            }
        }

        bool FileSystem::directoryExists(const Path& path) const {
            try {
                if (path.isAbsolute()) {
                    throw FileSystemException("Path is absolute: '" + path.asString() + "'");
                }

                return _directoryExists(path);
            } catch (const PathException& e) {
                throw FileSystemException("Invalid path: '" + path.asString() + "'", e);
            }
        }

        bool FileSystem::fileExists(const Path& path) const {
            try {
                if (path.isAbsolute()) {
                    throw FileSystemException("Path is absolute: '" + path.asString() + "'");
                }
                return _fileExists(path);
            } catch (const PathException& e) {
                throw FileSystemException("Invalid path: '" + path.asString() + "'", e);
            }
        }

        Path::List FileSystem::findItemsWithBaseName(const Path& path, const StringList& extensions) const {
            if (path.isEmpty()) {
                return Path::List(0);
            }

            const auto directoryPath = path.deleteLastComponent();
            if (!directoryExists(directoryPath)) {
                return Path::List(0);
            }

            const auto basename = path.basename();
            return findItems(directoryPath, FileBasenameMatcher(basename, extensions));
        }

        Path::List FileSystem::findItems(const Path& directoryPath) const {
            return findItems(directoryPath, FileTypeMatcher());
        }

        Path::List FileSystem::findItemsRecursively(const Path& directoryPath) const {
            return findItemsRecursively(directoryPath, FileTypeMatcher());
        }

        Path::List FileSystem::getDirectoryContents(const Path& directoryPath) const {
            try {
                if (directoryPath.isAbsolute()) {
                    throw FileSystemException("Path is absolute: '" + directoryPath.asString() + "'");
                }
                if (!directoryExists(directoryPath)) {
                    throw FileSystemException("Directory not found: '" + directoryPath.asString() + "'");
                }

                return _getDirectoryContents(directoryPath);
            } catch (const PathException& e) {
                throw FileSystemException("Invalid path: '" + directoryPath.asString() + "'", e);
            }
        }

        MappedFile::Ptr FileSystem::openFile(const Path& path) const {
            try {
                if (path.isAbsolute()) {
                    throw FileSystemException("Path is absolute: '" + path.asString() + "'");
                }

                return _openFile(path);
            } catch (const PathException& e) {
                throw FileSystemException("Invalid path: '" + path.asString() + "'", e);
            }
        }

        bool FileSystem::_canMakeAbsolute(const Path& path) const {
            return doCanMakeAbsolute(path) || (m_next && m_next->_canMakeAbsolute(path));
        }

        Path FileSystem::_makeAbsolute(const Path& path) const {
            if (doCanMakeAbsolute(path)) {
                return doMakeAbsolute(path);
            } else if (m_next) {
                return m_next->_makeAbsolute(path);
            } else {
                throw FileSystemException("Path does not exist: '" + path.asString() + "'");
            }
        }

        bool FileSystem::_directoryExists(const Path& path) const {
            return doDirectoryExists(path) || (m_next && m_next->_directoryExists(path)) ;
        }

        bool FileSystem::_fileExists(const Path& path) const {
            return doFileExists(path) || (m_next && m_next->_fileExists(path));
        }

        Path::List FileSystem::_getDirectoryContents(const Path& directoryPath) const {
            auto result = doGetDirectoryContents(directoryPath);
            if (m_next) {
                VectorUtils::append(result, m_next->_getDirectoryContents(directoryPath));
            }

            VectorUtils::sortAndRemoveDuplicates(result);
            return result;
        }

        MappedFile::Ptr FileSystem::_openFile(const Path& path) const {
            if (doFileExists(path)) {
                return doOpenFile(path);
            } else if (m_next) {
                return m_next->_openFile(path);
            } else {
                throw FileSystemException("File not found: '" + path.asString() + "'");
            }
        }

        bool FileSystem::doCanMakeAbsolute(const Path& path) const {
            return false;
        }

        Path FileSystem::doMakeAbsolute(const Path& path) const {
            throw FileSystemException("Cannot make absolute path of '" + path.asString() + "'");
        }

        WritableFileSystem::WritableFileSystem() = default;
        WritableFileSystem::~WritableFileSystem() = default;

        void WritableFileSystem::createFile(const Path& path, const String& contents) {
            try {
                if (path.isAbsolute()) {
                    throw FileSystemException("Path is absolute: '" + path.asString() + "'");
                }
                doCreateFile(path, contents);
            } catch (const PathException& e) {
                throw FileSystemException("Invalid path: '" + path.asString() + "'", e);
            }
        }

        void WritableFileSystem::createDirectory(const Path& path) {
            try {
                if (path.isAbsolute()) {
                    throw FileSystemException("Path is absolute: '" + path.asString() + "'");
                }
                doCreateDirectory(path);
            } catch (const PathException& e) {
                throw FileSystemException("Invalid path: '" + path.asString() + "'", e);
            }
        }

        void WritableFileSystem::deleteFile(const Path& path) {
            try {
                if (path.isAbsolute()) {
                    throw FileSystemException("Path is absolute: '" + path.asString() + "'");
                }
                doDeleteFile(path);
            } catch (const PathException& e) {
                throw FileSystemException("Invalid path: '" + path.asString() + "'", e);
            }
        }

        void WritableFileSystem::copyFile(const Path& sourcePath, const Path& destPath, const bool overwrite) {
            try {
                if (sourcePath.isAbsolute()) {
                    throw FileSystemException("Source path is absolute: '" + sourcePath.asString() + "'");
                }
                if (destPath.isAbsolute()) {
                    throw FileSystemException("Destination path is absolute: '" + destPath.asString() + "'");
                }
                doCopyFile(sourcePath, destPath, overwrite);
            } catch (const PathException& e) {
                throw FileSystemException("Invalid source or destination path: '" + sourcePath.asString() + "', '" + destPath.asString() + "'", e);
            }
        }

        void WritableFileSystem::moveFile(const Path& sourcePath, const Path& destPath, const bool overwrite) {
            try {
                if (sourcePath.isAbsolute()) {
                    throw FileSystemException("Source path is absolute: '" + sourcePath.asString() + "'");
                }
                if (destPath.isAbsolute()) {
                    throw FileSystemException("Destination path is absolute: '" + destPath.asString() + "'");
                }
                doMoveFile(sourcePath, destPath, overwrite);
            } catch (const PathException& e) {
                throw FileSystemException("Invalid source or destination path: '" + sourcePath.asString() + "', '" + destPath.asString() + "'", e);
            }
        }
    }
}
