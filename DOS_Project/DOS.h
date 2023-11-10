#pragma once
#include"Utility.h"

struct file;
struct dir { //folder

	string dirName;
	time_t creationTime;

	dir* parent;
	list<dir*>childern; // subFolders
	vector<file*>files;

	list<dir*> getChildern() {
		return childern;
	}
	dir* getParent() {
		return parent;
	}
	string getName() {
		return dirName;
	}
};

struct file { //file

	string txtName;
	time_t creationTime;
	vector<string>content; // actuall file text

	string extension;
	dir* parent;

	string getName() {
		return txtName;
	}
	
};

class DOS {

	string command;
	string txtName;

public:

	dir* root; // main C: folder
	dir* curr;
	vector<string> fileNames; // Contains the name of all Files, so when inserting check for uniqueness
	string Version;
	string Credits;


public:
	DOS() {
		root = new dir{ "C", time(nullptr), nullptr,{},{} };
		curr = root;

		Version = "DOS -Shell [Version 1.0.1 - Batman & Mouvli]";
		Credits = "(c) ITU corporation . All rights reserved";
		command = " ";
		txtName = "Text.txt";
	}

	dir* getRoot() {
		return root;
	}
	dir* getCurr() {
		return curr;
	}
	
	void run() {
		clearScreen();
		string input;
		vector<string>argument(3);
	
		while (true) {
			argument.resize(3);
			printWorkingDirectory();
			getline(cin, input);
			if (input.size() > 1) {
				istringstream iss(input);
				iss >> command >> argument[0] >> argument[1] >> argument[2];
				std::for_each(command.begin(), command.end(), [](char& ch) {
					ch = std::tolower(ch); });
			}


			if (command == "cd") {
				changeDirectory(argument[0]);
			}
			else if (command == "cd.") {
				printWorkingDirectoy();
			}
			else if (command == "cd..") {
				previousDirectory();
			}
			else if (command == "cd//") {
				changeDirectory_root();
			}
			else if (command == "convert") {
				convert(argument[0], argument[1]);
			}
			else if (command == "delete") {
				Delete(root, argument[0]);
			}
			else if (command == "dir") {
				displayDIR(root);
			}
			else if (command == "exit") {
				Exit();
			}
			else if (command == "find") {
				FIND(argument[0]);
			}
			else if (command == "findf") {
				FINDF(argument[0],argument[1]);
			}
			else if (command == "findstr") {
				FINDSTR(curr, argument[0]);
			}
			else if (command == "format") {
				format();
			}
			else if (command == "loadtree") {
				loadTree();
			}
			else if (command == "save") {
				SAVE();
			}
			else if (command == "mkdir") {
				makeDirectory(argument[0]);
			}
			else if (command == "pwd") {
				printWorkingDirectory();
				cout << endl;
			}
			else if (command == "rename") {
				rename(argument[0], argument[1]);
			}
			else if (command == "ver") {
				version();
			}
			else if (command == "attrib") {
				attribute(curr);
			}
			else if (command == "cls") {
				clearScreen();
			}
			
			else if(command.size() > 2) {
				cout << "'" <<command << "'" <<
				" is not recognized as an internal or external command, operatable program or batch file"<< endl;
			}
			if(command.size() != 0)
			    command.clear();
			if(argument.size() != 0)
			  argument.clear();

		}
	}

	// Command : CD
	void changeDirectory(string directoryName) {  

		for (dir* child : curr->childern) {
			if (child->dirName == directoryName) {
				curr = child;
				return;
			}
		}
		cout << "\n " << directoryName << " no such directory exsists in " << curr->getName()<<endl;
	}
	
	// Command : CD.
	void printWorkingDirectoy(string sym= ":\\", char endSym = '>') {
		pDirectoryhelper(curr, sym);
		cout << endSym;
	}
	void pDirectoryhelper(dir* currentDir,string sym ) {
		if (currentDir == root) {
			cout << curr->getName() << sym;
			return;
		}
		pDirectoryhelper(currentDir->getParent(), sym);
		cout << currentDir->getName() << "\\";
	}

	// Command : CD..
	void previousDirectory() {
		if (curr->parent) {
			curr = curr->getParent();
		}
	}
 
	//Command : CD(slash)
	void changeDirectory_root() {
		curr = root;
	}

	// Command : CONVERT
	void convert(string extension1, string extension2) {
		dir* temp = root;
		convertHelper(temp, extension1, extension2);
	}
	void convertHelper(dir* currentDir, string extension1, string extension2) {
		for (file* f : currentDir->files) {
			if (f->extension == extension1) {
				f->extension = extension2;
				cout << "Converted file: " << f->txtName << " to " << extension2 << " in " << currentDir->dirName << endl;
			}
		}
		for (dir* subDir : currentDir->getChildern()) {
			convertHelper(subDir, extension1, extension2);
		}
	}

	// Command : DELETE 
	void Delete(dir* root,string txtName) {
		dir* temp = root;
		deleteFileHelper(temp, txtName);
		fileNames.erase(remove(fileNames.begin(), fileNames.end(), txtName), fileNames.end());
	}
	void deleteFileHelper(dir* currentDir, string txtName) {
		auto fileIter = find_if(currentDir->files.begin(), currentDir->files.end(),
			[txtName](file* f) { return f->txtName == txtName; });
		if (fileIter != currentDir->files.end()) {
			cout << "Deleted file: " << txtName << " in " << currentDir->dirName << endl;
			currentDir->files.erase(fileIter);
			return;
		}
		for (dir* subDir : currentDir->getChildern()) {
			deleteFileHelper(subDir, txtName);
		}
	}

	// Command : DIR
	void displayDIR(dir*root) {
		dir* temp = root;
		displayDIRHelper(temp);
	}
	void displayDIRHelper(dir* currentDir) {
		for (dir* subDir : currentDir->getChildern()) {
			cout << currentDir->creationTime << " <DIR> " << subDir->dirName << endl;
			displayDIRHelper(subDir);
		}
		for (file* f : currentDir->files) {
			cout << f->creationTime << "        " << f->txtName << "    " << f->extension << endl;
		}
	}

	// Command : EXIT (My Favourite ;)  )
	void Exit() {
		SAVE();
		exit(0);
	}

	// Command : FIND
	void FIND(string txtName) {
		dir* temp = root;
		if (findFileHelper(temp, txtName)) {
			cout << "File found: " << txtName << " in " << temp->dirName << endl;
		}
		else {
			cout << "File not found: " << txtName << endl;
		}
	}
	bool findFileHelper(dir* currentDir, string txtName) {
		auto fileIter = find_if(currentDir->files.begin(), currentDir->files.end(), // STL power bro!!
			[txtName](file* f) { return f->txtName == txtName; });
		if (fileIter != currentDir->files.end()) {
			return true; 
		}
		for (dir* subDir : currentDir->getChildern()) {
			if (findFileHelper(subDir, txtName)) {
				return true; 
			}
		}
		return false; 
	}

	// Command : FINDF
	void FINDF(string txtName, string textString) {
		file* requiredFile = findFile(curr, txtName);
		if (requiredFile != nullptr) {
			FINDFhelper(requiredFile, textString);
		}
		else {
			cout << "File not found: " << txtName << endl;
		}
	}
	void FINDFhelper(file* targetFile, string textString) {
		cout << "Searching for \"" << textString << "\" in file: " << targetFile->txtName << endl;
		bool found = false;
		for (const string& line : targetFile->content) {
			if (line.find(textString) != string::npos) {
				found = true;
				cout << "Found in line: " << line << endl;
			}
		}
		if (!found) {
			cout << "Text string not found in the file." << endl;
		}
	}
	file* findFile(dir* currentDir, string txtName) {
		auto fileIter = find_if(currentDir->files.begin(), currentDir->files.end(),
			[txtName](file* f) { return f->txtName == txtName; });

		if (fileIter != currentDir->files.end()) {
			return *fileIter; 
		}
		for (dir* subDir : currentDir->getChildern()) {
			file* foundFile = findFile(subDir, txtName);
			if (foundFile) {
				return foundFile; 
			}
		}
		return nullptr;
	}

	// Command : FINDSTR
	void FINDSTR(dir* currentDir, string textString) {
		stack<file*> fileStack;
		for (file* f : currentDir->files) {
			fileStack.push(f);
		}
		while (!fileStack.empty()) {
			file* currentFile = fileStack.top();
			fileStack.pop();

			if (FINDSTRhelper(currentFile, textString)) {
				cout << "Found in file : " << currentFile->txtName << "in " << currentDir->dirName << endl;
			}
			for (dir* subDir : currentDir->getChildern()) {
				for (file* f : subDir->files) {
					fileStack.push(f);
				}
			}
		}
	}
	bool FINDSTRhelper(file* targetFile, string textString){
		for (const string& line : targetFile->content) {
			if (line.find(textString) != string::npos) {
				return true;
			}
		}

		return false;
	}

	// Command : FORMAT
	void format() {
		cout << "\n\n Formating ..... " << endl;
		root->childern.clear();
		fileNames.clear();
		clearScreen();
	}

	// Command : HELP plz i am ladis
	

	// Command : LOADTREE
	void loadTree() {
		ifstream rdr(txtName);
		string rootName;
		getline(rdr, rootName);
		int numFolders;
		rdr >> numFolders;
		rdr.ignore(); 
		root = loadDirectory(rdr, root, numFolders);
	}
	dir* loadDirectory(ifstream& inputFile, dir* parentDir, int numFolders) {
		dir* currentDir = new dir();
		currentDir->parent = parentDir;
		getline(inputFile, currentDir->dirName);
		int numSubfolders;
		inputFile >> numSubfolders;
		inputFile.ignore(); 

		for (int i = 0; i < numSubfolders; ++i) {
			currentDir->childern.push_back(loadDirectory(inputFile, currentDir, numFolders));
		}

		int noFiles;
		inputFile >> noFiles;
		inputFile.ignore(); 

		for (int i = 0; i < noFiles; ++i) {
			file* newFile = loadFile(inputFile, currentDir);
			currentDir->files.push_back(newFile);
			fileNames.push_back(newFile->txtName); 
		}

		return currentDir;
	}
	file* loadFile(ifstream& inputFile, dir* parentDir) {
		file* newFile = new file();
		newFile->parent = parentDir;

		getline(inputFile, newFile->txtName); // for file Name
		getline(inputFile, newFile->extension); // for file Extensions

		string line;
		while (getline(inputFile, line)) {     // for each file conetnt line
			newFile->content.push_back(line);
		}
		return newFile;
	}

	// Command : SAVE
	void SAVE() {
		ofstream wrt(txtName);
		wrt << root->dirName << endl;
		wrt << root->getChildern().size() << endl;
		saveDirectory(wrt, root);
	}
	void saveDirectory(ofstream& wrt, dir* currentDir) {
		wrt << currentDir->dirName << endl;

		wrt << currentDir->getChildern().size() << endl;
		for (dir*  subDir : currentDir->getChildern()) {
			saveDirectory(wrt, subDir);
		}
		wrt << currentDir->files.size() << endl;
		for (file*f : currentDir->files) {
			saveFile(wrt, f);
		}
	}
	void saveFile(ofstream& wrt, file* currentFile) {
		wrt << currentFile->txtName << endl;
		wrt << currentFile->extension << endl;
		for (const string&  line:  currentFile->content) {
			 wrt << line << endl;
		}
	}

	// Command : MKDIR
	void makeDirectory(string directoryName) {
		auto dirExists = std::ranges::find(fileNames, directoryName);

		if (dirExists == fileNames.end()) {
			dir* newDir = new dir{ directoryName,  time(nullptr),  curr, {},{} };
			curr->childern.push_back(newDir);
		}
		else {
			cout << "File " << directoryName << " already exsists. " << endl;
		}
	}

	// Command : PWD
	void printWorkingDirectory(string sym = ":\\", char endSym = '>') {
		if (curr == root) {
			cout << curr->getName() << sym << endSym;
		}
		else {
			printWorkingDirectoryHelper(curr, sym);
			cout << endSym;
		}
	}
	void printWorkingDirectoryHelper(dir* currentDir, string sym) {
		if (currentDir == root) {
			cout << currentDir->getName() << sym;
			return;
		}
		printWorkingDirectoryHelper(currentDir->getParent(), sym);
		cout << currentDir->getName() << "\\";
	}

	// Command : RENAME
	void rename(string fileNamee, string newName) {
		auto fileIter = find_if(curr->files.begin(), curr->files.end(),
			[fileNamee](file* f) { return f->txtName == fileNamee; });

		if (fileIter != curr->files.end()) {
			(*fileIter)->txtName = newName;
			cout << "File renamed... " << endl;
			fileNames.erase(remove(fileNames.begin(), fileNames.end(), txtName), fileNames.end());
			fileNames.push_back(newName);
		}
		else {
			cout << "File not found: " << txtName << endl;
		}
	}

	// Command : VER
	void version() {
		cout << Version << endl;
	}

	// Command : ATTRIB
	void attribute(dir * currentDir) {
		cout << "File attributes in  " << currentDir->dirName << ":" << endl;
		for (file* f : currentDir->files) {
			cout << "File Name: " << f->txtName << endl;
			cout << "File Path: " << getPath(f) << endl;
			cout << "---------------------" << endl;
		}
		for (dir* subDir : currentDir->getChildern()) {
			attribute(subDir);
		}
	}
	string getPath(file* currentFile) {
		string path = currentFile->txtName;
		dir* currentDir = currentFile->parent;
		while (currentDir != nullptr) {
			path = currentDir->dirName + "\\" + path;
			currentDir = currentDir->parent;
		}

		return path;
	}

	// Command : CLS
	void clearScreen() {
		system("cls");
		displayHeader(Version, Credits);
		gotoRowCol(5, 0);
	}

	

};

