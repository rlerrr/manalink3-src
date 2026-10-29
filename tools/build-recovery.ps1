param(
    [string]$Toolchain = 'C:\bin\gcc-9.3.0-no-debug',
    [int]$Jobs = 4
)

$ErrorActionPreference = 'Stop'
$buildRoot = Split-Path -Parent $PSScriptRoot
$artifactDirectory = Join-Path $buildRoot '.recovery\build-gcc93'
$objectDirectory = Join-Path $artifactDirectory 'obj'
foreach ($directory in @($artifactDirectory, $objectDirectory,
        (Join-Path $objectDirectory 'cards'), (Join-Path $objectDirectory 'functions'))) {
    New-Item -ItemType Directory -Force -Path $directory | Out-Null
}
foreach ($name in @('ManalinkEh.dll', 'ManalinkEh.dbg', 'ManalinkEh.ao',
        'libDrawcardlib.a', 'libcapturestackbacktrace.a')) {
    $existing = Join-Path $buildRoot $name
    $backupDirectory = Join-Path $artifactDirectory 'before'
    if ((Test-Path -LiteralPath $existing) -and
        -not (Test-Path -LiteralPath (Join-Path $backupDirectory $name))) {
        New-Item -ItemType Directory -Force -Path $backupDirectory | Out-Null
        Copy-Item -LiteralPath $existing -Destination (Join-Path $backupDirectory $name)
    }
}
$previousPath = $env:PATH
$env:PATH = "$(Join-Path $Toolchain 'bin');$buildRoot;C:\Program Files\Git\usr\bin;$previousPath"
$log = Join-Path $artifactDirectory 'build.log'
Push-Location $buildRoot
try {
    # Recreate the import archives for the same architecture as the DLL.
    foreach ($library in @('Drawcardlib', 'capturestackbacktrace')) {
        & (Join-Path $Toolchain 'bin\dlltool.exe') '-m' 'i386' '-f' '--32' `
            '-S' (Join-Path $Toolchain 'bin\as.exe') '-k' `
            '-d' "$library.def" '-l' "lib$library.a"
        if ($LASTEXITCODE -ne 0) { throw "Could not create the 32-bit $library import library." }
    }
    # Windows PowerShell 5.1 wraps redirected native stderr as error records.
    # GCC warnings must reach the log without terminating an otherwise valid
    # build. The native process exit code still determines success or failure.
    $ErrorActionPreference = 'Continue'
    try {
        & (Join-Path $buildRoot 'make.exe') '-k' "-j$Jobs" 'CC=gcc -m32' 'CXX=g++ -m32' `
            'DLLTOOL=dlltool -m i386 -f --32' 'ASM=yasm' 'WERROR=' `
            'OBJDIR=.recovery/build-gcc93/obj/' *> $log
        $buildExitCode = $LASTEXITCODE
    } finally {
        $ErrorActionPreference = 'Stop'
    }
} finally {
    Pop-Location
    $env:PATH = $previousPath
}
if ($buildExitCode -eq 0 -and (Test-Path -LiteralPath (Join-Path $buildRoot 'docs/recovery-evidence/native-jump-table-recovery.json'))) {
    # Fixed DLL entry addresses are consumed by Magic.exe and Cards.dat.
    # A link can succeed with a stale/truncated veneer object; reject it here.
    Push-Location $buildRoot
    try {
        & python (Join-Path $buildRoot 'tools/recovery/verify_native_jump_table.py') '--binary-only'
        if ($LASTEXITCODE -ne 0) { $buildExitCode = $LASTEXITCODE }
    } finally {
        Pop-Location
    }
}
Write-Host "Build log: $log"
Get-Content -LiteralPath $log -Tail 25 | ForEach-Object {
    if ($_.Length -gt 280) { $_.Substring(0, 280) + ' ...' } else { $_ }
}
exit $buildExitCode
