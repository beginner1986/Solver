#ifndef __SOLVER_H__
#define __SOLVER_H__

class Solver
{
public:
    static Solver& getInstance()
    {
        static Solver instance;
        return instance;
    }

private:
    Solver() = default;
    ~Solver() = default;
    Solver(const Solver&) = delete;
    Solver& operator=(const Solver&) = delete;
};

#endif // __SOLVER_H__