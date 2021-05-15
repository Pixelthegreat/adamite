# adamite
A simple programming language developed in C.

## inspiration

The Adamite programming language is loosely based off of CodePulse's series on how to make a simple programming language. I wanted to challenge myself and see if I could create a language based off of the series but entirely in the C programming language. I started a few months ago, and I have only recently finished the main features, such as If Statements, For Loops, While Loops, and Structs similar to the ones in C. I am glad to say that I have finished work on the language and I would like to share the code with the users of Github.

## building the project

To compile the project, simple type "build" on windows and "./build.sh" on linux and presumably also on Mac.

I chose not to use Makefiles for this since I am only beginning to learn how to use them in the first place.

The files "cppbuild.bat" and "./cppbuild.sh" exist for the project now and are in the same folder as the regular build scripts.

### For clarity

The actual folder that the main build scripts are located in is "src/main".

## syntax

I wanted the syntax to be fairly similar to C's and also to be a bit unique. Some of the syntax is based off of that of languages like Ruby, whilst some of the syntax is entirely made up or close to C.

To grasp an idea of the basic syntax, consider these two examples which are respectively titled "hello world" and "variable demonstration".

```
puts 'hello, world!';
```

```
int x = 0;
char c = 'a';
str s = 'hello, world!';
```

You can learn more about the features and syntax through the syntax example found in the main folder in the source.

## features

Adamite includes various features which are listed here:

- User keyboard input
- Text output
- Static type checking (very basic, not actually worth my effort)
- Basic Types
- Strings
- Arrays
- Basic Math
- Functions
- If Statements
- For Loops
- While Loops
- Structs
- Pointers

Although most of them work flawlessly in my eyes, some of them don't work entirely how I or a typical programmer reading this would expect it to work.

## problems

I currently have a few problems that I want to address before you go ahead and download everything. For one, I have never done any Mac testing, only Linux and Windows so far. The reason for this is simply that I don't have an Apple computer. However, the language just skips newline characters for windows and linux so it should be fine, and files written in all three operating systems should work fine, since CR and LF chars are ignored. Another problem is that variable storage is always global. You may think that it would be easy to fix, but the way that I have implemented everything, it would presumably be a little hard to implement it. As such, I have never gotten around to doing it. Finally, some syntax should be carefully placed, as the parser system does not provide syntax errors for everything.

Other than that, most things should be fine. I shall mention here that if you find any problems such as bugs or flaws in the code, please do report them in the Issues tab.

## standard library

The standard library is currently very small and only consists of a few files. While I do plan on expanding it in the future, I don't have any ideas right now.

## variable storage information

As I have mentioned, due to the way that I implemented it, I am currently using a system in which variables are stored globally. Although this is a little problematic, I have found ways to get around it.

Main variable storage consists of two main sections:
- The Name System
- The Storage System

The Storage System doesn't actually store objects, but rather the pointers to them. This way, at the end of a program's execution, it will automatically free all of the objects from memory, since all of them were dynamically allocated.

The Name System just stores a list of names that are tied to actually object pointers. It has nothing to do with the Storage System, and is just a simple method of keeping track of variables through their names.

## going further

As I have gotten closer to finishing the main language features, I have also realized that a lot of things that I did I could have done much better. If I ever come around to it, then I would love to make Adamite 2.0, although chances are that I will continue to use the revision numbering system that I plan on using.

On top of that, this project was simply one for fun, and while I will continue updating it, I will choose to ignore suggestions such as "You could consider using LLVM" and stuff like that, as I believe it would take away the fun of programming something pretty much entirely from scratch.