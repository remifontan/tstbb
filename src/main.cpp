
#include <ranges>
#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>

#include <tbb/parallel_invoke.h>
#include <tbb/task_group.h>
#include <tbb/parallel_for.h>

int main()
{

    std::vector<int> tasks_progress;
    std::vector<std::unique_ptr<std::mutex>> tasks_mutex;

    tbb::task_group g;
    // for (size_t i = 0; i < 10; ++i)
    for (auto tid : std::ranges::views::iota(0, 10))
    {
        tasks_progress.push_back(0);
        tasks_mutex.push_back(std::make_unique<std::mutex>());

        g.run([=, &tasks_progress, &tasks_mutex]
              {
                  tbb::parallel_for(tbb::blocked_range<int>(0, 100),
                                    [&](tbb::blocked_range<int> r)
                                    {
                                        for (int i = r.begin(); i < r.end(); ++i)
                                        {
                                            tasks_mutex[tid]->lock();
                                            tasks_progress[tid] += 1;
                                            tasks_mutex[tid]->unlock();

                                            std::this_thread::sleep_for(std::chrono::milliseconds{1});
                                        }
                                    });

                  tasks_mutex[tid]->lock();
                  // -1: done
                  tasks_progress[tid] = -1;
                  tasks_mutex[tid]->unlock();
              });
    }

    // poll
    size_t num_task_dones = 0;
    while (num_task_dones < tasks_progress.size())
    {
        num_task_dones = 0;
        for (auto tid : std::ranges::views::iota(size_t(0), tasks_progress.size()))
        {
            tasks_mutex[tid]->lock();
            int progress = tasks_progress[tid];
            tasks_mutex[tid]->unlock();

            if (progress == -1)
            {
                ++num_task_dones;
                std::cout << "done ";
            }
            else
            {
                std::cout << std::setw(4) << progress << " ";
            }
        }
        std::cout << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }

    g.wait();
    return 0;
}