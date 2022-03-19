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
    long int nod (int x, int y) { return ( x ? nod(y % x, x) : y); }

    // кючи
    std::vector<String> _keys;
    //значения
    std::vector<Task> _values;

  public:
    // необходимый интервал выполнения заданий
    unsigned Interval = INT_MAX;

    // добавить задание в очередь
    void AddTask(String Key, bool (*taskPrt)(void), unsigned _interval){
      
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

      Interval = nod(min(Interval, _interval),max(Interval, _interval));
    }

    // выполнить задания ожидающие выполнения
    void Invoke(){
      for(unsigned i=0; i <_values.size(); i++){
        Task currentElement = _values[i];
        currentElement.lastInvoke += Interval;
        if(currentElement.lastInvoke >= currentElement.Interval){
          try{
            bool result = currentElement.TaskPrt();
            //если успешно, выставляем что выполнено
            if(result)
              currentElement.lastInvoke = 0;
          }
          catch(...) { }
        }
      }
    }
};

#endif