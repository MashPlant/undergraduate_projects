#include <iostream> // std::cout, std::endl
#include <cstring>  // strlen , strcpy

class bug
{
    char *data_;

  public:
    bug(const char *str)
    {
        data_ = new char[strlen(str) + 1];
        strcpy(data_, str);
    }

    void swap(bug &rhs) noexcept
    {
        using std::swap;
        swap(data_, rhs.data_);
    }

    bug(const bug &rhs)
    {
        data_ = new char[strlen(rhs.data_) + 1];
        strcpy(data_, rhs.data_);
    }

    bug &operator=(const bug &rhs)
    {
        bug tmp(rhs);
        swap(tmp);
        return *this;
    }

    bug(bug &&rhs) noexcept
    {
        data_ = rhs.data_;
        rhs.data_ = nullptr;
    }

    bug &operator=(bug &&rhs) noexcept
    {
        using std::move;
        bug tmp(move(rhs));
        swap(tmp);
        return *this;
    }

    ~bug()
    {
        delete[] data_;
    }

    void show()
    {
        std::cout << data_ << std::endl;
    }
};

void test(bug str1)
{
    str1.show();
    bug str2("tsinghua");
    str2.show();
    str2 = str1;
    str2.show();
}

int main()
{
    bug str1("2011");
    str1.show();
    test(str1);
    str1.show();
    return 0;
}