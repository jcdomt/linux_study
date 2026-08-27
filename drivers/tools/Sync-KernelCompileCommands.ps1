param(
    [string]$Source = "\\10.249.254.67\luban\LubanCat_SDK\kernel\compile_commands.linux.json",
    [string]$Destination = "\\10.249.254.67\luban\LubanCat_SDK\kernel\compile_commands.json",
    [string]$LinuxKernelRoot = "/root/LubanCat_SDK/kernel",
    [string]$WindowsKernelRoot = "\\10.249.254.67\luban\LubanCat_SDK\kernel",
    [string]$LinuxDriversRoot = "/root/work/drivers",
    [string]$WindowsDriversRoot = "\\10.249.254.67\luban\work\drivers"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $Source)) {
    throw "Linux compile database not found: $Source"
}

$entries = Get-Content -LiteralPath $Source -Raw | ConvertFrom-Json
foreach ($entry in $entries) {
    foreach ($property in @("directory", "file", "output")) {
        if ($null -ne $entry.$property) {
            $entry.$property = $entry.$property.Replace($LinuxKernelRoot, $WindowsKernelRoot).Replace($LinuxDriversRoot, $WindowsDriversRoot)
        }
    }

    if ($null -ne $entry.arguments) {
        $entry.arguments = @($entry.arguments | ForEach-Object {
            $_.Replace($LinuxKernelRoot, $WindowsKernelRoot).Replace($LinuxDriversRoot, $WindowsDriversRoot)
        })
    }

    if ($null -ne $entry.command) {
        $entry.command = $entry.command.Replace($LinuxKernelRoot, $WindowsKernelRoot).Replace($LinuxDriversRoot, $WindowsDriversRoot)
    }
}

$entries | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $Destination -Encoding utf8
Write-Host "clangd database updated: $Destination"
