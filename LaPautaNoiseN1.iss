; Instalador de Windows para La Pauta Noise N1 (Inno Setup 6)
; Se compila desde la raiz del repo:  ISCC installer\windows\LaPautaNoiseN1.iss

#define AppName "La Pauta Noise N1"
#define AppVersion "1.0.0"
#define Artefacts "..\..\build\LaPautaNoiseN1_artefacts\Release"

[Setup]
AppId={{B3F1C6E2-5A7D-4C1B-9E55-4A1F0D2C7A11}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher=La Pauta
DefaultDirName={autopf}\La Pauta\Noise N1
DefaultGroupName=La Pauta
OutputDir=..\..\dist
OutputBaseFilename=LaPautaNoiseN1-Setup-Windows
Compression=lzma2
SolidCompression=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
WizardStyle=modern
UninstallDisplayName={#AppName}
DisableProgramGroupPage=yes

[Languages]
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Types]
Name: "full"; Description: "Instalacion completa"
Name: "custom"; Description: "Personalizada"; Flags: iscustom

[Components]
Name: "vst3"; Description: "Plugin VST3 (carpeta comun de VST3)"; Types: full custom
Name: "standalone"; Description: "Aplicacion Standalone"; Types: full

[Files]
; VST3 -> C:\Program Files\Common Files\VST3
Source: "{#Artefacts}\VST3\La Pauta Noise N1.vst3\*"; DestDir: "{commoncf64}\VST3\La Pauta Noise N1.vst3"; Flags: recursesubdirs createallsubdirs ignoreversion; Components: vst3
; Standalone
Source: "{#Artefacts}\Standalone\La Pauta Noise N1.exe"; DestDir: "{app}"; Flags: ignoreversion; Components: standalone

[Icons]
Name: "{group}\La Pauta Noise N1"; Filename: "{app}\La Pauta Noise N1.exe"; Components: standalone
Name: "{group}\Desinstalar La Pauta Noise N1"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\La Pauta Noise N1.exe"; Description: "Abrir La Pauta Noise N1"; Flags: nowait postinstall skipifsilent unchecked; Components: standalone
