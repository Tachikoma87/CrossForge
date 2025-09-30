/*****************************************************************************\
*                                                                           *
* File(s): FileUtility.h and FileUtility.cpp      *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_FILEUTILITY_H__
#define __CROSSFORGE_FILEUTILITY_H__

#include <crossforge/core/CoreDefinitions.h>

namespace crossforge {
	class FileUtility {
	public:

		/**
		* \brief Checks whether a file exists at the specified path.
		* \param[in] Path Path to the file.
		* \return True if the file exists, false otherwise.
		*/
		static bool exists(const std::string Path);

		/**
		* \brief Checks whether the specified path points to a directory.
		*
		* \param[in] Path Path to check.
		* \return True if directory, false otherwise.
		*/
		static bool isDirectory(const std::string Path);

		/**
		* \brief Creates a directory. The parent directory has to exist!
		*
		* \param[in] Path Directory path to create.
		* return True on success, false otherwise.
		*/
		static bool createDirectory(const std::string Path);

		/**
		* \brief Creates a directory. Creates parent directories recursively if they do not exist.
		*
		* \param[in] Path Directory to create.
		* return True on success, false otherwise.
		*/
		static bool createDirectories(const std::string Path);

		/**
		* \brief Returns the parent directory path of the specified path.
		*
		* \param[in] Path Path to extract parent from.
		* \return Parent path or empty string if no parent path exists.
		*/
		static std::string parentPath(const std::string Path);

		/**
		* \brief Retrieves the filename from a path.
		*
		* \param[in] Path The path to the file.
		* \return Filename.
		*/
		static std::string retrieveFilename(const std::string Path);

		/**
		* \brief Returns size of the specified file.
		*
		* \param[in] Path Path to the file.
		* \return File's size in bytes.
		* \throws std::fileystem::filesystem_error if an error occurs.
		*/
		static int64_t size(const std::string Path);

		/**
		* \brief Creates the absolute file path of a given path.
		*
		* \param[in] Path The path.
		* \return Absolute version of the path.
		*/
		static std::string absolute(const std::string Path);

		/**
		* \brief Creates a relative path from a given path and base path.
		*
		* \param[in] Path The input path.
		* \param[in] Basepath Base path.
		* \return Path of input path relative to specified base path.
		*/
		static std::string relative(const std::string Path, const std::string Basepath);

		/**
		* \brief Removes the filename from a given path.
		*
		* \param[in] Path The path to a file.
		* \return Path without the filename.
		*/
		static std::string removeFilename(const std::string Path);

	protected:
		FileUtility();
		~FileUtility();
	};
}

#endif 