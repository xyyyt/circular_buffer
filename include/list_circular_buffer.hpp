#ifndef LIST_CIRCULAR_BUFFER_HPP_
# define LIST_CIRCULAR_BUFFER_HPP_

# include <memory>
# include <mutex>
# include <utility>
# include <stdexcept>
# include <cstdint>

template <typename T>
class list_circular_buffer
{
public :
    list_circular_buffer() noexcept : _full(false) { }

    list_circular_buffer(size_t size) :
        _list(size),
        _start(_list._head),
        _end(_list._head),
        _full(false)
    { }

    list_circular_buffer(const list_circular_buffer& other)
    {
        std::lock_guard<std::recursive_mutex> lock(other._mutex);

        this->lcb_copy(other);
    }

    list_circular_buffer(list_circular_buffer&& other)
    {
        std::lock_guard<std::recursive_mutex> lock(other._mutex);

        this->lcb_move(std::move(other));
    }

    list_circular_buffer& operator=(const list_circular_buffer& other)
    {
        std::lock_guard<std::recursive_mutex> lock(other._mutex);
        std::lock_guard<std::recursive_mutex> lock2(_mutex);

        if (this != &other)
        {
            this->lcb_copy(other);
        }

        return *this;
    }

    list_circular_buffer& operator=(list_circular_buffer&& other)
    {
        std::lock_guard<std::recursive_mutex> lock(other._mutex);
        std::lock_guard<std::recursive_mutex> lock2(_mutex);

        if (this != &other)
        {
            this->lcb_move(std::move(other));
        }

        return *this;
    }

    size_t buffer_size() const
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        return _list._size;
    }

    bool is_empty() const
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (_full)
        {
            return false;
        }

        return _start == _end;
    }

    bool is_full() const
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        return _full;
    }

    void clear()
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        _start = _list._head;
        _end = _list._head;
        _full = false;
    }

    void resize(size_t buffer_size)
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (buffer_size == _list._size)
        {
            return;
        }

        if (buffer_size == 0)
        {
            *this = list_circular_buffer();
        }
        else if (buffer_size > _list._size && _list._size == 0)
        {
            *this = list_circular_buffer(buffer_size);
        }
        else
        {
            auto curr = _list._head;
            uint32_t n = 0;

            while (curr != _start)
            {
                curr = curr->next;
                ++n;
            }

            auto lcb_old = std::move(*this);

            _list = circular_singly_linked_list(buffer_size);
            curr = _list._head;

            for (uint32_t n2 = 0; n2 < n; ++n2)
            {
                curr = curr->next;
            }

            _start = curr;
            _end = curr;

            while (!lcb_old.is_empty())
            {
                this->add(lcb_old.get());
            }
        }
    }

    list_circular_buffer& add(const T& value)
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (_list._size == 0)
        {
            throw std::out_of_range(
                      "circular buffer doesn't have space memory to store");
        }

        _end->value = value;
        _end = _end->next;

        this->post_add();

        return *this;
    }

    list_circular_buffer& add(T&& value)
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (_list._size == 0)
        {
            throw std::out_of_range(
                      "circular buffer doesn't have space memory to store");
        }

        _end->value = std::move(value);
        _end = _end->next;

        this->post_add();

        return *this;
    }

    T get()
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (this->is_empty())
        {
            throw std::out_of_range("circular buffer is empty");
        }

        return this->_get();
    }

    bool try_get(T& value)
    {
        std::unique_lock<std::recursive_mutex> lock(_mutex, std::try_to_lock);

        if (!lock.owns_lock())
        {
            return false;
        }

        if (this->is_empty())
        {
            return false;
        }

        value = this->_get();

        return true;
    }

private :
    class circular_singly_linked_list final
    {
    public :
        struct node
        {
            T value;
            std::shared_ptr<node> next;

            node() = default;

            node(const T& value) noexcept(noexcept(T(std::declval<T&>()))) :
                value(value)
            { }
        };

        size_t _size;
        std::shared_ptr<node> _head;
        std::shared_ptr<node> _tail;

        circular_singly_linked_list() noexcept : _size(0) { }

        circular_singly_linked_list(size_t size) : _size(size)
        {
            if (size > 0)
            {
                _head = std::make_shared<node>();

                auto curr = _head;

                for (uint32_t n = 1; n < size; ++n)
                {
                    curr->next = std::make_shared<node>();
                    curr = curr->next;
                }

                _tail = std::move(curr);
                _tail->next = _head;
            }
        }

        circular_singly_linked_list(const circular_singly_linked_list& other)
        {
            this->csll_copy(other);
        }

        circular_singly_linked_list(circular_singly_linked_list&& other)
        {
            this->csll_move(std::move(other));
        }

        circular_singly_linked_list& operator=(
            const circular_singly_linked_list& other)
        {
            if (this != &other)
            {
                this->csll_copy(other);
            }

            return *this;
        }

        circular_singly_linked_list& operator=(
            circular_singly_linked_list&& other)
        {
            if (this != &other)
            {
                this->csll_move(std::move(other));
            }

            return *this;
        }

    private :
        void csll_copy(const circular_singly_linked_list& other)
        {
            _size = other._size;

            if (other._size == 0)
            {
                _head.reset();
                _tail.reset();
            }
            else
            {
                _head = std::make_shared<node>(other._head->value);

                auto curr = _head;
                auto curr2 = other._head;

                for (uint32_t n = 1; n < other._size; ++n)
                {
                    curr->next = std::make_shared<node>(curr2->next->value);
                    curr = curr->next;
                    curr2 = curr2->next;
                }

                _tail = std::move(curr);
                _tail->next = _head;
            }
        }

        void csll_move(circular_singly_linked_list&& other)
        {
            _size = other._size;
            _head = std::move(other._head);
            _tail = std::move(other._tail);
        }
    };


    using node_t = typename circular_singly_linked_list::node;

    circular_singly_linked_list _list;
    std::shared_ptr<node_t> _start;
    std::shared_ptr<node_t> _end;
    bool _full;
    mutable std::recursive_mutex _mutex;

    void lcb_copy(const list_circular_buffer& other)
    {
        _list = other._list;

        auto curr = _list._head;
        auto curr2 = _list._head;
        auto curr3 = other._list._head;
        bool start_found = false;
        bool end_found = false;

        while (1)
        {
            if (curr3 == other._start)
            {
                start_found = true;
            }

            if (curr3 == other._end)
            {
                end_found = true;
            }

            if (!start_found)
            {
                curr = curr->next;
            }

            if (!end_found)
            {
                curr2 = curr2->next;
            }

            if (start_found && end_found)
            {
                break;
            }

            curr3 = curr3->next;
        }

        _start = curr;
        _end = curr2;
        _full = other._full;
    }

    void lcb_move(list_circular_buffer&& other)
    {
        _list = std::exchange(other._list, circular_singly_linked_list());
        _start = std::exchange(other._start, nullptr);
        _end = std::exchange(other._end, nullptr);
        _full = std::exchange(other._full, false);
    }

    void post_add() noexcept
    {
        if (_end == _start)
        {
            _full = true;
        }
        else if (_full)
        {
            _start = _start->next;
        }
    }

    T _get()
    {
        T value = std::move(_start->value);

        _start = _start->next;

        if (_full)
        {
            _full = false;
        }

        return value;
    }
};

#endif
