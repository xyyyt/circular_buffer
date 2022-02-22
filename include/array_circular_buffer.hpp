#ifndef ARRAY_CIRCULAR_BUFFER_HPP_
# define ARRAY_CIRCULAR_BUFFER_HPP_

# include <memory>
# include <mutex>
# include <stdexcept>
# include <cstdint>

template <typename T>
class array_circular_buffer
{
public :
    array_circular_buffer() noexcept :
        _buffer_size(0), _start(0), _end(0), _full(false)
    { }

    array_circular_buffer(size_t buffer_size) :
        _buffer_size(buffer_size),
        _buffer(std::make_unique<T[]>(buffer_size)),
        _start(0),
        _end(0),
        _full(false)
    { }

    array_circular_buffer(const array_circular_buffer& other)
    {
        std::lock_guard<std::recursive_mutex> lock(other._mutex);

        this->acb_copy(other);
    }

    array_circular_buffer(array_circular_buffer&& other)
    {
        std::lock_guard<std::recursive_mutex> lock(other._mutex);

        this->acb_move(std::move(other));
    }

    array_circular_buffer& operator=(const array_circular_buffer& other)
    {
        std::lock_guard<std::recursive_mutex> lock(other._mutex);
        std::lock_guard<std::recursive_mutex> lock2(_mutex);

        if (this != &other)
        {
            this->acb_copy(other);
        }

        return *this;
    }

    array_circular_buffer& operator=(array_circular_buffer&& other)
    {
        std::lock_guard<std::recursive_mutex> lock(other._mutex);
        std::lock_guard<std::recursive_mutex> lock2(_mutex);

        if (this != &other)
        {
            this->acb_move(std::move(other));
        }

        return *this;
    }

    size_t buffer_size() const
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        return _buffer_size;
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

        _start = 0;
        _end = 0;
        _full = false;
    }

    void resize(size_t buffer_size)
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (buffer_size == _buffer_size)
        {
            return;
        }

        if (buffer_size == 0)
        {
            *this = array_circular_buffer();
        }
        else if (buffer_size > _buffer_size && _buffer_size == 0)
        {
            *this = array_circular_buffer(buffer_size);
        }
        else
        {
            uint32_t n = 0;

            if (_start < buffer_size)
            {
                n = _start;
            }
            else
            {
                while (n < _start)
                {
                    ++n;
                }
            }

            auto acb_old = std::move(*this);

            _buffer_size = buffer_size;
            _buffer = std::make_unique<T[]>(buffer_size);
            _start = n % buffer_size;
            _end = _start;

            while (!acb_old.is_empty())
            {
                this->add(acb_old.get());
            }
        }
    }

    array_circular_buffer& add(const T& value)
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (_buffer_size == 0)
        {
            throw std::out_of_range(
                      "circular buffer doesn't have space memory to store");
        }

        _buffer[_end++] = value;
        this->post_add();

        return *this;
    }

    array_circular_buffer& add(T&& value)
    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (_buffer_size == 0)
        {
            throw std::out_of_range(
                      "circular buffer doesn't have space memory to store");
        }

        _buffer[_end++] = std::move(value);
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
    size_t _buffer_size;
    std::unique_ptr<T[]> _buffer;
    uint32_t _start;
    uint32_t _end;
    bool _full;
    mutable std::recursive_mutex _mutex;

    void acb_copy(const array_circular_buffer& other)
    {
        _buffer_size = other._buffer_size;
        _buffer.reset(new T[other._buffer_size]);

        for (uint32_t n = 0; n < other._buffer_size; ++n)
        {
            _buffer[n] = other._buffer[n];
        }

        _start = other._start;
        _end = other._end;
        _full = other._full;
    }

    void acb_move(array_circular_buffer&& other)
    {
        _buffer_size = std::exchange(other._buffer_size, 0);
        _buffer = std::exchange(other._buffer, nullptr);
        _start = std::exchange(other._start, 0);
        _end = std::exchange(other._end, 0);
        _full = std::exchange(other._full, false);
    }

    void post_add() noexcept
    {
        _end %= _buffer_size;

        if (_end == _start)
        {
            _full = true;
        }
        else if (_full)
        {
            ++_start;
            _start %= _buffer_size;
        }
    }

    T _get()
    {
        T value = std::move(_buffer[_start++]);

        _start %= _buffer_size;

        if (_full)
        {
            _full = false;
        }

        return value;
    }
};

#endif
