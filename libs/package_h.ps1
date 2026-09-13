param([string]$Name = 'h-portable-v1.0-independent', [string]$Dependencies = "$PSScriptRoot/../../h-build-deps")
& "$PSScriptRoot/package_installer.ps1" -Name $Name -Dependencies $Dependencies -Portable
