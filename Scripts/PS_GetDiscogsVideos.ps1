<#
.SYNOPSIS
 A script which parses arguments for and then invokes the executable GetDiscogsVideos.
 Given a Discogs release file the executable extracts those records which have videos, with the list of videos and some additional information.

.DESCRIPTION
 Arguments for this script follow PowerShell conventions. They are parsed, validated and passed to the executable.
 The executable requires arguments to have a specific form and order and does not validate them. So while it is possible to invoke it directly without this script it is not recommended.

.PARAMETER input_file
 Mandatory. The Discogs xml file from which the videos extract is sought. Must be a complete path including the filename and extension.

.PARAMETER path_output
 Optional. The folder to which the extract files are written. If not supplied the files are written to the folder where this script resides.
 If supplied it must exist and be a valid folder name and may not include a file name.

.PARAMETER output_file
 Optional. The root name of the output file. If not supplied then the default value 'output' is used. If there is only one output file it has the root name, e.g. output.csv. If there are multiple output files then a number is appended to the root name, e.g. output_01.csv, output_02.csv, etc.
 
.PARAMETER rows_output.
Optional. The number of rows per output file. Must be an integer from 150,000 to [int]::MaxValue (the largest integer value in PowerShell, 2,147,483,647). When the process has written this number of rows to the output file it closes it and opens a new one. The default is MaxValue, which ensures there will only be one ouptut file. The number of output files is capped at 99. The lower bound of 150,000 is intended to ensure that this number will not be exceeded.

.PARAMETER size_block
 Optional. Because the input file is too large to hold in memory the executable reads data in blocks. A block must be large enough to ensure that it always contains a complete record, so must be twice the length of the longest record, which is currently around 825,000 bytes. The read block size in bytes is 2^size_block, so for the default value of 26 this is 64 MiB. The minimum allowed is 21, corresponding to 2 MiB. The maximum is 32 (4 GiB).

.PARAMETER update_percentage.
Optional. The percentage of the total expected processing time per progress update. Must be an integer from 1 to 100. The default is 5 (which implies (100 / 5) = 20 progress updates).

.PARAMETER no_check
 Switch parameter. Omit to validate that the script and executable are in the same folder. Include to skip this check.

.EXAMPLE
PS_GetDiscogsVideos.ps1   "D:\discogs_releases.xml" 

Description
---------------------------------------------------------------
EXAMPLE 1. Writes a single output file, output.csv, to the folder in which the script resides.

.EXAMPLE
PS_GetDiscogsRelease.ps1   "D:\discogs_releases.xml"   -r 250000

Description
---------------------------------------------------------------
EXAMPLE 2. Same as EXAMPLE 1 but with 250,000 rows per output file. The files are then output_01.csv, output_02.csv, . . . with as many files as required. The last file can be expected to have fewer than 250,000 rows.

.EXAMPLE
PS_GetDiscogsRelease.ps1   "D:\discogs_releases.xml"   -r 250000   -p "D:\DiscogsFiles\" 

Description
---------------------------------------------------------------
EXAMPLE 3. Same as EXAMPLE 2 but with a specified output folder.

.EXAMPLE
PS_GetDiscogsRelease.ps1   -input_file "D:\discogs_releases.xml"   -rows_output 250000   -path_output "D:\DiscogsFiles\"   -output_file output_test   -size_block 24

Description
---------------------------------------------------------------
EXAMPLE 4. Same as EXAMPLE 3 but with parameters identified by their full names. In addition a specified output file root name is supplied and data is read in blocks of size 2^24 = 16 MiB. 

.INPUTS
 None.

.OUTPUTS
 None. Success or failure is indicated by messages to the console.

.NOTES
 Created by    : User2429
 Date          : June 2022
#>

[CmdletBinding()]

Param(
    [Parameter(Mandatory)] [ValidateScript({$_.Exists})] [System.IO.FileInfo] $input_file,
    [Parameter()] [ValidateScript({Test-Path $_})] [System.IO.FileInfo] $path_output,
    [Parameter()] [string] $output_file = 'output',
    [Parameter()] [ValidateRange(150000, [int]::MaxValue)] [int] $rows_output = [int]::MaxValue,
    [Parameter()] [ValidateRange(21,32)] [int] $size_block = 26,
    [Parameter()] [ValidateRange(1,100)] [int] $update_percentage = 5,
    [Parameter()] [switch] $no_check)
    
# StrictMode; stop script on error.
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

New-Variable -Name max_file_count   -Value 99       -Option Constant

Function Test-Output
# Test if full output path already exists.
{
    Param([string]$path_output, [string]$output_file)
    $output_file += '.csv'
    $output_file_full = Join-Path -Path $path_output -ChildPath $output_file 
    if(Test-Path -Path $output_file_full) { throw "Output file $output_file already exists in the output folder. Process terminated."}
}

# multiple_files is true if rows_output is less than the maximum integer value. 
# The corresponding string argument for the executable is m with values of 'true' or 'false'.
$multiple_files = ($rows_output -lt [int]::MaxValue)
if ($multiple_files){ $m = 'true' } else { $m = 'false' }

# The executable name is the name of this script after removing the prefix "PS_" and extension ".ps1".
$executable_name = [System.IO.Path]::GetFileNameWithoutExtension($MyInvocation.MyCommand.Name)
$executable_name = $executable_name.Substring($executable_name.IndexOf("_") + 1)
$not_found = "not found in folder $PSScriptRoot\"

# If the path_output argument is not supplied make it the local path (i.e. the path of this script as given by $PSScriptRoot).
if ($null -eq $path_output ) { 
    Write-Verbose "Output file will be written to folder $PSScriptRoot"
    $path_output = $PSScriptRoot 
}

# The output folder must be a folder, so may not have a file extension.
$output_ext = Split-Path -Path $path_output -Extension
if($output_ext.Length -gt 0) { throw "Invalid output $path_output. Must be a folder." }

# If output files already exist then terminate. Uses function Test-Output.
if ($multiple_files){
    For ($i=1; $i -lt $max_file_count; $i++){
        $s = [string]$i
        if ($i -lt 10) {$s = '0' + $s}
        Test-Output $path_output ($output_file + '_' + $s)
    }
}else { Test-Output $path_output $output_file}

# If there is no delimiter at the end of the folder name one is added.
# Convert path_output to string in order to apply Trimend().
$path_output_string = ([string]$path_output).Trim()
$delimiter ='\'
if($IsLinux) {$delimiter = '/'}
$path_output_string = $path_output_string.Trimend($delimiter) + $delimiter 

# Check that the executable files are found in the same folder as the script. 
# $no_check = $false (the default) means the check is carried out.
if (-not $no_check){
    if($IsWindows){ 
        if(-not (Test-Path -Path "$PSScriptRoot/$executable_name.exe")) {throw "Executable $executable_name.exe $not_found"}
    }
    if($IsLinux){
        if (-not (Test-Path -Path "$PSScriptRoot/$executable_name")){throw "Executable $executable_name $not_found"}
    }
}

# Call the executable. All parameters must be supplied, must be in the given order and may not be empty.
Write-Host "`n*** CALLING  $executable_name with inputs:   $input_file    $path_output_string    $output_file    $size_block    $m    $rows_output  $update_percentage"

if($IsWindows) { $cmd = "$PSScriptRoot/$executable_name.exe" }
if($IsLinux) { $cmd = "$PSScriptRoot/$executable_name" }
& $cmd $input_file $path_output_string $output_file $size_block $m $rows_output $update_percentage