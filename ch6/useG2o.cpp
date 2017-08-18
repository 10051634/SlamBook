#include<iostream>
#include <g2o/core/base_vertex.h>
#include <g2o/core/base_unary_edge.h>
#include <g2o/core/block_solver.h>
#include <g2o/core/optimization_algorithm_levenberg.h>
#include <g2o/core/optimization_algorithm_gauss_newton.h>
#include <g2o/core/optimization_algorithm_dogleg.h>
#include <g2o/solvers/dense/linear_solver_dense.h>
#include <Eigen/Core>
#include <opencv2/core/core.hpp>
#include <cmath>
#include <chrono>

using namespace std;

//曲线模型的顶点，参数：优化变量维度和数据类型
class CurveFittingVertex : public g2o::BaseVertex<3, Eigen::Vector3d> {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

//    重置
    virtual void setToOriginImpl() {
        _estimate << 0, 0, 0;
    }

//    更新
    virtual void oplusImpl(const double *update) {
        _estimate += Eigen::Vector3d(update);
    }

//    存盘和读盘：留空
    virtual bool read(istream &in) {}

    virtual bool write(ostream &out) const {}
};

//误差模型，参数：观测值维度，类型，连接顶点类型
class CurveFittingEdge : public g2o::BaseUnaryEdge<1, double, CurveFittingVertex> {
public:
//    x值，y值为_measurement
    double _x;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    CurveFittingEdge(double x) : BaseUnaryEdge(), _x{x} {}

//    计算曲线模型误差
    void computeError() {
        const CurveFittingVertex *v = static_cast<const CurveFittingVertex *>(_vertices[0]);
        const Eigen::Vector3d abc = v->estimate();
        _error(0, 0) = _measurement - exp(abc(0, 0) * _x * _x + abc(1, 0) * _x + abc(2, 0));
    }

    virtual bool read(istream &in) {}

    virtual bool write(ostream &out) const {}
};

/**
 * 本程序演示了g2o的使用
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv) {
//    真实参数值
    double a = 1.0, b = 2.0, c = 1.0;
//    数据点个数
    int N = 100;
//    噪声sigma值
    double w_sigma = 1.0;
//    OpenCV随机数产生器
    cv::RNG rng;
//    abc参数估计值
    double abc[3] = {0, 0, 0};
//    数据
    vector<double> x_data, y_data;

    cout << "生成数据：" << endl;
    for (int i = 0; i < N; ++i) {
        double x = i / 100.0;
        x_data.push_back(x);
        y_data.push_back(exp(a * x * x + b * x + c) + rng.gaussian(w_sigma));
        cout << x_data[i] << " " << y_data[i] << endl;
    }

    return 0;
}
