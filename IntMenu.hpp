#ifndef INTMENU_HPP
# define INTMENU_HPP
# define MAX_LIST_SIZE 20
# include <vector>
# include <string>
# include <termios.h>
# include <iostream>
# include <vector>
# include <unistd.h>
# include <fcntl.h>

// class IntMenu
// {
// public:
//     IntMenu();
//     ~IntMenu();
//     std::string interactiveMenu(std::vector<std::string> list, int max_size);
// private:
// };

class TerminalSettingsGuard {
public:
    TerminalSettingsGuard() {
        this->_hideCursor();
        tcgetattr(STDIN_FILENO, &old_tattr);
        new_tattr = old_tattr;
        new_tattr.c_lflag &= ~(ICANON | ECHO); // Réglages initiaux
        tcsetattr(STDIN_FILENO, TCSANOW, &new_tattr);
    }

    ~TerminalSettingsGuard() {
        restoreDefault();
        this->_showCursor();
    }

    void restoreDefault() {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_tattr);
        this->_showCursor();
    }

    void changeSettings() {
        this->_hideCursor();
        new_tattr.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_tattr);
    }

private:
    struct termios old_tattr, new_tattr;

    void _hideCursor() {
        std::cout << "\e[?25l" << std::flush; // Cache le curseur
    }

    void _showCursor() {
        std::cout << "\e[?25h" << std::flush; // Affiche le curseur
    }
};

void    printLine(bool selected, std::string line, std::string preLine, std::string preLineSelect)
{
	if (selected)
        std::cout << preLineSelect;
    else
        std::cout << preLine;
    std::cout << line << "\033[0m" << std::endl;
}

void printList(std::vector<std::string> list, std::string titleMenu, int cursor, int size) {
    std::cout << "\033[2J\033[1;1H";  // Effacer l'écran
	int i = MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE);
	int pageMaxSize = i + MAX_LIST_SIZE > size ? size : i + MAX_LIST_SIZE;
    if (!titleMenu.empty())
		std::cout << titleMenu << std::endl;
	std::cout << "\033[34;1;4m" << i + 1 << " to " << pageMaxSize << " for " << size << " result(s)\033[0m" << std::endl;
    while (i < pageMaxSize) {
        printLine(cursor == i, list[i], "   ", "\033[32;1m--> \033[4m");
		++i;
    }
}

void printMenu(std::vector<std::string> list, std::string titleMenu, int cursor, int size) {
    std::cout << "\033[2J\033[1;1H";  // Effacer l'écran
	int i = MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE);
	int pageMaxSize = i + MAX_LIST_SIZE > size ? size : i + MAX_LIST_SIZE;
	if (!titleMenu.empty())
		std::cout << titleMenu << std::endl;
    while (i < pageMaxSize) {
        printLine(cursor == i, list[i], "   ", "\033[32;1m--> \033[4m");
		++i;
    }
}

template<typename T>
void interactiveMenu(std::string titleMenu, std::vector<std::string> list, int (*fonctionPtr)(int, T&), T& objet, char quitChar) {
	if (list.empty())
	{
		std::cout << "\033[2J\033[1;1H" << "\033[34;1;4mNo results\033[0m\nPress enter to continue" << std::endl;
		read(1, NULL, 1);
		return;
	}
    TerminalSettingsGuard guard;
    int		cursor = 0;
	int 	size = list.size();
	int		funcRet;
    printMenu(list, titleMenu, cursor, size);
    while (true) {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
            if (c == '\033') {  // caractère d'échappement
                read(STDIN_FILENO, &c, 1);  // skip '['
                read(STDIN_FILENO, &c, 1);  // lire le caractère suivant
                if (c == 'A') {  // flèche haut
                    if (cursor % MAX_LIST_SIZE > 0)
                        --cursor;
                    else
					{
                        cursor = MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE + 1) - 1;
						if (cursor > size - 1)
							cursor = size - 1;
					}
                } else if (c == 'B') {  // flèche bas
                    if (cursor + 1 < size && cursor + 1 < MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE + 1))
                        ++cursor;
                    else
                        cursor = MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE);
                } else if (c == 'C') {  // flèche droite
                    if (MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE + 1) < size)
                        cursor = MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE + 1);
                    else
                        cursor = 0;
                } else if (c == 'D') {  // flèche gauche
                    if (cursor - MAX_LIST_SIZE >= 0)
                        cursor = (cursor / MAX_LIST_SIZE - 1) * MAX_LIST_SIZE;
                    else
                        cursor = (size / MAX_LIST_SIZE) * MAX_LIST_SIZE;
                }
            } else if (c == quitChar) {
				return ;
			} else if (c == '\n') {
				std::cout << "\033[2J\033[1;1H";
				guard.restoreDefault();
				funcRet = fonctionPtr(cursor, objet);
				guard.changeSettings();
				if (funcRet)
					return;
			}
			if (c == '\n' || c == 'A' || c == 'B' || c == 'C' || c == 'D')
				printMenu(list, titleMenu, cursor, size);
        }
    }
}

template<typename T>
void interactiveMenu(std::string titleMenu, std::vector<std::string> list, int (*fonctionPtr)(std::string&, T&, char), T& objet, char quitChar) {
	if (list.empty())
	{
		std::cout << "\033[2J\033[1;1H" << "\033[34;1;4mNo results\033[0m\nPress enter to continue" << std::endl;
		read(1, NULL, 1);
		return;
	}
    TerminalSettingsGuard guard;
    int		cursor = 0;
	int 	size = list.size();
	int		funcRet;
    printList(list, titleMenu, cursor, size);
    while (true) {
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0) {
            if (c == '\033') {  // caractère d'échappement
                read(STDIN_FILENO, &c, 1);  // skip '['
                read(STDIN_FILENO, &c, 1);  // lire le caractère suivant
                if (c == 'A') {  // flèche haut
                    if (cursor % MAX_LIST_SIZE > 0)
                        --cursor;
                    else
					{
                        cursor = MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE + 1) - 1;
						if (cursor > size - 1)
							cursor = size - 1;
					}
                } else if (c == 'B') {  // flèche bas
                    if (cursor + 1 < size && cursor + 1 < MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE + 1))
                        ++cursor;
                    else
                        cursor = MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE);
                } else if (c == 'C') {  // flèche droite
                    if (MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE + 1) < size)
                        cursor = MAX_LIST_SIZE * (cursor / MAX_LIST_SIZE + 1);
                    else
                        cursor = 0;
                } else if (c == 'D') {  // flèche gauche
                    if (cursor - MAX_LIST_SIZE >= 0)
                        cursor = (cursor / MAX_LIST_SIZE - 1) * MAX_LIST_SIZE;
                    else
                        cursor = (size / MAX_LIST_SIZE) * MAX_LIST_SIZE;
                }
            } else if (c == quitChar) {
				return ;
			} else {
				std::cout << "\033[2J\033[1;1H";
				guard.restoreDefault();
				funcRet = fonctionPtr(list[cursor], objet, c);
				guard.changeSettings();
				if (funcRet)
					return;
			}
            printList(list, titleMenu, cursor, size);
        }
    }
}

#endif
