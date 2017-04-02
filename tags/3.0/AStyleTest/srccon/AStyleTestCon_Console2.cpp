// AStyleTestCon_Console2.cpp
// Copyright (c) 2017 by Jim Pattee <jimp03@email.com>.
// This code is licensed under the MIT License.
// License.md describes the conditions under which this software may be distributed.

// AStyleTestCon tests the ASConsole class only. This class is used only in
// the console build. It also tests the parseOption function for options used
// by only by the console build (e.g. recursive, preserve-date, verbose). It
// does not explicitely test the ASStreamIterator class or any other part
// of the program.

//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

#include <algorithm>
#include "AStyleTestCon.h"
#ifdef _WIN32
	#include <windows.h>
#endif

//----------------------------------------------------------------------------
// anonymous namespace
//----------------------------------------------------------------------------

namespace {
//
//----------------------------------------------------------------------------
// AStyle test getFilePaths(), wildcmp(), and fileName vector
//----------------------------------------------------------------------------

struct GetFilePathsF : public Test
{
	ASFormatter formatter;
	ASConsole* console;
	vector<string> fileNames;

	// build fileNames vector and write the output files
	GetFilePathsF()
	{
		char textIn[] =
		    "\nvoid foo()\n"
		    "{\n"
		    "bar();\n"
		    "}\n";
		cleanTestDirectory(getTestDirectory());
		console = new ASConsole(formatter);
		fileNames.push_back(getTestDirectory() + "/getFilePaths1.cpp");
		fileNames.push_back(getTestDirectory() + "/getFilePaths2.cpp");
		fileNames.push_back(getTestDirectory() + "/getFilePaths3.cpp");
		fileNames.push_back(getTestDirectory() + "/getFilePaths4.cpp");
		for (size_t i = 0; i < fileNames.size(); i++)
		{
			console->standardizePath(fileNames[i]);
			createTestFile(fileNames[i], textIn);
		}
	}

	~GetFilePathsF()
	{
		delete console;
	}
};

TEST_F(GetFilePathsF, FilePaths1)
// test fileName vector and getFilePaths with *.cpp
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check the files found
	vector<string> fileName = console->getFileName();
	ASSERT_EQ(fileNames.size(), fileName.size()) << "Vector sizes not equal.";
	for (size_t i = 0; i < fileName.size(); i++)
		EXPECT_EQ(fileNames[i], fileName[i]);
}

TEST_F(GetFilePathsF, FilePaths2)
// test fileName vector and getFilePaths with *.c??
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.c??");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check the files found
	vector<string> fileName = console->getFileName();
	ASSERT_EQ(fileNames.size(), fileName.size()) << "Vector sizes not equal.";
	for (size_t i = 0; i < fileName.size(); i++)
		EXPECT_EQ(fileNames[i], fileName[i]);
}

TEST_F(GetFilePathsF, FilePaths3)
// test fileName vector and getFilePaths with getFilePaths*.cpp
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/getFilePaths*.cpp");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check the files found
	vector<string> fileName = console->getFileName();
	ASSERT_EQ(fileNames.size(), fileName.size()) << "Vector sizes not equal.";
	for (size_t i = 0; i < fileName.size(); i++)
		EXPECT_EQ(fileNames[i], fileName[i]);
}

TEST_F(GetFilePathsF, FilePaths4)
// test fileName vector and getFilePaths with *.c*
// * at the end WITH remaining data allows complete coverage of wildcmp function
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.c*");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check the files found
	vector<string> fileName = console->getFileName();
	ASSERT_EQ(fileNames.size(), fileName.size()) << "Vector sizes not equal.";
	for (size_t i = 0; i < fileName.size(); i++)
		EXPECT_EQ(fileNames[i], fileName[i]);
}

TEST_F(GetFilePathsF, FilePaths5)
// test fileName vector and getFilePaths with *.cpp*
// * at the end WITHOUT remaining data allows complete coverage of wildcmp function
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp*");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check the files found
	vector<string> fileName = console->getFileName();
	ASSERT_EQ(fileNames.size(), fileName.size()) << "Vector sizes not equal.";
	for (size_t i = 0; i < fileName.size(); i++)
		EXPECT_EQ(fileNames[i], fileName[i]);
}

TEST_F(GetFilePathsF, FilePathsError)
// test fileName vector and getFilePaths with bad file path
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/AStyleError*");
	console->processOptions(astyleOptionsVector);
	// change special chars in filepath to '.' for windows regex comparison
	size_t i = 0;
	while (i < astyleOptionsVector.back().length())
	{
		i = astyleOptionsVector.back().find_first_of("\\/*?", i);
		if (i == string::npos)
			break;
		astyleOptionsVector.back()[i] = '.';
		++i;
	}
	string regex = "No file to process " + astyleOptionsVector.back() +
	               "\nDid you intend to use --recursive?";
	// cannot use death test with leak finder
#if GTEST_HAS_DEATH_TEST && !(LEAK_DETECTOR || LEAK_FINDER)
	// test processFiles with bad file path
	EXPECT_EXIT(console->processFiles(),
	            ExitedWithCode(EXIT_FAILURE),
	            regex);
#endif
}

TEST_F(GetFilePathsF, FilePathsErrorRecursive)
// test fileName vector and getFilePaths with bad file path using recursive option
{
	console->setIsQuiet(true);		// change this to see results
	console->setIsRecursive(true);
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/AStyleError*");
	console->processOptions(astyleOptionsVector);
	// change special chars in filepath to '.' for windows regex comparison
	size_t i = 0;
	while (i < astyleOptionsVector.back().length())
	{
		i = astyleOptionsVector.back().find_first_of("\\/*?", i);
		if (i == string::npos)
			break;
		astyleOptionsVector.back()[i] = '.';
		++i;
	}
	string regex = "No file to process " + astyleOptionsVector.back();
	// cannot use death test with leak finder
#if GTEST_HAS_DEATH_TEST && !(LEAK_DETECTOR || LEAK_FINDER)
	// test processFiles with bad file path
	EXPECT_EXIT(console->processFiles(),
	            ExitedWithCode(EXIT_FAILURE),
	            regex);
#endif
}

TEST_F(GetFilePathsF, FilePathsErrorSansFilename)
// test fileName vector and getFilePaths with no filename and no recursive option
{
	console->setIsQuiet(true);		// change this to see results
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/");	// no file
	console->processOptions(astyleOptionsVector);
	// change special chars in filepath to '.' for windows regex comparison
	size_t i = 0;
	while (i < astyleOptionsVector.back().length())
	{
		i = astyleOptionsVector.back().find_first_of("\\/*?", i);
		if (i == string::npos)
			break;
		astyleOptionsVector.back()[i] = '.';
		++i;
	}
	string regex = "Missing filename in " + astyleOptionsVector.back();
	// cannot use death test with leak finder
#if GTEST_HAS_DEATH_TEST && !(LEAK_DETECTOR || LEAK_FINDER)
	// test processFiles with bad file path
	EXPECT_EXIT(console->processFiles(),
	            ExitedWithCode(EXIT_FAILURE),
	            regex);
#endif
}

TEST_F(GetFilePathsF, FilePathsErrorRecursiveSansWildcard)
// test fileName vector and getFilePaths with recursive option and no wildcard
{
	console->setIsQuiet(true);		// change this to see results
	console->setIsRecursive(true);
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/noWildcard.cpp");
	console->processOptions(astyleOptionsVector);
	// change special chars in filepath to '.' for windows regex comparison
	size_t i = 0;
	while (i < astyleOptionsVector.back().length())
	{
		i = astyleOptionsVector.back().find_first_of("\\/*?", i);
		if (i == string::npos)
			break;
		astyleOptionsVector.back()[i] = '.';
		++i;
	}
	string regex = "Recursive option with no wildcard\n";
#ifndef _WIN32
	regex.append("Did you intend quote the filename?\n");
#endif
	// cannot use death test with leak finder
#if GTEST_HAS_DEATH_TEST && !(LEAK_DETECTOR || LEAK_FINDER)
	// test processFiles with bad file path
	EXPECT_EXIT(console->processFiles(),
	            ExitedWithCode(EXIT_FAILURE),
	            regex);
#endif
}

TEST_F(GetFilePathsF, FilePathsErrorInDirectoryName)
// test fileName vector and getFilePaths with a non-existant directory
{
	console->setIsQuiet(true);		// change this to see results
	console->setIsRecursive(true);
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/errorInDirectoryName/*.cpp");
	console->processOptions(astyleOptionsVector);
	// change special chars in filepath to '.' for windows regex comparison
	size_t i = 0;
	while (i < astyleOptionsVector.back().length())
	{
		i = astyleOptionsVector.back().find_first_of("\\/*?", i);
		if (i == string::npos)
			break;
		astyleOptionsVector.back()[i] = '.';
		++i;
	}
	string regex = "Cannot open directory " + astyleOptionsVector.back();
	regex = regex.substr(0, regex.length() - 6);	// remove the wilscard
	// cannot use death test with leak finder
#if GTEST_HAS_DEATH_TEST && !(LEAK_DETECTOR || LEAK_FINDER)
	// test processFiles with bad file path
	EXPECT_EXIT(console->processFiles(),
	            ExitedWithCode(EXIT_FAILURE),
	            regex);
#endif
}

//----------------------------------------------------------------------------
// AStyle test getFileType() - C_TYPE, JAVA_TYPE, SHARP_TYPE
//----------------------------------------------------------------------------

struct GetFileTypeF : public Test
{
	ASFormatter formatter;
	ASConsole* console;
	vector<string> fileNames;

	// build fileNames vector and write the output files
	GetFileTypeF()
	{
		char textIn[] =
		    "\nvoid foo()\n"
		    "{\n"
		    "    bar();\n"
		    "}\n";
		cleanTestDirectory(getTestDirectory());
		console = new ASConsole(formatter);
		fileNames.push_back(getTestDirectory() + "/getFileType1.cpp");
		fileNames.push_back(getTestDirectory() + "/getFileType2.java");
		fileNames.push_back(getTestDirectory() + "/getFileType3.cs");
		fileNames.push_back(getTestDirectory() + "/getFileType4.error");
		for (size_t i = 0; i < fileNames.size(); i++)
		{
			console->standardizePath(fileNames[i]);
			createTestFile(fileNames[i], textIn);
		}
	}

	~GetFileTypeF()
	{
		delete console;
	}
};

TEST_F(GetFileTypeF, FileTypeC)
// test getFileType with *.cpp
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check the file type
	EXPECT_EQ(C_TYPE, formatter.getFileType());
}

TEST_F(GetFileTypeF, FileTypeJava)
// test getFileType with *.java
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.java");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check the file type
	EXPECT_EQ(JAVA_TYPE, formatter.getFileType());
}

TEST_F(GetFileTypeF, FileTypeSharp)
// test getFileType with *.cs
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cs");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check the file type
	EXPECT_EQ(SHARP_TYPE, formatter.getFileType());
}

TEST_F(GetFileTypeF, FileTypeError)
// test getFileType with an invalid type, should return C_TYPE
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.error");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check the file type
	EXPECT_EQ(C_TYPE, formatter.getFileType());
}

//----------------------------------------------------------------------------
// AStyle Language Vectors
//----------------------------------------------------------------------------

struct LanguageVectorsF : public Test
{
	ASFormatter formatter;
	ASConsole* console;
	vector<string> fileNames;

	// build fileNames vector and write the output files
	LanguageVectorsF()
	{
		char textIn[] =
		    "\nvoid foo()\n"
		    "{\n"
		    "    bar();\n"
		    "}\n";
		cleanTestDirectory(getTestDirectory());
		console = new ASConsole(formatter);
		fileNames.push_back(getTestDirectory() + "/getFileType1.cpp");
		fileNames.push_back(getTestDirectory() + "/getFileType2.java");
		fileNames.push_back(getTestDirectory() + "/getFileType3.cs");
		fileNames.push_back(getTestDirectory() + "/getFileType4.error");
		for (size_t i = 0; i < fileNames.size(); i++)
		{
			console->standardizePath(fileNames[i]);
			createTestFile(fileNames[i], textIn);
		}
	}

	~LanguageVectorsF()
	{
		delete console;
	}
};

TEST_F(LanguageVectorsF, FileTypeC)
// Test the language vector setting with *.cpp.
{
	console->setIsQuiet(true);		// change this to see results
	// verify initial setting
	EXPECT_EQ(9, formatter.getFormatterFileType());
	EXPECT_EQ(9, formatter.getBeautifierFileType());
	// test language vectors for C++ files
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp");
	console->processOptions(astyleOptionsVector);
	console->processFiles();
	EXPECT_EQ(C_TYPE, formatter.getFormatterFileType());
	EXPECT_EQ(C_TYPE, formatter.getBeautifierFileType());
}

TEST_F(LanguageVectorsF, FileTypeJava)
// Test the language vector setting with *.java.
{
	console->setIsQuiet(true);		// change this to see results
	// verify initial setting
	EXPECT_EQ(9, formatter.getFormatterFileType());
	EXPECT_EQ(9, formatter.getBeautifierFileType());
	// test language vectors for Java files
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.java");
	console->processOptions(astyleOptionsVector);
	console->processFiles();
	EXPECT_EQ(JAVA_TYPE, formatter.getFormatterFileType());
	EXPECT_EQ(JAVA_TYPE, formatter.getBeautifierFileType());
}

TEST_F(LanguageVectorsF, FileTypeSharp)
// Test the language vector setting with *.cs.
{
	console->setIsQuiet(true);		// change this to see results
	// verify initial setting
	EXPECT_EQ(9, formatter.getFormatterFileType());
	EXPECT_EQ(9, formatter.getBeautifierFileType());
	// test language vectors for C# files
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cs");
	console->processOptions(astyleOptionsVector);
	console->processFiles();
	EXPECT_EQ(SHARP_TYPE, formatter.getFormatterFileType());
	EXPECT_EQ(SHARP_TYPE, formatter.getBeautifierFileType());
}

TEST(LanguageVectors, MultipleObjects)
// Static language vectors were removed in release 2.02
// to allow multiple ASFormatter objects.
// This was requested by KDevelop to allow multiple objects
// via the factory method.
// This checks initialization of the previously static vectors.
// Additional tests are done by the "GetFileTypeF" and
// "LanguageVectorsF" tests.
{
	// TODO fix this?
	// some compilers don't like the ASStreamIterator template
#if !defined(__clang__) && !defined(__INTEL_COMPILER) && !defined(__APPLE__)
	ASFormatter formatter1;
	// Aborted here in Debug for static objects:
	// "Assertion `container == __null' failed."
	ASFormatter formatter2;
	ASFormatter formatter3;
	// verify initial value
	EXPECT_EQ(9, formatter1.getFormatterFileType());
	EXPECT_EQ(9, formatter2.getFormatterFileType());
	EXPECT_EQ(9, formatter3.getFormatterFileType());
	EXPECT_EQ(9, formatter1.getBeautifierFileType());
	EXPECT_EQ(9, formatter2.getBeautifierFileType());
	EXPECT_EQ(9, formatter3.getBeautifierFileType());
	// initialize formatter1 with C++
	stringstream in;
	formatter1.setCStyle();
	// NOTE: For some reason this will not compile
	//       with gcc if speed optimizations are used???
	//       With intel use disable optimizations.
	ASStreamIterator<stringstream> streamIterator1(&in);
	formatter1.init(&streamIterator1);
	// initialize formatter2 with Java
	formatter2.setJavaStyle();
	ASStreamIterator<stringstream> streamIterator2(&in);
	formatter2.init(&streamIterator2);
	// initialize formatter3 with C#
	formatter3.setSharpStyle();
	ASStreamIterator<stringstream> streamIterator3(&in);
	formatter3.init(&streamIterator3);
	// check the file types
	EXPECT_EQ(C_TYPE, formatter1.getFormatterFileType());
	EXPECT_EQ(C_TYPE, formatter1.getBeautifierFileType());
	EXPECT_EQ(JAVA_TYPE, formatter2.getFormatterFileType());
	EXPECT_EQ(JAVA_TYPE, formatter2.getBeautifierFileType());
	EXPECT_EQ(SHARP_TYPE, formatter3.getFormatterFileType());
	EXPECT_EQ(SHARP_TYPE, formatter3.getBeautifierFileType());
#endif
}

//----------------------------------------------------------------------------
// AStyle test recursive option - getFilePaths(), wildcmp(), and fileName vector
//----------------------------------------------------------------------------

struct RecursiveF : public Test
{
	ASFormatter formatter;
	ASConsole* console;
	vector<string> fileNames;

	// build fileNames vector and write the output files
	RecursiveF()
	{
		char textIn[] =
		    "\nvoid foo()\n"
		    "{\n"
		    "bar();\n"
		    "}\n";
		cleanTestDirectory(getTestDirectory());
		console = new ASConsole(formatter);
		// create directories
		string sub1  = getTestDirectory() + "/subdir1";
		string sub1a = getTestDirectory() + "/subdir1/subdir1a";
		string sub1b = getTestDirectory() + "/subdir1/subdir1b";
		string sub2  = getTestDirectory() + "/subdir2";
		console->standardizePath(sub1);
		console->standardizePath(sub1a);
		console->standardizePath(sub1b);
		console->standardizePath(sub2);
		createTestDirectory(sub1);
		createTestDirectory(sub1a);
		createTestDirectory(sub1b);
		createTestDirectory(sub2);
		// create test files
		fileNames.push_back(getTestDirectory() + "/recursive1.cpp");
		fileNames.push_back(getTestDirectory() + "/recursive2.cpp");
		fileNames.push_back(sub1 + "/recursive3.cpp");
		fileNames.push_back(sub1 + "/recursive4.cpp");
		fileNames.push_back(sub1a + "/recursive5.cpp");
		fileNames.push_back(sub1a + "/recursive6.cpp");
		fileNames.push_back(sub1b + "/recursive7.cpp");
		fileNames.push_back(sub1b + "/recursive8.cpp");
		fileNames.push_back(sub2 + "/recursive9.cpp");
		fileNames.push_back(sub2 + "/recursive10.cpp");
		for (size_t i = 0; i < fileNames.size(); i++)
		{
			console->standardizePath(fileNames[i]);
			createTestFile(fileNames[i], textIn);
		}
		// sort file names for comparison
		sort(fileNames.begin(), fileNames.end());
	}

	~RecursiveF()
	{
		delete console;
	}
};

TEST_F(RecursiveF, Default)
// test recursive option
{
	console->setIsQuiet(true);		// change this to see results
	console->setIsRecursive(true);
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check the fileName vector
	vector<string> fileName = console->getFileName();
	ASSERT_EQ(fileNames.size(), fileName.size());
	for (size_t i = 0; i < fileNames.size(); i++)
		EXPECT_EQ(fileNames[i], fileName[i]);
}

TEST_F(RecursiveF, Sans)
// test *.cpp WITHOUT recursive
{
	console->setIsQuiet(true);		// change this to see results
	console->setIsRecursive(false);
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// delete sub directory files from the fileName vector
	size_t searchStart = getTestDirectory().length() + 1;
	for (int i = 0; i < (int) fileNames.size(); i++)
	{
		if (fileNames[i].find_first_of("\\/", searchStart) != string::npos)
		{
			// cout << fileNames[i] << endl;
			vector<string>::iterator iter = fileNames.begin() + i;
			fileNames.erase(iter);
			i--;
			continue;
		}
	}
	// check the files
	EXPECT_TRUE(fileNames.size() > 0);
	vector<string> fileName = console->getFileName();
	ASSERT_EQ(fileNames.size(), fileName.size());
	for (size_t i = 0; i < fileNames.size(); i++)
		EXPECT_EQ(fileNames[i], fileName[i]);
}

TEST_F(RecursiveF, Exclude)
// test recursive option with exclude
{
	console->setIsQuiet(true);		// change this to see results
	console->setIsRecursive(true);
	// call astyle processOptions()
	size_t filesExcluded = 0;
	vector<string> astyleOptionsVector;
	// file
	astyleOptionsVector.push_back("--exclude=recursive1.cpp");
	filesExcluded += 1;
	// directory
	astyleOptionsVector.push_back("--exclude=subdir1a");
	filesExcluded += 2;
	// full path file
	astyleOptionsVector.push_back("--exclude=/subdir2/recursive9.cpp");
	filesExcluded += 1;
	// full path directory
	astyleOptionsVector.push_back("--exclude=/subdir1/subdir1b");
	filesExcluded += 2;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// verify excludes
	vector<string> fileName = console->getFileName();
	EXPECT_EQ(fileNames.size() - filesExcluded, fileName.size());
}

TEST_F(RecursiveF, ExcludeErrors)
// test recursive option with unmatched excludes
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	// partial match on file
	astyleOptionsVector.push_back("--exclude=ecursive1.cpp");
	// partial match on directory
	astyleOptionsVector.push_back("--exclude=ubdir1a");
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp");
	console->processOptions(astyleOptionsVector);
	// error message regular expression
	string regex = "Exclude .unmatched.  ecursive1.cpp\n"
	               "Exclude .unmatched.  ubdir1a\n"
	               "Did you intend to use --recursive";
	// cannot use death test with leak finder
#if GTEST_HAS_DEATH_TEST && !(LEAK_DETECTOR || LEAK_FINDER)
	// test processFiles with unmatched excludes
	EXPECT_EXIT(console->processFiles(),
	            ExitedWithCode(EXIT_FAILURE),
	            regex);
#endif
}

TEST_F(RecursiveF, ExcludeErrorsRecursive)
// test recursive option with unmatched excludes and recursive option
{
	console->setIsQuiet(true);		// change this to see results
	console->setIsRecursive(true);
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	// partial match on file
	astyleOptionsVector.push_back("--exclude=ecursive1.cpp");
	// partial match on directory
	astyleOptionsVector.push_back("--exclude=ubdir1a");
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp");
	console->processOptions(astyleOptionsVector);
	// error message regular expression
	string regex = "Exclude .unmatched.  ecursive1.cpp\n"
	               "Exclude .unmatched.  ubdir1a";
	// cannot use death test with leak finder
#if GTEST_HAS_DEATH_TEST && !(LEAK_DETECTOR || LEAK_FINDER)
	// test processFiles with unmatched excludes
	EXPECT_EXIT(console->processFiles(),
	            ExitedWithCode(EXIT_FAILURE),
	            regex);
#endif
}

TEST_F(RecursiveF, HiddenFiles)
// test recursive option with hidden files
{
	console->setIsQuiet(true);		// change this to see results
	console->setIsRecursive(true);
	// write the hidden files
	char textIn[] = "void foo(){}\n";
	// hidden on Linux and Windows
	string dirOut1 = getTestDirectory() + "/.hidden";
	console->standardizePath(dirOut1);
	createTestDirectory(dirOut1);
	string fileOut1 = getTestDirectory() + "/.hidden/hidden1.cpp";
	console->standardizePath(fileOut1);
	createTestFile(fileOut1, textIn);
	string fileOut2 = getTestDirectory() + "/.hidden2.cpp";
	console->standardizePath(fileOut2);
	createTestFile(fileOut2, textIn);
	// hidden on Windows only
#ifdef _WIN32
	string dirOut1w = getTestDirectory() + "/readonlywin";
	console->standardizePath(dirOut1w);
	createTestDirectory(dirOut1w);
	SetFileAttributes(dirOut1w.c_str(), FILE_ATTRIBUTE_READONLY);
	string fileOut1w = getTestDirectory() + "/readonlywin/readonly1win.cpp";
	console->standardizePath(fileOut1w);
	createTestFile(fileOut1w, textIn);
	string fileOut2w = getTestDirectory() + "/hidden2win.cpp";
	console->standardizePath(fileOut2w);
	createTestFile(fileOut2w, textIn);
	SetFileAttributes(fileOut2w.c_str(), FILE_ATTRIBUTE_HIDDEN);
	string fileOut3w = getTestDirectory() + "/readonly2win.cpp";
	console->standardizePath(fileOut3w);
	createTestFile(fileOut3w, textIn);
	SetFileAttributes(fileOut3w.c_str(), FILE_ATTRIBUTE_READONLY);
#endif
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp");
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// hidden files should not be in console vector
	vector<string> fileName = console->getFileName();
	EXPECT_EQ(fileNames.size(), fileName.size());
#ifdef _WIN32
	// reset file attributes
	SetFileAttributes(dirOut1w.c_str(), FILE_ATTRIBUTE_NORMAL);
	SetFileAttributes(fileOut2w.c_str(), FILE_ATTRIBUTE_NORMAL);
	SetFileAttributes(fileOut3w.c_str(), FILE_ATTRIBUTE_NORMAL);
#endif
}

//----------------------------------------------------------------------------
// AStyle test dry-run option - getFilePaths(), wildcmp(), and fileName vector
//----------------------------------------------------------------------------

struct DryRunF : public Test
{
	ASFormatter formatter;
	ASConsole* console;
	vector<string> fileNames;

	// build fileNames vector and write the output files
	DryRunF()
	{
		char textIn[] =
		    "\nvoid foo()\n"
		    "{\n"
		    "    bar();\n"
		    "}\n";
		cleanTestDirectory(getTestDirectory());
		console = new ASConsole(formatter);
		// create test files
		fileNames.push_back(getTestDirectory() + "/dry-run1.cpp");
		fileNames.push_back(getTestDirectory() + "/dry-run2.cpp");
		for (size_t i = 0; i < fileNames.size(); i++)
		{
			console->standardizePath(fileNames[i]);
			createTestFile(fileNames[i], textIn);
		}
	}

	~DryRunF()
	{
		delete console;
	}
};

TEST_F(DryRunF, DryRunTest)
// Test dry-run option.
// Backup files should NOT be created.
{
	console->setIsQuiet(true);		// change this to see results
	console->setIsDryRun(true);
	// set the formatter options
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp");
	astyleOptionsVector.push_back("--style=attach");
	// process the file
	console->processOptions(astyleOptionsVector);
	console->processFiles();
	// check that no backup files were created
	vector<string> fileName = console->getFileName();
	ASSERT_EQ(fileNames.size(), fileName.size());
	for (size_t i = 0; i < fileNames.size(); i++)
	{
		EXPECT_EQ(fileNames[i], fileName[i]);
		// check for .orig file
		string origFileName = fileNames[i] + ".orig";
		struct stat stBuf;
		// display error if file is present
		if (stat(origFileName.c_str(), &stBuf) != -1)
			EXPECT_STREQ("no .orig file", origFileName.c_str());
	}
}

TEST_F(DryRunF, DryRunSans)
// Test without dry-run option.
// Backup files should be created.
{
	console->setIsQuiet(true);		// change this to see results
	// set the formatter options
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(getTestDirectory() + "/*.cpp");
	astyleOptionsVector.push_back("--style=attach");
	// process the file
	console->processOptions(astyleOptionsVector);
	console->processFiles();
	// check that no backup files were created
	vector<string> fileName = console->getFileName();
	ASSERT_EQ(fileNames.size(), fileName.size());
	for (size_t i = 0; i < fileNames.size(); i++)
	{
		EXPECT_EQ(fileNames[i], fileName[i]);
		// check for .orig file
		string origFileName = fileNames[i] + ".orig";
		struct stat stBuf;
		// display error if file is present
		if (stat(origFileName.c_str(), &stBuf) == -1)
			EXPECT_STREQ("expected an .orig file", origFileName.c_str());
	}
}

//----------------------------------------------------------------------------
// AStyle line ends formatted
// tests if a line end change formats the file
//----------------------------------------------------------------------------

struct LineEndsFormattedF : public Test
{
	ASFormatter formatter;
	ASConsole* console;
	vector<string> fileNames;

	string textLinuxStr;
	string textWindowsStr;
	string textMacOldStr;
	const char* textLinux;
	const char* textWindows;
	const char* textMacOld;

	string pathLinux;
	string pathWindows;
	string pathMacOld;

	// build fileNames vector and write the output files
	LineEndsFormattedF()
	{
		textLinuxStr =			// has macold line ends
		    "\rvoid foo()\r"
		    "{\r"
		    "    bar()\r"
		    "}\r";
		textWindowsStr =		// has linux line ends
		    "\nvoid foo()\n"
		    "{\n"
		    "    bar()\n"
		    "}\n";
		textMacOldStr =			// has windows line ends
		    "\r\nvoid foo()\r\n"
		    "{\r\n"
		    "    bar()\r\n"
		    "}\r\n";
		cleanTestDirectory(getTestDirectory());
		console = new ASConsole(formatter);
		// build text strings
		textLinux = textLinuxStr.c_str();
		textWindows = textWindowsStr.c_str();
		textMacOld = textMacOldStr.c_str();
		// create test files
		pathLinux = getTestDirectory() + "/textLinux.cpp";
		console->standardizePath(pathLinux);
		createTestFile(pathLinux, textLinux);
		pathWindows = getTestDirectory() + "/textWindows.cpp";
		console->standardizePath(pathWindows);
		createTestFile(pathWindows, textWindows);
		pathMacOld = getTestDirectory() + "/textMac.cpp";
		console->standardizePath(pathMacOld);
		createTestFile(pathMacOld, textMacOld);
	}

	~LineEndsFormattedF()
	{
		delete console;
	}
};

TEST_F(LineEndsFormattedF, LineEndWindows)
// test if lineend=windows formats the file
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back("--lineend=windows");
	astyleOptionsVector.push_back(pathWindows);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathWindows + ".orig";
	struct stat stBuf;
	// display error if file is not present
	if (stat(origFileName.c_str(), &stBuf) == -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
}

TEST_F(LineEndsFormattedF, LineEndLinux)
// test if lineend=linux formats the file
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back("--lineend=linux");
	astyleOptionsVector.push_back(pathLinux);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathLinux + ".orig";
	struct stat stBuf;
	// display error if file is not present
	if (stat(origFileName.c_str(), &stBuf) == -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
}

TEST_F(LineEndsFormattedF, LineEndMacOld)
// test if lineend=macold formats the file
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back("--lineend=macold");
	astyleOptionsVector.push_back(pathMacOld);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathMacOld + ".orig";
	struct stat stBuf;
	// display error if file is not present
	if (stat(origFileName.c_str(), &stBuf) == -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
}

//----------------------------------------------------------------------------
// AStyle line ends unchanged
// tests if an unchanged line end leaves the file unchanged
//----------------------------------------------------------------------------

struct LineEndsUnchangedF : public Test
{
	ASFormatter formatter;
	ASConsole* console;
	vector<string> fileNames;

	string textLinuxStr;
	string textWindowsStr;
	string textMacOldStr;
	const char* textLinux;
	const char* textWindows;
	const char* textMacOld;

	string pathLinux;
	string pathWindows;
	string pathMacOld;

	// build fileNames vector and write the output files
	LineEndsUnchangedF()
	{
		textLinuxStr =			// has linux line ends
		    "\nvoid foo()\n"
		    "{\n"
		    "    bar()\n"
		    "}\n";
		textWindowsStr =		// has windows line ends
		    "\r\nvoid foo()\r\n"
		    "{\r\n"
		    "    bar()\r\n"
		    "}\r\n";
		textMacOldStr =			// has macold line ends
		    "\rvoid foo()\r"
		    "{\r"
		    "    bar()\r"
		    "}\r";
		cleanTestDirectory(getTestDirectory());
		console = new ASConsole(formatter);
		// build text strings
		textLinux = textLinuxStr.c_str();
		textWindows = textWindowsStr.c_str();
		textMacOld = textMacOldStr.c_str();
		// create test files
		pathLinux = getTestDirectory() + "/textLinux.cpp";
		console->standardizePath(pathLinux);
		createTestFile(pathLinux, textLinux);
		pathWindows = getTestDirectory() + "/textWindows.cpp";
		console->standardizePath(pathWindows);
		createTestFile(pathWindows, textWindows);
		pathMacOld = getTestDirectory() + "/textMac.cpp";
		console->standardizePath(pathMacOld);
		createTestFile(pathMacOld, textMacOld);
	}

	~LineEndsUnchangedF()
	{
		delete console;
	}
};

TEST_F(LineEndsUnchangedF, LineEndWindows)
// test if lineend=windows leaves the file unchanged
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back("--lineend=windows");
	astyleOptionsVector.push_back(pathMacOld);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathWindows + ".orig";
	struct stat stBuf;
	// display error if file is present
	if (stat(origFileName.c_str(), &stBuf) != -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
}

TEST_F(LineEndsUnchangedF, LineEndLinux)
// test if lineend=linux leaves the file unchanged
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back("--lineend=linux");
	astyleOptionsVector.push_back(pathWindows);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathLinux + ".orig";
	struct stat stBuf;
	// display error if file is present
	if (stat(origFileName.c_str(), &stBuf) != -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
}

TEST_F(LineEndsUnchangedF, LineEndMacOld)
// test if lineend=macold leaves the file unchanged
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back("--lineend=macold");
	astyleOptionsVector.push_back(pathLinux);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathMacOld + ".orig";
	struct stat stBuf;
	// display error if file is present
	if (stat(origFileName.c_str(), &stBuf) != -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
}

//----------------------------------------------------------------------------
// AStyle default line ends
// tests if default line end leaves the file unchanged
//----------------------------------------------------------------------------

struct LineEndsDefaultF : public Test
{
	ASFormatter formatter;
	ASConsole* console;
	vector<string> fileNames;

	string textLinuxStr;
	string textWindowsStr;
	string textMacOldStr;
	const char* textLinux;
	const char* textWindows;
	const char* textMacOld;

	string pathLinux;
	string pathWindows;
	string pathMacOld;

	// build fileNames vector and write the output files
	LineEndsDefaultF()
	{
		textLinuxStr =			// has linux line ends
		    "\nvoid foo()\n"
		    "{\n"
		    "    bar()\n"
		    "}\n";
		textWindowsStr =		// has windows line ends
		    "\r\nvoid foo()\r\n"
		    "{\r\n"
		    "    bar()\r\n"
		    "}\r\n";
		textMacOldStr =			// has macold line ends
		    "\rvoid foo()\r"
		    "{\r"
		    "    bar()\r"
		    "}\r";
		cleanTestDirectory(getTestDirectory());
		console = new ASConsole(formatter);
		// build text strings
		textLinux = textLinuxStr.c_str();
		textWindows = textWindowsStr.c_str();
		textMacOld = textMacOldStr.c_str();
		// create test files
		pathLinux = getTestDirectory() + "/textLinux.cpp";
		console->standardizePath(pathLinux);
		createTestFile(pathLinux, textLinux);
		pathWindows = getTestDirectory() + "/textWindows.cpp";
		console->standardizePath(pathWindows);
		createTestFile(pathWindows, textWindows);
		pathMacOld = getTestDirectory() + "/textMac.cpp";
		console->standardizePath(pathMacOld);
		createTestFile(pathMacOld, textMacOld);
	}

	~LineEndsDefaultF()
	{
		delete console;
	}
};

TEST_F(LineEndsDefaultF, LineEndWindows)
// test if default line end leaves a windows file unchanged
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(pathWindows);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathWindows + ".orig";
	struct stat stBuf;
	// display error if file is present
	if (stat(origFileName.c_str(), &stBuf) != -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
}

TEST_F(LineEndsDefaultF, LineEndLinux)
// test if default line end leaves a linux file unchanged
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(pathLinux);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathLinux + ".orig";
	struct stat stBuf;
	// display error if file is present
	if (stat(origFileName.c_str(), &stBuf) != -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
}

TEST_F(LineEndsDefaultF, LineEndMacOld)
// test if default line end leaves a macold file unchanged
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(pathMacOld);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathMacOld + ".orig";
	struct stat stBuf;
	// display error if file is present
	if (stat(origFileName.c_str(), &stBuf) != -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
}

//----------------------------------------------------------------------------
// AStyle line ends format
// tests if default line ends calls the convertLineEnds() function when needed
//----------------------------------------------------------------------------

struct LineEndsDefaultMixedF : public Test
{
	ASFormatter formatter;
	ASConsole* console;
	vector<string> fileNames;

	string textLinuxStr;
	string textWindowsStr;
	string textMacOldStr;
	const char* textLinux;
	const char* textWindows;
	const char* textMacOld;

	string pathLinux;
	string pathWindows;
	string pathMacOld;

	// build fileNames vector and write the output files
	LineEndsDefaultMixedF()
	{
		textLinuxStr =
		    "\r\nvoid foo()\r\n"	// not a linux line end
		    "{\n"
		    "    bar1()\n"
		    "    bar2()\n"
		    "}\n";
		textWindowsStr =
		    "\rvoid foo()\r"		// not a windows line end
		    "{\r\n"
		    "    bar1()\r\n"
		    "    bar2()\r\n"
		    "}\r\n";
		textMacOldStr =
		    "\r\nvoid foo()\r\n"	// not a macold line end
		    "{\r"
		    "    bar1()\r"
		    "    bar2()\r"
		    "}\r";
		cleanTestDirectory(getTestDirectory());
		console = new ASConsole(formatter);
		// build text strings
		textLinux = textLinuxStr.c_str();
		textWindows = textWindowsStr.c_str();
		textMacOld = textMacOldStr.c_str();
		// create test files
		pathLinux = getTestDirectory() + "/textLinux.cpp";
		console->standardizePath(pathLinux);
		createTestFile(pathLinux, textLinux);
		pathWindows = getTestDirectory() + "/textWindows.cpp";
		console->standardizePath(pathWindows);
		createTestFile(pathWindows, textWindows);
		pathMacOld = getTestDirectory() + "/textMac.cpp";
		console->standardizePath(pathMacOld);
		createTestFile(pathMacOld, textMacOld);
	}

	~LineEndsDefaultMixedF()
	{
		delete console;
	}
};

TEST_F(LineEndsDefaultMixedF, LineEndWindows)
// test if default line ends outputs windows line ends by calling convertLineEnds()
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(pathWindows);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathWindows + ".orig";
	struct stat stBuf;
	// display error if file is not present
	if (stat(origFileName.c_str(), &stBuf) == -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
	// check that convertLineEnds is called
	EXPECT_TRUE(console->getLineEndsMixed());
	// the line ends must be checked manually
//	systemPause("Check Windows Line Ends");
}

TEST_F(LineEndsDefaultMixedF, LineEndLinux)
// test if default line ends outputs linux line ends by calling convertLineEnds()
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(pathLinux);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathLinux + ".orig";
	struct stat stBuf;
	// display error if file is not present
	if (stat(origFileName.c_str(), &stBuf) == -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
	// check that convertLineEnds is called
	EXPECT_TRUE(console->getLineEndsMixed());
	// the line ends must be checked manually
//	systemPause("Check Linux Line Ends");
}

TEST_F(LineEndsDefaultMixedF, LineEndMacOld)
// test if default line ends outputs linux line ends by calling convertLineEnds()
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(pathMacOld);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathMacOld + ".orig";
	struct stat stBuf;
	// display error if file is not present
	if (stat(origFileName.c_str(), &stBuf) == -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
	// check that convertLineEnds is called
	EXPECT_TRUE(console->getLineEndsMixed());
	// the line ends must be checked manually
//	systemPause("Check MacOld Line Ends");
}

//----------------------------------------------------------------------------
// AStyle line ends formatted
// tests that default line ends does NOT call the convertLineEnds() function when not needed
//----------------------------------------------------------------------------

struct LineEndsDefaultMixedSansF : public Test
{
	ASFormatter formatter;
	ASConsole* console;
	vector<string> fileNames;

	string textLinuxStr;
	string textWindowsStr;
	string textMacOldStr;
	const char* textLinux;
	const char* textWindows;
	const char* textMacOld;

	string pathLinux;
	string pathWindows;
	string pathMacOld;

	// build fileNames vector and write the output files
	LineEndsDefaultMixedSansF()
	{
		textLinuxStr =
		    "\nvoid foo()\n"
		    "{\n"
		    "    bar()\r\n"		// not a linux line end
		    "}\n";
		textWindowsStr =
		    "\r\nvoid foo()\r\n"
		    "{\r\n"
		    "    bar()\r"		// not a windows line end
		    "}\r\n";
		textMacOldStr =
		    "\rvoid foo()\r"
		    "{\r"
		    "    bar()\r\n"		// not a macold line end
		    "}\r";
		cleanTestDirectory(getTestDirectory());
		console = new ASConsole(formatter);
		// build text strings
		textLinux = textLinuxStr.c_str();
		textWindows = textWindowsStr.c_str();
		textMacOld = textMacOldStr.c_str();
		// create test files
		pathLinux = getTestDirectory() + "/textLinux.cpp";
		console->standardizePath(pathLinux);
		createTestFile(pathLinux, textLinux);
		pathWindows = getTestDirectory() + "/textWindows.cpp";
		console->standardizePath(pathWindows);
		createTestFile(pathWindows, textWindows);
		pathMacOld = getTestDirectory() + "/textMac.cpp";
		console->standardizePath(pathMacOld);
		createTestFile(pathMacOld, textMacOld);
	}

	~LineEndsDefaultMixedSansF()
	{
		delete console;
	}
};

TEST_F(LineEndsDefaultMixedSansF, LineEndWindows)
// test if default line ends outputs windows line ends without calling convertLineEnds()
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(pathWindows);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathWindows + ".orig";
	struct stat stBuf;
	// display error if file is not present
	if (stat(origFileName.c_str(), &stBuf) == -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
	// check that convertLineEnds is NOT called
	EXPECT_FALSE(console->getLineEndsMixed());
	// the line ends must be checked manually
//	systemPause("Check Windows Line Ends");
}

TEST_F(LineEndsDefaultMixedSansF, LineEndLinux)
// test if default line ends outputs linux line ends without calling convertLineEnds()
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(pathLinux);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathLinux + ".orig";
	struct stat stBuf;
	// display error if file is not present
	if (stat(origFileName.c_str(), &stBuf) == -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
	// check that convertLineEnds is NOT called
	EXPECT_FALSE(console->getLineEndsMixed());
	// the line ends must be checked manually
//	systemPause("Check Linux Line Ends");
}

TEST_F(LineEndsDefaultMixedSansF, LineEndMacOld)
// test if default line ends outputs linux line ends without calling convertLineEnds()
{
	console->setIsQuiet(true);		// change this to see results
	// call astyle processOptions()
	vector<string> astyleOptionsVector;
	astyleOptionsVector.push_back(pathMacOld);
	console->processOptions(astyleOptionsVector);
	// call astyle processFiles()
	console->processFiles();
	// check for .orig file
	string origFileName = pathMacOld + ".orig";
	struct stat stBuf;
	// display error if file is not present
	if (stat(origFileName.c_str(), &stBuf) == -1)
		EXPECT_STREQ("no .orig file", origFileName.c_str());
	// check that convertLineEnds is NOT called
	EXPECT_FALSE(console->getLineEndsMixed());
	// the line ends must be checked manually
//	systemPause("Check MacOld Line Ends");
}

//----------------------------------------------------------------------------

}  // namespace

//----------------------------------------------------------------------------

// TODO: test waitForRemove function - line 1925
