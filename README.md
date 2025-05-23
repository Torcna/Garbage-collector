
# Аллокатор памяти и сборщик мусора

Этот документ описывает основные компоненты нашей системы управления памятью, включая архитектуру аллокатора, алгоритм выделения памяти, механизм пометки живых объектов, а также основные структуры данных.
 ## Общий обзор
Наша система использует собственный аллокатор, который работает с фиксированными **чанками** памяти (обычно 4096 байт). Внутри каждого чанка память разбивается на блоки для маленьких объектов. Для эффективного управления свободными блоками используется механизм **free lists** или внешнее хранилище для очень маленьких блоков (например, 4 байта). Центральным элементом является **Memory Manager (memManager)**, который отвечает за:
1. Запрос новых чанков у операционной системы.
2. Разбиение чанков на блоки фиксированного размера.
3. Управление свободными блоками посредством free lists.

Во время сборки мусора используется алгоритм mark-sweep, который помечает живые объекты, сканируя корневые указатели (со стека, регистров и статических данных), а затем проходит по куче, используя наши метаданные для определения границ объектов.

---

## Основные структуры данных

### Чанк (Chunk)

**Описание:**  
Чанк – это большой непрерывный блок памяти (обычно 4096 байт), выделяемый у операционной системы.

**Назначение:**  
Чанки служат основным источником памяти для небольших объектов. Каждый чанк разбивается на блоки фиксированного размера.

**Ключевые функции:**  
- **Выделение чанка:** При исчерпании свободных блоков определённого размера выделяется новый чанк и разбивается на блоки.
- - **Доступ к объектам:** Каждый блок в чанке может быть получен по индексу (например, через функцию `chunk.getObject(i)`).

### Free lists (Списки свободных блоков)

**Описание:**  
Free lists управляют доступными (свободными) блоками для маленьких объектов. При выделении нового чанка свободные блоки добавляются в free lists.

**Назначение:**  
Они позволяют быстро выделять память, отслеживая свободные блоки фиксированного размера.

**Реализация:**  
- **Встроенный free list:** Если блок достаточно велик для хранения указателя (например, ≥ `sizeof(void*)`), он может содержать поле `next` для формирования связанного списка.
- **Внешний free list:** Для очень маленьких блоков (например, 4 байта) можно использовать внешнюю структуру (например, std::set), чтобы хранить адреса свободных блоков.

**Поведение:**  
Free list обеспечивает выдачу блоков в определённом порядке – например, при выделении возвращается блок с наименьшим адресом (самый левый блок).

### Memory Manager (memManager)

**Описание:**  
Memory Manager – центральный компонент, координирующий выделение памяти, создание чанков и управление free lists.

**Обязанности:**
- **Управление чанками:** Выделяет новые чанки, когда свободные блоки определённого размера исчерпаны.
- **Регистрация free lists:** Поддерживает отображение (например, `std::unordered_map<size_t, FreeList>`) между размерами объектов и соответствующими free lists.
- **Интерфейс аллокации/освобождения:** Предоставляет низкоуровневые функции для выделения памяти, которые используются выше по уровню API.

---

## Пометка живых объектов (Marking)

Процесс сборки мусора реализован по схеме mark-sweep:
1. **Сканирование корней:**
     Сборщик сканирует регистры, стек и статические данные для поиска потенциальных указателей. Каждый найденный кандидат проверяется с помощью метаданных (например, через malloc‑tables и jump‑tables), чтобы определить, принадлежит ли он выделенной области памяти.

2. **Фаза пометки (Mark):**  
   Для каждого найденного указателя сборщик определяет соответствующий объект (используя его начальный адрес и размер) и помечает его как живой. Если объект находится в чанке, информация о нем извлекается через структуры Memory Manager.
  
3. **Фаза очистки (Sweep):**  
   После фазы пометки сборщик проходит по куче (или free lists) и освобождает память от объектов, которые не были помечены как живые.

---
## API для пользователя

Наша система предоставляет минимальный API для выделения памяти:

 - **`gc_new<T>(Args&&... args)`**  
  Выделяет и конструирует один объект типа `T` с помощью placement new. Эта функция использует memManager для получения свободного блока размером `sizeof(T)`.

- **`gc_new_array<T>(std::size_t count, Args&&... args)`**
 Выделяет массив из `count` объектов типа `T`. Функция пытается зарезервировать непрерывную последовательность блоков из free lists. Если такая последовательность найдена, выполняется placement new для каждого элемента, а информация (начальный адрес, количество элементов, размер элемента) сохраняется в таблице массивов для последующей обработки сборщиком мусора. 
  
  ---

## TODO

- **Оптимизация выделения непрерывной памяти:**  
Исследовать методы резервирования непрерывных блоков при выделении массива, если блоки выделяются по отдельности.
- **Реализовать merge для chuncks одного типа**  
Для того чтобы гарантировать непрерывность хранения массивов в памяти 

- **Улучшение управления free lists:**  
  Оценить производительность различных реализаций free lists (встроенной vs. внешней) для очень маленьких блоков.

- **Тестирование и бенчмаркинг:**  
  Разработать набор тестов и бенчмарков для валидации и сравнения производительности выделения памяти и сборки мусора.
