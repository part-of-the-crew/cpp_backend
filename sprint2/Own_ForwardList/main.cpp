#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iostream>
#include <iterator>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value{};
        Node* next_node = nullptr;
        /*
        Node& operator=(const Node& rhs){
            value = rhs.value;
            next_node = rhs.next_node;
            return *this;
        }
        */
        /*
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
        Node(Type&& value, Node* next = nullptr) noexcept
            : value(std::move(value)), next_node(next) {}
        Node(Node&& node) noexcept
            : value(std::move(node.value))
            , next_node(node.next) {
            node.next_node = nullptr;
        }
        */
    };

public:
    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> values) {
        for (auto it = values.end(); it != values.begin(); ) {
            --it; // Move backward
            PushFront(*it);
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        if (this != &other) {
            //auto other_copy(other);
            SingleLinkedList<Type> other_copy;
            for (const auto& value : other) {
                other_copy.PushFront(value);
            }
            for (const auto& value : other_copy) {
                PushFront(value);
            }
        }
    }
    /*
    SingleLinkedList(SingleLinkedList&& other)
        : head_(std::move(other.head_))
        , size_(std::move(other.size_))
    {}
    */
    SingleLinkedList& operator=(SingleLinkedList rhs) noexcept {
        this->swap(rhs);     
        return *this;
    }

   
    void swap(SingleLinkedList& other) noexcept {
        if (this != &other) {
            //auto other_copy(other);
            std::swap(head_.next_node, other.head_.next_node);
            std::swap(size_, other.size_);
            /*
            auto other_copy_size = other.size_;
            auto other_copy_head = other.head_.next_node;
            other.size_ = this->size_;
            other.head_.next_node = this->head_.next_node;
            this->size_ = other_copy_size;
            this->head_.next_node = other_copy_head;
            */
        }
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }
    
    size_t GetSize() const noexcept {
        return size_;
    }
    void Clear() noexcept {
        while (head_.next_node){
            auto e = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete e;
            size_--;
        }
    }
    ~SingleLinkedList() {
        Clear();
        head_.next_node = nullptr;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }
    size_t GetSize() noexcept {
        return size_;
    }

    bool IsEmpty() noexcept {
        return size_ == 0;
    }


    
    template <typename ValueType>
    class BasicIterator {

    private:
        // Разрешаем SingleLinkedList обращаться к приватной области
        friend class SingleLinkedList;
        //explicit BasicIterator(Node* node): node_(node) {};
        explicit BasicIterator(const Node* node): node_(const_cast<Node*>(node)) {}
    public:
        using value_type = ValueType;
        using difference_type = std::ptrdiff_t;
        using pointer = const ValueType*;
        using reference = const ValueType&;
        using iterator_category = std::forward_iterator_tag;

        Node* node_ = nullptr;
        /*
        BasicIterator& operator= (const BasicIterator<const Type>& rhs){
            if (this != &rhs) {
                auto rhs_copy(rhs);
                node_ = rhs_copy.node_;
                rhs_copy.node_ = nullptr;
            }
            return *this;
        };
        */
        BasicIterator& operator= (BasicIterator<Type>& rhs){
            if (this != &rhs) {
                auto rhs_copy(rhs);
                node_ = rhs_copy.node_;
                rhs_copy.node_ = nullptr;
            }
            return *this;
        };
        
        template <typename OtherValueType>
        BasicIterator& operator=(const BasicIterator<OtherValueType>& rhs) {
            if (this != reinterpret_cast<const BasicIterator*>(&rhs)) {
                node_ = rhs.node_;
            }
            return *this;
        }
        BasicIterator( const BasicIterator<Type>& rhs ): node_(rhs.node_){};
        BasicIterator( const BasicIterator<const Type>& rhs ): node_(rhs.node_){};

        explicit BasicIterator() = default;
        using Iterator = BasicIterator<Type>;
        using ConstIterator = BasicIterator<const Type>;
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            //return node_->next_node == rhs.node_->next_node && node_->value == rhs.node_->value;
            return node_ == rhs.node_;
        }
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return node_!= rhs.node_;
        }
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return node_!= rhs.node_;
        }

        [[nodiscard]] Type& operator*() noexcept {
            return node_->value;
        }
        [[nodiscard]] Type* operator->() noexcept {
            return &node_->value;
        }
        [[nodiscard]] Type& operator*() const noexcept {
            return node_->value;
        }
        [[nodiscard]] Type* operator->() const noexcept {
            return &node_->value;
        }       
        /*
        [[nodiscard]] Iterator begin() noexcept {
            // Благодаря дружбе SingleLinkedList имеет доступ к приватному конструктору своего итератора
            return Iterator{head_.next_node};
        }
        */
        BasicIterator& operator++() noexcept {
            /* изменяем текущий объект */
            node_ = node_->next_node;
            return *this;
        }
        // Перегрузка постфиксной формы
        BasicIterator operator++(int) noexcept {
            auto old_value(*this); // Сохраняем прежнее значение объекта для последующего возврата
            //++(*this); // используем логику префиксной формы инкремента
            node_ = node_->next_node;
            return old_value;
        }
    };

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }
    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }
    // Константные версии begin/end для обхода списка без возможности модификации его элементов
    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{head_.next_node};
    }
    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator{nullptr};
    }
    // Методы для удобного получения константных итераторов у неконстантного контейнера
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }
    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }
    
    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{&head_};
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator{&head_};
    }

    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        auto new_node{new Node(value, pos.node_->next_node) };
        //new_node->value{value};
        //new_node->next_node = pos.node_->next_node;
        pos.node_->next_node = new_node;
        size_++;
        return Iterator{new_node};
    }

    void PopFront() noexcept {
        if (head_.next_node) {
            auto next_node = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = next_node;
            --size_;
        }
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        // Заглушка. Реализуйте метод самостоятельно
        if (pos.node_->next_node) {
            auto next_node = pos.node_->next_node->next_node;
            delete pos.node_->next_node;
            pos.node_->next_node = next_node;
            --size_;
        }
        return Iterator{pos.node_->next_node};
    }



    void Print() {
        Node* current = &head_.next_node;
        while (current) {
            std::cout << current->value << " ";
            current = current->next_node;
        }
        std::cout << std::endl;
    }
    void Print() const {
        Node* current = &head_.next_node;
        while (current) {
            std::cout << current->value << " ";
            current = current->next_node;
        }
        std::cout << std::endl;
    }
private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    if (lhs != rhs){
        lhs.swap(rhs);
    }
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()){
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lhs < rhs || lhs == rhs;
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs <= rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
} 


void Test0() {
    using namespace std;
    {
        const SingleLinkedList<int> empty_int_list;
        assert(empty_int_list.GetSize() == 0u);
        assert(empty_int_list.IsEmpty());
    }
    {
        SingleLinkedList<int> l1;
        SingleLinkedList<int> l2{1,2};
        swap(l1, l2);
        assert(l2.GetSize() == 0u);
        assert(l1.GetSize() == 2 );
        //std::cout << *(l1.begin()) << *(++l1.begin()) << std::endl;

    }
    {
        const SingleLinkedList<string> empty_string_list;
        assert(empty_string_list.GetSize() == 0u);
        assert(empty_string_list.IsEmpty());
    }
}
// Эта функция тестирует работу SingleLinkedList
void Test1() {
    // Шпион, следящий за своим удалением
    struct DeletionSpy {
        DeletionSpy() = default;
        explicit DeletionSpy(int& instance_counter) noexcept
            : instance_counter_ptr_(&instance_counter)  //
        {
            OnAddInstance();
        }
        DeletionSpy(const DeletionSpy& other) noexcept
            : instance_counter_ptr_(other.instance_counter_ptr_)  //
        {
            OnAddInstance();
        }
        DeletionSpy& operator=(const DeletionSpy& rhs) noexcept {
            if (this != &rhs) {
                auto rhs_copy(rhs);
                std::swap(instance_counter_ptr_, rhs_copy.instance_counter_ptr_);
            }
            return *this;
        }
        ~DeletionSpy() {
            OnDeleteInstance();
        }

    private:
        void OnAddInstance() noexcept {
            if (instance_counter_ptr_) {
                ++(*instance_counter_ptr_);
            }
        }
        void OnDeleteInstance() noexcept {
            if (instance_counter_ptr_) {
                assert(*instance_counter_ptr_ != 0);
                --(*instance_counter_ptr_);
            }
        }

        int* instance_counter_ptr_ = nullptr;
    };

    // Проверка вставки в начало
    {
        SingleLinkedList<int> l;
        assert(l.IsEmpty());
        assert(l.GetSize() == 0u);

        l.PushFront(0);
        l.PushFront(1);
        assert(l.GetSize() == 2);
        assert(!l.IsEmpty());

        l.Clear();
        assert(l.GetSize() == 0);
        assert(l.IsEmpty());
    }

    // Проверка фактического удаления элементов
    {
        int item0_counter = 0;
        int item1_counter = 0;
        int item2_counter = 0;
        {
            SingleLinkedList<DeletionSpy> list;
            list.PushFront(DeletionSpy{item0_counter});
            list.PushFront(DeletionSpy{item1_counter});
            list.PushFront(DeletionSpy{item2_counter});

            assert(item0_counter == 1);
            assert(item1_counter == 1);
            assert(item2_counter == 1);
            list.Clear();
            assert(item0_counter == 0);
            assert(item1_counter == 0);
            assert(item2_counter == 0);

            list.PushFront(DeletionSpy{item0_counter});
            list.PushFront(DeletionSpy{item1_counter});
            list.PushFront(DeletionSpy{item2_counter});
            assert(item0_counter == 1);
            assert(item1_counter == 1);
            assert(item2_counter == 1);
        }
        assert(item0_counter == 0);
        assert(item1_counter == 0);
        assert(item2_counter == 0);
    }

    // Вспомогательный класс, бросающий исключение после создания N-копии
    struct ThrowOnCopy {
        ThrowOnCopy() = default;
        explicit ThrowOnCopy(int& copy_counter) noexcept
            : countdown_ptr(&copy_counter) {
        }
        ThrowOnCopy(const ThrowOnCopy& other)
            : countdown_ptr(other.countdown_ptr)  //
        {
            if (countdown_ptr) {
                if (*countdown_ptr == 0) {
                    throw std::bad_alloc();
                } else {
                    --(*countdown_ptr);
                }
            }
        }
        // Присваивание элементов этого типа не требуется
        ThrowOnCopy& operator=(const ThrowOnCopy& rhs) = delete;
        // Адрес счётчика обратного отсчёта. Если не равен nullptr, то уменьшается при каждом копировании.
        // Как только обнулится, конструктор копирования выбросит исключение
        int* countdown_ptr = nullptr;
    };

    {
        bool exception_was_thrown = false;
        // Последовательно уменьшаем счётчик копирований до нуля, пока не будет выброшено исключение
        for (int max_copy_counter = 5; max_copy_counter >= 0; --max_copy_counter) {
            // Создаём непустой список
            SingleLinkedList<ThrowOnCopy> list;
            list.PushFront(ThrowOnCopy{});
            try {
                int copy_counter = max_copy_counter;
                list.PushFront(ThrowOnCopy(copy_counter));
                // Если метод не выбросил исключение, список должен перейти в новое состояние
                assert(list.GetSize() == 2);
            } catch (const std::bad_alloc&) {
                exception_was_thrown = true;
                // После выбрасывания исключения состояние списка должно остаться прежним
                assert(list.GetSize() == 1);
                break;
            }
        }
        assert(exception_was_thrown);
    }
}

// Эта функция тестирует работу SingleLinkedList
void Test2() {
    // Итерирование по пустому списку
    {
        SingleLinkedList<int> list;
        // Константная ссылка для доступа к константным версиям begin()/end()
        const auto& const_list = list;

        // Итераторы begin и end у пустого диапазона равны друг другу
        assert(list.begin() == list.end());
        assert(const_list.begin() == const_list.end());
        assert(list.cbegin() == list.cend());
        assert(list.cbegin() == const_list.begin());
        assert(list.cend() == const_list.end());
    }

    // Итерирование по непустому списку
    {
        SingleLinkedList<int> list;
        const auto& const_list = list;

        list.PushFront(1);
        assert(list.GetSize() == 1u);
        assert(!list.IsEmpty());

        assert(const_list.begin() != const_list.end());
        assert(const_list.cbegin() != const_list.cend());
        assert(list.begin() != list.end());

        assert(const_list.begin() == const_list.cbegin());

        assert(*list.cbegin() == 1);
        *list.begin() = -1;
        assert(*list.cbegin() == -1);

        const auto old_begin = list.cbegin();
        list.PushFront(2);
        assert(list.GetSize() == 2);

        const auto new_begin = list.cbegin();
        assert(new_begin != old_begin);
        // Проверка прединкремента
        {
            auto new_begin_copy(new_begin);
            assert((++(new_begin_copy)) == old_begin);
        }
        // Проверка постинкремента
        {
            auto new_begin_copy(new_begin);
            assert(((new_begin_copy)++) == new_begin);
            assert(new_begin_copy == old_begin);
        }
        // Итератор, указывающий на позицию после последнего элемента, равен итератору end()
        {
            auto old_begin_copy(old_begin);
            assert((++old_begin_copy) == list.end());
        }
    }
    // Преобразование итераторов
    {
        SingleLinkedList<int> list;
        list.PushFront(1);
        // Конструирование ConstIterator из Iterator
        auto it(list.cbegin());
        assert(it == list.cbegin());
        SingleLinkedList<int>::ConstIterator const_it(list.begin());
        assert(const_it == list.cbegin());
        assert(*const_it == *list.cbegin());

        SingleLinkedList<int>::ConstIterator const_it1;
        // Присваивание ConstIterator'у значения Iterator
        const_it1 = list.begin();
        assert(const_it1 == const_it);
    }
    // Проверка оператора ->
    {
        using namespace std;
        SingleLinkedList<std::string> string_list;

        string_list.PushFront("one"s);
        assert(string_list.cbegin()->length() == 3u);
        string_list.begin()->push_back('!');
        assert(*string_list.begin() == "one!"s);
    }
}

// Эта функция проверяет работу класса SingleLinkedList
void Test3() {
    // Проверка списков на равенство и неравенство
    {
        SingleLinkedList<int> list_1;
        list_1.PushFront(1);
        list_1.PushFront(2);

        SingleLinkedList<int> list_2;
        list_2.PushFront(1);
        list_2.PushFront(2);
        list_2.PushFront(3);

        SingleLinkedList<int> list_1_copy;
        list_1_copy.PushFront(1);
        list_1_copy.PushFront(2);

        SingleLinkedList<int> empty_list;
        SingleLinkedList<int> another_empty_list;

        // Список равен самому себе
        assert(list_1 == list_1);
        assert(empty_list == empty_list);

        // Списки с одинаковым содержимым равны, а с разным - не равны
        assert(list_1 == list_1_copy);
        assert(list_1 != list_2);
        assert(list_2 != list_1);
        assert(empty_list == another_empty_list);
    }

    // Обмен содержимого списков
    {
        SingleLinkedList<int> first;
        first.PushFront(1);
        first.PushFront(2);

        SingleLinkedList<int> second;
        second.PushFront(10);
        second.PushFront(11);
        second.PushFront(15);

        const auto old_first_begin = first.begin();
        const auto old_second_begin = second.begin();
        const auto old_first_size = first.GetSize();
        const auto old_second_size = second.GetSize();

        first.swap(second);

        assert(second.begin() == old_first_begin);
        assert(first.begin() == old_second_begin);
        assert(second.GetSize() == old_first_size);
        assert(first.GetSize() == old_second_size);

        // Обмен при помощи функции swap
        {
            using std::swap;

            // В отсутствие пользовательской перегрузки будет вызвана функция std::swap, которая
            // выполнит обмен через создание временной копии
            swap(first, second);

            // Убеждаемся, что используется не std::swap, а пользовательская перегрузка

            // Если бы обмен был выполнен с созданием временной копии,
            // то итератор first.begin() не будет равен ранее сохранённому значению,
            // так как копия будет хранить свои узлы по иным адресам
            assert(first.begin() == old_first_begin);
            assert(second.begin() == old_second_begin);
            assert(first.GetSize() == old_first_size);
            assert(second.GetSize() == old_second_size);
        }
    }

    // Инициализация списка при помощи std::initializer_list
    {
        SingleLinkedList<int> list{1, 2, 3, 4, 5};
        assert(list.GetSize() == 5);
        assert(!list.IsEmpty());
        assert(std::equal(list.begin(), list.end(), std::begin({1, 2, 3, 4, 5})));
    }

    // Лексикографическое сравнение списков
    {
        using IntList = SingleLinkedList<int>;

        assert((IntList{1, 2, 3} < IntList{1, 2, 3, 1}));
        assert((IntList{1, 2, 3} <= IntList{1, 2, 3}));
        assert((IntList{1, 2, 4} > IntList{1, 2, 3}));
        assert((IntList{1, 2, 3} >= IntList{1, 2, 3}));
    }

    // Копирование списков
    {
        const SingleLinkedList<int> empty_list{};
        // Копирование пустого списка
        {
            auto list_copy(empty_list);
            assert(list_copy.IsEmpty());
        }

        SingleLinkedList<int> non_empty_list{1, 2, 3, 4};
        // Копирование непустого списка
        {
            auto list_copy(non_empty_list);

            assert(non_empty_list.begin() != list_copy.begin());
            assert(list_copy == non_empty_list);
        }
    }

    // Присваивание списков
    {
        const SingleLinkedList<int> source_list{1, 2, 3, 4};

        SingleLinkedList<int> receiver{5, 4, 3, 2, 1};
        receiver = source_list;
        assert(receiver.begin() != source_list.begin());
        assert(receiver == source_list);
    }

    // Вспомогательный класс, бросающий исключение после создания N-копии
    struct ThrowOnCopy {
        ThrowOnCopy() = default;
        explicit ThrowOnCopy(int& copy_counter) noexcept
            : countdown_ptr(&copy_counter) {
        }
        ThrowOnCopy(const ThrowOnCopy& other)
            : countdown_ptr(other.countdown_ptr)  //
        {
            if (countdown_ptr) {
                if (*countdown_ptr == 0) {
                    throw std::bad_alloc();
                } else {
                    --(*countdown_ptr);
                }
            }
        }
        // Присваивание элементов этого типа не требуется
        ThrowOnCopy& operator=(const ThrowOnCopy& rhs) = delete;
        // Адрес счётчика обратного отсчёта. Если не равен nullptr, то уменьшается при каждом копировании.
        // Как только обнулится, конструктор копирования выбросит исключение
        int* countdown_ptr = nullptr;
    };

    // Безопасное присваивание списков
    {
        SingleLinkedList<ThrowOnCopy> src_list;
        src_list.PushFront(ThrowOnCopy{});
        src_list.PushFront(ThrowOnCopy{});
        auto thrower = src_list.begin();
        src_list.PushFront(ThrowOnCopy{});

        int copy_counter = 0;  // при первом же копировании будет выброшено исключение
        thrower->countdown_ptr = &copy_counter;

        SingleLinkedList<ThrowOnCopy> dst_list;
        dst_list.PushFront(ThrowOnCopy{});
        int dst_counter = 10;
        dst_list.begin()->countdown_ptr = &dst_counter;
        dst_list.PushFront(ThrowOnCopy{});

        try {
            dst_list = src_list;
            // Ожидается исключение при присваивании
            assert(false);
        } catch (const std::bad_alloc&) {
            // Проверяем, что состояние списка-приёмника не изменилось
            // при выбрасывании исключений
            assert(dst_list.GetSize() == 2);
            auto it = dst_list.begin();
            assert(it != dst_list.end());
            assert(it->countdown_ptr == nullptr);
            ++it;
            assert(it != dst_list.end());
            assert(it->countdown_ptr == &dst_counter);
            assert(dst_counter == 10);
        } catch (...) {
            // Других типов исключений не ожидается
            assert(false);
        }
    }
}

void Test4() {
    struct DeletionSpy {
        ~DeletionSpy() {
            if (deletion_counter_ptr) {
                ++(*deletion_counter_ptr);
            }
        }
        int* deletion_counter_ptr = nullptr;
    };

    // Проверка PopFront
    {
        SingleLinkedList<int> numbers{3, 14, 15, 92, 6};
        numbers.PopFront();
        assert((numbers == SingleLinkedList<int>{14, 15, 92, 6}));

        SingleLinkedList<DeletionSpy> list;
        list.PushFront(DeletionSpy{});
        int deletion_counter = 0;
        list.begin()->deletion_counter_ptr = &deletion_counter;
        assert(deletion_counter == 0);
        list.PopFront();
        assert(deletion_counter == 1);
    }

    // Доступ к позиции, предшествующей begin
    {
        SingleLinkedList<int> empty_list;
        const auto& const_empty_list = empty_list;
        assert(empty_list.before_begin() == empty_list.cbefore_begin());
        assert(++empty_list.before_begin() == empty_list.begin());
        assert(++empty_list.cbefore_begin() == const_empty_list.begin());

        SingleLinkedList<int> numbers{1, 2, 3, 4};
        const auto& const_numbers = numbers;
        assert(numbers.before_begin() == numbers.cbefore_begin());
        assert(++numbers.before_begin() == numbers.begin());
        assert(++numbers.cbefore_begin() == const_numbers.begin());
    }

    // Вставка элемента после указанной позиции
    {  // Вставка в пустой список
        {
            SingleLinkedList<int> lst;
            const auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);
            assert((lst == SingleLinkedList<int>{123}));
            assert(inserted_item_pos == lst.begin());
            assert(*inserted_item_pos == 123);
        }

        // Вставка в непустой список
        {
            SingleLinkedList<int> lst{1, 2, 3};
            auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);

            assert(inserted_item_pos == lst.begin());
            assert(inserted_item_pos != lst.end());
            assert(*inserted_item_pos == 123);
            assert((lst == SingleLinkedList<int>{123, 1, 2, 3}));

            inserted_item_pos = lst.InsertAfter(lst.begin(), 555);
            assert(++SingleLinkedList<int>::Iterator(lst.begin()) == inserted_item_pos);
            assert(*inserted_item_pos == 555);
            assert((lst == SingleLinkedList<int>{123, 555, 1, 2, 3}));
        };
    }

    // Вспомогательный класс, бросающий исключение после создания N-копии
    struct ThrowOnCopy {
        ThrowOnCopy() = default;
        explicit ThrowOnCopy(int& copy_counter) noexcept
            : countdown_ptr(&copy_counter) {
        }
        ThrowOnCopy(const ThrowOnCopy& other)
            : countdown_ptr(other.countdown_ptr)  //
        {
            if (countdown_ptr) {
                if (*countdown_ptr == 0) {
                    throw std::bad_alloc();
                } else {
                    --(*countdown_ptr);
                }
            }
        }
        // Присваивание элементов этого типа не требуется
        ThrowOnCopy& operator=(const ThrowOnCopy& rhs) = delete;
        // Адрес счётчика обратного отсчёта. Если не равен nullptr, то уменьшается при каждом копировании.
        // Как только обнулится, конструктор копирования выбросит исключение
        int* countdown_ptr = nullptr;
    };

    // Проверка обеспечения строгой гарантии безопасности исключений
    {
        bool exception_was_thrown = false;
        for (int max_copy_counter = 10; max_copy_counter >= 0; --max_copy_counter) {
            SingleLinkedList<ThrowOnCopy> list{ThrowOnCopy{}, ThrowOnCopy{}, ThrowOnCopy{}};
            try {
                int copy_counter = max_copy_counter;
                list.InsertAfter(list.cbegin(), ThrowOnCopy(copy_counter));
                assert(list.GetSize() == 4u);
            } catch (const std::bad_alloc&) {
                exception_was_thrown = true;
                assert(list.GetSize() == 3u);
                break;
            }
        }
        assert(exception_was_thrown);
    }

    // Удаление элементов после указанной позиции
    {
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto& const_lst = lst;
            const auto item_after_erased = lst.EraseAfter(const_lst.cbefore_begin());
            assert((lst == SingleLinkedList<int>{2, 3, 4}));
            assert(item_after_erased == lst.begin());
        }
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto item_after_erased = lst.EraseAfter(lst.cbegin());
            assert((lst == SingleLinkedList<int>{1, 3, 4}));
            assert(item_after_erased == (++lst.begin()));
        }
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto item_after_erased = lst.EraseAfter(++(++lst.cbegin()));
            assert((lst == SingleLinkedList<int>{1, 2, 3}));
            assert(item_after_erased == lst.end());
        }
        {
            SingleLinkedList<DeletionSpy> list{DeletionSpy{}, DeletionSpy{}, DeletionSpy{}};
            auto after_begin = ++list.begin();
            int deletion_counter = 0;
            after_begin->deletion_counter_ptr = &deletion_counter;
            assert(deletion_counter == 0u);
            list.EraseAfter(list.cbegin());
            assert(deletion_counter == 1u);
        }
    }
}

int main() {
    Test0();
    Test1();
    Test2();
    Test3();
    Test4();
}