// https://stackoverflow.com/questions/27827923/c-object-pool-that-provides-items-as-smart-pointers-that-are-returned-to-pool !!!!

#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include <tuple>

template <class T>
class SharedPool
{
 private:

    struct External_Deleter
    {
    explicit External_Deleter(std::weak_ptr<SharedPool<T>* > pool)
        : pool_(pool) {}

    void operator()(T* ptr)
    {
      if (auto pool_ptr = pool_.lock())
      {
        try {
          (*pool_ptr.get())->add(std::unique_ptr<T>{ptr});
          return;
        } catch(...) {}
      }
      std::default_delete<T>{}(ptr);
    }
   private:
    std::weak_ptr<SharedPool<T>* > pool_;
  };

 public:

  using ptr_type = std::unique_ptr<T, External_Deleter >;

  SharedPool() : this_ptr_(new SharedPool<T>*(this)) {}
  virtual ~SharedPool(){}

  ptr_type get()
  {
      if( pool_.empty() )
      {
         pool_.push(std::make_unique<T>());
      }
      ptr_type tmp(pool_.top().release(),
                   External_Deleter{std::weak_ptr<SharedPool<T>*>{this_ptr_}});
      pool_.pop();
      return std::move(tmp);
  }

  bool empty() const
  {
    return pool_.empty();
  }

  size_t size() const
  {
    return pool_.size();
  }

 private:

  std::shared_ptr<SharedPool<T>* > this_ptr_;
  std::stack<std::unique_ptr<T> > pool_;

  void add(std::unique_ptr<T> t)
  {
      pool_.push(std::move(t));
  }

};

