# Database

Консольное приложение на C++ для работы с базой SQLite. Оно позволяет создавать таблицу пользователей, добавлять, удалять и обновлять записи, искать пользователей, просматривать их списком и постранично, а также добавлять записи пакетно.

Зависимость SQLite устанавливается автоматически через vcpkg из манифеста `vcpkg.json`. Сборка управляется CMake Presets. Проект использует стандарт C++20; для MinGW проверена версия GCC 16.1.0.

## Работа с базой данных

`Database` реализует интерфейс `IDatabase`, поэтому клиентский код может зависеть от интерфейса, а новые реализации — наследоваться от него. Объект `Database` владеет соединением SQLite через `SQLiteConnection`; соединение автоматически закрывается при уничтожении владельца.

## Требования

- CMake 3.25 или новее.
- Git.
- Компилятор C++ с поддержкой C++20.
- vcpkg, установленный отдельно.

Перед сборкой задайте `VCPKG_ROOT` — путь к корню установленного vcpkg.

## Windows (MinGW)

Установите CMake и MinGW-w64, добавьте их в `PATH`, затем установите vcpkg, например в `C:\tools\vcpkg`. В PowerShell задайте переменную окружения:

```powershell
$env:VCPKG_ROOT = "C:\tools\vcpkg"
```

Из корня проекта выполните:

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug
```

Для Release используйте пресет `mingw-release` вместо `mingw-debug`.
Исполняемый файл: `out\build\mingw-debug\Database.exe`.

## Debian / Ubuntu (Linux x64)

Установите инструменты сборки и зависимости, необходимые vcpkg:

```bash
sudo apt update
sudo apt install build-essential cmake curl git zip unzip tar pkg-config
```

Установите vcpkg и задайте переменную окружения. Например:

```bash
git clone https://github.com/microsoft/vcpkg.git "$HOME/vcpkg"
"$HOME/vcpkg/bootstrap-vcpkg.sh"
export VCPKG_ROOT="$HOME/vcpkg"
```

Из корня проекта выполните:

```bash
cmake --preset linux-x64-debug
cmake --build --preset linux-x64-debug
```

Для Release используйте пресет `linux-x64-release` вместо `linux-x64-debug`.
Исполняемый файл: `out/build/linux-x64-debug/Database`.

В новых терминалах снова задайте `VCPKG_ROOT` или добавьте его в конфигурацию оболочки (например, `~/.bashrc`). Убедитесь, что установленный в системе компилятор поддерживает C++20.

## Запуск

При конфигурации CMake копирует `example.db` в каталог сборки. Запускайте приложение из этого каталога, чтобы оно работало с копией базы данных:

```bash
cd out/build/linux-x64-debug
./Database
```

В PowerShell:

```powershell
Set-Location .\out\build\mingw-debug
.\Database.exe
```

## Тесты

GoogleTest устанавливается через vcpkg. После конфигурации и сборки запустите тесты из корня проекта:

```powershell
ctest --test-dir out/build/mingw-debug --output-on-failure
```

Для Linux используйте каталог выбранного пресета, например:

```bash
ctest --test-dir out/build/linux-x64-debug --output-on-failure
```

Тесты работают с SQLite в памяти и не изменяют `example.db`.
