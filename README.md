<h1 align="center">🚀 CopyXPaste</h1>

<p align="center">
  <b>Instant cross-platform clipboard power tool</b><br>
  <sub>Works on macOS, Linux, and Windows.</sub>
</p>

---

## 🧩 Installation

### 🐧 macOS / Linux

> 🧠 **Zero setup required** — just unzip and run the included installer.

```bash
# 1. Unzip the release
unzip copyxpaste.zip && cd copyxpaste

# 2. Run the installer
chmod +x install.sh
./install.sh

# 3. Verify installation
which copyxpaste
copyxpaste --help
# copyxpaste
``` 

The installer will:
1. Copy the binary to ~/.local/bin/ 
2. Add that folder to your PATH (if needed)
3. Make the command immediately available 
4. Fix macOS “quarantine” flags automatically

🧹 Once installed, you can safely delete the zip and folder — your command lives in
~/.local/bin/copyxpaste.

### ⚙️ Windows

> 💡 No terminal needed — just double-click install.bat.

#### Steps:
1. Download and unzip the release.
2. Double-click install.bat.
3. Watch it copy copyxpaste.exe to: `C:\Users\<you>\AppData\Local\bin\
`
4. It will automatically:
   1. Add that path to your User PATH 
   2. Make the command immediately usable in PowerShell or CMD
5. To verify installation open a new cmd window and type: `copyxpaste` you should see: 
`Usage: copyxpaste [options]`
