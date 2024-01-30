
#if (defined(__EMSCRIPTEN__) || defined(_MSC_VER) || (defined(__GNUC__) && (__GNUC__ > 7)) )
#include <filesystem>
#define STD_FS std::filesystem
#else
#define EXPERIMENTAL_FILESYSTEM 1
#include <experimental/filesystem>
#define STD_FS std::experimental::filesystem
#endif

#include "File.h"

namespace CForge {

	bool File::exists(const std::string Path) {
		return STD_FS::exists(Path);
	}//exists

	int64_t File::size(const std::string Path) {
		return STD_FS::file_size(Path);
	}//size

	std::string File::absolute(const std::string Path) {
		STD_FS::path P = Path.c_str();
		return STD_FS::absolute(P).string();
	}//absolute

	std::string File::retrieveFilename(const std::string Path) {
		STD_FS::path P = Path;
		return P.filename().string();
	}//retrieveFilename

	void File::createDirectory(const std::string Path) {
		STD_FS::create_directory(Path);
	}//createDirectory

	void File::createDirectories(const std::string Path) {
		STD_FS::create_directories(Path);
	}//createDirectories

	std::string File::parentPath(const std::string Path) {
		STD_FS::path P = Path;
		return P.parent_path().string();
	}//parentPath

	std::string File::relative(const std::string Path, const std::string Basepath) {
#ifndef EXPERIMENTAL_FILESYSTEM
		STD_FS::path Relpath = STD_FS::relative(STD_FS::path(Path.c_str()), STD_FS::path(Basepath.c_str()));
		return Relpath.string();
#else
		STD_FS::path Relpath = STD_FS::path(Path.c_str());
		return Relpath.string();
#endif
	}//relative

	std::string File::removeFilename(const std::string Path) {
		STD_FS::path P = Path.c_str();
		P = P.remove_filename();
		return P.string();
	}//remove filename

	bool File::isDirectory(const std::string Path) {
		return STD_FS::is_directory(Path);
	}//isDirectory

	bool File::createDirecotry(const std::string Path) {
		return STD_FS::create_directory(Path);
	}//createDirectory


	File::File(void): CForgeObject("File") {
		m_pFile = nullptr;
	}//Constructor

	File::~File(void) {
		end();
		
	}//Destructor

	void File::begin(const std::string Path, const std::string Mode) {
		end();
		if (Path.empty()) return;

		if ( ((0 == Mode.compare("r")) || (0 == Mode.compare("rb")) ) && !exists(Path)) throw CForgeExcept("File " + Path + " does not exist!");

		m_pFile = fopen(Path.c_str(), Mode.c_str());

		if (nullptr == m_pFile) {
			// try to create directory and open again
			STD_FS::path P = Path;
			P = P.remove_filename();
			STD_FS::create_directories(P);

			m_pFile = fopen(Path.c_str(), Mode.c_str());
		}

		if (nullptr == m_pFile) throw CForgeExcept("Error opening file " + Path + " in mode " + Mode);
		m_Path = Path;
	}//begin

	void File::end(void) {
		if (nullptr != m_pFile) fclose(m_pFile);
		m_pFile = nullptr;
		m_Path = "";
	}//end

	void File::clear(void) {
		end();
	}//clear

	uint32_t File::write(const void* pBuffer, uint64_t ByteCount) {
		if (nullptr == m_pFile) throw NotInitializedExcept("No open file to write to!");
		if (nullptr == pBuffer) throw NullpointerExcept("pBuffer");
		if (ByteCount == 0) throw IndexOutOfBoundsExcept("ByteCount");
		uint32_t Written = fwrite(pBuffer, 1, ByteCount, m_pFile);
		if (0 == Written) throw CForgeExcept("An error occurred writing to file " + m_Path);
		return Written;
	}//write

	uint32_t File::read(void* pBuffer, uint64_t ByteCount) {
		if (nullptr == m_pFile) throw NotInitializedExcept("No open file to read from!");
		if (nullptr == pBuffer) throw NullpointerExcept("pBuffer");
		uint32_t Read = fread(pBuffer, 1, ByteCount, m_pFile);
		if (Read == 0) throw CForgeExcept("An error occurred reading from file " + m_Path);
		return Read;
	}//read


	FILE* File::handle(void)const {
		return m_pFile;
	}//handle

	bool File::eof(void)const {
		if (nullptr == m_pFile) throw NotInitializedExcept("No open file!");
		return (0 != feof(m_pFile));
	}//eof

	void File::rewind(void) {
		if (nullptr == m_pFile) throw NotInitializedExcept("No open file!");
		fseek(m_pFile, 0, SEEK_SET);
	}//rewind

	bool File::valid(void)const {
		return (nullptr != m_pFile);
	}//valid

}//name-space
