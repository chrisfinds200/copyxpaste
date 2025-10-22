#!/usr/bin/env bash
# install.sh — Zero-arg installer for a prebuilt binary (macOS + Linux, Bash 3.2+)
# Usage (no args needed): ./install.sh
# Optional: ./install.sh [<binary>] [--name=cmd] [--prefix=/usr/local] [--symlink] [--no-path] [--no-force]

set -euo pipefail

die() { echo "Error: $*" >&2; exit 1; }
warn() { echo "[!] $*" >&2; }
info() { echo "[*] $*"; }
ok()   { echo "[✓] $*"; }
have_cmd() { command -v "$1" >/dev/null 2>&1; }
is_macos() { [[ "$(uname -s)" == "Darwin" ]]; }

# Portable absolute path resolver without heredocs
abspath() {
  if have_cmd realpath; then realpath "$1" 2>/dev/null && return 0; fi
  if have_cmd python3; then python3 -c 'import os,sys;print(os.path.abspath(sys.argv[1]))' "$1" && return 0; fi
  ( cd "$(dirname "$1")" 2>/dev/null || exit 1; echo "$PWD/$(basename "$1")" )
}

# Try to find the binary automatically (prefer ./copyxpaste)
autodetect_binary() {
  if [[ -f "./copyxpaste" ]]; then echo "./copyxpaste"; return 0; fi

  # Gather executables in CWD (excluding this script and obvious shell scripts)
  local f
  local candidates=()
  # First pass: exclude *.sh/*.bash and install.sh
  while IFS= read -r f; do candidates+=("$f"); done \
    < <(find . -maxdepth 1 -type f -perm -u+x ! -name 'install.sh' ! -name '*.sh' ! -name '*.bash' -print)

  # Fallback: allow any executable except install.sh
  if (( ${#candidates[@]} == 0 )); then
    while IFS= read -r f; do candidates+=("$f"); done \
      < <(find . -maxdepth 1 -type f -perm -u+x ! -name 'install.sh' -print)
  fi

  if   (( ${#candidates[@]} == 1 )); then echo "${candidates[0]}";
  elif (( ${#candidates[@]}  > 1 )); then
    warn "Multiple executable files detected:"
    local c; for c in "${candidates[@]}"; do echo "  - $c"; done
    die "Please rerun as: ./install.sh <binary>  (or leave only the one you want)"
  else
    die "No executable file found in the current directory."
  fi
}

# --------------------------- defaults & args ----------------------------------
BIN_SRC="${1:-}"
CMD_NAME=""                    # default to filename
PREFIX="${HOME}/.local"        # default prefix (no sudo)
DO_SYMLINK=0
AUTO_PATH=1
FORCE=1

# If first arg is a binary path (not an option), consume it
if [[ -n "${BIN_SRC}" && "${BIN_SRC}" != --* ]]; then
  shift
else
  BIN_SRC=""
fi

# Parse flags (Bash 3-safe)
while (( "$#" )); do
  case "$1" in
    --name=*)    CMD_NAME="${1#*=}";;
    --prefix=*)  PREFIX="${1#*=}";;
    --symlink)   DO_SYMLINK=1;;
    --no-path)   AUTO_PATH=0;;
    --no-force)  FORCE=0;;
    -h|--help)   sed -n '1,200p' "$0"; exit 0;;
    *)           die "Unknown option: $1";;
  esac
  shift
done

# Autodetect the binary if not provided
if [[ -z "${BIN_SRC}" ]]; then
  BIN_SRC="$(autodetect_binary)"
fi
[[ -e "$BIN_SRC" ]] || die "binary not found: $BIN_SRC"

# Ensure executable
if [[ ! -x "$BIN_SRC" ]]; then
  info "making binary executable: $BIN_SRC"
  chmod +x "$BIN_SRC" || die "failed to chmod +x $BIN_SRC"
fi

# Default command name is the file name
CMD_NAME="${CMD_NAME:-$(basename "$BIN_SRC")}"
DESTBIN="${PREFIX%/}/bin"
TARGET="${DESTBIN}/${CMD_NAME}"

# --------------------------- privilege strategy --------------------------------
NEED_SUDO=0
if [[ -d "$DESTBIN" && ! -w "$DESTBIN" ]]; then NEED_SUDO=1; fi
if [[ ! -d "$DESTBIN" ]]; then
  PARENT="$(dirname "$DESTBIN")"
  if [[ ! -w "$PARENT" ]]; then NEED_SUDO=1; fi
fi
SUDO=""
if [[ "$NEED_SUDO" -eq 1 ]]; then
  if have_cmd sudo; then SUDO="sudo"; else die "Need sudo to write ${DESTBIN}, but 'sudo' not found."; fi
fi

# --------------------------- create destination --------------------------------
if [[ ! -d "$DESTBIN" ]]; then
  info "creating ${DESTBIN}"
  if [[ -n "$SUDO" ]]; then
    $SUDO mkdir -p "$DESTBIN" && $SUDO chmod 755 "$DESTBIN"
  else
    mkdir -p "$DESTBIN"
  fi
fi

# ------------------------------ install ----------------------------------------
if [[ -e "$TARGET" ]]; then
  if [[ "$FORCE" -eq 1 ]]; then
    info "removing existing ${TARGET}"
    if [[ -n "$SUDO" ]]; then $SUDO rm -f "$TARGET"; else rm -f "$TARGET"; fi
  else
    die "Target already exists: ${TARGET} (rerun without --no-force or remove it)"
  fi
fi

if [[ "$DO_SYMLINK" -eq 1 ]]; then
  SRC_ABS="$(abspath "$BIN_SRC")"
  info "creating symlink ${TARGET} -> ${SRC_ABS}"
  if [[ -n "$SUDO" ]]; then $SUDO ln -s "$SRC_ABS" "$TARGET"; else ln -s "$SRC_ABS" "$TARGET"; fi
else
  info "copying ${BIN_SRC} -> ${TARGET}"
  if [[ -n "$SUDO" ]]; then
    $SUDO cp "$BIN_SRC" "$TARGET"; $SUDO chmod +x "$TARGET"
  else
    cp "$BIN_SRC" "$TARGET"; chmod +x "$TARGET"
  fi
fi

# Remove macOS quarantine if present
if is_macos && have_cmd xattr; then
  if xattr -p com.apple.quarantine "$TARGET" >/dev/null 2>&1; then
    info "removing macOS quarantine from ${TARGET}"
    if [[ -n "$SUDO" ]]; then $SUDO xattr -dr com.apple.quarantine "$TARGET"; else xattr -dr com.apple.quarantine "$TARGET"; fi
  fi
fi

ok "installed: ${TARGET}"

# ------------------------ ensure PATH persistence ------------------------------
ensure_path() {
  local bin_dir="$1"
  local shell_name="${SHELL##*/}"

  [[ "$AUTO_PATH" -eq 1 ]] || { info "skipping PATH updates (--no-path)"; return 0; }

  case ":${PATH}:" in *:"${bin_dir}":*) return 0;; esac

  if [[ "$shell_name" == "zsh" ]]; then
    # On macOS zsh often needs both files
    if is_macos; then grep -qF "$bin_dir" ~/.zprofile 2>/dev/null || echo "export PATH=\"$bin_dir:\$PATH\"" >> ~/.zprofile; fi
    grep -qF "$bin_dir" ~/.zshrc 2>/dev/null    || echo "export PATH=\"$bin_dir:\$PATH\"" >> ~/.zshrc
    ok "added ${bin_dir} to PATH in your zsh config."
  elif [[ "$shell_name" == "bash" ]]; then
    grep -qF "$bin_dir" ~/.bashrc 2>/dev/null   || echo "export PATH=\"$bin_dir:\$PATH\"" >> ~/.bashrc
    grep -qF "$bin_dir" ~/.profile 2>/dev/null  || echo "export PATH=\"$bin_dir:\$PATH\"" >> ~/.profile
    ok "added ${bin_dir} to PATH in your bash config."
  elif [[ "$shell_name" == "fish" ]]; then
    mkdir -p ~/.config/fish
    if ! grep -qF "$bin_dir" ~/.config/fish/config.fish 2>/dev/null; then
      echo "set -Ux PATH $bin_dir \$PATH" >> ~/.config/fish/config.fish
    fi
    ok "added ${bin_dir} to PATH in fish config."
  else
    grep -qF "$bin_dir" ~/.profile 2>/dev/null  || echo "export PATH=\"$bin_dir:\$PATH\"" >> ~/.profile
    ok "added ${bin_dir} to PATH in ~/.profile."
  fi
}

ensure_path "$DESTBIN"

# ------------------------- update current session ------------------------------
case ":${PATH}:" in *:"${DESTBIN}":*) :;; *)
  export PATH="${DESTBIN}:${PATH}"
  info "PATH updated for current shell session."
esac

hash -r 2>/dev/null || true

if have_cmd "$CMD_NAME"; then
  INSTALLED_PATH="$(command -v "$CMD_NAME")"
  ok "command available now: $CMD_NAME  (at $INSTALLED_PATH)"
else
  warn "command not yet found in this shell."
  if [[ -n "${ZSH_VERSION-}" ]]; then
    echo "  source ~/.zprofile 2>/dev/null || true"
    echo "  source ~/.zshrc"
  elif [[ -n "${BASH_VERSION-}" ]]; then
    echo "  source ~/.profile 2>/dev/null || true"
    echo "  source ~/.bashrc"
  else
    echo "  Open a new terminal window"
  fi
fi

echo
echo "Try:"
echo "  ${CMD_NAME} --help    # or: ${CMD_NAME} --version"
