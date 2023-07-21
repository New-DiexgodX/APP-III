#include <bits/stdc++.h>

#define MAX_SIZE 50

/* Funcion que genera un numero aleatorio, y que retonar dicho valor */
int numAleatorio(int, int, int);

/*  */
class Monitor
{
private:
    std::mutex mtx;
    std::vector<int> SSD_A;
    std::vector<int> SSD_B;
    std::condition_variable cvA, cvAA;
    std::condition_variable cvB, cvBB;

public:
    Monitor()
    {
        SSD_A.reserve(MAX_SIZE);
        SSD_B.reserve(MAX_SIZE);
    }

    void Produc_A();
    void Produc_B();
    int Consumidor_A();
    int Consumidor_B();
    int Consumidor_A_B();
};

std::random_device rd;
std::mt19937 generator(rd()); // Mersenne Twister

int numAleatorio(int a, int b, int c)
{
    std::uniform_int_distribution<int> distribution(a, b);
    int numero_aleatorio;
    if (c == 0)
        numero_aleatorio = distribution(generator);
    else if (c == 1) // Generar numero aleatorio impar
    {
        do
        {
            numero_aleatorio = distribution(generator);
        } while (numero_aleatorio % 2 != 0);
    }
    else if (c == 2) // Generar numero aleatorio par
    {
        do
        {
            numero_aleatorio = distribution(generator);
        } while (numero_aleatorio % 2 == 0);
    }

    return numero_aleatorio;
}

void Monitor::Produc_A()
{
    std::unique_lock<std::mutex> Lock(this->mtx);
    if (SSD_A.size() == MAX_SIZE)
        cvA.wait(Lock);
    int num;
    for (int i = 0; i < numAleatorio(1, 3, 0); i++)
    {
        num = numAleatorio(1, 100, 1);
        std::cout << "Productor de Clase A, produciendo . . ." << num << std::endl;
        this->SSD_A.push_back(num);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    cvAA.notify_one();
}

void Monitor::Produc_B()
{
    std::unique_lock<std::mutex> Lock(this->mtx);
    if (SSD_B.size() == MAX_SIZE)
        cvB.wait(Lock);
    int num;
    for (int i = 0; i < numAleatorio(1, 3, 0); i++)
    {
        num = numAleatorio(1, 100, 2);
        std::cout << "Productor de Clase B, produciendo . . ." << num << std::endl;
        this->SSD_B.push_back(num);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    cvBB.notify_one();
}

int Monitor::Consumidor_A()
{
    std::unique_lock<std::mutex> Lock(this->mtx);
    if (SSD_A.size() == 0)
        cvAA.wait(Lock);
    int Elemento;
    Elemento = this->SSD_A.front();
    this->SSD_A.erase(this->SSD_A.begin());
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    cvA.notify_one();
    return Elemento;
}

int Monitor::Consumidor_B()
{
    std::unique_lock<std::mutex> Lock(this->mtx);
    if (SSD_B.size() == 0)
        cvBB.wait(Lock);
    int Elemento = this->SSD_B.front();
    this->SSD_B.erase(this->SSD_B.begin());
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    cvB.notify_one();
    return Elemento;
}

int Monitor::Consumidor_A_B()
{
    std::unique_lock<std::mutex> Lock(this->mtx);
    int Elemento;
    if (numAleatorio(1, 2, 0) == 1) // Consume Dato A
    {
        if (SSD_A.size() == 0)
            cvAA.wait(Lock);
        Elemento = this->SSD_A.front();
        this->SSD_A.erase(this->SSD_A.begin());
        cvA.notify_one();
    }
    else
    {
        if (SSD_B.size() == 0)
            cvBB.wait(Lock);
        Elemento = this->SSD_B.front();
        this->SSD_B.erase(this->SSD_B.begin());
        cvB.notify_one();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    return Elemento;
}

int main()
{
    Monitor Procesos_Consumidores;

    int random;

    random = numAleatorio(1, 10, 0);
    std::vector<std::thread> HilosP(random);

    random = numAleatorio(1, 10, 0);
    std::vector<std::thread> HilosC(random);

    auto P = [&Procesos_Consumidores](int a) {
        if (a == 1) // Productor A
        {
            while(true)
            {
            	Procesos_Consumidores.Produc_A();
			}
        }
        else if (a == 2)
        {
            while(true)
            {
               Procesos_Consumidores.Produc_B();
			}
            Procesos_Consumidores.Produc_B();
        }
        else if (a == 3)
        {
            while(true)
            {
           	    std::cout << "Consumidor A, consumiendo <" << Procesos_Consumidores.Consumidor_A() << ">" << std::endl;
			}
        }
        else if (a == 4)
        {
            while(true)
            {
            std::cout << "Consumidor B, consumiendo <" << Procesos_Consumidores.Consumidor_B() << ">" << std::endl;            	
			}
        }
        else
        {
            while(true)
            {
            	std::cout << "Consumidor A & B, consumiendo <" << Procesos_Consumidores.Consumidor_A_B() << ">" << std::endl;

			}
        }
    };

    for (auto &H : HilosP)
    {
        random = numAleatorio(1, 2, 0);
        if (random == 1)
        {
            H = std::thread(P, 1);
        }
        else
        {
            H = std::thread(P, 2);
        }
    }

    for (auto &H : HilosC)
    {
        random = numAleatorio(1, 3, 0);
        if (random == 1)
        {
            H = std::thread(P, 3);
        }
        else if (random == 2)
        {
            H = std::thread(P, 4);
        }
        else
        {
            H = std::thread(P, 5);
        }
    }

    for (auto &H : HilosP)
        H.join();

    for (auto &H : HilosC)
        H.join();

    return 0;
}
