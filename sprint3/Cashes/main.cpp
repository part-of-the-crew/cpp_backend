#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace std::string_literals;

template <typename Key, typename Value, typename ValueFactoryFn>
class Cache {
public:
    // Создаёт кеш, инициализированный фабрикой значений, созданной по умолчанию
    Cache() = default;

    // Создаёт кеш, инициализированный объетом, играющим роль фабрики,
    // создающей (при помощи operator() значение по его ключу)
    // Фабрика должна вернуть shared_ptr<Value> либо unique_ptr<Value>.
    // Пример использования:
    // shared_ptr<Value> value = value_factory(key);
    explicit Cache(ValueFactoryFn value_factory) {
        m_value_factory = value_factory;
    }

    // Возвращает закешированное значение по ключу. Если значение отсутствует или уже удалено,
    // оно должно быть создано с помощью фабрики и сохранено в кеше.
    // Если на объект нет внешних ссылок, он должен быть удалён из кеша
    std::shared_ptr<Value> GetValue(const Key& key) {
        auto it = m_key.find(key);
        if (it != m_key.end()) {
            // Если внешние ссылки на объект уже не нулевые, возвращаем его
            if (auto value = it->second.lock()) {
                return value;
            }
            // Если внешние ссылки на объект нулевые, удаляем его из кеша
            m_key.erase(it);
        }
        auto sp = m_value_factory(key);
        m_key[key] = sp;
        return sp;
    }
    private:
    std::unordered_map<Key, std::weak_ptr<Value>> m_key;
    // weak_ptr для того, чтобы удаление не приводило к удалению ссылок на объект
    std::unordered_map<Key, std::weak_ptr<Value>> m_value;
    // Значение фабрики, создаваемое при инициализации кеша
    ValueFactoryFn m_value_factory;

};

// Пример объекта, находящегося в кеше
class Object {
public:
    explicit Object(std::string id)
        : id_(std::move(id))  //
    {
        using namespace std;
        cout << "Object '"sv << id_ << "' has been created"sv << endl;
    }

    const std::string& GetId() const {
        return id_;
    }

    ~Object() {
        using namespace std;
        cout << "Object '"sv << id_ << "' has been destroyed"sv << endl;
    }

private:
    std::string id_;
};

using ObjectPtr = std::shared_ptr<Object>;

struct ObjectFactory {
    ObjectPtr operator()(std::string id) const {
        return std::make_shared<Object>(std::move(id));
    }
};

void Test1() {
    using namespace std;
    std::shared_ptr<Object> alice1;
    // Кеш, объектов Object, создаваемых при помощи ObjectFactory,
    // доступ к которым предоставляется по ключу типа string
    Cache<string, Object, ObjectFactory> cache;

    // Извлекаем объекты Alice и Bob
    alice1 = cache.GetValue("Alice"s);
    auto bob = cache.GetValue("Bob"s);
    // Должны вернуться два разных объекта с правильными id
    assert(alice1 != bob);
    assert(alice1->GetId() == "Alice"s);
    assert(bob->GetId() == "Bob"s);

    // Повторный запрос объекта Alice должен вернуть существующий объект
    auto alice2 = cache.GetValue("Alice"s);
    assert(alice1 == alice2);

    // Указатель alice_wp следит за жизнью объекта Alice
    weak_ptr alice_wp{alice1};
    alice1.reset();
    assert(!alice_wp.expired());
    cout << "---"sv << endl;
    alice2.reset();
    // Объект Alice будет удалён, так как на него больше не ссылаются shared_ptr
    assert(alice_wp.expired());
    cout << "---"sv << endl;
    // Объект Bob будет удалён после разрушения указателя bob

    alice1 = cache.GetValue("Alice"s);  // объект 'Alice' будет создан заново
    cout << "---"sv << endl;
}

struct Book {
    Book(std::string title, std::string content)
        : title(std::move(title))
        , content(std::move(content)) {
    }

    std::string title;
    std::string content;
};

// Функциональный объект, загружающий книги из unordered_map
class BookLoader {
public:

    using BookStore = std::unordered_map<std::string, std::string>;

    // Принимает константную ссылку на хранилище книг и ссылку на переменную-счётчик загрузок
    explicit BookLoader(const BookStore& store, size_t& load_count) {
        // Реализуйте конструктор самостоятельно
    }

    // Загружает книгу из хранилища по её названию и возвращает указатель
    // В случае успешной загрузки (книга есть в хранилище)
    // нужно увеличить значения счётчика загрузок load_count, переданного в конструктор, на 1.
    // Если книга в хранилище отсутствует, нужно выбросить исключение std::out_of_range,
    // а счётчик не увеличивать
    std::shared_ptr<Book> operator()(const std::string& title) const {
        // Заглушка, реализуйте метод самостоятельно
        (void) title;
        throw std::out_of_range("Not implemented"s);
    }

private:
    // Добавьте необходимые данные и/или методы
};

void Test2() {
    using namespace std;
    // Хранилище книг.
    BookLoader::BookStore books{
        {"Sherlock Holmes"s,
         "To Sherlock Holmes she is always the woman. I have seldom heard him mention her under any other name."s},
        {"Harry Potter"s, "Chapter 1. The boy who lived. ...."s},
    };
    using BookCache = Cache<string, Book, BookLoader>;

    size_t load_count = 0;
    // Создаём кеш, который будет использщовать BookLoader для загрузки книг из хранилища books
    BookCache book_cache{BookLoader{books, load_count}};

    // Загруженная книга должна содержать данные из хранилища
    auto book1 = book_cache.GetValue("Sherlock Holmes"s);
    assert(book1);
    assert(book1->title == "Sherlock Holmes"s);
    assert(book1->content == books.at(book1->title));
    assert(load_count == 1);

    // Повторный запрос книги должен возвращать закешированное значение
    auto book2 = book_cache.GetValue("Sherlock Holmes"s);
    assert(book2);
    assert(book1 == book2);
    assert(load_count == 1);

    weak_ptr<Book> weak_book{book1};
    assert(!weak_book.expired());
    book1.reset();
    book2.reset();
    // Книга удаляется, как только на неё перестают ссылаться указатели вне кеша
    assert(weak_book.expired());

    book1 = book_cache.GetValue("Sherlock Holmes"s);
    assert(load_count == 2);
    assert(book1);

    try {
        book_cache.GetValue("Fifty Shades of Grey"s);
        // BookLoader выбросит исключение при попытке загрузить несуществующую книгу
        assert(false);
    } catch (const std::out_of_range&) {
        //Всё нормально. Такой книги нет в книгохранилище
    } catch (...) {
        cout << "Unexpected exception"sv << endl;
    }
    // Счётчик загрузок не должен обновиться, так как книги нет в хранилище
    assert(load_count == 2);

    // Добавляем книгу в хранилище
    books["Fifty Shades of Grey"s] = "I scowl with frustration at myself in the mirror..."s;

    try {
        auto book = book_cache.GetValue("Fifty Shades of Grey"s);
        // Теперь книга должна быть успешно найдена
        assert(book->content == books.at("Fifty Shades of Grey"s));
    } catch (...) {
        assert(false);
    }
    // Счётчик загрузок должен обновиться, так как книга была загружена из хранилища
    assert(load_count == 3);
}

int main() {
    Test1();
    //Test2();
}