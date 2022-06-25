# GetDiscogsVideos

## Description
Given a Discogs releases `xml` file this extracts those records which have YouTube videos, including the list of videos and some additional information.<br>Windows and Linux versions are available.

## Basic Operation
The extract function is invoked by a PowerShell script which parses its arguments and then calls a C++ executable.<br>The script may be run from a PowerShell terminal in Windows or the PowerShell environment in Linux.<br>As an example, the command 

`PS_GetDiscogsVideos.ps1` &emsp; `D:\discogs_20220501_releases.xml` &emsp; `-r 250000`

will extract the records with videos from the May 2022 releases file. The records will be written to `csv` output files, e.g. `output_01.csv`, `output_02.csv`, etc.  in the same folder as the PowerShell script. Each output file (except possibly the last) will contain 250,000 records.

## Installation
The user need only copy the executable and script files to whatever folder they choose, but the files must reside in the same folder.

For Windows the files are `GetDiscogsVideos.exe` and `PS_GetDiscogsVideos.ps1`.<br>For Linux they are `GetDiscogsVideos` and either of `PS_GetDiscogsVideos.ps1` or `AB_GetDiscogsVideos.sh`.<br>The latter is a bash script which may be used in place of the `.ps1` script if PowerShell is not available in Linux.

## Details
Details of usage and methodology are given in the [documentation](https://github.com/User2429/GetDiscogsRelease/blob/master/docs/Documentation.md).