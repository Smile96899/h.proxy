[Setup]
AppId={{82364378-58AC-47A0-A512-B42D6BC7B571}
AppName=h.
AppVersion=1.0
VersionInfoVersion=1.0.0.0
DefaultDirName={localappdata}\Programs\h
DefaultGroupName=h.
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir=..\deployment
OutputBaseFilename=h-v1.0-setup
SetupIconFile=..\res\h.ico
UninstallDisplayIcon={app}\h.exe
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
DisableProgramGroupPage=yes
DisableWelcomePage=no
CloseApplications=yes
RestartApplications=no

[Languages]
Name: "chinesesimp"; MessagesFile: "ChineseSimplified.isl"

[Files]
Source: "..\deployment\h-install-payload\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\h."; Filename: "{app}\h.exe"; WorkingDir: "{app}"

[UninstallDelete]
Type: files; Name: "{userdesktop}\h.lnk"

[Code]
var DesktopCheck: TNewCheckBox;

procedure InitializeWizard;
begin
  DesktopCheck := TNewCheckBox.Create(WizardForm);
  DesktopCheck.Parent := WizardForm.FinishedPage;
  DesktopCheck.Left := WizardForm.FinishedLabel.Left;
  DesktopCheck.Top := WizardForm.FinishedLabel.Top + WizardForm.FinishedLabel.Height + ScaleY(16);
  DesktopCheck.Width := ScaleX(300);
  DesktopCheck.Height := ScaleY(24);
  DesktopCheck.Caption := '创建桌面快捷方式';
  DesktopCheck.Checked := False;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
  if (CurPageID = wpFinished) and DesktopCheck.Checked and not WizardSilent then
    CreateShellLink(ExpandConstant('{userdesktop}\h.lnk'), 'h.', ExpandConstant('{app}\h.exe'), '', ExpandConstant('{app}'), ExpandConstant('{app}\h.exe'), 0, SW_SHOWNORMAL);
end;
