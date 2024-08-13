#ifndef GAME_MATH_HELPER
#define GAME_MATH_HELPER
#include <cmath>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <queue>
#include <initializer_list>
namespace gMath
{

    using axisV = unsigned int;
    class Angle;
    // 二维向量
    class tVector
    {
    public:
        double x, y;
        tVector(double a, double b) : x(a), y(b) {}
        tVector(axisV a, axisV b) : x(a), y(b) {}
        tVector(const tVector &r) : x(r.x), y(r.y) {}
        tVector(double len, const tVector &direction) : tVector(direction * (len / direction.sLen())) {}
        /**
         * Calculates the dot product of the current vector and the given vector.
         *
         * @param v The vector to calculate the dot product with.
         *
         * @return The dot product of the current vector and the given vector.
         */
        double dot(const tVector &v) const
        {
            return x * v.x + y * v.y;
        }
        tVector operator-(const tVector &v) const
        {
            return tVector(x - v.x, y - v.y);
        }
        tVector operator+(const tVector &v) const{
            return tVector(x + v.x, y + v.y);
        }
        // 正交
        tVector normal() const
        {
            return tVector(-y, x);
        }
        // 返回一个正交的单位向量
        tVector normal_and_unify() const
        {
            return normal() / std::sqrt(sLen());
        }
        // 单位化
        void unify()
        {
            double r = std::sqrt(sLen());
            x /= r;
            y /= r;
        }
        tVector &&unify() const
        {
            double r = std::sqrt(sLen());
            return tVector(x / r, y / r);
        }
        tVector operator=(const tVector &r)
        {
            x = r.x, y = r.y;
            return *this;
        }
        bool operator==(const tVector &r) const
        {
            return x == r.x && y == r.y;
        }
        // 模长的平方
        double sLen() const
        {
            return x * x + y * y;
        }
        double magnitude() const{
            return std::sqrt(sLen());
        }
        tVector operator/(double a) const
        {
            return tVector(x / a, y / a);
        }
        tVector &operator+=(const tVector &r)
        {
            x += r.x;
            y += r.y;
            return *this;
        }
        void rotate(const Angle &angle)
        {
            double sinA = std::sin(angle.getRadians()), cosA = std::cos(angle.getRadians()), tx = x, ty = y;
            x = tx * cosA - ty * sinA;
            y = tx * sinA + ty * cosA;
        }
        tVector &&reverse()
        {
            return tVector(-x, -y);
        }
        tVector &&operator*(double a) const
        {
            return tVector(x * a, y * a);
        }
        double cross(const tVector &r) const{
            return x * r.y - y * r.x;
        }
    };
    // 坐标类
    class Crdinate
    {
    private:
        axisV x;
        axisV y;

    public:
        Crdinate(axisV x_, axisV y_) : x(x_), y(y_) {}

        void set_x(axisV x_)
        {
            this->x = x_;
        }
        void set_y(axisV y_)
        {
            this->y = y_;
        }
        axisV get_x()
        {
            return x;
        }
        axisV get_y()
        {
            return y;
        }
        Crdinate operator=(const Crdinate &crd)
        {
            x = crd.x;
            y = crd.y;
            return *this;
        }
        tVector operator+(const tVector &v) const
        {
            return tVector(x + v.x, y + v.y);
        }
        tVector operator-(const Crdinate &r) const
        {
            return tVector(x - r.x, y - r.y);
        }
        Crdinate &operator+=(const tVector &v)
        {
            x += static_cast<axisV>(v.x);
            y += static_cast<axisV>(v.y);
            return *this;
        }
    };
    class mRectangle
    {
    protected:
        axisV l, r, t, b;

    public:
        mRectangle(axisV left, axisV right, axisV top, axisV bottom) : l(left), r(right), t(top), b(bottom) {}
        bool Inside(Crdinate crd)
        {
            return crd.get_x() <= r && crd.get_y() <= t && crd.get_x() >= l && crd.get_y() >= b;
        }
        bool intersects(const mRectangle &rec)
        {
            return l <= rec.r && r >= rec.l && t >= rec.b && b <= rec.t;
        }
    };
    class Angle
    {
    public:
        // 构造函数
        Angle(double angleInDegrees = 0.0) : degrees(angleInDegrees), radians(degreesToRadians(degrees))
        {
            normalize();
        }

        // 获取以度数表示的角度
        double getDegrees() const
        {
            return degrees;
        }

        // 设置以度数表示的角度
        void setDegrees(double angleInDegrees)
        {
            degrees = angleInDegrees;
            normalize();
            radians = degreesToRadians(degrees);
        }

        // 获取以弧度表示的角度
        double getRadians() const
        {
            return radians;
        }

        // 设置以弧度表示的角度
        void setRadians(double angleInRadians)
        {
            degrees = radiansToDegrees(angleInRadians);

            normalize();
            radians = degreesToRadians(degrees);
        }

        // 角度加法
        Angle operator+(const Angle &other) const
        {
            return Angle(degrees + other.degrees);
        }

        // 角度减法
        Angle operator-(const Angle &other) const
        {
            return Angle(degrees - other.degrees);
        }
        Angle &operator+=(const Angle &other)
        {
            degrees += other.degrees;
            return *this;
        }
        bool operator!=(const Angle &r) const
        {
            return degrees != r.degrees;
        }
        operator double()
        {
            return degrees;
        }

    private:
        double degrees;
        double radians;
        // 将角度规范化到0到360度之间
        void normalize()
        {
            degrees = fmod(degrees, 360.0);
            if (degrees < 0)
            {
                degrees += 360.0;
            }
        }

        // 辅助函数：度数转弧度
        static double degreesToRadians(double degrees)
        {
            return degrees * M_PI / 180.0;
        }

        // 辅助函数：弧度转度数
        static double radiansToDegrees(double radians)
        {
            return radians * 180.0 / M_PI;
        }
    };
    // 无向图
    // gpt-generated
    template <typename T>
    class Graph
    {
    public:
        Gragh(std::initializer_list<std::initializer_list<T>>&& edges)
        {
            for (const auto &edge : edges)
            {
                addEdge(edge[0], edge[1]);
            }
        }
        void addNode(const T &value)
        {
            if (adjacencyList.find(value) == adjacencyList.end())
            {
                adjacencyList[value] = std::unordered_set<T>();
            }
        }

        void addEdge(const T &from, const T &to)
        {
            addNode(from);
            addNode(to);
            adjacencyList[from].insert(to);
            adjacencyList[to].insert(from);
        }
        template <typename FunctionT>
        void bfs(const T &start, FunctionT f)
        {
            std::unordered_set<T> visited;
            std::queue<T> q;
            q.push(start);
            visited.insert(start);

            while (!q.empty())
            {
                T node = q.front();
                q.pop();
                f(node);

                for (const T &neighbor : adjacencyList[node])
                {
                    if (visited.find(neighbor) == visited.end())
                    {
                        visited.insert(neighbor);
                        q.push(neighbor);
                    }
                }
            }
        }
        template <typename FunctionT>
        void dfs(const T &start, FunctionT f)
        {
            std::unordered_set<T> visited;
            std::stack<T> s;
            s.push(start);
            visited.insert(start);

            while (!s.empty())
            {
                T node = s.top();
                s.pop();
                f(node);

                for (const T &neighbor : adjacencyList[node])
                {
                    if (visited.find(neighbor) == visited.end())
                    {
                        visited.insert(neighbor);
                        s.push(neighbor);
                    }
                }
            }
        }

        bool containsNode(const T &value) const
        {
            return adjacencyList.find(value) != adjacencyList.end();
        }

        bool containsEdge(const T &from, const T &to) const
        {
            if (adjacencyList.find(from) != adjacencyList.end())
            {
                return adjacencyList.at(from).find(to) != adjacencyList.at(from).end();
            }
            return false;
        }

    private:
        std::unordered_map<T, std::unordered_set<T>> adjacencyList;
    };
}

template <>
struct std::hash<gMath::tVector>
{
    std::size_t operator()(const gMath::tVector &vec) const
    {
        std::size_t hx = std::hash<int>()(vec.x);
        std::size_t hy = std::hash<int>()(vec.y);

        // 选择一个大的常数进行混合
        return hx ^ (hy * 31);
    }
};

#endif
