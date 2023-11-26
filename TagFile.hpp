#ifndef TAGFILE_HPP
# define TAGFILE_HPP
# include "Ids.hpp"
# include <unordered_map>
# include <filesystem>
# include <fstream>

class TagFile
{
	public:
		TagFile() : _fileN(0), _tagN(0), _folderName("./") {};
		TagFile(std::string folderName);
		~TagFile();
		int		initByFolder(std::string newFolderName);
		void	saveTags();
		std::vector<std::string>   searchByTagAll(std::vector<std::string> tagsName);
		void	addLinkedTagFile(std::string fileName, std::string tagName);
		void	addLinkedTagFile(std::string fileName, std::vector<std::string> tagsName);
		void	addTag(std::string tagName);
		void	addFile(std::string fileName);
		void	printFiles();
		void	printTags();
		void	printTagsOnFile(std::string fileName);
		unsigned short	cmptTagInFile(std::string fileName);
		void	delTagInFile(std::string fileName, std::string tagName);
		void	delAllTag(std::string tagName);
		unsigned short	getRealTagN();
		std::string	getFolderName() const;
		std::vector<std::string>	getFileVec() const;
		std::vector<std::string>	getTagVec() const;
		bool			noTag();
		std::vector<std::string>	memStrVec;

	private:
		std::vector<std::string>				_tagVec;
		std::vector<std::string>				_fileVec;
		std::unordered_map<std::string, Ids*>	_tagMap;
		std::unordered_map<std::string, Ids*>	_fileMap;
		unsigned short							_fileN;
		unsigned short							_tagN;
		std::string								_folderName;
		const std::string						_memFileName = ".mem.tag";
		void	_deleteMapValue(std::unordered_map<std::string, Ids*> &unMap);
		void	_linkTagFile(std::string fileName, std::string tagName);
		void	_printTagFile(std::vector<std::string> &vec1, std::vector<std::string> &vec2, std::unordered_map<std::string, Ids*> &unMap);
		int		_readFolderAndSetFileMap();
		void	_setTagFromMemFile(std::string memFilePath);
};

#endif