#include "gg_to_tt.h"
#include <iomanip>
#include <fstream>

using namespace std;
using namespace gg_to_tt;

// Process gg -> t tbar
//
// / E/2 \     / E/2 \     / E/2 \   / E/2 \   
// |  0  |     |  0  |     | px  |   | -px |
// |  0  |  +  |  0  |  =  | py  | + | -py |
// \ E/2 /     \-E/2 /     \ pz /    \ -pz /  
//
// E in Center of mass energy
//
// (E/2)^2 - p^2 = mt^2  
//  -->  p^2 = E^2/4 - mt^2
//  Define s === E^2 Mandelstaam variable
//  Define K === sqrt(1 - 4*mt^2/s)
//  --> p = sqrt(s)/2 * K
//
// s-type kinematical variables
// s12 = p1.p2               = s/2
// s34 = p3.p4 = E^4/4 + p^2 = s/2 - mt^2
//
// t-type kinematical variables
// s13 = p1.p3 = E^2/4 - E*p*cos(theta)/2
//             = s/4 ( 1 - K*cos(theta) )
// s24 = p2.p4 = s13
//
// u-type kinematical variables
// s14 = p1.p4 = E^2/4 + E*p*cos(theta)/2
//             = s/4 ( 1 + K*cos(theta) )
// s23 = p2.p3 = s14

// PDG 2020 p. 259
double dsigmaPDG(
        double as,
        double mQ,
        double s,
        double t,
        double u
        )
{
    double mQ2 = mQ*mQ;
    double L = (mQ2 - t) * (mQ2 - u);
    return as*as / (32*s) * sqrt(1 - 4*mQ2/s) * (
            6*L/(s*s)
            -mQ2*(s-4*mQ2)/(3*L)
            +4./3*(L - 2*mQ2*(mQ2+t)) / pow(mQ2-t, 2)
            +4./3*(L - 2*mQ2*(mQ2+u)) / pow(mQ2-u, 2)
            -3*(L + mQ2*(u - t)) / (s*(mQ2 - t))
            -3*(L + mQ2*(t - u)) / (s*(mQ2 - u))
        );
}

int main() {

    // Fixed parameters
    double mt = 172.76;//172; // PDG 2020 direct measurements
    double alphas_mZ = 0.1179; // PDG 2020
    int prec = 5; // significative digits

    std::ofstream fout("data.txt");
    fout << left;
    for (double E_CM = 400; E_CM < 5000; E_CM += 25) {
        for (double theta = M_PI/20; theta < M_PI; theta += M_PI/20) {
            // Definitions
            double ct = cos(theta);
            double gs = sqrt(4*M_PI*alphas_mZ);
            double mt2 = mt*mt;
            double s = E_CM*E_CM;
            double K = sqrt(1 - 4*mt2/s);

            // Kinematical variables
            double s12 = s/2;
            double s34 = s/2 - mt2;
            double s13 = s/4 * (1 - K*ct);
            double s24 = s13;
            double s14 = s/4 * (1 + K*ct);
            double s23 = s14;

            // t and u for PDG's formula
            double t = mt2 - 2*s13;
            double u = mt2 - 2*s14;

            param_t param;
            param.g_s = gs;
            param.m_t = mt;
            param.s_12 = s12;
            param.s_13 = s13;
            param.s_14 = s14;
            param.s_23 = s23;
            param.s_24 = s24;
            param.s_34 = s34;
            param.reg_prop = 0;

            double m2   = M2(param).real();
            double m2g  = M2_ghost (param).real();
            double m2gc = M2_ghostc(param).real();
            m2 = m2 - m2g - m2gc;

            double dsigmaPDG_value = dsigmaPDG(alphas_mZ, mt, s, t, u);

            double dsigma = K * m2 / (64 * M_PI*M_PI * s); // 1 / GeV^2
            
            double conversion = 2.56819e-9; // GeV-2 to pb
            dsigma          /= conversion; 
            dsigmaPDG_value /= conversion;
            fout << setw(prec + 8) << E_CM;
            fout << setw(prec + 5) << theta;
            fout << setw(prec + 10) << dsigma;
            fout << setw(prec + 10) << dsigmaPDG_value;
            fout << endl;
        }
    }
    fout.close();

    return 0;
}
