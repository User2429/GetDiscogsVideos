# GetDiscogsVideos
# Documentation

## Executable and script files 
The executable files are `GetDiscogsVideos.exe` for Windows and `GetDiscogsVideos` for Linux. These are found under Releases. The script files are in the Scripts folder.

The executable is invoked by the PowerShell script `PS_GetDiscogsVideos.ps1` which runs on Windows or Linux.<br>If PowerShell is not available in Linux the bash script `AB_GetDiscogsVideos.sh` may be used instead.

## Compilation
The executable files in Releases were compiled under a CMake project in Visual Studio 2022 in the Windows environment.

## Input file
Discogs release files are found at [Discogs Data](http://data.discogs.com/). The input file for the current process is the unzipped Discogs release file, with name of the form `discogs_yyyymmdd_releases.xml`. The file is assumed to be a valid `xml` file in which all necessary tags exist and are well-formed. The release ids are assumed to be unique and in increasing order and not longer than 10 digits. The file is assumed to begin with the tag `<releases>` on its own line. These assumptions are not validated by the current process.

## Output files
The output files are in `csv` format with the output for each discogs release being written to its own line. The user may specify the output file root name; the default is `output`. The user may also specify via the `rows_output` argument the number of lines to be written per output file. The number of output files is determined by this choice. The default value for `rows_output` is the largest integer value in PowerShell, which is 2,147,483,647 (2<sup>31</sup> - 1). This is far larger than the number of discogs records, so in this case all records will be written to just one file. If multiple files are required the names will have a number appended, e.g. `output_01.csv`, `output_02.csv`, etc. (but note that for a large value of `rows_output` other than the default the result will be a single file `output_01.csv`; only the default value will result in `output.csv`).

The maximum number of output files allowed is 99, so `rows_output` cannot be so small that this is exceeded. As a practical guide, for the  May 2022 discogs file (68 GB) the choice for `rows_output` of 250,000 would produce around 30 files of a size of around 100 MB each.

## Installation
The user need only copy the executable and script files to whatever folder they choose.

In Windows these are `GetDiscogsVideos.exe` and `PS_GetDiscogsVideos.ps1`.<br>In Linux they are `GetDiscogsVideos` and either of `PS_GetDiscogsVideos.ps1` or `AB_GetDiscogsVideos.sh`. 
 
 The executable and script files must reside in the same folder. The file root name `GetDiscogsVideos` may be changed by the user but must be the same for the executable and the script files (e.g. `abc.exe` and `PS_abc.ps1`). File extensions may not be changed.

## Operation
The PowerShell script `PS_GetDiscogsVideos.ps1` parses and validates its arguments which are then passed to the executable. The argument syntax for the script follows the PowerShell conventions. Help, including usage examples, is available via `Get-Help` or the `-?` argument.

## Arguments
The PowerShell script takes 7 parameters (arguments). The `input_file` argument is the only mandatory argument. It is also positional, which means that it is sufficient to supply the value only and that the parameter name (`-input_file` or `-i`) is not required.

| Parameter  | Details |
| ---------- | ------- |
| `input_file` | The discogs release file (full path including the file name). |

There are 5 optional parameters.

| Parameter  | Details |
| ---------- | ------- |
| `path_output` | Omit to have the output file(s) in the same folder as the script.<br>Include to specify a different path to the output file(s).
| `output_file` | Omit to use the default output file root name, `output`.<br>Include to specify an alternative root name. |
| `rows_output` | Omit to have all records written to a single file.<br>Include to specify the number of records per file. |
| `update_percentage` | Omit to use the default status update frequency.<br>Include to specify a different value. |
| `size_block` | Omit to use the default block size for reading from the file.<br>Include to specify a different value. |

Finally there is 1 switch parameter (optional flag).

| Parameter  | Details |
| ---------- | ------- |
| `no_check` | Omit to validate that the script and executable are in the same folder.<br>Include to skip this validation. |

The optional arguments `path_output` and `rows_output` may be required by the user; in most cases it will be preferable simply to omit the other optional arguments. 

## Script functionality
##### PS_GetDiscogsVideos.ps1
This script performs type checking on the arguments and also validates the following:
 - `input_file` (including the full pathname) must exist.
 - `path_output` must be a valid path not including a file name.
 - `path_output` must end in a backslash (if not, the script adds it).
 - `rows_output` must be a positive integer not less than 150,000.
 - `size_block` must be an integer in the range 21 to 32.
 - `update_percentage` must be an integer in the range 1 to 100.

In addition the script does not allow output to be overwritten, so if any output files are detected in the output folder the script terminates. The user must ensure that there is no naming conflict between proposed and extant output files.

Default values reside in the script, not the executable.

Once the arguments have been parsed and validated the script calls the executable, passing all arguments other than `no_check` in the required order. The call is echoed on the screen. 

##### AB_GetDiscogsVideos.sh
If PowerShell is not available in Linux this bash script may be used instead. It was created using the script generator [argbash](https://github.com/matejak/argbash) with some additional validation code added at the end. It uses the same argument names as the PowerShell script and attempts to replicate its functionality. 
 
## Executable functionality
##### Arguments
The executable reads the arguments as a C++ `argv[]` argument array. The argument order is:

`input_file` &ensp; `path_output` &ensp; `output_file` &ensp; `size_block` &ensp; `multiple_files` &ensp; `rows_output` &ensp; `update_percentage`

The `multiple_files` argument is supplied to the executable only, not to the script. It is derived from `rows_output` and is the string 'true' if `rows_output` is less than the default and 'false' otherwise. It is required because the default value for `rows_output` exists only in the script, so the executable cannot determine whether multiple output files are required by comparing `rows_output` to its default value.

The `no-check` argument is not required by the executable. The remaining arguments are the same as the script arguments with the only difference that for the script `path-output` may or may not end in a delimiter but for the executable is must (\ for Windows, / for Linux).

The executable may be invoked directly from the command line, but this is not recommended as the executable does not validate its arguments, so the user would then be solely responsible for ensuring the validity and ordering of the arguments, including that all arguments are supplied and that default values are entered explicitly. In addition, while the script protects existing output files from being overwritten the executable does not.

##### Reading data
Because of the size of the input file (currently 68 GB) it must be read in blocks. A block must be large enough to ensure that it always contains at least one complete record, which means it must be twice the length of the longest record in the file (currently 825 KB). So the block size must be at least 1.65 MiB. The block size in bytes is given by 2<sup>`size_block`</sup>, so for the `size_block` default value of 26 this is 2<sup>26</sup> = 64 MiB. The minimum allowed value is 21 (block size of 2<sup>21</sup> =  2 MiB). The maximum is 32 (block size 4 GiB).

##### Process
The program reads in a block of data from the discogs releases `xml` file. It determines the number of complete records in the block and then searches for those which have videos. For each such release a new line is written to the `csv` output file. When the specified number of rows has been written the output file is closed and a new output file is opened. When the last complete record in a block has been processed a new block is read in. The process continues until all records in the `xml` have been read.

A progress message is posted whenever another `u%` of the total estimated completion time has elapsed, where `u` is the `update_percentage`. For the default value of 5 an expected number of (100 / 5) = 20 progress messages will be posted. 

##### Output
Each line of the output file contains the data for one release.<br>The data written are `Release ID`, `Artists`, `Title`, `Labels`, `GenreStye`, `Release Date` and `Videos`.

`Release ID` displays the discogs release id as a number without quotation marks.
`Artists` displays the discogs artists for the release as an array in the form ["artist1", "artist2", ...]. If there is only one element it is displayed as ["artist"].
`Title` displays the title as a string without quotation marks.
`Labels` displays the labels as an array in the same format as `Artists`.
`GenreStyle` displays the discogs styles for the release. If there are no styles it displays the genres. It is an array in the same format as `Artists`. 
`Release Date` displays the release date as a string with no quotation marks.
`Videos` displays the videos associated with the release as an array in the format<br>[{"title_1","video_00001"},{"title_2","video_00002"}, ...]</br>where "title_1" is the title of the first video and "video_00001" represents the 11-digit YouTube video identifier. The number of videos per release is capped at 500. 

The output `csv` file has no header row.

##### Record Processing
Repeated instances of the first label in the input file are omitted in `Labels`. So, e.g. Label1, Label1 in the file will appear as Label1 in the ouput. This does not apply, however, to labels after the first, so Label1, Label2, Label1, Label2 would appear in the output as Label1, Label2, Label2.

The XML escape strings `&amp;` `&apos;` `&gt;` `&lt;` and `&quot;` are replaced in the output by `&` `'` `>` `<` and `"` respectively. The alternate representations `&#38;` `&#39;` `&#62;` `&#60;` and `&#34;` are replaced in the same way. `\n` `\r` and `&#13;` (XML code for `\r`) are replaced by strings of length zero `""`.

The erroneous string `&#039;` in the input file is not replaced.


## Return codes
The non-zero return codes are as follows:
| Error | Description |
| ----- | ----------- |
| `10` |	Input file not found or not open. |
| `20` |	File read failed. |
| `30` |	Read block did not contain at least one complete record. |

In PowerShell the executable return code may be retrieved from the variable `$LastExitCode`. 
If the bash script is used in Linux then the executable return code may be retrieved from the Linux variable `$?` but only immediately after the script has run as it will be overwritten by the return code from the next Linux command.