#include <condition_variable>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <string>

std::mutex mtx;

std::vector<std::vector<bool>> getGarden(const int &n, const int &m)
{
    std::vector<std::vector<bool>> vec = std::vector<std::vector<bool>>();
    for (int i = 0; i < n; i++)
    {
        auto newVec = std::vector<bool>();
        vec.push_back(newVec);
        for (int j = 0; j < m; j++)
            vec[i].push_back(false);
    }
    return vec;
}

std::vector<std::vector<bool>> getRandomPlan(const int &n, const int &m)
{
    std::srand(std::time(NULL));
    std::vector<std::vector<bool>> vec = std::vector<std::vector<bool>>();
    for (int i = 0; i < n; i++)
    {
        auto newVec = std::vector<bool>();
        vec.push_back(newVec);
        for (int j = 0; j < m; j++)
            vec[i].push_back(std::rand() % 2 == 0);
    }
    return vec;
}

void DoWork(const int xx, const int yy,
            const int stepXX, const int stepYY,
            std::vector<std::vector<bool>> &garden,
            const std::string &name,
            const std::vector<std::vector<bool>> &plan)
{
    int x = xx, y = yy, stepX = stepXX, stepY = stepYY;
    int amountOfSteps = 0;
    const int maxAmountOfSteps = garden.size() * garden[0].size();
    do
    {
        mtx.lock();
        if (!plan[x][y]){
            std::cout <<  name + " are on cell [" + std::to_string(y)
                      << "][" + std::to_string(x) + "]. It is not part of a plan.\n";
            mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else if (!garden[x][y]) {
            garden[x][y] = true;
            std::cout <<  name + " do something on cell [" + std::to_string(y)
                      << "][" + std::to_string(x) + "].\n";
            mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));

        }
        else {
            std::cout <<  name + " are on cell [" + std::to_string(y)
                      << "][" + std::to_string(x) + "]. It is already ready.\n";
            mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        x += stepX;
        y += stepY;
        amountOfSteps += 1;
        if (stepX != 0 && (x == -1  || x == garden.size())){
            x -= stepX;
            stepX *= -1;
            y += 1;
        }
        else if (stepY != 0 && (y == -1  || y == garden[0].size())){
            y -= stepY;
            stepY *= -1;
            x -= 1;
        }
    } while (amountOfSteps != maxAmountOfSteps);
}

int main() {
    int n, m;
    std::cout << "Input width of garden [1, 1000]: ";
    scanf("%d", &n);
    if (n < 1 || n > 1000)
    {
        std::cout << "Wrong width input.\n";
        exit(0);
    }
    std::cout << "Input height of garden [1, 1000]: ";
    scanf("%d", &m);
    if (m < 1 || m > 1000)
    {
        std::cout << "Wrong height input.\n";
        exit(0);
    }
    auto garden = getGarden(n, m);
    auto plan = getRandomPlan(n, m);

    std::thread gardener1(DoWork, 0, 0, 1, 0,
                          std::ref(garden),
                          "Frost", plan);
    std::thread gardener2(DoWork, n - 1, m - 1, 0, -1,
                          std::ref(garden),
                          "Susan", plan);
    gardener1.join();
    gardener2.join();
    return 0;
}
