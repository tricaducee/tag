#include "TagFile.hpp"
#include "IntMenu.hpp"
#include "srch.hpp"
#include <sstream>
#define SEARCH_LIMIT 15

std::vector<std::string> splitVec(const std::string &s) {
    std::vector<std::string> result;
    std::istringstream iss(s);
    std::string mot;

    while (iss >> mot) {
        result.push_back(mot);
    }

    return result;
}

void	stopMessage(std::string message)
{
	char c;
	if (!message.empty())
		std::cout << message << std::endl;
	std::cout << "Press enter to continue" << std::endl;
	read(1, &c, 1);
}

void	consolAddTags(std::string &line, TagFile &tagsObj)
{
	std::string	str;
	std::vector<std::string> tagsVec;
	std::cout << "File tag(s) :" << std::endl;
	tagsObj.printTagsOnFile(line);
	std::cout << "Enter new tag(s) : " << std::flush;
	std::getline(std::cin, str);
	if (str.empty())
	{
		// stopMessage("No Tag");
		return;
	}
	tagsVec = splitVec(str);
	tagsObj.addLinkedTagFile(line, tagsVec);
	// std::cout << "File tag(s) :" << std::endl;
	// tagsObj.printTagsOnFile(line);
	// stopMessage("");
}

void	consolDelTags(std::string &line, TagFile &tagsObj)
{
	std::string	str;
	std::vector<std::string> tagsVec;
	std::cout << "File tag(s) :" << std::endl;
	tagsObj.printTagsOnFile(line);
	if (tagsObj.cmptTagInFile(line))
	{
		std::cout << "Enter tag(s) to delete : " << std::flush;
		std::getline(std::cin, str);
		if (str.empty())
		{
			// stopMessage("No Tag");
			return;
		}
		tagsVec = splitVec(str);
		for (auto tag : tagsVec)
			tagsObj.delTagInFile(line, tag);
		// std::cout << "File tag(s) :" << std::endl;
		// tagsObj.printTagsOnFile(line);
		// stopMessage("");
	}
}

int	optionTagFile(std::string &line, TagFile &tagsObj, char c)
{
	if (c == 's')
	{
		std::cout << "Saving..." << std::endl;
		tagsObj.saveTags();
	}
	else if (c == 'a')
		consolAddTags(line, tagsObj);
	else if (c == 'd')
		consolDelTags(line, tagsObj);
	else if (c == 't') {
		std::cout << "File tag(s) :" << std::endl;
		tagsObj.printTagsOnFile(line);
		stopMessage("");
	} else if (c == '\n')
	{
		if (!system(("open '" + tagsObj.getFolderName() + line + "'").c_str()))
			tagsObj.addLinkedTagFile(line, "vu");
		else
			stopMessage("File [ " + line + " ] cannot be opened");
	}
	return 0;
}

bool accessFolder(const std::string& chemin) {
    if (std::filesystem::exists(chemin) && std::filesystem::is_directory(chemin)) {
        try {
            std::filesystem::directory_iterator{chemin};
            return true;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << e.what() << '\n';
        }
    }
    return false;
}

void printMan()
{
	std::cout << "\033[1m-------------------- MAN --------------------\033[0m\n" <<std::endl;
	std::cout << "\033[1;4mUsage\033[0;1m :\033[0m tag" << std::endl;
	std::cout << "        tag <folder>\n" << std::endl;
	std::cout << "Si aucun dossier n'est spécifié le dossier courant est utilisé par défaut.\n" << std::endl;
	std::cout << "\033[1;4mMENU PRINCIPAL:\033[0m\n" << std::endl;
	std::cout << "    Menu option de sélection les fichiers" << std::endl;
	std::cout << "    qui seront afficher dans la liste interactive.\n" << std::endl;
	std::cout << "\033[4mOptions du menu principal:\033[0m\n" << std::endl;
	std::cout << "    \033[1mSearch by tag:\033[0m" << std::endl;
	std::cout << "        Affiche tous les fichiers qui contiennent tout" << std::endl;
	std::cout << "        les tags spécifiés (séparés par des espaces)." << std::endl;
	std::cout << "        Pour exclure des tags utiliser no::<tag> au lieu de <tag>." << std::endl;
	std::cout << "        Pour des tags optionnel utiliser op::<tag> au lieu de <tag>." << std::endl;
	std::cout << "    \033[1mSearch by tag (with list):\033[0m" << std::endl;
	std::cout << "        Séléctionné les tags à l'aide d'une liste interactive" << std::endl;
	std::cout << "        de tout les tags existant." << std::endl;
	std::cout << "        (`a` pour <tag> `n` pour no::<tag> et `o` pour op::<tag>)" << std::endl;
	std::cout << "    \033[1mSearch by name:\033[0m" << std::endl;
	std::cout << "        Affiche tout les dossier ayant un nom similaire" << std::endl;
	std::cout << "        à l'entrée de l'utilisateur par ordre de pertinence." << std::endl;
	std::cout << "    \033[1mList all files:\033[0m" << std::endl;
	std::cout << "        Affiche tous les fichiers du dossier.\n" << std::endl;
	std::cout << "\033[4mCommandes du menu principal:\033[0m\n" << std::endl;
	std::cout << "    \033[1m`enter`\033[0m : Sélectionne une option." << std::endl;
	std::cout << "        \033[1m`q`\033[0m : Quitte le programme.\n" << std::endl;
	std::cout << "\033[1;4mLISTE INTERACTIVE:\033[0m\n" << std::endl;
	std::cout << "    Liste les fichiers sélectionné" << std::endl;
	std::cout << "    et permet d'effectuer des actions dessus.\n" << std::endl;
	std::cout << "\033[4mDéplacement:\033[0m\n" << std::endl;
	std::cout << "    \033[1mflèche du haut\033[0m   : Se déplacer vers le haut de la liste." << std::endl;
	std::cout << "    \033[1mflèche du bas\033[0m    : Se déplacer vers le bas de la liste." << std::endl;
	std::cout << "    \033[1mflèche de gauche\033[0m : Se déplacer à la page précédente." << std::endl;
	std::cout << "    \033[1mflèche de droite\033[0m : Se déplacer à la page suivante.\n" << std::endl;
	std::cout << "\033[4mCommande de la liste interactive:\033[0m\n" << std::endl;
	std::cout << "    \033[1m`enter`\033[0m : Ouvre le fichier présélectionné." << std::endl;
	std::cout << "        \033[1m`t`\033[0m : Affiche les tags du fichier présélectionné." << std::endl;
	std::cout << "        \033[1m`a`\033[0m : Ajoute les tags spécifié (séparé par des espaces)" << std::endl;
	std::cout << "              au fichier présélectionné." << std::endl;
	std::cout << "              Press enter sans tags pour annuler." << std::endl;
	std::cout << "        \033[1m`d`\033[0m : Supprime les tags spécifié (séparé par des espaces)" << std::endl;
	std::cout << "              au fichier présélectionné." << std::endl;
	std::cout << "              Press enter sans tags pour annuler." << std::endl;
	std::cout << "        \033[1m`s`\033[0m : Sauvegarder les modifications." << std::endl;
	std::cout << "              PS : Sauvegarde automatique à la fermeture." << std::endl;
	std::cout << "        \033[1m`q`\033[0m : Retourne au menu.\n" << std::endl;
	stopMessage("");
}

void consolSearchByName(TagFile &tagsObj)
{
	std::string searchLine;
	std::cout << "Enter search line : " << std::flush;
	getline(std::cin, searchLine);
	if (!searchLine.empty())
	{
		std::vector<std::string> searchRet = searchFiles(searchLine, tagsObj.getFileVec(), SEARCH_LIMIT);
		if (searchRet.empty())
			stopMessage("No results");
		else
			interactiveMenu("Results for [ " + searchLine + " ] :", searchRet, optionTagFile, tagsObj, 'q');
	}
	// else
	// {
	// 	stopMessage("No input");
	// }
}

void	consolSearchByTag(TagFile &tagsObj)
{
	std::string	tagsLine;
	std::vector<std::string> tagsVec;
	std::cout << "<tag> for mandatory tag\nop::<tag> for optionnal tag\nno::<tag> for prohibited tag\nEnter one or more Tags separated by a space : " << std::flush;
	getline(std::cin, tagsLine);
	tagsVec = splitVec(tagsLine);
	if (!tagsVec.empty())
	{
		std::vector<std::string> ret = tagsObj.searchByTagAll(tagsVec);
		if (ret.empty())
			stopMessage("No results");
		else
			interactiveMenu("Results for [ " + tagsLine + " ] :", ret, optionTagFile, tagsObj, 'q');
	}
	//else
	//{
	// 	stopMessage("No input");
	//}
}

int	optionTagMenu(int cursor, TagFile &tagsObj)
{
	switch (cursor)
	{
		case 0:
			tagsObj.memStrVec.pop_back();
			break;
		case 1:
			tagsObj.memStrVec.clear();
			break;
		case 2:
			std::string title = "Results for [ ";
			for (auto tagName : tagsObj.memStrVec)
				title += tagName + " ";
			title += "] :";
			interactiveMenu(title, tagsObj.searchByTagAll(tagsObj.memStrVec), optionTagFile, tagsObj, 'q');
			tagsObj.memStrVec.clear();
	}
	return 1;
}

int	tagList(std::string &tag, TagFile &tagsObj, char c)
{
	std::string newTag = tag;
	std::string title = "Actual tag(s) :\n";
	std::vector<std::string> optionsMenu{"Cancel last tag", "Cancel all tags", "Search", "Continue"};
	if (c == 's')
	{
		std::cout << "Saving..." << std::endl;
		tagsObj.saveTags();
	}
	if (c == 'a' || c == '\n' || c == 'n' || c == 'o')
	{
		if (c == 'n')
			newTag = "no::" + newTag;
		else if (c == 'o')
			newTag = "op::" + newTag;
		tagsObj.memStrVec.push_back(newTag);
		for (auto tagName : tagsObj.memStrVec)
			title += tagName + " ";
		interactiveMenu(title, optionsMenu, optionTagMenu, tagsObj, 'q');
	}
	if (c == 'd')
	{
		std::cout << "You will remove the [ " << tag << " ] tag link from all files.\nAre you sure ?\nEnter \033[1m`y`\033[0m for \033[1myes\033[0m or \033[1m`n`\033[0m for \033[1mno\033[0m : " << std::flush;
		read(1, &c, 1);
		if (c == 'y')
			tagsObj.delAllTag(tag);
		read(1, &c, 1);
		std::cout << "Saving..." << std::endl;
		tagsObj.saveTags();
	}
	return 0;
}

void	searchByTagList(TagFile &tagsObj)
{
	tagsObj.memStrVec.clear();
	std::vector<std::string> tags = tagsObj.getRealTagVec();
	interactiveMenu("\033[1m`a`\033[0m or \033[1m`enter`\033[0m for add tag\n\033[1m`n`\033[0m for exclude tag\n\033[1m`o`\033[0m for optional tag\nTags :", tags, tagList, tagsObj, 'q');
}

int	principalMenu(int cursor, TagFile &tagsObj)
{
	std::vector<std::string> fileVec = tagsObj.getFileVec();
	switch (cursor)
	{
		case 0:
			consolSearchByTag(tagsObj);
			break;
		case 1:
			searchByTagList(tagsObj);
			break;
		case 2:
			consolSearchByName(tagsObj);
			break;
		case 3:
			interactiveMenu("Files :", fileVec, optionTagFile, tagsObj, 'q');
			break;
		default:
			printMan();
	}
	return 0;
}

//Cancel last entry

int main(int ac, char **av)
{
	std::string folderName;
	if (ac > 1)
	{
		folderName = av[1];
		if (!accessFolder(folderName))
		{
			std::cerr << "Folder : [" << folderName << "] cannot be opened" << std::endl;
			return 1;
		}
	} else
		folderName = "./";
	TagFile tagsObj(folderName);
	std::vector<std::string> menuVec{"Search by tag", "Search by tag (with list)", "Search by Name", "List all files", "Man"};
	interactiveMenu("Choose mode :", menuVec, principalMenu, tagsObj, 'q');
	std::cout << "Good By ;)" << std::endl;
	return 0;
}


