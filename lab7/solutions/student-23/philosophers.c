#include <pthread.h>  // Подключаем библиотеку для работы с потоками
#include <stdio.h>    // Подключаем библиотеку для ввода/вывода
#include <unistd.h>   // Подключаем библиотеку для usleep() и других системных вызовов

#define PHILO_COUNT 5   // Определяем количество философов (и вилок) - 5
#define EAT_COUNT 3     // Определяем сколько раз каждый философ должен поесть - 3

// Перечисление возможных состояний философа
typedef enum e_state
{
    THINKING,   // Состояние: философ думает
    HUNGRY,     // Состояние: философ голоден и ждёт вилки
    EATING      // Состояние: философ ест
}   t_state;

// Перечисление флагов для отслеживания вилок
typedef enum e_philo_flags
{
    PHILO_NONE      = 0,      // Философ не держит ни одной вилки (флаги = 00)
    PHILO_HAS_LEFT  = 1 << 0, // Философ держит левую вилку (флаг = 01)
    PHILO_HAS_RIGHT = 1 << 1  // Философ держит правую вилку (флаг = 10)
}   t_philo_flags;

// Структура, описывающая одного философа
typedef struct s_philosopher
{
    int             id;          // Уникальный номер философа (1,2,3,4,5)
    t_state         state;       // Текущее состояние философа (думает/голоден/ест)
    t_philo_flags   flags;       // Какие вилки сейчас держит философ
    pthread_t       thread;      // Идентификатор потока философа
    pthread_mutex_t *left_fork;  // Указатель на мьютекс левой вилки
    pthread_mutex_t *right_fork; // Указатель на мьютекс правой вилки
    pthread_mutex_t *print_mutex; // Указатель на мьютекс для печати в консоль
}   t_philosopher;

// Функция безопасной печати сообщений из разных потоков
static void print_status(t_philosopher *philo, const char *message)
{
    pthread_mutex_lock(philo->print_mutex);        // Блокируем мьютекс печати
    printf("Philosopher %d %s\n", philo->id, message); // Печатаем сообщение
    pthread_mutex_unlock(philo->print_mutex);      // Разблокируем мьютекс печати
}

// Функция - философ думает
void    think(t_philosopher *philo)
{
    philo->state = THINKING;              // Устанавливаем состояние "думает"
    print_status(philo, "is THINKING");   // Сообщаем что философ думает
    usleep(200000);                       // Ждём 0.2 секунды (имитация мышления)
}

// Функция - философ берёт вилки
void    take_forks(t_philosopher *philo)
{
    philo->state = HUNGRY;               // Устанавливаем состояние "голоден"
    print_status(philo, "is HUNGRY");    // Сообщаем что философ голоден

    if (philo->id % 2 == 0)              // Если философ с чётным номером
    {
        pthread_mutex_lock(philo->left_fork);      // Берём левую вилку
        philo->flags |= PHILO_HAS_LEFT;           // Отмечаем что левая вилка есть
        print_status(philo, "took left fork");    // Сообщаем о взятии левой вилки

        pthread_mutex_lock(philo->right_fork);     // Берём правую вилку
        philo->flags |= PHILO_HAS_RIGHT;          // Отмечаем что правая вилка есть
        print_status(philo, "took right fork");   // Сообщаем о взятии правой вилки
    }
    else                                 // Если философ с нечётным номером
    {
        pthread_mutex_lock(philo->right_fork);     // Берём правую вилку
        philo->flags |= PHILO_HAS_RIGHT;          // Отмечаем что правая вилка есть
        print_status(philo, "took right fork");   // Сообщаем о взятии правой вилки

        pthread_mutex_lock(philo->left_fork);      // Берём левую вилку
        philo->flags |= PHILO_HAS_LEFT;           // Отмечаем что левая вилка есть
        print_status(philo, "took left fork");    // Сообщаем о взятии левой вилки
    }
}

// Функция - философ ест
void    eat(t_philosopher *philo)
{
    philo->state = EATING;               // Устанавливаем состояние "ест"
    print_status(philo, "is EATING");    // Сообщаем что философ ест
    usleep(200000);                      // Ждём 0.2 секунды (имитация еды)
}

// Функция - философ кладёт вилки
void    put_forks(t_philosopher *philo)
{
    if (philo->flags & PHILO_HAS_LEFT)   // Если есть левая вилка
    {
        pthread_mutex_unlock(philo->left_fork);   // Кладём левую вилку
        philo->flags &= ~PHILO_HAS_LEFT;         // Снимаем флаг левой вилки
    }

    if (philo->flags & PHILO_HAS_RIGHT)  // Если есть правая вилка
    {
        pthread_mutex_unlock(philo->right_fork);  // Кладём правую вилку
        philo->flags &= ~PHILO_HAS_RIGHT;        // Снимаем флаг правой вилки
    }

    print_status(philo, "put forks");     // Сообщаем что вилки положены
}

// Функция, которую выполняет каждый поток-философ
void    *philosopher_routine(void *arg)
{
    t_philosopher   *philo;  // Указатель на структуру философа
    int             i;       // Счётчик приёмов пищи

    philo = (t_philosopher *)arg;  // Преобразуем void* в указатель на философа
    i = 0;                         // Начинаем с нуля
    while (i < EAT_COUNT)          // Пока не поели нужное количество раз
    {
        think(philo);              // Философ думает
        take_forks(philo);         // Философ берёт вилки
        eat(philo);                // Философ ест
        put_forks(philo);          // Философ кладёт вилки
        i++;                       // Увеличиваем счётчик приёмов пищи
    }
    return (NULL);                 // Завершаем поток
}

// Главная функция - точка входа в программу
int main(void)
{
    pthread_mutex_t forks[PHILO_COUNT];     // Массив мьютексов для вилок
    pthread_mutex_t print_mutex;            // Мьютекс для синхронизации печати
    t_philosopher   philosophers[PHILO_COUNT]; // Массив структур философов
    int             i;                      // Счётчик для циклов

    pthread_mutex_init(&print_mutex, NULL); // Инициализируем мьютекс печати

    i = 0;                                   // Начинаем с нуля
    while (i < PHILO_COUNT)                  // Проходим по всем вилкам
    {
        pthread_mutex_init(&forks[i], NULL); // Инициализируем каждую вилку
        i++;                                 // Переходим к следующей вилке
    }

    i = 0;                                   // Начинаем с нуля
    while (i < PHILO_COUNT)                  // Проходим по всем философам
    {
        philosophers[i].id = i + 1;          // Присваиваем номер (1-5)
        philosophers[i].state = THINKING;    // Начинаем в состоянии "думает"
        philosophers[i].flags = PHILO_NONE;  // Изначально нет вилок
        philosophers[i].left_fork = &forks[i]; // Левая вилка - своя
        philosophers[i].right_fork = &forks[(i + 1) % PHILO_COUNT]; // Правая - следующая
        philosophers[i].print_mutex = &print_mutex; // Указываем мьютекс для печати

        // Создаём поток для философа
        if (pthread_create(&philosophers[i].thread,
                NULL,
                philosopher_routine,
                &philosophers[i]) != 0)     // Если ошибка создания потока
        {
            return (1);                      // Завершаем программу с ошибкой
        }
        i++;                                 // Переходим к следующему философу
    }

    i = 0;                                   // Начинаем с нуля
    while (i < PHILO_COUNT)                  // Проходим по всем философам
    {
        pthread_join(philosophers[i].thread, NULL); // Ждём завершения потока
        i++;                                 // Переходим к следующему
    }

    i = 0;                                   // Начинаем с нуля
    while (i < PHILO_COUNT)                  // Проходим по всем вилкам
    {
        pthread_mutex_destroy(&forks[i]);    // Уничтожаем мьютекс вилки
        i++;                                 // Переходим к следующей
    }

    pthread_mutex_destroy(&print_mutex);     // Уничтожаем мьютекс печати

    return (0);                              // Успешное завершение программы
}