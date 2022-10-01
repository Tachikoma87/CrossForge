
#ifdef WIN32
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include "File.h"

namespace CForge {

	bool File::exists(const std::string Path) {
#ifdef WIN32
		return std::filesystem::exists(Path);
#else
#if __GNUC__ > 7
		return std::filesystem::exists(Path);
#else
		return std::experimental::filesystem::exists(Path);
#endif
#endif
	}//exists

	int64_t File::size(const std::string Path) {
#ifdef WIN32
		return std::filesystem::file_size(Path);
#else
#if __GNUC__ > 7
		return std::filesystem::file_size(Path);
#else
		return std::experimental::filesystem::file_size(Path);
#endif
#endif
	}//size

	std::string File::absolute(const std::string Path) {
#ifdef WIN32
		std::filesystem::path P = Path.c_str();
		return std::filesystem::absolute(P).string();
#else
#if __GNUC__ > 7
		std::filesystem::path P = Path.c_str();
		return std::filesystem::absolute(P).string();
#else
		std::experimental::filesystem::path P = Path.c_str();
		return std::experimental::filesystem::absolute(P).string();
#endif
#endif
	}//absolute

	std::string File::relative(const std::string Path, const std::string Basepath) {
#ifdef WIN32
		std::filesystem::path Relpath = std::filesystem::relative(std::filesystem::path(Path.c_str()), std::filesystem::path(Basepath.c_str()));
		return Relpath.string();
#else
#if __GNUC__ > 7
		std::filesystem::path Relpath = std::filesystem::relative(std::filesystem::path(Path.c_str()), std::filesystem::path(Basepath.c_str()));
		return Relpath.string();
#else
		std::experimental::filesystem::path Relpath = std::experimental::filesystem::path(Path.c_str());
		return Relpath.string();
#endif
#endif
	}//relative

	std::string File::removeFilename(const std::string Path) {
#ifdef WIN32
		std::filesystem::path P = Path.c_str();
#else
#if __GNUC__ > 7
		std::filesystem::path P = Path.c_str();
#else
		std::experimental::filesystem::path P = Path.c_str();
#endif
#endif
		P = P.remove_filename();
		return P.string();
	}//remove filename

	bool File::isDirectory(const std::string Path) {
#ifdef WIN32
		return std::filesystem::is_directory(Path);
#else
#if __GNUC__ > 7
		return std::filesystem::is_directory(Path);
#else 
		return std::experimental::filesystem::is_directory(Path);
#endif
#endif
	}//isDirectory

	bool File::createDirecotry(const std::string Path) {
#ifdef WIN32
		return std::filesystem::create_directory(Path);
#else
#if __GNUC__ > 7
		return std::filesystem::create_directory(Path);
#else
		return std::experimental::filesystem::create_directory(Path);
#endif
#endif
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
#ifdef WIN32
			std::filesystem::path P = Path;
			P = P.remove_filename();
			std::filesystem::create_directories(P);

#else
			std::experimental::filesystem::path P = Path;
			P = P.remove_filename();
			std::experimental::filesystem::create_directories(P);
#endif
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