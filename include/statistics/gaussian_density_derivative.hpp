#ifndef GAUSSIAN_DENSITY_DERIVATIVE_HPP
#define GAUSSIAN_DENSITY_DERUVATIVE_HPP

#include <vector>

#include <gromacs/utility/real.h>


/*!
 *
 */
class GaussianDensityDerivative
{
    public:

        std::vector<real> estimateApprox(
                std::vector<real> &sample,
                std::vector<real> &eval);
        std::vector<real> estimateDirect(
                const std::vector<real> &sample,
                const std::vector<real> &eval);

        // setter functions:
        void setBandWidth(real bw);
        void setDerivOrder(unsigned int r);
        void setErrorBound(real eps);

//    private:

        // internal constants:
        const real SQRT2PI_ = std::sqrt(2.0*M_PI);

        // internal variables:
        real bw_;
        unsigned int r_;
        unsigned int rFac_;
        unsigned int trunc_;
        unsigned int numIntervals_;
        real ri_;
        real rc_;
        real q_;
        real eps_;
        real epsPrime_;

        std::vector<real> centres_;
        std::vector<unsigned int> idx_;
        std::vector<real> coefA_;
        std::vector<real> coefB_;



        // 
        real estimDirectAt(
                const std::vector<real> &sample,
                real eval);
        real estimApproxAt(
                const std::vector<real> &sample,
                real eval);

        // space partitioning:
        std::vector<real> setupClusterCentres();
        std::vector<unsigned int> setupClusterIndices(
                const std::vector<real> &sample);

        // calculation of coefficients:
        std::vector<real> setupCoefA();
        std::vector<real> setupCoefB();
        real setupCoefQ(unsigned int n);
        real setupCutoffRadius();
        real setupScaledTolerance(unsigned int n);
        unsigned int setupTruncationNumber();

        // internal utilities:
        unsigned int factorial(unsigned int n);
};

#endif

