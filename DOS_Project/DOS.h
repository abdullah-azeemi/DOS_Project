#pragma once
#include"Utility.h"
#include"TextEditor.h"
struct file;
struct dir { //folder

	string dirName;
	string creationTime;

	dir* parent;
	list<dir*>childern; // subFolders
	list<file*>files;

	list<dir*> getChildern() {
		return childern;
	}
	dir* getParent() {
		if (parent) {
			return parent;
		}
	}
	string getName() {
		return dirName;
	}
};

struct file { //file

	string txtName;
	string creationTime;
	vector<string>content; // actuall file text

	string extension;
	dir* parent;
	bool isHighPriority;

	string getName() {
		return txtName;
	}
	
};
struct CompareFiles {
	bool operator()(const file* f1, const file* f2) const {
		return f1->isHighPriority > f2->isHighPriority;
	}
};

class DOS {

	string command;
	string txtName;
	textEditor editor;
	priority_queue<file*, vector<file*>, CompareFiles> priorityPrintQ;
	queue<file*> printQ;

	bool isPromt;
	string symPromt;

	//----------- private functions------------///
	void pDirectoryhelper(dir* currentDir, string sym) {
		if (currentDir == root) {
			cout << curr->getName() << sym;
			return;
		}
		pDirectoryhelper(currentDir->getParent(), sym);
		cout << currentDir->getName() << "\\";
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
	void displayDIRHelper(dir* currentDir) {
		for (dir* subDir : currentDir->getChildern()) {
			cout << currentDir->creationTime << " <DIR> " << subDir->dirName << endl;
			displayDIRHelper(subDir);
		}
		for (file* f : currentDir->files) {
			cout << f->creationTime << "        " << f->txtName << "    " << f->extension << endl;
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
	bool FINDSTRhelper(file* targetFile, string textString) {
		for (const string& line : targetFile->content) {
			if (line.find(textString) != string::npos) {
				return true;
			}
		}

		return false;
	}
	dir* loadDirectory(ifstream& inputFile, dir* parentDir, int numFolders) {
		dir* currentDir = new dir();
		currentDir->parent = parentDir;
		getline(inputFile, currentDir->dirName);
		getline(inputFile, currentDir->creationTime);
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
		getline(inputFile, newFile->creationTime);
		getline(inputFile, newFile->extension); // for file Extensions

		string line;
		while (getline(inputFile, line)) {     // for each file conetnt line
			newFile->content.push_back(line);
		}
		return newFile;
	}
	void saveDirectory(ofstream& wrt, dir* currentDir) {
		wrt << currentDir->dirName << endl;
		wrt << currentDir->creationTime << endl;
		wrt << currentDir->getChildern().size() << endl;
		for (dir* subDir : currentDir->getChildern()) {
			saveDirectory(wrt, subDir);
		}
		wrt << currentDir->files.size() << endl;
		for (file* f : currentDir->files) {
			saveFile(wrt, f);
		}
	}
	void saveFile(ofstream& wrt, file* currentFile) {
		wrt << currentFile->txtName << endl;
		wrt << currentFile->creationTime << endl;
		wrt << currentFile->extension << endl;
		for (const string& line : currentFile->content) {
			wrt << line << endl;
		}
	}
	void deleteDirectory(dir* targetDir) {
		for (dir* subDir : targetDir->childern) {
			deleteDirectory(subDir);
		}

		for (file* f : targetDir->files) {
			deleteFileContent(f);
			delete f;
		}
		targetDir->childern.clear();
	}
	void deleteFileContent(file* targetFile) {
		targetFile->content.clear();
	}
	void printWorkingDirectoryHelper(dir* currentDir, string sym) {
		if (currentDir == root) {
			cout << currentDir->getName() << sym;
			return;
		}
		printWorkingDirectoryHelper(currentDir->getParent(), sym);
		cout << currentDir->getName() << "\\";
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


public:

	dir* root; // main C: folder
	dir* curr;
	vector<string> fileNames; // Contains the name of all Files, so when inserting check for uniqueness
	string Version;
	string Credits;

	
public:
	DOS():editor(){
		root = new dir{ "C", getTimeDate(), nullptr,{},{}};
		curr = root;
		Version = "DOS -Shell [Version 1.0.1 - Batman & Mouvli]";
		Credits = "(c) ITU corporation . All rights reserved";
		command = " ";
		txtName = "Text.txt";
		isPromt = false;
		symPromt = ":\\";
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
			printWorkingDirectory(symPromt);
			getline(cin, input);
			if (input.size() > 1) {
				istringstream iss(input);
				iss >> command >> argument[0] >> argument[1] >> argument[2];
				std::for_each(command.begin(), command.end(), [](char& ch) {
					ch = std::toupper(ch); });
			}


			if (command == "CD") {
				changeDirectory(argument[0]);
			}
			else if (command == "CD.") {
				printWorkingDirectoy();
			}
			else if (command == "CD..") {
				previousDirectory();
			}
			else if (command == "CD//") {
				changeDirectory_root();
			}
			else if (command == "CONVERT") {
				convert(argument[0], argument[1]);
			}
			else if (command == "DELETE") {
				Delete(root, argument[0]);
			}
			else if (command == "DIR") {
				displayDIR(root);
			}
			else if (command == "EXIT") {
				Exit();
			}
			else if (command == "FIND") {
				FIND(argument[0]);
			}
			else if (command == "FINDF") {
				FINDF(argument[0],argument[1]);
			}
			else if (command == "FINDSTR") {
				FINDSTR(curr, argument[0]);
			}
			else if (command == "FORMAT") {
				format();
			}
			else if (command == "LOADTREE") {
				loadTree();
			}
			else if (command == "SAVE") {
				SAVE();
			}
			else if (command == "MKDIR") {
				makeDirectory(argument[0]);
			}
			else if (command == "PWD") {
				printWorkingDirectory();
				cout << endl;
			}
			else if (command == "RENAME") {
				rename(argument[0], argument[1]);
			}
			else if (command == "VER") {
				version();
			}
			else if (command == "ATTRIB") {
				attribute(curr);
			}
			else if (command == "CLS") {
				clearScreen();
			}
			else if (command == "COPY") {
				Copy(argument[0]);
			}
			else if (command == "CREATE") {
				create(argument[0]);
			}
			else if (command == "EDIT") {
				edit(argument[0]);
			}
			else if (command == "MOVE") {
				move(argument[0], argument[1]);
			}
			else if (command == "PRINT") {
				PRINT(argument[0]); // argumnet here is the name of the file 
			}
			else if (command == "PPRINT") {
				PPRINT(argument[0]); // argumnet here is the name of the file 
			}
			else if (command == "QUEUE") {
				QUEUE();
			}
			else if (command == "PQUEUE") {
				pQUEUE();
			}
			else if (command == "HELP") {
				HELP();
			}
			else if (command == "RMDIR") {
				removeDirectory(argument[0]);
			}
			else if (command == "PROMT") {
				PROMT(argument[0]);
			}
			else if (command == "PRINTTREE") {
				printTree(root);
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
	
	// Command : DELETE 
	void Delete(dir* root,string txtName) {
		dir* temp = root;
		deleteFileHelper(temp, txtName);
		fileNames.erase(remove(fileNames.begin(), fileNames.end(), txtName), fileNames.end());
	}
	// Command : DIR
	void displayDIR(dir*root) {
		dir* temp = root;
		displayDIRHelper(temp);
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
	// Command : FORMAT
	void format() {
		cout << "\n\n Formating ..... " << endl;
		root->childern.clear();
		fileNames.clear();
		clearScreen();
	}

	// Command : LOADTREE
	void loadTree() {
		ifstream rdr("Text.txt");
		string rootName;
		getline(rdr, rootName);
		int numFolders;
		rdr >> numFolders;
		rdr.ignore(); 
		root = loadDirectory(rdr, root, numFolders);
		curr = root;
	}
	// Command : SAVE
	void SAVE() {
		ofstream wrt(txtName);
		wrt << root->dirName << endl;
		wrt << root->getChildern().size() << endl;
		saveDirectory(wrt, root);
	}
	
	// Command : MKDIR
	void makeDirectory(string directoryName) {
		auto dirExists = std::ranges::find(fileNames, directoryName);

		if (dirExists == fileNames.end()) {
			string _time = getTimeDate();
			dir* newDir = new dir{ directoryName,  _time,  curr, {},{} };
			curr->childern.push_back(newDir);
		}
		else {
			cout << "File " << directoryName << " already exsists. " << endl;
		}
	}
	// Command : RMDIR
	void removeDirectory(string directoryName) {
		auto dirExists = std::ranges::find_if(curr->childern, [directoryName](const dir* d) {
			return d->dirName == directoryName;
			});

		if (dirExists != curr->childern.end()) {
			dir* targetDir = *dirExists;
			deleteDirectory(targetDir); 
			curr->childern.erase(dirExists); 
			cout << "Directory " << directoryName << " and its contents removed." << endl;
		}
		else {
			cout << "Directory " << directoryName << " does not exist." << endl;
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
	
	// Command : CLS
	void clearScreen() {
		system("cls");
		displayHeader(Version, Credits);
		gotoRowCol(5, 0);
	}
	// Command : COPY
	void Copy(string fName) {
		auto fileIter = find_if(curr->files.begin(), curr->files.end(),
			[fName](file* f) { return f->getName()  == fName; });

		if (fileIter != curr->files.end()) {
			file* sourceFileObj = *fileIter;
			file* copyFile = new file{ *sourceFileObj };
			copyFile->parent = curr;
			curr->files.push_back(copyFile);
			cout << "File copied  from " << curr->dirName << "\\" << fName
				<< " to " << curr->dirName << "\\" << fName << endl;
		}
		else {
			cout << "Source file not  found : " << fName << endl;
		}
	}
	// Command : CREATE
	/*
	void create(string fName, string createFname) {
		ifstream rdr(createFname);
		auto fileIter = find_if(curr->files.begin(), curr->files.end(),
			[fName](file* f) { return f->getName() == fName; });

		if (fileIter == curr->files.end()) {
			file* newFile = new file{ fName, getTimeDate(), {}, "", curr};
			string line;
			while (getline(rdr, line)) {
				newFile->content.push_back(line);
			}
			curr->files.push_back(newFile);
			cout << "File created: " << curr->dirName << "\\" << fName << endl;
		}
		else {
			cout << "File already exists: " << curr->dirName << "\\" << fName << endl;
		}
	}
	*/
	void create(string fName) {
		auto fileIter = find_if(curr->files.begin(), curr->files.end(),
			[fName](file* f) { return f->getName() == fName; });

		if (fileIter == curr->files.end()) {
			file* newFile = new file{ fName, getTimeDate(), {}, "", curr };
			editor.editFile(fName);
			string format = fName;
			format += ".txt";
			ifstream reader(format);
			while (reader)
			{
				string dummy = "";
				getline(reader, dummy);
				newFile->content.push_back(dummy);
				dummy.clear();
			}
			curr->files.push_back(newFile);
			cout << "File created: " << curr->dirName << "\\" << fName << endl;
		}
		else {
			cout << "File already exists: " << curr->dirName << "\\" << fName << endl;
		}
	}

	//Command::EDIT
	void edit(string fName) {
		auto fileIter = find_if(curr->files.begin(), curr->files.end(),
			[fName](file* f) { return f->getName() == fName; });

		if (fileIter != curr->files.end()) {
			//file* newFile = new file{ fName, getTimeDate(), {}, "", curr };
			editor.editFile(fName,true);
			string format = fName;
			format += ".txt";
			ifstream reader(format);
			while (reader)
			{
				string dummy = "";
				getline(reader, dummy);
				(*fileIter)->content.clear();
				(*fileIter)->content.push_back(dummy);
				dummy.clear();
			}
			cout << "File Edited: " << curr->dirName << "\\" << fName << endl;
		}
		else {
			cout << "Such File Does Not exists: " << curr->dirName << "\\" << fName << endl;
		}
	}


	// Command : MOVE
	void move(string fName, string destinationDirectory) {
		auto fileIter = find_if(curr->files.begin(), curr->files.end(),
			[fName](file* f) { return f->getName() == fName; });

		if (fileIter != curr->files.end()) {
			auto destDirIter = find_if(curr->childern.begin(), curr->childern.end(),
				[destinationDirectory](dir* d) { return d->dirName == destinationDirectory; });

			if (destDirIter != curr->childern.end()) {
				dir* destinationDirObj = *destDirIter;
				file* fileToMove = *fileIter;
				curr->files.erase(fileIter);
				fileToMove->parent = destinationDirObj;
				destinationDirObj->files.push_back(fileToMove);

				cout << "File moved from dir :  " << curr->dirName << "\\" << fName
					<< " to " << destinationDirObj->dirName << "\\" << fName << endl;
			}
			else {
				cout << "Destination directory : " << destinationDirectory << "not found " << endl;
			}
		}
		else {
			cout << "Source file not found: " << fName << endl;
		}
	}

	// Command : PRINT
	void PRINT(string textFileq) {
		dir* temp = root;
		file * file = findFile(root, textFileq);
		if (file) {
			printQ.push(file);
			cout << "File : " << file->getName() << " is pushed to the printing queue.. "  << endl;
		}
		else {
			cout << "File : " << txtName << " not found: " << endl;
		}
	}
	// Command : QUEUE
	void QUEUE() {
		cout << "Print Queue:" << endl;
		queue<file*> tempQueue = printQ;
		while (!tempQueue.empty()) {
			cout << tempQueue.front()->getName() << endl;
			tempQueue.pop();
		}
		cout << "______________________________ " << endl;
	}
	// Command : PPRINT
	void PPRINT(string textFilepq) {
		dir* temp = root;
		file* file = findFile(root, textFilepq);
		if (file) {
			priorityPrintQ.push(file);
			
			cout << "File : " << file->getName() << " is pushed to the priority printing queue.. " << endl;
		}
		else {
			cout << "File : " << txtName << " not found: "  << endl;
		}
	}
	// Command : PQUEUE
	void pQUEUE() {
		cout << "Print Priority Queue:" << endl;
		auto tempQueue = priorityPrintQ; 
		while (!tempQueue.empty()) {
			file* currentFile = tempQueue.top();
			tempQueue.pop();
			cout << currentFile->getName() << " (Priority -> " << ((currentFile->isHighPriority ? "High" : "Low")) << ")" << endl;
		}
		cout << "__________________________" << endl;
	}
	
	// Command : HELP
	void HELP() {
		format();
		fstream rdr("Help.txt");
		int size; string line;
		rdr >> size;
		for (int itr = 0; itr < size; itr++) {
			getline(rdr, line);
			cout << line << endl;
		}
		displayDIR(root);
	}
	
	// Command : PROMT
	void PROMT(string _symPromt) {
		if (!isPromt) {
			isPromt = true;
			symPromt = _symPromt;
		}
		else { // reset to deafualt
			isPromt = false;
			symPromt = ":\\";
		}
	}
	// Command : PRINTTREE
	void printTree(dir* currentDir, int depth = 0) {
		cout << std::setw(depth * 4) << ""; 
		cout << "+-- " << currentDir->dirName << "/\n";
		for (const auto& file : currentDir->files) {
			cout << std::setw((depth + 1) * 4) << "";
			cout << "|-- " << file->getName() << "\n";
		}
		for (const auto& subDir : currentDir->getChildern()) {
			printTree(subDir, depth + 1);
		}
	}

};

