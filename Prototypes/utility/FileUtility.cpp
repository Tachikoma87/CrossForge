#if (defined(__EMSCRIPTEN__) || defined(_MSC_VER) || (defined(__GNUC__) && (__GNUC__ > 7)) )
#include <filesystem>
#define STD_FS std::filesystem
#else
#define EXPERIMENTAL_FILESYSTEM 1
#include <experimental/filesystem>
#define STD_FS std::experimental::filesystem
#endif

#include "FileUtility.h"

namespace crossforge {

	bool FileUtility::exists(const std::string Path) {
		return STD_FS::exists(Path);
	}//exists

	int64_t FileUtility::size(const std::string Path) {
		return STD_FS::file_size(Path);
	}//size

	std::string FileUtility::absolute(const std::string Path) {
		STD_FS::path P = Path.c_str();
		return STD_FS::absolute(P).string();
	}//absolute

	std::string FileUtility::retrieveFilename(const std::string Path) {
		STD_FS::path P = Path;
		return P.filename().string();
	}//retrieveFilename

	bool FileUtility::createDirectory(const std::string Path) {
		return STD_FS::create_directory(Path);
	}//createDirectory

	bool FileUtility::createDirectories(const std::string Path) {
		return STD_FS::create_directories(Path);
	}//createDirectories

	std::string FileUtility::parentPath(const std::string Path) {
		STD_FS::path P = Path;
		return P.parent_path().string();
	}//parentPath

	std::string FileUtility::relative(const std::string Path, const std::string Basepath) {
#ifndef EXPERIMENTAL_FILESYSTEM
		STD_FS::path Relpath = STD_FS::relative(STD_FS::path(Path.c_str()), STD_FS::path(Basepath.c_str()));
		return Relpath.string();
#else
		STD_FS::path Relpath = STD_FS::path(Path.c_str());
		return Relpath.string();
#endif
	}//relative

	std::string FileUtility::removeFilename(const std::string Path) {
		STD_FS::path P = Path.c_str();
		P = P.remove_filename();
		return P.string();
	}//remove filename

	bool FileUtility::isDirectory(const std::string Path) {
		return STD_FS::is_directory(Path);
	}//isDirectory
}