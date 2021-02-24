﻿# powershell -ExecutionPolicy ByPass -File Install-win64-mingw.ps1
# get-executionpolicy
# Start-Process powershell -Verb runAs
# Set-ExecutionPolicy RemoteSigned
# powershell -ExecutionPolicy ByPass ".\Install-win64-mingw.ps1 -installPrefix "C:\usr" "


#Parameters
#The script could take 2 arguments.
param(  [string]$installPrefix="C:\usr",
        [string]$mingwPath ="C:\Qt5\Tools\mingw730_64\bin",
        [string]$cmakeVersion="3.16.8",
        [string]$curlVersion="7.75.0_3",
        [string]$ssh2Version="1.9.0_3",
        [string]$opensslVersion="1.1.1j_3"
        )



#Functions
#2)	Functions

#Create a function named Check-Folder that checks for the existence of a specific directory/folder that is passed 
#to it as a parameter. Also, include a switch parameter named create. If the directory/folder does not exist and 
#the create switch is specified, a new folder should be created using the name of the folder/directory that was 
#passed to the function.
function Check-Folder([string]$path, [switch]$create){
    $exists = Test-Path $path

    if(!$exists -and $create){
        #create the directory because it doesn't exist
        mkdir $path | out-null
        $exists = Test-Path $path
    }
    return $exists
}


# Load data from url to installationPath
Function Load-To ( [string] $url, [string] $fname )
{
  Write-Host ('Downloading {0} installer from {1} ..' -f $fname, $url);
  #Invoke-WebRequest -Uri $url -OutFile $outpath
  (New-Object System.Net.WebClient).DownloadFile($url, $fname);
  Write-Host ('Downloaded {0} bytes' -f (Get-Item $fname).length);
}

# Uzip 
# https://stackoverflow.com/questions/43715949/powershell-extract-specific-files-folders-from-a-zipped-archive
Function Unzip-Dir ( [string] $fname, [string] $dir, [string] $destination )
{
 Add-Type -Assembly System.IO.Compression.FileSystem

 #extract list entries for dir myzipdir/c/ into myzipdir.zip
 $zip = [IO.Compression.ZipFile]::OpenRead($fname)

 $outputFile = [io.path]::GetFileNameWithoutExtension($fname)
 $outputFile = "$outputFile/$dir/"

 $entries=$zip.Entries | where {$_.FullName -like "$outputFile*" -and $_.FullName -ne "$outputFile"} 

 #create dir for result of extraction
 $destinationexists = Check-Folder $destination -create

 #extraction
 $entries | foreach {[IO.Compression.ZipFileExtensions]::ExtractToFile( $_, "$destination\" + $_.Name, $true ) }

  #free object
  $zip.Dispose()
}


#3)	Main processing

#a) Main links.

$localPrefix="$installPrefix\local"
$binPrefix="$localPrefix\bin"
$libPrefix="$localPrefix\lib"
$includePrefix="$localPrefix\include"

$major, $minor, $patch = $cmakeVersion.split('.')
$cmakeUrl = ('https://cmake.org/files/v{0}.{1}/cmake-{2}-win64-x64.zip' -f $major, $minor, $cmakeVersion)
$cmakeName = "$installPrefix/cmake-$cmakeVersion-win64-x64.zip"

#https://curl.haxx.se/windows/dl-7.67.0/curl-7.67.0-win64-mingw.zip
#https://curl.se/windows/dl-7.73.0/
#https://curl.se/windows/dl-7.75.0_3/
$curlUrl = ('https://curl.haxx.se/windows/dl-{0}/curl-{0}-win64-mingw.zip' -f $curlVersion)
$curlName = "$installPrefix/curl-$curlVersion-win64-mingw.zip"

#https://curl.haxx.se/windows/dl-7.67.0/libssh2-1.9.0-win64-mingw.zip
#
$ssh2Url = ('https://curl.haxx.se/windows/dl-{0}/libssh2-{1}-win64-mingw.zip' -f $curlVersion, $ssh2Version )
$ssh2Name = "$installPrefix/libssh2-$ssh2Version-win64-mingw.zip"

#https://curl.haxx.se/windows/dl-7.67.0_2/openssl-1.1.1d_2-win64-mingw.zip
$opensslUrl = ('https://curl.haxx.se/windows/dl-{0}_2/openssl-{1}-win64-mingw.zip' -f $curlVersion, $opensslVersion )
$opensslName = "$installPrefix/openssl-$opensslVersion-win64-mingw.zip"


#b) Test for the existence of the destination folders; create it if it is not found.
$destinationexists = Check-Folder $installPrefix -create

if (!$destinationexists){
    Write-Host "The destination directory is not found. Script can not continue."
    Exit
}

$destinationexists = Check-Folder $localPrefix -create
$destinationexists = Check-Folder $binPrefix -create
$destinationexists = Check-Folder $libPrefix -create
$destinationexists = Check-Folder $includePrefix -create

#c) Install cmake

# Download from "https://cmake.org/download/"
Load-To $cmakeUrl $cmakeName
# Unpack to  "C:/usr" 
Expand-Archive $cmakeName -DestinationPath $installPrefix -Force
# Delete $cmakeName
Remove-Item –path $cmakeName

#d) Install curl

# Download curl from https://curl.haxx.se/windows/
Load-To $curlUrl $curlName
# Unpack and Copy lib, bin and include to c:/usr/local
Unzip-Dir $curlName "bin" "$localPrefix/bin"
Unzip-Dir $curlName "lib" "$localPrefix/lib"
Unzip-Dir $curlName "include/curl" "$localPrefix/include/curl"
# Expand-Archive $curlName -DestinationPath $installPrefix -Force
# Delete $curlName
Remove-Item –path $curlName


#) Install ssh2

## Download libssh2 1.9.0 from https://curl.haxx.se/windows/
#Load-To $ssh2Url $ssh2Name
## Unpack and Copy lib, bin and include to c:/usr/local
#Unzip-Dir $ssh2Name "bin" "$localPrefix/bin"
#Unzip-Dir $ssh2Name "lib" "$localPrefix/lib"
#Unzip-Dir $ssh2Name "include" "$localPrefix/include"
##Expand-Archive $ssh2Name -DestinationPath $installPrefix -Force


#e) Install OpenSSL

# Download OpenSSL  from https://curl.haxx.se/windows/
Load-To $opensslUrl $opensslName
# Unpack 
#Expand-Archive $ssh2Name -DestinationPath $installPrefix
##  copy libcrypto-1_1-x64.dll, libssl-1_1-x64.dll and libcurl-x64.dll to executable in pro


