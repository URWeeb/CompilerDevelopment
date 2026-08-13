#!/usr/bin/env bash
set -euo pipefail

LLVM_DIR="${LLVM_DIR:?Задайте LLVM_DIR, например: export LLVM_DIR=/home/parallels/llvm-project}"
TARGET="${TARGET:-x86_64-unknown-linux-gnu}"

BIN="$LLVM_DIR/build/bin"
CLANG="$BIN/clang"
OPT="$BIN/opt"
FILECHECK="$BIN/FileCheck"

for tool in "$CLANG" "$OPT" "$FILECHECK"; do
  if [[ ! -x "$tool" ]]; then
    echo "Не найден исполняемый файл: $tool" >&2
    echo "Проверьте LLVM_DIR и что LLVM собран (для FileCheck нужен LLVM_INCLUDE_UTILS=ON, включено по умолчанию)." >&2
    exit 1
  fi
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKDIR="$(mktemp -d)"
trap 'rm -rf "$WORKDIR"' EXIT

FAIL=0

run_case() {
  local name="$1" src="$2" plugin="$3" pass="$4"
  local ll="$WORKDIR/$name.ll"

  echo "== $name =="

  "$CLANG" -O0 -S -emit-llvm --target="$TARGET" "$src" -o "$ll"

  if "$OPT" -load-pass-plugin "$SCRIPT_DIR/$plugin" -passes="$pass" \
       -disable-output "$ll" 2>&1 | "$FILECHECK" "$src"; then
    echo "  OK"
  else
    echo "  FAILED"
    FAIL=1
  fi
}

run_case "rpo_pass"              "$SCRIPT_DIR/tests/loop_test.c"               "libRPOPass.so"       "rpo-pass"
run_case "rpo_pass_two_continues" "$SCRIPT_DIR/tests/loop_test_two_continues.c" "libRPOPass.so"       "rpo-pass"
run_case "inst_count_pass"       "$SCRIPT_DIR/tests/inst_count_test.c"         "libInstCountPass.so" "inst-count-pass"
run_case "inst_count_pass_switch" "$SCRIPT_DIR/tests/switch_test.c"            "libInstCountPass.so" "inst-count-pass"

if [[ "$FAIL" -ne 0 ]]; then
  echo
  echo "Некоторые тесты упали."
  exit 1
fi

echo
echo "Все тесты прошли."
