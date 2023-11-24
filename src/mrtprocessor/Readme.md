# mrtprocessor

This is a C++ replacement (with some enhancements) for the C# tool previously used to process MRT files.

A substantive difference is that this no longer uses ZEBRA to turn MRT files into
textual representations before processing them.  This program processes MRT files 
(zipped or unzipped, gz, Z) directly.  In its default mode it processes entire directories
of RIPE/RouteView data files so that they can be simply downloaded and used directly.

This version by default runs multiple threads on multicore processors so help speed
up processing of the files.

##  Usage

Usage: 

(DIRECTORY MODE) mrtprocessor -d WORKDIR -s DATA-SRC -c DATA-COLLECTOR YYYYMMDD -o OUTPUT

(FILE MODE) mrtprocessor -o OUTPUT -f file1 file2 file3\n\n"

Switches:

    WORKDIR: directory cyberdefense is using (default cyberdefense)
    DATA-SRC: ripe or routeviews (default ripe)
    DATA-COOLECTOR: specific collector (default rrc04)
    OUTPUT: path to output file (default: stdout)
    [-np]: run non-parallel
    -f: file mode, process the files list directly, in order they should be processed
    -T: output posix timestamps in feature data

Examples:

mrtprocessor -d /Users/ballanty/cyberdefense_work -s ripe -T -c rrc04 20101112 -o testdata/cpp-output2.txt

mrtprocessor -T -np -f /Users/ballanty/cyberdefense_work/ripe/rrc04/2010.11.12/updates.20101112.0000.gz

## Building

This tool depends on the bgpdump library from RIPE-NCC (https://github.com/RIPE-NCC/bgpdump).

You will have to download and build/install that library and modify CMakeLists.txt
appropriately to get the tool to build and link on your system.