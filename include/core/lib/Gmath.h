#ifndef GAME_MATH_HELPER
#define GAME_MATH_HELPER
#include <cmath>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <queue>
#include <initializer_list>
#include <string>
#include "logger/logger.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace gMath
{
    
    /**
     * @brief Clamp a value between a minimum and maximum value.
     * 
     * @param value The value to clamp.
     * @param min The minimum value.
     * @param max The maximum value.
     * @return The clamped value.
     */
    inline double clamp(double value, double min, double max);
    using axisV = int;
    //角度
    class Angle;

    //二维图像到轴的投影
    struct Projection;
    // 二维向量
    class tVector
    {
    public:
        double x, y;
        tVector() : x(0), y(0) {}
        tVector(double a, double b) : x(a), y(b) {}
        tVector(axisV a, axisV b) : x(a), y(b) {}
        tVector(const tVector &r) : x(r.x), y(r.y) {}
        tVector(double len, const tVector &direction) : tVector(direction * (len / direction.sLen())) {}
        tVector(tVector&& r) = default;
        tVector& operator=(tVector &&r) = default;
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
        tVector getUnifiedV() const
        {
            double r = std::sqrt(sLen());
            return tVector(x / r, y / r);
        }
        tVector& operator=(const tVector &r)
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
        void rotate(const Angle &angle);
        tVector reverse() const
        {
            return tVector(-x, -y);
        }
        tVector operator*(double a) const
        {
            return tVector(x * a, y * a);
        }
        //产生的是3维的赝向量
        //用有正负的数表示方向
        double cross(const tVector &r) const{
            return x * r.y - y * r.x;
        }
        //和3维的赝向量叉乘
        tVector cross(const double &r) const{
            return tVector(y * r, -x * r);
        }
        tVector& operator*=(double a){
            x *= a;
            y *= a;
            return *this;
        }
        tVector keepDirectionWith(const tVector &r) const{
            if (dot(r) >= 0 ){
                return *this;
            }
            return this->reverse();
        }
        std::string log() const{
            return "(" + std::to_string(x) + "," + std::to_string(y) +")\n";
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
        Crdinate(const Crdinate &crd) : x(crd.x), y(crd.y) {}
        Crdinate(Crdinate &&v) = default;
        void set_x(axisV x_)
        {
            this->x = x_;
        }
        void set_y(axisV y_)
        {
            this->y = y_;
        }
        axisV get_x() const
        {
            return x;
        }
        axisV get_y() const
        {
            return y;
        }
        Crdinate& operator=(const Crdinate &crd)
        {
            x = crd.x;
            y = crd.y;
            return *this;
        }
        Crdinate& operator=(Crdinate &&v) = default;
        Crdinate operator+(const tVector &v) const
        {
            return Crdinate(x + (axisV)v.x, y + (axisV)v.y);
        }
        tVector operator-(const Crdinate &r) const
        {
            return tVector(x - r.x, y - r.y);
        }
        Crdinate operator-(const tVector &v) const{
            return Crdinate(x - (axisV)v.x, y - (axisV)v.y);
        }
        Crdinate &operator+=(const tVector &v)
        {
            x += static_cast<axisV>(v.x);
            y += static_cast<axisV>(v.y);
            return *this;
        }
        std::string log() const{
            return "(" + std::to_string(x) + "," + std::to_string(y) +")";
        }
    };
    class mRectangle
    {

    public:

        axisV l, r, t, b;
        mRectangle(axisV left, axisV right, axisV top, axisV bottom) : l(left), r(right), t(top), b(bottom) {}
        bool Inside(const Crdinate& crd) const
        {
            return crd.get_x() <= r && crd.get_y() <= t && crd.get_x() >= l && crd.get_y() >= b;
        }
        bool intersects(const mRectangle &rec) const
        {
            return l <= rec.r && r >= rec.l && t >= rec.b && b <= rec.t;
        }
        bool Inside(const Crdinate& crd, const std::vector<tVector>* vs) const ;
        mRectangle& operator=(const mRectangle&) = default;

        std::string log() const{
            return "{ \n left = " + std::to_string(l) + ",\n right = " + std::to_string(r) + ",\n top = " + std::to_string(t) + ",\n bootom = " + std::to_string(b) +"}\n";
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
            normalize();
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
        Angle &operator *=(double a){
            degrees *= a;
            radians *= a;
            normalize();
            return *this;
        }
        Angle &operator=(Angle&& r) = default;
        Angle &operator=(const Angle& r) = default;
        Angle(Angle&& r) = default;
        Angle(const Angle& r) = default;
        std::string log() const{return std::to_string(degrees) ;} 
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
            resetRadians();
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
        void resetRadians(){
            radians = degreesToRadians(degrees);
        }
        
    };
    // 无向图
    // bascially gpt-generated
    /*template <typename T>
    class Graph
    {
    public:
        Graph(std::initializer_list<std::initializer_list<T>>&& edges)
        {
            for (const auto &edge : edges)
            {
                addEdge(edge[0], edge[1]);
            }
        }
        Graph() = default;
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
        std::unordered_set<T>& operator[](const T& key){
            return adjacencyList[key];
        }
        void removeEdge(const T& from, const T& to){
            adjacencyList[from].erase(to);
            adjacencyList[to].erase(from);
        }
        
    private:
        std::unordered_map<T, std::unordered_set<T>> adjacencyList;
    };
    
    */
    template <typename T, typename EgdeT>
    class Graph
    {
    public:
        Graph() = default;
        void addNode(const T &value)
        {
            if (adjacencyList.find(value) == adjacencyList.end())
            {
                adjacencyList[value] = std::unordered_map<T, EgdeT>();
            }
        }

        void addEdge(const T &from, const T &to, const EgdeT &edge)
        {
            addNode(from);
            addNode(to);
            adjacencyList[from].emplace(to, edge);
            adjacencyList[to].emplace(from, edge);
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
        std::unordered_map<T,EgdeT>& operator[](const T& key){
            return adjacencyList[key];
        }

        void removeEdge(const T& from, const T& to){
            adjacencyList[from].erase(to);
            adjacencyList[to].erase(from);
        }
        
    private:
        std::unordered_map<T, std::unordered_map<T, EgdeT>> adjacencyList;
    };
    
    
    
    struct Projection
        {
            double low;
            const tVector toLowPoint;
             double high;
            const tVector toHighPoint;
            Projection() = default;
            Projection(const double l, const tVector&& tlp, const double h, const tVector&& thp) : low(l), toLowPoint(tlp), high(h), toHighPoint(thp) {}
            //只启用移动语义
            Projection(Projection&& p ) = default;
            Projection(const Projection& p ) = delete;
            Projection& operator=(Projection&& p ) = default;
            Projection& operator=(const Projection& p ) = delete;
            void addOffset(double off)
            {
                low += off;
                high += off;
            }
            std::string log() const{
                return "{ low = " + std::to_string(low) + ",\n toLowPoint = " + toLowPoint.log() + ",\n high = " + std::to_string(high) + ",\n toHighPoint = " + toHighPoint.log() +"}\n";
            }
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


double gMath::clamp(double value, double min, double max)
{
    return std::max(min, std::min(max, value));
}


inline void gMath::tVector::rotate(const Angle &angle)
        {
            double sinA = std::sin(angle.getRadians()), cosA = std::cos(angle.getRadians()), tx = x, ty = y;
            x = tx * cosA - ty * sinA;
            y = tx * sinA + ty * cosA;
        }
#endif
