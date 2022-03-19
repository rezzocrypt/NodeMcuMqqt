#ifndef TaskRunner_h
#define TaskRunner_h

#include <vector> 

// структура с параметрами от выполяющихся заданий
typedef struct {
  bool (*TaskPrt)(void);
  unsigned Interval;
  unsigned lastInvoke;
} Task;

class TaskRunner {
  private:

    //наибольший общий делитель
    long int nod (int a, int b) { 
      while (a != b) {
        if (a > b) {
          a = a - b;
        } else {
          b = b - a;
        }
      }
      return a;
     }

    // кючи
    std::vector<String> _keys;
    //значения
    std::vector<Task> _values;

  public:
    // необходимый интервал выполнения заданий
    unsigned Interval;

    // добавить задание в очередь
    void AddTask(String Key, bool (*taskPrt)(void), unsigned _interval){

      if(_keys.size() == 0)
        Interval = _interval;

      for(unsigned i=0; i < _keys.size(); i++){
        // элемент уже в массиве, пропускаем добавление
        if(_keys[i] == Key)
          return;
      }
      Task t;
      t.TaskPrt = taskPrt;
      t.Interval = _interval;
      t.lastInvoke = 0;

      _keys.push_back(Key);
      _values.push_back(t);
      Interval = nod(Interval, _interval);
    }

    // выполнить задания ожидающие выполнения
    void Invoke(){
      for(unsigned i=0; i <_values.size(); i++){
        Task currentElement = _values[i];
        currentElement.lastInvoke += Interval;
        if(currentElement.lastInvoke >= currentElement.Interval){
          try{
            if(currentElement.TaskPrt())
              currentElement.lastInvoke = 0;
          }
          catch(...) { }
        }
      }
    }
};

#endif