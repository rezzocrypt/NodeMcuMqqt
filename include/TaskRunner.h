#ifndef TaskRunner_h
#define TaskRunner_h

#include <Arduino.h>
#include <vector>

// структура с параметрами от выполяющихся заданий
typedef struct {
  bool (*TaskPrt)(void);
  unsigned Interval;
  unsigned lastInvoke;
} Task;

class TaskRunner {
  private:

    //наибольший общий делитель через остаток от деления
    unsigned nod (unsigned a, unsigned b) {
      while (b != 0) {
        unsigned tmp = b;
        b = a % b;
        a = tmp;
      }
      return a;
     }

    // кючи
    std::vector<String> _keys;
    //значения
    std::vector<Task> _values;

  public:
    // необходимый интервал выполнения заданий
    unsigned Interval = 0;

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
        _values[i].lastInvoke += Interval;
        if(_values[i].lastInvoke >= _values[i].Interval){
          try{
            if(_values[i].TaskPrt())
              _values[i].lastInvoke -= _values[i].Interval;
          }
          catch(...) { }
        }
      }
    }
};

#endif
