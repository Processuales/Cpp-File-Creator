#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <windows.h>
#include <unordered_map>
#include <vector>
#include <limits>
#include <utility>
#include <unordered_map>
#include <filesystem>
#include <unistd.h>
#include <direct.h>
#include <cstdlib> 




using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::unordered_map;
using std::make_pair;
using std::unordered_map;
using std::vector;
using std::ofstream;
using std::getline;
using std::ifstream;
using std::cerr;

string readerFile, writerFile, currentLine, FileName, creatorPath;
string checkerPath1, checkerPath2; //path of original file, path for the new python file
string temp; bool hidden;
unordered_map<int, string> lineMap;

void reader(); void writer(int lines); 

void clear(){
    std::cout << "\033c" << std::flush; 
}

void checker(const string& path) {
    if (!std::filesystem::exists(path)) {
        clear();
        cout << "Error! There is no file at : "<< "\n" << path << "\n\nMake sure that you have entered the correct path." << endl;
        exit(1);
    } 
}

void transformation(string path, string name){
    namespace fs = std::filesystem;
    string result = "";
    fs::path current = fs::current_path();
    string currentPath = current.string();
    //convert the file that we want to read
    string transformedPath;
    bool prevIsBackslash = false;
    for (int i = 0; i < path.length(); i++) {
        if (path[i] == '\\') {
            if (!prevIsBackslash) {
                transformedPath += "\\\\";
                prevIsBackslash = true;
            }
        } else {
            transformedPath += path[i];
            prevIsBackslash = false;
        }
    }
    //convert the current path
    string transformedCurrentPath;
    prevIsBackslash = false;
    for (int i = 0; i < currentPath.length(); i++) {
        if (currentPath[i] == '\\') {
            if (!prevIsBackslash) {
                transformedCurrentPath += "\\\\";
                prevIsBackslash = true;
            }
        } else {
            transformedCurrentPath += currentPath[i];
            prevIsBackslash = false;
        }
    }
    //add the name of the file
    creatorPath = transformedCurrentPath + "\\\\" + name + ".py";
    transformedCurrentPath = transformedCurrentPath + "\\\\" + name + ".cpp";
    readerFile = transformedPath;
    writerFile = transformedCurrentPath;
}

void txtCreator(int lines){
    //create a txt that copies from the readerFile, but with the necessary changes for each line, thes passes it back to reader
    
    ifstream infile(readerFile);
    if (!infile) {
        cerr << "Error: Text file could not be opened.";
        exit(1);
    }
    char ch; int wordCounter = 0; int quationsFixed = 0;
    while (infile.get(ch)) {
        wordCounter++;
    }
    infile.close();
    cout << "Characters Found : " << wordCounter << "\n";

    for (auto & [key, value] : lineMap){
        for (int i = 0; i < value.size(); ++i){
            if (value[i] == '"'){
                value.insert(i, "\\");
                i++;
                quationsFixed++;
            }
            if (value[i] == '\\'){
                value.insert(i, "\\");
                i++;
                quationsFixed++;
            }
        }
    }
    cout << "Quatations and Backslashes fixed : " << quationsFixed << "\n";
    string name = "temp" + std::to_string(lines) + ".txt";
    ofstream outfile(name);
    if (!outfile) {
        cerr << "Error: Temp file could not be created.\n";
        exit(1);
    }
    int line = 0;
    for (auto pair : lineMap){
        line++;
        outfile << lineMap.at(line) << "\n";
    }
    outfile.close();
    writer(lines);
}

void reader(){
    //read from readerFile
    ifstream infile(readerFile);
    if (!infile.is_open()) {
        cerr << "Error! Could not open the file at : \n" << readerFile << "\nMake sure this is a file!";
        exit(1);
    }
    string line; int lineNumber = 0;
    while (getline(infile, line)){
        ++lineNumber;
        lineMap.insert(make_pair(lineNumber, line));
    }
    infile.close();
    cout << "Lines Found : " << lineNumber << "\n";
    txtCreator(lineNumber);
}

void writer(int lines){
    //write to writerFile
    string txtName = "temp" + std::to_string(lines) + ".txt"; string name = FileName; string path = writerFile; int lineNumber = 0; string line; string temp;
    lineMap.clear();
    ifstream infile(txtName);
    while (getline(infile, line)){
        ++lineNumber;
        lineMap.insert(make_pair(lineNumber, line));
    }
    infile.close();
    std::filesystem::remove(txtName);
    ofstream outfile(path);
    if (!outfile) {
        cerr << "Error: C++ file could not be created. Check parent folder\n";
        exit(1);
    }
    lineNumber = 0;
    outfile << "#include <iostream>" << "\n";
    outfile << "#include <fstream>" << "\n";
    outfile << "#include <string>" << "\n";
    if (hidden == true){
        outfile << "#include <windows.h>" << "\n";
        outfile << "#include <locale>" << "\n";
        outfile << "#include <codecvt>" << "\n";
    }
    outfile << "void fileCreator(); int main();" << "\n";
    if (hidden == true){
        outfile << "void fileHider(std::string filePath){" << "\n";
        outfile << "    std::wstring wideFilePath = std::wstring(filePath.begin(), filePath.end());" << "\n";
        outfile << "    DWORD attributes = GetFileAttributesW(wideFilePath.c_str());" << "\n";
        outfile << "    SetFileAttributesW(wideFilePath.c_str(), attributes + FILE_ATTRIBUTE_HIDDEN);" << "\n";
        outfile << "}" << "\n";
    }
    outfile << "int main(){" << "\n";
    outfile << "    fileCreator();" << "\n";
    outfile << "    return -1;" << "\n";
    outfile << "}" << "\n";
    outfile << "void fileCreator(){" << "\n";
    temp = "    std::string filePath = \"" + creatorPath + "\";"; outfile << temp << "\n";
    outfile << "    std::ofstream file(filePath);" << "\n";
    for (auto pair : lineMap){
        lineNumber++;
        temp = lineMap.at(lineNumber);
        if (lineNumber != lines) temp = "    file << \"" + temp + "\" << \"" + "\\n\";";
        else temp = "    file << \"" + temp + "\";";
        outfile << temp << "\n";
    }
    outfile << "    file.close();" << "\n";
    outfile << "    std::cout << \"Successfully created file created at : \" << filePath << std::endl;" << "\n";
    if (hidden == true){
        outfile << "    fileHider(filePath);" << "\n";
    }
    outfile << "    std::cout << \"Press enter to start comparing files... \" << std::endl; //ignore this" << "\n";
    outfile << "}" << "\n";
    outfile.close(); cout << endl;
    cout << "Succesfully created cpp file created at : " << path << "\n";
    lineMap.clear();
    checkerPath2 = creatorPath;
}

void compare(){
    string compileCommand = ".\\" + FileName + ".exe";
    std::system(compileCommand.c_str());
    ifstream file1(checkerPath1);
    ifstream file2(checkerPath2);
    if (!file1.is_open() || !file2.is_open()) {
        cerr << "Error! Could not open the file at : \n" << checkerPath1 << "\nMake sure this is a file!" << endl;
        cerr << "Error! Could not open the file at : \n" << checkerPath2 << "\nMake sure this is a file!" << endl;
        cin.get();
        exit(1);
    }
    cout << "\nPress enter to start checking files... "; cin.get();
    clear();
    if(file1.eof() && file2.eof()){
        cout << "\n\nThe files are identical! Program has finished!" << endl;
        exit(1);
    }
    string line1; string line2; int lineNumber = 1;
    cout << "Finding mistakes...\nOriginal file is : " << checkerPath1 << "\nCreated python file is : " << checkerPath2 << "\n"; bool mistake = false;
    while (getline(file1, line1) && getline(file2, line2)){
        if (line1 != line2){
            mistake = true;
            cout << "Line " << lineNumber << " is different!" << endl;
            cout << "File 1 : " << line1 << endl;
            cout << "File 2 : " << line2 << endl;
            cout << "\n\n";
        }
        ++lineNumber;
    }
    if (!mistake) cout << "\nNo mistakes found!\nDeleted newly created python file" << endl;
    file1.close();
    file2.close();
    std::filesystem::remove(checkerPath2); 
    cin.get();
    exit(1);
}

int main(){
    cout << "Enter the path of the existing code file : \n"; string FilePath; getline(cin, FilePath);
    checker(FilePath); clear(); checkerPath1 = FilePath;
    cout << "Enter the name that you would like to give to the new code file : "; getline(cin, FileName); clear();
    transformation(FilePath, FileName);
    cout << "Would you like to make the python file hidden when you run the newly created cpp file? (y/n) : "; char choice; cin >> choice; clear();
    (choice == 'y') ? hidden = true : hidden = false;
    reader();
    string compileCommand = "g++ " + writerFile + " -o " + FileName + ".exe";
    int result = std::system(compileCommand.c_str());
    if (result != 0) {
        cout << "Compilation failed with error code: " << result << endl;
        cout << "Looks like you will have to compile the .cpp yourself!\nPress enter to exit..."; cin.get();
        exit(1);
    }
    sleep(1);
    compare();
    return -1;
}
