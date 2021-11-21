// This file is part of MARTY.
// 
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.3
 
 * \brief
 */
#ifndef FEYNMANINTEGRAL_H_INCLUDED
#define FEYNMANINTEGRAL_H_INCLUDED

#include <csl.h>
#include <complex>
namespace mty {

enum class IntegralType {
    None, 
    A, 
    B, 
    C, 
    D, 
    E,
};


///////////////////////////////////////////////////
// Utility functions
///////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out,
                         IntegralType  type);

void printLooptoolsId(IntegralType type, 
                      int id, 
                      std::ostream& out);

void printLooptoolsId_A(int id, 
                        std::ostream& out);

void printLooptoolsId_B(int id, 
                        std::ostream& out);

void printLooptoolsId_C(int id, 
                        std::ostream& out);

void printLooptoolsId_D(int id, 
                        std::ostream& out);

void printLooptoolsId_E(int id, 
                        std::ostream& out);

inline constexpr int loopToolsStep = 3;

int loopToolsBegin(IntegralType type,
                   size_t       nIndices);

int loopToolsIdOf(
        IntegralType               type,
        std::vector<size_t> const &indices
        );

int loopToolsIdOf_A(
        std::vector<size_t> const &indices
        );

int loopToolsIdOf_B(
        std::vector<size_t> const &indices
        );

int loopToolsIdOf_C(
        std::vector<size_t> const &indices
        );

int loopToolsIdOf_D(
        std::vector<size_t> const &indices
        );

size_t nPropagatorForIntegral(IntegralType type);

size_t nIndicesForIntegral(IntegralType type);

size_t nArgumentsForIntegral(IntegralType type);

csl::Expr getDivergentFactor(IntegralType type,
                        int          integral_id,
                        std::vector<csl::Expr> const& arguments);

csl::Expr psquared(csl::Expr const &p);
csl::Expr psquared(csl::Expr const &p1, csl::Expr const &p2);
csl::Expr psum(csl::Expr const &p1, csl::Expr const &p2);

class FeynmanIntegral: public csl::AbstractMultiFunc {

    public:

    inline static bool evaluateIntegrals = false;
    // inline static const csl::Expr eps = csl::constant_s("reg_int");

    static csl::Expr replaceIntegral(csl::Expr const& expr);

    static csl::Expr replaceIntegral(csl::Expr        const& argument,
                                csl::Parent const& variable);

    static csl::Expr applyQSquared(
            csl::Index              const &squaredIndex,
            csl::Expr                    const &argument,
            csl::Parent             const &variable,
            std::vector<csl::Expr>       const &factor,
            std::vector<csl::Expr>       const &momentum,
            std::vector<csl::Expr>       const &mass,
            std::vector<csl::Index> const &indices,
            csl::Expr                    const &firstTerm
            );

    static void applyIndices(std::vector<csl::Expr>& momentum);

    static csl::Expr applyIndicesToTensors(
            csl::Space              const* space,
            std::vector<csl::Expr>       const& momentum,
            std::vector<csl::Index> const& indices,
            std::vector<size_t>            tensorPos);

    static csl::Expr computeFinalIntegralDecomposition(
            IntegralType                   type,
            std::vector<csl::Expr>       const& momentum,
            std::vector<csl::Index> const& indices,
            std::vector<csl::Expr>       const& arguments);

    static csl::Expr replaceIntegral(
            std::vector<csl::Expr> const& factors,
            std::vector<csl::Expr> const& momentums,
            std::vector<csl::Expr> const& masses,
            std::vector<csl::Index> const& indices);

    static csl::Expr replaceIntegral_A(
            std::vector<csl::Expr> const& factors,
            std::vector<csl::Expr> const& momentums,
            std::vector<csl::Expr> const& masses,
            std::vector<csl::Index> const& indices);

    static csl::Expr replaceIntegral_B(
            std::vector<csl::Expr> const& factors,
            std::vector<csl::Expr> const& momentums,
            std::vector<csl::Expr> const& masses,
            std::vector<csl::Index> const& indices);

    static csl::Expr replaceIntegral_C(
            std::vector<csl::Expr> const& factors,
            std::vector<csl::Expr> const& momentums,
            std::vector<csl::Expr> const& masses,
            std::vector<csl::Index> const& indices);

    static csl::Expr replaceIntegral_D(
            std::vector<csl::Expr> const& factors,
            std::vector<csl::Expr> const& momentums,
            std::vector<csl::Expr> const& masses,
            std::vector<csl::Index> const& indices);

    static csl::Expr replaceIntegral_E(
            std::vector<csl::Expr> const& factors,
            std::vector<csl::Expr> const& momentums,
            std::vector<csl::Expr> const& masses,
            std::vector<csl::Index> const& indices);

    static void removeExternalMomenta(
            csl::Expr &expr,
            csl::Parent const &Q
            );

    FeynmanIntegral(IntegralType             t_type,
                    int                      t_looptoolsId,
                    std::vector<csl::Expr> const& t_argument);

    FeynmanIntegral(IntegralType               t_type,
                    int                        t_looptoolsId,
                    std::vector<csl::Expr>   const &t_argument,
                    std::vector<size_t> const &t_indices);

    ~FeynmanIntegral() override {}

    int getLoopToolsID() const {
        return loopToolsId;
    }

    IntegralType getIntegralType() const {
        return type;
    }

    std::vector<size_t> const &getIntegralIndices() const {
        return integralIndices;
    }

    bool isIndexed() const override { return false; }

    void print(int mode = 0,
               std::ostream& out = std::cout,
               bool lib = false) const override;

    std::string printLaTeX(int mode = 0) const override;

    void printLib(int mode,
                  std::ostream& out) const;

    csl::LibDependency getLibDependency() const override;

    std::optional<csl::Expr> evaluate(csl::eval::mode user_mode
            = csl::eval::base) const override;

    csl::unique_Expr copy_unique() const override;

    csl::Expr deepCopy() const override;

    csl::Expr deepRefresh() const override;

    bool operator==(csl::Expr_info other) const override;

    bool operator<(csl::Expr_info other) const override;

    csl::Expr getDivergentFactor() const;
        
    std::vector<csl::Expr> getMomenta() const;

    std::vector<csl::Expr> getMasses() const;

    private:

    std::pair<csl::Expr, csl::Expr> getPair(size_t i) const;

    bool compare(size_t i, size_t j) const;

    void sortArgument();

    static csl::Expr metricTerm(
            csl::Space              const *space,
            std::vector<csl::Index> const &indices
            );

    std::complex<double> evaluateIntegral() const;

    private:

    IntegralType type;

    int loopToolsId;

    std::vector<size_t> integralIndices;
};

csl::Expr feynmanintegral_s(IntegralType             t_type,
                       int                      t_looptoolsId,
                       std::vector<csl::Expr> const& t_argument);

csl::Expr feynmanintegral_s(IntegralType               t_type,
                       int                        t_looptoolsId,
                       std::vector<csl::Expr>   const &t_argument,
                       std::vector<size_t> const &indices);

template<class IntegralID>
csl::Expr feynmanintegral_s(IntegralType             t_type,
                       IntegralID               t_looptoolsId,
                       std::vector<csl::Expr> const& t_argument)
{
    return feynmanintegral_s(t_type,
                             static_cast<int>(t_looptoolsId), 
                             t_argument);
}

enum class IntegralID_E {
    ee0 = 0,
    ee1 = 3,
    ee2 = 6,
    ee3 = 9,
    ee4 = 12,

    ee00 = 15,
    ee11 = 18,
    ee12 = 21,
    ee13 = 24,
    ee14 = 27,
    ee22 = 30,
    ee23 = 33,
    ee24 = 36,
    ee33 = 39,
    ee34 = 42,
    ee44 = 45,

    ee001 = 48,
    ee002 = 51,
    ee003 = 54,
    ee004 = 57,
    ee111 = 60,
    ee112 = 63,
    ee113 = 66,
    ee114 = 69,
    ee122 = 72,
    ee123 = 75,
    ee124 = 78,
    ee133 = 81,
    ee134 = 84,
    ee144 = 87,
    ee222 = 90,
    ee223 = 93,
    ee224 = 96,
    ee233 = 99,
    ee234 = 102,
    ee244 = 105,
    ee333 = 108,
    ee334 = 111,
    ee344 = 114,
    ee444 = 117,
  
    ee0000 = 120,
    ee0011 = 123,
    ee0012 = 126,
    ee0013 = 129,
    ee0014 = 132,
    ee0022 = 135,
    ee0023 = 138,
    ee0024 = 141,
    ee0033 = 144,
    ee0034 = 147,
    ee0044 = 150,
    ee1111 = 153,
    ee1112 = 156,
    ee1113 = 159,
    ee1114 = 162,
    ee1122 = 165,
    ee1123 = 168,
    ee1124 = 171,
    ee1133 = 174,
    ee1134 = 177,
    ee1144 = 180,
    ee1222 = 183,
    ee1223 = 186,
    ee1224 = 189,
    ee1233 = 192,
    ee1234 = 195,
    ee1244 = 198,
    ee1333 = 201,
    ee1334 = 204,
    ee1344 = 207,
    ee1444 = 210,
    ee2222 = 213,
    ee2223 = 216,
    ee2224 = 219,
    ee2233 = 222,
    ee2234 = 225,
    ee2244 = 228,
    ee2333 = 231,
    ee2334 = 234,
    ee2344 = 237,
    ee2444 = 240,
    ee3333 = 243,
    ee3334 = 246,
    ee3344 = 249,
    ee3444 = 252,
    ee4444 = 255,
  
    ee00001 = 258,
    ee00002 = 261,
    ee00003 = 264,
    ee00004 = 267,
    ee00111 = 270,
    ee00112 = 273,
    ee00113 = 276,
    ee00114 = 279,
    ee00122 = 282,
    ee00123 = 285,
    ee00124 = 288,
    ee00133 = 291,
    ee00134 = 294,
    ee00144 = 297,
    ee00222 = 300,
    ee00223 = 303,
    ee00224 = 306,
    ee00233 = 309,
    ee00234 = 312,
    ee00244 = 315,
    ee00333 = 318,
    ee00334 = 321,
    ee00344 = 324,
    ee00444 = 327,
    ee11111 = 330,
    ee11112 = 333,
    ee11113 = 336,
    ee11114 = 339,
    ee11122 = 342,
    ee11123 = 345,
    ee11124 = 348,
    ee11133 = 351,
    ee11134 = 354,
    ee11144 = 357,
    ee11222 = 360,
    ee11223 = 363,
    ee11224 = 366,
    ee11233 = 369,
    ee11234 = 372,
    ee11244 = 375,
    ee11333 = 378,
    ee11334 = 381,
    ee11344 = 384,
    ee11444 = 387,
    ee12222 = 390,
    ee12223 = 393,
    ee12224 = 396,
    ee12233 = 399,
    ee12234 = 402,
    ee12244 = 405,
    ee12333 = 408,
    ee12334 = 411,
    ee12344 = 414,
    ee12444 = 417,
    ee13333 = 420,
    ee13334 = 423,
    ee13344 = 426,
    ee13444 = 429,
    ee14444 = 432,
    ee22222 = 435,
    ee22223 = 438,
    ee22224 = 441,
    ee22233 = 444,
    ee22234 = 447,
    ee22244 = 450,
    ee22333 = 453,
    ee22334 = 456,
    ee22344 = 459,
    ee22444 = 462,
    ee23333 = 465,
    ee23334 = 468,
    ee23344 = 471,
    ee23444 = 474,
    ee24444 = 477,
    ee33333 = 480,
    ee33334 = 483,
    ee33344 = 486,
    ee33444 = 489,
    ee34444 = 492,
    ee44444 = 495,
    eeLAST  = 498
};

enum class IntegralID_D {
    dd0 = 0,
    dd1 = 3,
    dd2 = 6,
    dd3 = 9,

    dd00 = 12,
    dd11 = 15,
    dd12 = 18,
    dd13 = 21,
    dd22 = 24,
    dd23 = 27,
    dd33 = 30,

    dd001 = 33,
    dd002 = 36,
    dd003 = 39,
    dd111 = 42,
    dd112 = 45,
    dd113 = 48,
    dd122 = 51,
    dd123 = 54,
    dd133 = 57,
    dd222 = 60,
    dd223 = 63,
    dd233 = 66,
    dd333 = 69,

    dd0000 = 72,
    dd0011 = 75,
    dd0012 = 78,
    dd0013 = 81,
    dd0022 = 84,
    dd0023 = 87,
    dd0033 = 90,
    dd1111 = 93,
    dd1112 = 96,
    dd1113 = 99,
    dd1122 = 102,
    dd1123 = 105,
    dd1133 = 108,
    dd1222 = 111,
    dd1223 = 114,
    dd1233 = 117,
    dd1333 = 120,
    dd2222 = 123,
    dd2223 = 126,
    dd2233 = 129,
    dd2333 = 132,
    dd3333 = 135,

    dd00001 = 138,
    dd00002 = 141,
    dd00003 = 144,
    dd00111 = 147,
    dd00112 = 150,
    dd00113 = 153,
    dd00122 = 156,
    dd00123 = 159,
    dd00133 = 162,
    dd00222 = 165,
    dd00223 = 168,
    dd00233 = 171,
    dd00333 = 174,
    dd11111 = 177,
    dd11112 = 180,
    dd11113 = 183,
    dd11122 = 186,
    dd11123 = 189,
    dd11133 = 192,
    dd11222 = 195,
    dd11223 = 198,
    dd11233 = 201,
    dd11333 = 204,
    dd12222 = 207,
    dd12223 = 210,
    dd12233 = 213,
    dd12333 = 216,
    dd13333 = 219,
    dd22222 = 222,
    dd22223 = 225,
    dd22233 = 228,
    dd22333 = 231,
    dd23333 = 234,
    dd33333 = 237,
    ddLAST = 240
};

enum class IntegralID_C {
    cc0 = 0,
    cc1 = 3,
    cc2 = 6,

    cc00 = 9,
    cc11 = 12,
    cc12 = 15,
    cc22 = 18,

    cc001 = 21,
    cc002 = 24,
    cc111 = 27,
    cc112 = 30,
    cc122 = 33,
    cc222 = 36,

    cc0000 = 39,
    cc0011 = 42,
    cc0012 = 45,
    cc0022 = 48,
    cc1111 = 51,
    cc1112 = 54,
    cc1122 = 57,
    cc1222 = 60,
    cc2222 = 63,
    ccLAST = 66
};

enum class IntegralID_B{
    bb0 = 0,
    bb1 = 3,

    bb00 = 6,
    bb11 = 9,

    bb001 = 12,
    bb111 = 15,
    bbLAST = 18
};

enum class IntegralID_A {
    aa0 = 0,

    aa00 = 3,
    aaLAST = 6
};

}

#endif
