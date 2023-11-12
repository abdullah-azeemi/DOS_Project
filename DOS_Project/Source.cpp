#include "Utility.h"
#include"DOS.h"
#include"TextEditor.h"

int mains() {
    DOS dos;

    // Sample commands to demonstrate functionality
    dos.makeDirectory("Documents");
    dos.makeDirectory("Pictures");
    dos.makeDirectory("Music");

    dos.changeDirectory("Documents");
    dos.makeDirectory("TextFiles");

    // Create a file with content
    file* sampleFile = new file{ "file1.txt", getTimeDate(), {"This is the content of file1.", "Line 2", "Line 3"}, "txt", dos.curr};
    dos.getCurr()->files.push_back(sampleFile);
    dos.fileNames.push_back("file1.txt");

    dos.changeDirectory(".."); // Move up to the parent directory
    dos.displayDIR(dos.getRoot());

    dos.convert("txt", "md");

    dos.makeDirectory("Downloads");

    // Create another file with content
    file* anotherFile = new file{ "file2.txt", getTimeDate(), {"This is another file.", "Content goes here.", "More lines."}, "txt", dos.curr};
    dos.curr->files.push_back(anotherFile);
    dos.fileNames.push_back("file2.txt");

    dos.displayDIR(dos.getRoot());

    dos.rename("file1.txt", "newfile.txt");

    dos.displayDIR(dos.getRoot());

    dos.attribute(dos.getCurr());

    dos.FIND("newfile.txt");

    dos.FINDF("newfile.txt", "someText");

    dos.FINDSTR(dos.getRoot(), "searchString");

    //dos.format();

    dos.displayDIR(dos.getRoot());

   

    dos.Exit();  // This should save the changes and exit

    return 0;

}

int mains1()
{
    DOS d;
    d.run();

    return 0;
}


int main()//editor
{
    textEditor t;
    t.editFile();
}