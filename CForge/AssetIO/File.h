/*****************************************************************************\
*                                                                           *
* File(s): File.h and File.cpp                                              *
*                                                                           *
* Content: Basic IO class that reads and writes from or to a binary file.   *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#pragma once

#include "../Core/CForgeObject.h"

namespace CForge {
	/**
	* \brief Basic IO class which reads and writes from or to a binary file.
	* \ingroup AssetIO
	*
	* \todo Do full documentation
	*/
	class CFORGE_API File: public CForgeObject {
	public:
		
		/**
		* \brief Checks whether a file exists under the specified path.
		* \param[in] Path Path to the file.
		* \return True if the file exists, false otherwise.
		*/
		static bool exists(const std::string Path);
		static void createDirectory(const std::string Path);
		static void createDirectories(const std::string Path);

		static std::string parentPath(const std::string Path);

		/**
		* \brief Returns size of the specified file.
		* \param[in] Path Path to the file.
		* \return File's size in bytes.
		* \note Throws std::fileystem::filesystem_error if an error occurs. 
		*/
		static int64_t size(const std::string Path);
		
		static std::string absolute(const std::string Path);
		static std::string relative(const std::string Path, const std::string Basepath);
		static std::string removeFilename(const std::string Path);

		static bool isDirectory(const std::string Path);
		static bool createDirecotry(const std::string Path);

		/**
		* \brief Constructor
		*/
		File(void);

		/**
		* \brief Destructor
		*/
		~File(void);

		/**
		* \brief Starts communication with a file object at the specified location.
		* \param[in] Path Path to the file.
		* \param[in] Mode File mode. Valid arguments are "r", "w", "a" and binary versions "rb", "wb", and "ab".
		*/
		void begin(const std::string Path, const std::string Mode);

		/**
		* \brief Ends communication with the file object. Also closes the file.
		*/
		void end(void);

		/**
		* \brief Clear method. Same as end().
		*/
		void clear(void);

		/**
		* \brief Writes a chunk of data to an open file object.
		* \param[in] pBuffer Contains the data. Has to be at least ByteCount bytes large. Can not be nullptr.
		* \param[in] Number of bytes to write.
		*/
		uint32_t write(const void* pBuffer, uint64_t ByteCount);

		/**
		* \brief Reads a chunk of memory from the file.
		* \param[out] pBuffer Data buffer that will contain the read data. Must not be nullptr, must be at least ByteCount bytes in size.
		* \param[in] ByteCount Number of bytes to read.
		*/
		uint32_t read(void* pBuffer, uint64_t ByteCount);

		/**
		* \brief Returns the internal file handle. Be careful what you do with it.
		* \return Internal file handle.
		*/
		FILE* handle(void)const;

		/**
		* \brief Check whether end of file is read.
		* \return true if end of file is reached, false otherwise.
		*/
		bool eof(void)const;

		/**
		* \brief Sets file cursor to the beginning of the file.
		*/
		void rewind(void);

		bool valid(void)const;
		
	protected:
		FILE* m_pFile;	///< Internal file handle.
		std::string m_Path;	///< Path of the file.
	};//CFile

}//name-space