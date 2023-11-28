#include "TagFile.hpp"
#include "IntMenu.hpp"
#include "srch.hpp"
#include <sstream>
#include <random>
#define SEARCH_LIMIT 10

std::vector<std::string> splitVec(const std::string &s) {
    std::vector<std::string> result;
    std::istringstream iss(s);
    std::string mot;
    while (iss >> mot)
        result.push_back(mot);
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
		return;
	tagsVec = splitVec(str);
	tagsObj.addLinkedTagFile(line, tagsVec);
}

int	renameFile(std::string &line, TagFile &tagsObj)
{
	std::string					newName;
	std::vector<std::string>	fileTags;
	std::cout << "New name : " << std::flush;
	std::getline(std::cin, newName);
	if (newName.empty())
		return 0;
	if (system(("mv '" + tagsObj.getFolderName() + line + "' '" + tagsObj.getFolderName() + newName + "'").c_str()))
		stopMessage("File [ " + line + " ] cannot be renamed");
	else
	{
		fileTags = tagsObj.getFileTagsVec(line);
		tagsObj.addLinkedTagFile(newName, fileTags);
		tagsObj.refresh();
		return 1;
	}
	return 0;
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
			return;
		tagsVec = splitVec(str);
		for (auto tag : tagsVec)
			tagsObj.delTagInFile(line, tag);
	}
}

int	selectedTagMenu(int cursor, TagFile &tagsObj)
{
	switch (cursor)
	{
		case 1:
			tagsObj.memStrVec.pop_back();
			break;
		case 2:
			tagsObj.memStrVec.clear();
			break;
	}
	return 1;	
}

int	callSelectedTagMenu(std::string &line, TagFile &tagsObj, char c)
{
	if (c == 's')
	{
		std::cout << "Saving..." << std::endl;
		tagsObj.saveTags();
	} else if (c == '\n')
		tagsObj.memStrVec.push_back(line);
	if (c == '\n' || c == 'm')
	{
		std::vector<std::string>	menu{"Continue", "Cancel last selection", "Cancel all selection"};
		std::string title = "Your selection :";
		for (auto tag : tagsObj.memStrVec)
			title += " " + tag;
		interactiveMenu(title, menu, selectedTagMenu, tagsObj, 'q');
	}
	return 0;
}

int	optionTagFile(std::string &line, TagFile &tagsObj, char c)
{
	if (c == 's')
	{
		std::cout << "Saving..." << std::endl;
		tagsObj.saveTags();
	}
	else if (c == 'r')
		return renameFile(line, tagsObj);
	else if (c == 'S')
	{
		std::cout << "Refresh..." << std::endl;
		tagsObj.refresh();
		return 1;
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
	} else if (c == 'A' || c == 'D')
	{
		std::string title;
		if (c == 'd')
			title = "\033[1mDEL\033[0m MODE :\n";
		else
			title = "\033[1mADD\033[0m MODE :\n";
		title += "Press \033[1m`enter`\033[m to select tag or \033[1m`m`\033[m to go to menu without selection\nPress \033[1m`q`\033[m to validate your selections\nOld tag(s) :";
		for (auto tag : tagsObj.getFileTagsVec(line))
			title += " " + tag;
		tagsObj.memStrVec.clear();
		interactiveMenu(title, tagsObj.getRealTagVec(), callSelectedTagMenu, tagsObj, 'q');
		if (tagsObj.memStrVec.empty())
			return 0;
		if (c == 'D')
		{
			for (auto tag : tagsObj.memStrVec)
				tagsObj.delTagInFile(line, tag);
		} else
			tagsObj.addLinkedTagFile(line, tagsObj.memStrVec);
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
	std::cout << "    \033[1mAleatory by tag:\033[0m" << std::endl;
	std::cout << "        Ouvre un fichier au hasard contenant les tags spécifié.\n" << std::endl;
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
	std::cout << "        \033[1m`r`\033[0m : Renomme le fichier présélectionné." << std::endl;
	std::cout << "        \033[1m`a`\033[0m : Ajoute les tags spécifié (séparé par des espaces)" << std::endl;
	std::cout << "              au fichier présélectionné." << std::endl;
	std::cout << "              Press enter sans tags pour annuler." << std::endl;
	std::cout << "        \033[1m`A`\033[0m : Ajoute les tags spécifié (via une nouvelle liste)" << std::endl;
	std::cout << "        \033[1m`d`\033[0m : Supprime les tags spécifié (séparé par des espaces)" << std::endl;
	std::cout << "              au fichier présélectionné." << std::endl;
	std::cout << "              Press enter sans tags pour annuler." << std::endl;
	std::cout << "        \033[1m`D`\033[0m : Supprime les tags spécifié (via une nouvelle liste)" << std::endl;
	std::cout << "        \033[1m`s`\033[0m : Sauvegarder les modifications." << std::endl;
	std::cout << "              PS : Sauvegarde automatique à la fermeture." << std::endl;
	std::cout << "        \033[1m`S`\033[0m : Sauvegarder les modifications. Et refresh les infos." << std::endl;
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
}

void	aleatoryByTag(TagFile &tagsObj)
{
	std::string	tagsLine;
	std::vector<std::string> tagsVec;
	std::cout << "<tag> for mandatory tag\nop::<tag> for optionnal tag\nno::<tag> for prohibited tag\nEnter one or more Tags separated by a space : " << std::flush;
	getline(std::cin, tagsLine);
	tagsVec = splitVec(tagsLine);
	if (!tagsVec.empty())
	{
		std::random_device rd;  // Obtient un nombre aléatoire pour la graine
		std::default_random_engine engine(rd());
		std::vector<std::string> ret = tagsObj.searchByTagAll(tagsVec);
		if (ret.empty())
			stopMessage("No results");
		else
		{
			std::uniform_int_distribution<size_t> dist(0, ret.size() - 1);
			std::string randomString = ret[dist(engine)];
			if (!system(("open '" + tagsObj.getFolderName() + randomString + "'").c_str()))
				tagsObj.addLinkedTagFile(randomString, "vu");
			else
				stopMessage("File [ " + randomString + " ] cannot be opened");
		}
	}
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
}

int	optionTagMenu(int cursor, TagFile &tagsObj)
{
	std::string title;
	switch (cursor)
	{
		case 1:
			title = "Results for [ ";
			for (auto tagName : tagsObj.memStrVec)
				title += tagName + " ";
			title += "] :";
			interactiveMenu(title, tagsObj.searchByTagAll(tagsObj.memStrVec), optionTagFile, tagsObj, 'q');
			tagsObj.memStrVec.clear();
			break;
		case 2:
			tagsObj.memStrVec.pop_back();
			break;
		case 3:
			tagsObj.memStrVec.clear();
			break;
	}
	return 1;
}

int	tagList(std::string &tag, TagFile &tagsObj, char c)
{
	std::string newTag = tag;
	std::string title = "Actual tag(s) :\n";
	std::vector<std::string> optionsMenu{"Continue", "Search", "Cancel last tag", "Cancel all tags"};
	if (c == 's')
	{
		std::cout << "Saving..." << std::endl;
		tagsObj.saveTags();
	}
	if (c == 'a' || c == '\n' || c == 'n' || c == 'o' || c == 'm')
	{
		if (c != 'm')
		{
			if (c == 'n')
				newTag = "no::" + newTag;
			else if (c == 'o')
				newTag = "op::" + newTag;
			tagsObj.memStrVec.push_back(newTag);
		}
		for (auto tagName : tagsObj.memStrVec)
			title += tagName + " ";
		interactiveMenu(title, optionsMenu, optionTagMenu, tagsObj, 'q');
	}
	if (c == 'd')
	{
		std::cout << "You will remove the [ " << tag << " ] tag link from all files.\nAre you sure ?\nEnter \033[1m`y`\033[0m for \033[1myes\033[0m or \033[1m`n`\033[0m for \033[1mno\033[0m : " << std::flush;
		read(1, &c, 1);
		if (c == 'y')
		{
			tagsObj.delAllTag(tag);
			read(1, &c, 1);
			std::cout << "Saving and refresh..." << std::endl;
			tagsObj.refresh();
			return 1;
		}
		read(1, &c, 1);
	}
	return 0;
}

void	searchByTagList(TagFile &tagsObj)
{
	tagsObj.memStrVec.clear();
	std::vector<std::string> tags = tagsObj.getRealTagVec();
	interactiveMenu("\033[1m`a`\033[0m or \033[1m`enter`\033[0m for add tag\n\033[1m`n`\033[0m for exclude tag\n\033[1m`o`\033[0m for optional tag\n\033[1m`m`\033[0m to go to selection menu without selection\nTags :", tags, tagList, tagsObj, 'q');
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
		case 4:
			aleatoryByTag(tagsObj);
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
	std::vector<std::string> menuVec{"Search by tag", "Search by tag (with list)", "Search by Name", "List all files", "Aleatory by tag", "Man"};
	interactiveMenu("Choose mode :", menuVec, principalMenu, tagsObj, 'q');
	std::cout << "Good By ;)" << std::endl;
	return 0;
}
