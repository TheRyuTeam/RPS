Сетевая игра камень ножницы бумага.

Требования:
- Cmake 3.16...3.21
- C++17
- Qt6.5
    - Core
    - Quick
    - Network

Опции cmake:
- RPS_BUILD_CLIENT: Собрать клиентское приложение (ON | OFF). Дефолтно ON
- RPS_BUILD_SERVER: Собрать серверное приложение (ON | OFF). Дефолтно ON

Как скачать:
1. git clone https://github.com/TheRyuTeam/RPS

Процесс сборки:
- QtCreator:
    1. Открыть QtCreator
    2. Открыть меню Файл
    3. Открыть Файл или Проект
    4. Выбрать путь к проекту
    5. Сконфигурировать проект
- qt-cmake:
    1. Создать папку для сборки
    2. Зайти в нее
    3. Ввести команду <Путь к qt-cmake> <Путь к проекту>
    4. Ввести команду cmake --build .
- cmake:
    1. Создать папку для сборки
    2. Зайти в нее
    3. Ввести команду cmake -DCMAKE_PREFIX_PATH=<Путь к компилятору Qt> <Путь к проекту>
    4. Ввести команду cmake --build .

Установка:
- qt-cmake:
    1. Повторить 1-3 шаги сборки через qt-cmake
    2. Ввести cmake --build . --target Release
    3. Ввести cmake --install . --prefix <Куда установить>
- cmake:
    1. Повторить 1-3 шаги сборки через cmake
    2. Ввести cmake --build . --target Release
    3. Ввести cmake --install . --prefix <Куда установить>

Запуск:
1. Установить проект
2. Зайти в папку bin
3. Запустить RPSServer/RPSGame
4. Наслаждаться игрой

Проект состоит из 3 подпроектов:
- Клиентское приложение
- Серверное приложение
- Общая для клиетского и серверного приложений библиотека 

Клиентское приложение - оконное Qml приложение, взаимодействующее с сервером.
Серверное приложение - консольное Qt приложение.
Общая библиотека - набор enum-ов.


Как запустить сервер:
1. Запустить сервер
2. Если сервер не запустился с сообщением "The RPSServer is already running" - сервер уже запущен
3. Сервер запущен


Как в клиентском приложении подключиться к серверу:
1. Запускается клиентское приложение
2. В меню настроек вести адрес удаленого сервера
3. Нажать кнопку сохранить
4. Дождаться подключения (Сверху справа адрес сервера станет белым вместо серого)
5. Если подключение не произошло - проверить введеный адрес, повторить с пункта 2

Как в клиентском приложении создать комнату:
1. Подключиться к серверу
2. Зайти в меню создания комнаты
3. Ввести имя комнаты, по желанию изменить максимальное количество игроков (от 2 до 16)
4. Нажать кнопку создать
5. Вы в комнате

Как в клиентском приложении подключиться к существующей комнате:
1. Подключиться к серверу
2. Зайти в меню комнат
3. Обновить список комнат кнопкой справа
3. Выбрать понравившуюся комнату
4. Даблкликом попробовать в нее войти
5. Если войти не вышло - возможно, комната полная, в стадии игры, или уже не существует, повторить с пункта 3
6. Вы в комнате

Как в клиентском приложении выйти из комнаты:
1. Свеху справа написано имя комнаты, рядом с ним кнопка выхода из комнаты. Нажать на нее
2. Вы вышли из комнаты

Как играть:
1. Вы в комнате
2. Снизу справа есть кнопка Ready - нажмите ее
3. Когда все игроки готовы, начнется стади подтверждения готовности
4. Когда готовность всех игроков подтверждена, начнется стадия выбора
5. Перед Вами иконки камня, ножниц и бумаги, выбирайте что Вам по душе
6. Когда истекло время выбора или все игроки выбрали знак, начинается стадия показа результата
7. Перед Вами список игроков, выбранные ими знаки. Если имя игрока и его знак зеленые - он выиграл больше всего игроков,
    елси красные - он выиграл меньше всего игроков, т.е проиграл
8. Когда стадия показа результата закончена, идем к пункту 2