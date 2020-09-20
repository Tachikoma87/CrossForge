#include <filesystem>
#include "File.h"
#include "CrossForgeException.h"

namespace CForge {

	bool File::exists(const std::string Path) {
		return std::filesystem::exists(Path);
	}//exists

	int64_t File::size(const std::string Path) {
		return std::filesystem::file_size(Path);
	}//size

	File::File(void) {
		m_pFile = nullptr;
	}//Constructor

	File::~File(void) {
		end();
		
	}//Destructor

	void File::begin(const std::string Path, const std::string Mode) {
		end();
		if ((0 == Mode.compare("r")) || (0 == Mode.compare("rb")) && !exists(Path)) throw CForgeExcept("File " + Path + " does not exist!");

		m_pFile = fopen(Path.c_str(), Mode.c_str());
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

	void File::write(const void* pBuffer, uint64_t ByteCount) {
		if (nullptr == m_pFile) throw NotInitializedExcept("No open file to write to!");
		if (nullptr == pBuffer) throw NullpointerExcept("pBuffer");
		if (ByteCount == 0) throw IndexOutOfBoundsExcept("ByteCount");
		if (1 != fwrite(pBuffer, ByteCount, 1, m_pFile)) throw CForgeExcept("An error occurred writing to file " + m_Path);	
	}//write

	void File::read(void* pBuffer, uint64_t ByteCount) {
		if (nullptr == m_pFile) throw NotInitializedExcept("No open file to read from!");
		if (nullptr == pBuffer) throw NullpointerExcept("pBuffer");
		if (1 != fread(pBuffer, ByteCount, 1, m_pFile)) throw CForgeExcept("An error occurred reading from file " + m_Path);
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

}//name-space