# Руководство для разработчика

## Общая структура проекта

`quantlib` использует CMake в качестве основной системы сборки.
Все основные правила определены в модуле [`cmake/QlHelpers.cmake`](../cmake/QlHelpers.cmake).

Он предоставляет унифицированный интерфейс для добавления библиотек, тестов и бенчмарков:
- `ql_cpp_library()` — добавить библиотеку (в том числе header-only)
- `ql_cpp_test()` — добавить тест
- `ql_cpp_executable()` — добавить исполняемый файл или бенчмарк

Эти функции по смыслу аналогичны Bazel-правилам `cc_library`, `cc_test` и `cc_binary` и реализуют те же принципы: минимальные декларации, понятные зависимости, единый стиль именования.


## Префиксы и соглашения

- Все цели внутри проекта имеют префикс `ql_` (например, `ql_math`, `ql_core`).
- Для каждой цели автоматически создаётся алиас `ql::<имя>` для использования в `target_link_libraries`.
- Исходники библиотеки находятся в `ql/<имя_модуля>/`.
- В корне модуля обычно создаётся `CMakeLists.txt`, в котором объявляется соответствующая цель через одну из функций ниже.

---

## Добавление новой библиотеки

### 1. Обычная (компилируемая) библиотека

```cmake
ql_cpp_library(
  NAME
    math
  HDRS
    "math_utils.h"
  SRCS
    "math_utils.cc"
  DEPS
    ql::core
  COPTS
    "-Wall" "-Wextra"
  PUBLIC
)
```

**Что делает:**
- создаёт цель `ql_math`
- добавляет алиас `ql::math`
- подключает include-директории проекта
- линкует с `ql::core`
- выставляет компиляционные опции
- экспортирует интерфейсы для установки, если включён `QUANTLIB_ENABLE_INSTALL`

**Важно:**
Файлы, оканчивающиеся на `.h`, `.hpp`, `.inc` и т. д., автоматически исключаются из списка `SRCS`.

---

### 2. Header-only библиотека

Если библиотека не содержит `.cc` или `.cpp` файлов, то `ql_cpp_library()` создаёт **INTERFACE**-библиотеку:

```cmake
ql_cpp_library(
  NAME
    traits
  HDRS
    "traits.h"
  DEPS
    ql::core
)
```

Такой таргет не компилируется, но может быть подключён через `target_link_libraries()` другими библиотеками и исполняемыми файлами.

---

### 3. Только для тестов или бенчмарков

Иногда полезно создавать библиотеки, которые нужны только в тестах или бенчмарках.
Для этого используются флаги `TESTONLY` и `BENCHONLY`.

```cmake
ql_cpp_library(
  NAME
    math_test_utils
  HDRS
    "math_test_utils.h"
  SRCS
    "math_test_utils.cc"
  TESTONLY
)
```

Эта библиотека соберётся **только**, если включён `QL_BUILD_TESTS=ON`.

---

## Добавление тестов

Тесты создаются с помощью функции `ql_cpp_test()`:

```cmake
ql_cpp_test(
  NAME
    math_test
  SRCS
    "math_test.cc"
  DEPS
    ql::math
    GTest::gmock
    GTest::gtest_main
  COPTS
    "-O0" "-g"
)
```

**Особенности:**
- создаёт исполняемый файл `math_test`
- добавляет его в список CTest под тем же именем
- не создаётся, если `QL_BUILD_TESTS=OFF`

---

## Добавление бенчмарков

Бенчмарки оформляются как исполняемые файлы, но с опцией `BENCH`:

```cmake
ql_cpp_executable(
  NAME
    math_bench
  SRCS
    "math_bench.cc"
  DEPS
    ql::math
    benchmark::benchmark
  BENCH
)
```

**Особенности:**
- создаёт исполняемый файл `math_bench`
- компилируется только если `QL_BUILD_BENCH=ON`

---

## Сборка и запуск

### 1. Конфигурация проекта

```bash
cmake -B build -DQL_BUILD_TESTS=ON -DQL_BUILD_BENCH=ON
```

### 2. Сборка

```bash
cmake --build build -j
```

Все исполняемые файлы (тесты, бенчмарки, утилиты) будут собраны в:
```
build/bin/
```

### 3. Запуск тестов

```bash
cd build
ctest --output-on-failure
```

CTests автоматически регистрируются через `add_test()` при вызове `ql_cpp_test()`.

---

## Пример структуры модуля

```
ql/
 └── math/
     ├── CMakeLists.txt
     ├── math_utils.h
     ├── math_utils.cc
     ├── math_test.cc
     └── math_bench.cc
```

### Пример `CMakeLists.txt` для такого модуля:

```cmake
ql_cpp_library(
  NAME
    math
  HDRS
    "math_utils.h"
  SRCS
    "math_utils.cc"
  DEPS
    ql::core
)

ql_cpp_test(
  NAME
    math_test
  SRCS
    "math_test.cc"
  DEPS
    ql::math
    GTest::gtest_main
)

ql_cpp_executable(
  NAME
    math_bench
  SRCS
    "math_bench.cc"
  DEPS
    ql::math
    benchmark::benchmark
  BENCH
)
```

---

## Советы и best practices

- Все новые библиотеки следует оформлять через `ql_cpp_library()` — даже header-only.
- Не добавляйте `.h/.hpp` в `SRCS`. Это делает функция автоматически.
- Используйте алиасы (`ql::name`) вместо прямых имён целей.
- Для тестов и бенчмарков **всегда** используйте соответствующие опции `TESTONLY` и `BENCH` при необходимости.
- Собирайте проект с `-DQL_BUILD_TESTS=ON` только локально или в CI, чтобы не тащить тесты в релизные сборки.
- Для всех библиотек рекомендуется указывать `PUBLIC`, если они предназначены для дальнейшей установки.

---

## Быстрая шпаргалка

| Тип цели | Функция | Флаг | Сборка при | Пример |
|-----------|----------|------|-------------|---------|
| Обычная библиотека | `ql_cpp_library` | — | Всегда | `ql_math` |
| Header-only | `ql_cpp_library` | — | Всегда | `ql_traits` |
| Тестовая библиотека | `ql_cpp_library` | `TESTONLY` | `QL_BUILD_TESTS=ON` | `ql_math_test_utils` |
| Тест | `ql_cpp_test` | — | `QL_BUILD_TESTS=ON` | `math_test` |
| Бенчмарк | `ql_cpp_executable` | `BENCH` | `QL_BUILD_BENCH=ON` | `math_bench` |

---
