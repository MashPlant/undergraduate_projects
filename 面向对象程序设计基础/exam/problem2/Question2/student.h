#ifndef STUDENT_H
#define STUDENT_H

#include <ostream>

class Student
{
    int score1, score2;

  public:
    Student() = default;

    Student(int score1, int score2) : score1(score1), score2(score2) {}

    bool operator<(const Student &rhs) const
    {
        if (score1 < rhs.score1)
            return true;
        else if (score1 == rhs.score1)
        {
            if (score2 < rhs.score2)
                return true;
        }
        return false;
    }

    friend std::ostream &operator<<(std::ostream &os, const Student &stu)
    {
        os << "Student: score1 = " << stu.score1 << "; score2 = " << stu.score2;
    }
};

#endif // STUDENT_H