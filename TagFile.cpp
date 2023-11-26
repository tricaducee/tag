#include "TagFile.hpp"

TagFile::TagFile(std::string folderName) : _fileN(0), _tagN(0)
{
	this->initByFolder(folderName);
}

TagFile::~TagFile()
{
	this->saveTags();
	this->_deleteMapValue(this->_tagMap);
	this->_deleteMapValue(this->_fileMap);
}

void TagFile::_deleteMapValue(std::unordered_map<std::string, Ids*> &unMap)
{
    for (const auto& [key, value] : unMap)
        if (value)
            delete value;
}

std::vector<std::string>   TagFile::searchByTagAll(std::vector<std::string> tagsName)
{
    std::vector<std::string> ret;
    std::vector<unsigned short> tagsId;
	std::vector<unsigned short> noTagsId;
	std::vector<std::string> files;
	bool check;
    for (auto name : tagsName)
    {
		if (!name.compare(0, 4, "no::", 0, 4))
		{
			name = name.substr(4);
			if (!this->_tagMap[name])
        	{
        	    std::cout << "Tag : [" << name << "] does not exist." << std::endl;
        	    return ret;
        	}
        	else
        	    noTagsId.push_back(this->_tagMap[name]->getId());
		} else {
			if (!this->_tagMap[name])
        	{
        	    std::cout << "Tag : [" << name << "] does not exist." << std::endl;
        	    return ret;
        	}
        	else
        	    tagsId.push_back(this->_tagMap[name]->getId());
		}
    }
	if (noTagsId.empty() && tagsId.empty())
		return ret;
	if (tagsId.empty())
		files = this->_fileVec;
	else
	{
	    const std::vector<unsigned short> &filesID = this->_tagMap[this->_tagVec[tagsId.back()]]->getIdVec();
		tagsId.pop_back();
	    for (auto id : filesID)
	    {
	        const std::vector<unsigned short> &fileTagsId = this->_fileMap[this->_fileVec[id]]->getIdVec();
	        check = true;
	        for (auto tag : tagsId)
	        {
	            check = false;
	            for (auto fileTag : fileTagsId)
	            {
	                if (fileTag == tag)
	                {
	                    check = true;
	                    break;
	                }
	            }
	            if (!check)
	                break;
	        }
	        if (check)
	            files.push_back(this->_fileVec[id]);
	    }
		if (files.empty() || noTagsId.empty())
			return files;	
	}
	for (auto fileName : files)
	{
		const std::vector<unsigned short> &fileTagsId = this->_fileMap[fileName]->getIdVec();
		//check = true;
		for (auto tag : noTagsId)
	    {
			check = true;
	        for (auto fileTag : fileTagsId)
	        {
	            if (fileTag == tag)
	            {
	                check = false;
	                break;
	            }
	        }
	        if (!check)
	            break;
	    }
		if (check)
			ret.push_back(fileName);
	}
    return ret;
}

// std::vector<std::string>   TagFile::searchByTagAll(std::vector<std::string> tagsName)
// {
//     std::vector<std::string> ret;
//     std::vector<unsigned short> tagsId;
// 	std::vector<unsigned short> noTagsId;
//     for (auto name : tagsName)
//     {
// 			if (!this->_tagMap[name])
//         	{
//         	    std::cout << "Tag : [" << name << "] does not exist." << std::endl;
//         	    return ret;
//         	}
//         	else
//         	    tagsId.push_back(this->_tagMap[name]->getId());
// 		}
//     }
//     const std::vector<unsigned short> &filesID = this->_tagMap[tagsName[0]]->getIdVec();
//     for (auto id : filesID)
//     {
//         const std::vector<unsigned short> &fileTagsId = this->_fileMap[this->_fileVec[id]]->getIdVec();
//         bool check;
//         for (auto tag : tagsId)
//         {
//             check = false;
//             for (auto fileTag : fileTagsId)
//             {
//                 if (fileTag == tag)
//                 {
//                     check = true;
//                     break;
//                 }
//             }
//             if (!check)
//                 break;
//         }
//         if (check)
//             ret.push_back(this->_fileVec[id]);
//     }
//     return ret;
// }

void    TagFile::_linkTagFile(std::string fileName, std::string tagName)
{
    this->_fileMap[fileName]->addId(this->_tagMap[tagName]->getId());
    this->_tagMap[tagName]->addId(this->_fileMap[fileName]->getId());
}

void	TagFile::addLinkedTagFile(std::string fileName, std::vector<std::string> tagsName)
{
	this->addFile(fileName);
	for (auto tagName : tagsName)
	{
		this->addTag(tagName);
		this->_linkTagFile(fileName, tagName);
	}
}

void	TagFile::addLinkedTagFile(std::string fileName, std::string tagName)
{
	this->addFile(fileName);
	this->addTag(tagName);
	this->_linkTagFile(fileName, tagName);
}

void	TagFile::addFile(std::string fileName)
{
	if (!this->_fileMap[fileName])
	{
		this->_fileMap[fileName] = new Ids(this->_fileN++);
		this->_fileVec.push_back(fileName);
	}
}

int	TagFile::_readFolderAndSetFileMap()
{
    if (!std::filesystem::exists(this->_folderName) || !std::filesystem::is_directory(this->_folderName)) {
        std::cerr << "Le dossier n'existe pas ou n'est pas accessible." << std::endl;
        return 1;
    }
    for (const auto& entry : std::filesystem::directory_iterator(this->_folderName)) {
        if (entry.is_regular_file() && entry.path().filename().string()[0] != '.')
            this->addFile(entry.path().filename().string());
    }
    if (this->_fileN < 1)
	{
        std::cerr << "Aucun fichier trouvé." << std::endl;
		return 1;
	}
	return 0;
}

void	TagFile::_setTagFromMemFile(std::string memFilePath)
{
	std::ifstream				memFile(memFilePath);
	std::string					line;
	std::string					fileName;

	if (memFile.is_open())
	{
		while (getline(memFile, line))
		{
			if (line[0] == ':' && line[1] == ':')
			{
				fileName = line.substr(2);
				if (!this->_fileMap[fileName])
					fileName = "";
			} else if (!fileName.empty() && !line.empty())
				this->addLinkedTagFile(fileName, line);
		}
		memFile.close();
	}
}

int		TagFile::initByFolder(std::string newFolderName)
{
	if (!newFolderName.empty())
	{
		this->_folderName = newFolderName;
		if (this->_folderName[this->_folderName.size() - 1] != '/')
			this->_folderName += '/';
	}
	if (this->_folderName.empty())
		return 1;
	if (this->_readFolderAndSetFileMap())
		return 1;
	std::string	memFilePath = this->_folderName + this->_memFileName;
	this->_setTagFromMemFile(memFilePath);
	return 0;
}

void	TagFile::saveTags()
{
	if (this->_folderName.empty() || noTag())
		return ;
	std::ofstream	file(this->_folderName + this->_memFileName);
	if (file.is_open())
	{
		for (auto key : this->_fileVec)
		{
			file << "::" << key << std::endl;
			std::vector<unsigned short> tagIds = this->_fileMap[key]->getIdVec();
			for (auto id : tagIds)
			{
				file << this->_tagVec[id] << std::endl;
			}
		}
		file.close();
	} else {
		std::cerr << "Le programme n'as pas pu sauvegarder, impossible d'ouvrir le fichier de sauvegarde." << std::endl;
	}
}

void	TagFile::addTag(std::string tagName)
{
	if (!this->_tagMap[tagName])
	{
		this->_tagMap[tagName] = new Ids(this->_tagN++);
		this->_tagVec.push_back(tagName);
	}
}

void	TagFile::delTagInFile(std::string fileName, std::string tagName)
{
	if (!this->_tagMap[tagName])
		return;
	this->_fileMap[fileName]->delId(this->_tagMap[tagName]->getId());
	this->_tagMap[tagName]->delId(this->_fileMap[fileName]->getId());
}

void	TagFile::delAllTag(std::string tagName)
{
	if (!this->_tagMap[tagName])
		return;
	const std::vector<unsigned short>	&filesId = this->_tagMap[tagName]->getIdVec();
	for (auto id : filesId)
		this->delTagInFile(this->_fileVec[id], tagName);
}

void	TagFile::printFiles()
{
	this->_printTagFile(this->_fileVec, this->_tagVec, this->_fileMap);
}

void	TagFile::printTags()
{
	this->_printTagFile(this->_tagVec, this->_fileVec, this->_tagMap);
}

void	TagFile::_printTagFile(std::vector<std::string> &vec1, std::vector<std::string> &vec2, std::unordered_map<std::string, Ids*> &unMap)
{
    for (const auto& [key, value] : unMap)
    {
        const std::vector<unsigned short> &vectorIds = value->getIdVec();
        std::cout << vec1[value->getId()] << ": ";
        for (auto x : vectorIds)
            std::cout << vec2[x] << ", ";
        std::cout << std::endl;
    }
}

unsigned short	TagFile::getRealTagN()
{
	unsigned short	realTagN = 0;
	for (auto tag : this->_tagVec)
	{
		if (!this->_tagMap[tag]->getIdVec().empty())
			++realTagN;
	}
	return realTagN;
}

bool	TagFile::noTag()
{
	for (auto tag : this->_tagVec)
	{
		if (!this->_tagMap[tag]->getIdVec().empty())
			return false;
	}
	return true;
}

void	TagFile::printTagsOnFile(std::string fileName)
{
	std::cout << fileName << " :" << std::endl;
	if (!this->_fileMap[fileName]->getIdVec().size())
	{
		std::cout << "[has no tag]" << std::endl;
		return;
	}
	for (auto tagId : this->_fileMap[fileName]->getIdVec())
	{
		std::cout << this->_tagVec[tagId] << " ";
	}
	std::cout << std::endl;
}

unsigned short	TagFile::cmptTagInFile(std::string fileName)
{
	return this->_fileMap[fileName]->getIdVec().size();
}

std::string	TagFile::getFolderName() const
{
	return this->_folderName;
}

std::vector<std::string>	TagFile::getFileVec() const
{
	return this->_fileVec;
}

std::vector<std::string>	TagFile::getTagVec() const
{
	return this->_tagVec;
}