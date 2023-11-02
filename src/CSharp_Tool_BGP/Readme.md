# CSharp Tool BGP

## Description

C Sharp Tool is a tool for extracting features from BGP update files.

These files can be obtained from site collections such as RIPE and Routeviews. After downloading in MRT format, these files need to be converted to ASCII.

## Usage

For execution in Windows environments, the ConsoleApplication1.exe file can be executed directly.

In Linux and MacOS environments, Mono must be used to run the C Sharp Tool

### Case 1: No arguments passed on the command line

```bash
mono ConsoleApplication1.exe
```

* INPUT: The program will assume the existence of a DUMP file in the same directory where the .exe file is being executed. If the file does not exist, the program will abort with an error message.

* OUTPUT: The program will create two files in the same directory where the .exe file is being executed. The file names will be: DUMP_featureselection.txt and DUMP_out.txt.

### Case 2: With 1 argument passed on the command line
```bash
mono ConsoleApplication1.exe ./dump_examples/DUMP
```

* INPUT: The program will read the file passed by argument. In the case of the example, the file./dump_examples/DUMP. Any absolute or relative location can be passed. If the file does not exist, the program will abort with an error message.

* OUTPUT: The files created will have the same name and location as the input file, but with the suffixes _featureselection.txt and _out.txt.

## Building

```bash
cd src/CSharp_Tool_BGP/ConsoleApplication1
xbuild ConsoleApplication1.csproj
```

xbuild is Mono’s implementation of msbuild and it allows projects that have an msbuild file to be compiled natively on Linux or MacOS.