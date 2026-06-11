; Inno Setup Script for Student Management System
; Compile this with Inno Setup Compiler to generate the installer .exe

[Setup]
AppName=Student Management System
AppVersion=1.0
AppPublisher=Mikiyas
DefaultDirName={autopf}\StudentManagementSystem
DefaultGroupName=Student Management System
OutputDir=installer_output
OutputBaseFilename=StudentManagementSystemSetup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Files]
Source: "release\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Student Management System"; Filename: "{app}\SchoolGUI.exe"
Name: "{commondesktop}\Student Management System"; Filename: "{app}\SchoolGUI.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional icons:"

[Run]
Filename: "{app}\SchoolGUI.exe"; Description: "Launch Student Management System"; Flags: nowait postinstall skipifsilent
