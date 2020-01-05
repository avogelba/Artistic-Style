# Artistic-Style
This is a clone of an SVN repository at https://svn.code.sf.net/p/astyle/code. It had been cloned by http://svn2github.com/ , but the service was since closed. Please read a closing note on my blog post: http://piotr.gabryjeluk.pl/blog:closing-svn2github . If you want to continue synchronizing this repo, look at https://github.com/gabrys/svn2github 

# description
Artistic Style is a source code indenter, formatter, and beautifier for the C, C++, C++/CLI, Objective‑C, C# and Java programming languages.

When indenting source code, we as programmers have a tendency to use both spaces and tab characters to create the wanted indentation. Moreover, some editors by default insert spaces instead of tabs when pressing the tab key. Other editors (Emacs for example) have the ability to "pretty up" lines by automatically setting up the white space before the code on the line, possibly inserting spaces in code that up to now used only tabs for indentation.

The NUMBER of spaces for each tab character in the source code can change between editors (unless the user sets up the number to his liking...). One of the standard problems programmers face when moving from one editor to another is that code containing both spaces and tabs, which was perfectly indented, suddenly becomes a mess to look at. Even if you as a programmer take care to ONLY use spaces or tabs, looking at other people's source code can still be problematic.

To address this problem, Artistic Style was created – a filter written in C++ that automatically re-indents and re-formats C / C++ / Objective‑C / C++/CLI / C# / Java source files. It can be used from a command line, or it can be incorporated as a library in another program. 
