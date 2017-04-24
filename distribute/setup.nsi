; Windows installer script for Tribalia
; Copyright (C) 2017 Arthur M
;
;
;

!include x64.nsh

Name "Tribalia"
OutFile "TribaliaInstall.exe"
InstallDir "$PROGRAMFILES\Tribalia"


RequestExecutionLevel admin 


; pages
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

; Default install
Section ""
	SetOutPath "$TEMP\tribalia"
	File "depend\vc_redist.x86.exe"

	; Check if we need to install Visual C++ Redistributable
	${If} ${RunningX64}
		ReadRegStr $1 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Installed"
		StrCmp $1 1 vcrinstalled
	${Else}
		ReadRegStr $1 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86" "Installed"
		StrCmp $1 1 vcrinstalled
	${EndIf}

	; Install
	DetailPrint "Installing prerequisite: Visual C++ 2015 Redistributable" ; because MS don't bundle it with windows
	ExecWait '$TEMP\tribalia\vc_redist.x86.exe' $0

	${If} $0 != 0 
		DetailPrint "Prerequisite install canceled"
		Abort
	${EndIf}

	vcrinstalled:
	SetOutPath "$INSTDIR"
	File "tribalia.exe"
	File "zlib1.dll"
	File "SDL2.dll"
	File "libpng14-14.dll"
	File "libfontconfig-1.dll"
	File "libexpat-1.dll"
	File "libcairo-2.dll"
	File "glew32.dll"
	File "freetype6.dll"
	File "DevIL.dll"
	
	File "assets.taif"
	File "terrain_test.trtb"

	SetOutPath "$INSTDIR\materials"
	File "materials\*.mtl"

	SetOutPath "$INSTDIR\models"
	File "models\*.obj"
	File "models\*.md2"

	SetOutPath "$INSTDIR\shaders"
	File "shaders\*.vert"
	File "shaders\*.frag"

	SetOutPath "$INSTDIR\textures"
	File "textures\*.bmp"
	File "textures\*.png"

	SetOutPath "$INSTDIR\textures\terrain"
	File "textures\terrain\*.*"

	SetOutPath "$INSTDIR"
	WriteUninstaller "uninstall.exe"
SectionEnd
Section "Create links"
	CreateDirectory "$SMPROGRAMS\Tribalia"
	CreateShortcut "$SMPROGRAMS\Tribalia\Tribalia.lnk" "$INSTDIR\tribalia.exe"
	CreateShortcut "$SMPROGRAMS\Tribalia\Uninstall Tribalia.lnk" "$INSTDIR\uninstall.exe"
	CreateShortcut "$DESKTOP\Tribalia.lnk" "$INSTDIR\tribalia.exe"


SectionEnd

Section "Uninstall"
	
	Delete "$INSTDIR\tribalia.exe"
	Delete "$INSTDIR\zlib1.dll"
	Delete "$INSTDIR\SDL2.dll"
	Delete "$INSTDIR\libpng14-14.dll"
	Delete "$INSTDIR\libfontconfig-1.dll"
	Delete "$INSTDIR\libexpat-1.dll"
	Delete "$INSTDIR\libcairo-2.dll"
	Delete "$INSTDIR\glew32.dll"
	Delete "$INSTDIR\freetype6.dll"
	Delete "$INSTDIR\DevIL.dll"
	
	Delete "$INSTDIR\assets.taif"
	Delete "$INSTDIR\terrain_test.trtb"

	Delete "$SMPROGRAMS\Tribalia\Tribalia.lnk"
	Delete "$SMPROGRAMS\Tribalia\Uninstall Tribalia.lnk"
	Delete "$DESKTOP\Tribalia.lnk"

	Delete "$INSTDIR\uninstall.exe"
	
	RMDir /r "$INSTDIR\materials"
	RMDir /r "$INSTDIR\models"
	RMDir /r "$INSTDIR\shaders"
	RMDIR /r "$INSTDIR\textures\terrain"
	RMDIR /r "$INSTDIR\textures"
	
	RMDir "$INSTDIR"
	RMDIR "$SMPROGRAMS\Tribalia"
SectionEnd
