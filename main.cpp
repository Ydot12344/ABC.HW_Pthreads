#include <iostream>
#include <pthread.h>
#include <ctime>


int N = -1;//Размер массива A
int *A;//Динамический массив
int max_sum;//Максимальная сумма элементов знакочередующегося ряда
int l, r;//Левая и правая границы максимальной суммы
pthread_barrier_t barr;//Барьер для синхронизации потоков
pthread_rwlock_t lock;//Блокировка для записи общих данных потоками


void *func(void *args) {
    int i = *((int *) args);//номер потока

    //первый блок потока
    for(int t = (i * N)/16; t <= ((i+1)*N)/16; t++) {
        int cur_sum = A[t];//текущая сумма элементов
        int sign = -1;//знак следующего элемента
        for (int j = t + 1; j < N; j++) {
            cur_sum += sign * A[j];//добавление элемента к текущей сумме
            sign *= -1;//смена знака

            pthread_rwlock_wrlock(&lock);//блокировка чтения записи
            if (cur_sum > max_sum) {
                //изменение общих показателей
                max_sum = cur_sum;
                l = t;
                r = j;
            }
            pthread_rwlock_unlock(&lock);
        }
    }

    //второй блок потока
    for(int t = ((15 - i)*N)/16 ; t <= ((16 - i)*N)/16; t++) {
        int cur_sum = A[t];
        int sign = -1;
        for (int j = t + 1; j < N; j++) {
            cur_sum += sign * A[j];
            sign *= -1;

            pthread_rwlock_wrlock(&lock);
            if (cur_sum > max_sum) {
                max_sum = cur_sum;
                l = t;
                r = j;
            }
            pthread_rwlock_unlock(&lock);
        }
    }
    //синхронизация потоков
    pthread_barrier_wait(&barr);
}


int main() {

    //инициализация барьера
    pthread_barrier_init(&barr, NULL,9);
    //инициализация блокиратора ввода вывода
    pthread_rwlock_init(&lock, NULL);

    //ввод N
    while (N <= 10) {
        std::cout << "Entre length of array A, > 10\n";
        std::cin >> N;
    }

    //инициализация A
    A = new int[N];

    //ввод элементов A
    char c = '1';
    while(c != 'y' && c != 'n'){
        std::cout << "Do you want to enter A random it?\nEnter 'y' if you want to enter it, else enter 'n'.\n";
        std::cin >> c;
    }

    if(c == 'y')
        for (int i = 0; i < N; i++) {
        std::cin >> A[i];
        }
    else{
        srand(std::time(NULL));
        for(int i = 0 ; i < N; i++){
            A[i] = rand();
            std::cout << A[i] << " ";
        }
        std::cout<<'\n';

    }

    //начальное значение максимальной суммы
    max_sum = A[0] - A[1];


    pthread_t threads[8];//массив потоков
    int thread_args[8];//массив аргументов

    //запуск потоков
    for(int i = 0; i < 8; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, func, (void *) (&thread_args[i]));
    }

    //синхронизация потоков
    pthread_barrier_wait(&barr);

    //вывод результата
    std::cout << "Sum: " << max_sum << '\n';
    std::cout << "i: " << l << "; j: " << r << '\n';
}
