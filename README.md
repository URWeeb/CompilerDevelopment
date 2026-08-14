# CompilerDevelopment
MCST tasks for Compiler Development

## Требования

- Собранный LLVM(написано на версии 24, если же вы используете 15ую, надо поменять некоторые '#include' в src/)
- CMake версии $>=$ 3.13.4, компилятор, поддерживающий C++17

## Сборка

```bash
export LLVM_DIR=<path-to-llvm-project>
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR .
cmake --build .
```

После успешного выполнения этого bash-скрипта, в корне репозитория должны появиться файлы `libRPOPass.so` и `libInstCountPass.so`

## Использование

Для начала, нам необходимо прогнать файл(здесь рассматриваются программы на языке C) через clang, чтобы транслировать код из изначального языка в LLVM IR

```
$LLVM_DIR/build/bin/clang -O0 -S -emit-llvm --target=x86_64-unknown-linux-gnu file.c -o file.ll
```

**Примечание: если вы такой же пользователь устройств на ARM, как и я, то вам следует прописать флаг --target и указать в нём архитектуру x86_64**

### Случай использования 1: RPOPass

```
$LLVM_DIR/build/bin/opt -load-pass-plugin ./libRPOPass.so -passes=rpo-pass -disable-output file.ll
```

### Случай использования 2: InstCountPass

```bash
$LLVM_DIR/build/bin/opt -load-pass-plugin ./libInstCountPass.so -passes=inst-count-pass -disable-output file.ll
```
