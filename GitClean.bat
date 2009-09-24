path=%PATH%;%PROGRAMFILES%\Microsoft Visual Studio 8\Common7\IDE
devenv.com FusionForever.sln /clean Debug
devenv.com FusionForever.sln /clean Release

C:\WINDOWS\system32\cmd.exe /c ""C:\Program Files\Git\bin\sh.exe" --login -i GitClean.sh"
