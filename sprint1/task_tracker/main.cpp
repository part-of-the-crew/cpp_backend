#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

// Перечислимый тип для статуса задачи
enum class TaskStatus {
    NEW,          // новая
    IN_PROGRESS,  // в разработке
    TESTING,      // на тестировании
    DONE          // завершена
};

// Объявляем тип-синоним для map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = map<TaskStatus, int>;

class TeamTasks {
public:
    // Получить статистику по статусам задач конкретного разработчика
    const TasksInfo& GetPersonTasksInfo(const string& person) const {
        return empl.find(person)->second;
    }

    // Добавить новую задачу (в статусе NEW) для конкретного разработчика
    void AddNewTask(const string& person) {
        empl[person][TaskStatus::NEW]++;
    }

    // Обновить статусы по данному количеству задач конкретного разработчика,
    // подробности см. ниже
    tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count) {
        TasksInfo changed;
        TasksInfo unchanged;
        auto it = empl.find (person);
        if (it == empl.end()) {
            return tuple(changed, unchanged);
        }
        int cur_count = 0;

        cur_count = it->second[TaskStatus::NEW] - task_count;
        if (cur_count >= 0) { //ещё осталось задач
            it->second[TaskStatus::NEW] -= task_count;
            //it->second[TaskStatus::IN_PROGRESS] += task_count;
            changed[TaskStatus::IN_PROGRESS] = task_count;
            for (auto const task: it->second)
            {
                if (task.first != TaskStatus::DONE)
                    unchanged[task.first] = task.second;
            }
            
            it->second[TaskStatus::IN_PROGRESS] += task_count;
            return tuple(changed, unchanged);
        }
        changed[TaskStatus::IN_PROGRESS] = it->second[TaskStatus::NEW];
        task_count = -cur_count;
        cur_count= it->second[TaskStatus::IN_PROGRESS] - task_count;

        it->second[TaskStatus::NEW] = 0;
        
    //
        //cout << it->second[TaskStatus::IN_PROGRESS] << task_count << endl;
        if (cur_count >= 0) { //part of news
            it->second[TaskStatus::IN_PROGRESS] -= task_count;
            unchanged[TaskStatus::IN_PROGRESS] = it->second[TaskStatus::IN_PROGRESS];
            it->second[TaskStatus::IN_PROGRESS] += changed[TaskStatus::IN_PROGRESS];
            changed[TaskStatus::TESTING] = task_count;

            it->second[TaskStatus::TESTING] += task_count;
            return tuple(changed, unchanged);
        }
       
        changed[TaskStatus::TESTING] = it->second[TaskStatus::IN_PROGRESS];
        task_count = -cur_count;
        cur_count= it->second[TaskStatus::TESTING] - task_count;
        it->second[TaskStatus::IN_PROGRESS] = 0;
        
        //
        
        if (cur_count >= 0) { //part of news
            it->second[TaskStatus::TESTING] -= task_count;
            unchanged[TaskStatus::TESTING] = it->second[TaskStatus::TESTING];
            it->second[TaskStatus::TESTING] += changed[TaskStatus::TESTING];
            changed[TaskStatus::DONE] = task_count;
            
            it->second[TaskStatus::DONE] += task_count;
            return tuple(changed, unchanged);
        }
        changed[TaskStatus::DONE] = it->second[TaskStatus::TESTING];
        it->second[TaskStatus::TESTING] = 0;
        
        it->second[TaskStatus::IN_PROGRESS] = changed[TaskStatus::IN_PROGRESS];
        it->second[TaskStatus::TESTING] = changed[TaskStatus::TESTING];
        it->second[TaskStatus::DONE] = changed[TaskStatus::DONE];
        return tuple(changed, unchanged);
    }
 private:
    map <string, map<TaskStatus, int>> empl;
};

// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь.
void PrintTasksInfo(TasksInfo tasks_info) {
    cout << tasks_info[TaskStatus::NEW] << " new tasks"s
         << ", "s << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress"s
         << ", "s << tasks_info[TaskStatus::TESTING] << " tasks are being tested"s
         << ", "s << tasks_info[TaskStatus::DONE] << " tasks are done"s << endl;
}

int main() {
    TeamTasks tasks;
    tasks.AddNewTask("Ilia");
    for (int i = 0; i < 3; ++i) {
        tasks.AddNewTask("Ivan");
    }
    cout << "Ilia's tasks: ";
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
    cout << "Ivan's tasks: ";
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));
  
    TasksInfo updated_tasks, untouched_tasks;
  
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan", 2);
    cout << "Updated Ivan's tasks: ";
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: ";
    PrintTasksInfo(untouched_tasks);
  
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan", 2);
    cout << "Updated Ivan's tasks: ";
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: ";
    PrintTasksInfo(untouched_tasks);
} 
