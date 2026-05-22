#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define PHILO_COUNT 5
#define EAT_COUNT 3

typedef enum e_state
{
    THINKING,
    HUNGRY,
    EATING
}   t_state;

typedef enum e_philo_flags
{
    PHILO_NONE      = 0,
    PHILO_HAS_LEFT  = 1 << 0,
    PHILO_HAS_RIGHT = 1 << 1
}   t_philo_flags;

typedef struct s_philosopher
{
    int             id;
    t_state         state;
    t_philo_flags   flags;
    pthread_t       thread;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    pthread_mutex_t *print_mutex;
}   t_philosopher;

static void print_status(t_philosopher *philo, const char *message)
{
    pthread_mutex_lock(philo->print_mutex);
    printf("Philosopher %d %s\n", philo->id, message);
    pthread_mutex_unlock(philo->print_mutex);
}

void    think(t_philosopher *philo)
{
    philo->state = THINKING;
    print_status(philo, "is THINKING");
    usleep(200000);
}

void    take_forks(t_philosopher *philo)
{
    philo->state = HUNGRY;
    print_status(philo, "is HUNGRY");

    if (philo->id % 2 == 0)
    {
        pthread_mutex_lock(philo->left_fork);
        philo->flags |= PHILO_HAS_LEFT;
        print_status(philo, "took left fork");

        pthread_mutex_lock(philo->right_fork);
        philo->flags |= PHILO_HAS_RIGHT;
        print_status(philo, "took right fork");
    }
    else
    {
        pthread_mutex_lock(philo->right_fork);
        philo->flags |= PHILO_HAS_RIGHT;
        print_status(philo, "took right fork");

        pthread_mutex_lock(philo->left_fork);
        philo->flags |= PHILO_HAS_LEFT;
        print_status(philo, "took left fork");
    }
}

void    eat(t_philosopher *philo)
{
    philo->state = EATING;
    print_status(philo, "is EATING");
    usleep(200000);
}

void    put_forks(t_philosopher *philo)
{
    if (philo->flags & PHILO_HAS_LEFT)
    {
        pthread_mutex_unlock(philo->left_fork);
        philo->flags &= ~PHILO_HAS_LEFT;
    }

    if (philo->flags & PHILO_HAS_RIGHT)
    {
        pthread_mutex_unlock(philo->right_fork);
        philo->flags &= ~PHILO_HAS_RIGHT;
    }

    print_status(philo, "put forks");
}

void    *philosopher_routine(void *arg)
{
    t_philosopher   *philo;
    int             i;

    philo = (t_philosopher *)arg;
    i = 0;
    while (i < EAT_COUNT)
    {
        think(philo);
        take_forks(philo);
        eat(philo);
        put_forks(philo);
        i++;
    }
    return (NULL);
}

int main(void)
{
    pthread_mutex_t forks[PHILO_COUNT];
    pthread_mutex_t print_mutex;
    t_philosopher   philosophers[PHILO_COUNT];
    int             i;

    pthread_mutex_init(&print_mutex, NULL);

    i = 0;
    while (i < PHILO_COUNT)
    {
        pthread_mutex_init(&forks[i], NULL);
        i++;
    }

    i = 0;
    while (i < PHILO_COUNT)
    {
        philosophers[i].id = i + 1;
        philosophers[i].state = THINKING;
        philosophers[i].flags = PHILO_NONE;
        philosophers[i].left_fork = &forks[i];
        philosophers[i].right_fork = &forks[(i + 1) % PHILO_COUNT];
        philosophers[i].print_mutex = &print_mutex;

        if (pthread_create(&philosophers[i].thread,
                NULL,
                philosopher_routine,
                &philosophers[i]) != 0)
        {
            return (1);
        }
        i++;
    }

    i = 0;
    while (i < PHILO_COUNT)
    {
        pthread_join(philosophers[i].thread, NULL);
        i++;
    }

    i = 0;
    while (i < PHILO_COUNT)
    {
        pthread_mutex_destroy(&forks[i]);
        i++;
    }

    pthread_mutex_destroy(&print_mutex);

    return (0);
}