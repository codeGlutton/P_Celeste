
xcopy .\*.h .\..\Engine\Include\ /D /Y /s /R
xcopy .\*.hpp .\..\Engine\Include\ /D /Y /s /R
xcopy .\*.cs .\..\Engine\Include\ /D /Y /s /R
xcopy .\*.inl .\..\Engine\Include\ /D /Y /s /R
xcopy .\..\Engine\Binary\EngineAsset\*.* .\..\Binary\EngineAsset\ /D /Y /s /R

attrib +R ..\Engine\Include\*.* /S
