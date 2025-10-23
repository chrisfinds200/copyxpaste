<h1 align="center">🚀 CopyXPaste</h1>

<p align="center">
  <b>Instant cross-platform clipboard power tool</b><br>
  <sub>Works on macOS, Linux, and Windows.</sub>
</p>

---
## 🧑‍💻 About
I created this project, so I could finally copy and paste across my devices, instead of using Google Docs or even good old 
WhatsApp Web. It's by no means complete, I plan to make more improvements moving forward, but it's at least in a working 
state, so I figured I would release it.

---

## 🧩 Installation
> ⚠️ Attention! For Windows users. Windows defender is not going to be happy if you download the binary straight from the release section. Please refer to Windows installation section below for steps on how to get around it :D 

### 🐧 macOS / Linux

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
Before attempting to follow the steps below you will most likely need to take care of Windows defender,
otherwise it will think it's a virus due to the binary not being signed by a trusted CA since I did not purchase a cert. 
Is it a virus?.... maybe :P

Not it's not but if you don't trust me you can take a look at the code for yourself, and even build it straight from the source using cmake if you wish! For everyone else, here's what you need to do:

1. Navigate to Windows Settings `````(ctrl + I)`````
2. Click on **Privacy & security** > **Windows Security** > **Virus & threat protection**
3. Once here, look for **Virus & threat protection settings** and click on **Manage settings**
4. Scroll down until you see **Exclusions**
5. Click on **Add or remove exclusion**
6. Click "Yes" on pop-up window if any.
7. Lastly click on **Add an exclusion** select **Folder** and select the target folder for your downloads, in most cases that would be the ```/Downloads``` folder
8. **Do not close this window yet.** Follow the installation steps below, and once finished make sure you remove your Downloads folder (or whatever folder you chose) from the exclusions list. 

#### Installation Steps:
1. Download and unzip the release.
2. Double-click install.bat.
3. Watch it copy copyxpaste.exe to: `C:\Users\<you>\AppData\Local\bin\
`
4. It will automatically:
   1. Add that path to your User PATH 
   2. Make the command immediately usable in PowerShell or CMD
5. To verify installation open a new cmd window and type: `copyxpaste` you should see: 
`Usage: copyxpaste [options]`

🧹 Once installed, you can safely delete the zip and folder — your command lives in `C:\Users\<you>\AppData\Local\bin\copyxpaste`.
