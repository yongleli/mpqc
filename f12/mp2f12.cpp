//
// Created by Chong Peng on 3/31/16.
//

#include "mp2f12.h"
#include "f12_utility.h"
#include "../utility/cc_utility.h"

void mpqc::f12::MP2F12::compute_mp2_f12_c() {

    auto& world = mo_int_.get_world();

    auto occ = tre_->get_occ();

    // create shape
    auto occ_tr1 = tre_->get_occ_tr1();
    TiledArray::TiledRange occ4_trange({occ_tr1,occ_tr1,occ_tr1,occ_tr1});
    auto ijij_ijji_shape = f12::make_ijij_ijji_shape(occ4_trange);

    //compute V term
    TArray V_ijij_ijji;
    {
        utility::print_par(world, "Compute V_ijij_ijji \n" );
        V_ijij_ijji("i1,j1,i2,j2") = (mo_integral(L"(Κ |GR|i2 i1)")*mo_integral(L"(Κ|GR|Λ)[inv]")*mo_integral(L"(Λ |GR|j1 j2)")).set_shape(ijij_ijji_shape);
//        std::cout << V_ijij_ijji << std::endl;
        V_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(i1 p|G|j1 q)[df]")*mo_integral(L"(i2 p|R|j2 q)[df]")).set_shape(ijij_ijji_shape);
//        std::cout << V_ijij_ijji << std::endl;
        V_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(i1 m|G|j1 a')[df]")*mo_integral(L"(m i2|R|a' j2)[df]")).set_shape(ijij_ijji_shape);
//        std::cout << V_ijij_ijji << std::endl;
        V_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(j1 m|G|i1 a')[df]")*mo_integral(L"(m j2|R|a' i2)[df]")).set_shape(ijij_ijji_shape);
//        std::cout << V_ijij_ijji << std::endl;
    }

    // compute C term
    TArray C_iajb;
    {
        utility::print_par(world, "Compute C_iajb \n" );
        C_iajb("i,a,j,b") = mo_integral(L"(i a|R|j a')[df]")*mo_integral(L"(b|F|a')[df]");
        C_iajb("i,a,j,b") += mo_integral(L"(i a'|R|j b)[df]")*mo_integral(L"(a|F|a')[df]");
    }

    TArray t2;
    {
        utility::print_par(world, "Compute T_abij\n" );

        TArray g_iajb;
        g_iajb = mo_int_.compute(L"(i a|G|j b)[df]");
        g_iajb("a,b,i,j") = g_iajb("i,a,j,b");
        t2 = mpqc::cc::d_abij(g_iajb,orbital_energy_,occ);

    }

    {
        utility::print_par(world, "Compute V_bar_ijij_ijji\n" );
        V_ijij_ijji("i1,j1,i2,j2") = (V_ijij_ijji("i1,j1,i2,j2") + C_iajb("i1,a,j1,b")*t2("a,b,i2,j2")).set_shape(ijij_ijji_shape);
//        std::cout << "V_bar_ijij_ijji" << std::endl;
//        std::cout << V_ijij_ijji << std::endl;

    }


    // compute energy E21
    double E21 = 0.0;
    {
        // diagonal sum
        E21 = V_ijij_ijji("i1,j1,i2,j2").reduce(f12::DiagonalSum<TA::TensorD>());

        // off diagonal sum
        E21 += 0.5*(5*V_ijij_ijji("i1,j1,i2,j2")-V_ijij_ijji("i1,j1,j2,i2")).reduce(f12::OffDiagonalSum<TA::TensorD>());
        utility::print_par(world, "E21: ", E21, "\n");

    }

    // compute X term
    TArray X_ijij_ijji;
    {
        utility::print_par(world, "X_ijij_ijji \n" );
        X_ijij_ijji("i1,j1,i2,j2") = (mo_integral(L"(Κ |R2|i1 i2)")*mo_integral(L"(Κ|R2|Λ)[inv]")*mo_integral(L"(Λ |R2|j1 j2)")).set_shape(ijij_ijji_shape);
//        std::cout << X_ijij_ijji << std::endl;
        X_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(i1 p|R|j1 q)[df]")*mo_integral(L"(i2 p|R|j2 q)[df]")).set_shape(ijij_ijji_shape);
//        std::cout << X_ijij_ijji << std::endl;
        X_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(i1 m|R|j1 a')[df]")*mo_integral(L"(m i2|R|a' j2)[df]")).set_shape(ijij_ijji_shape);
//        std::cout << X_ijij_ijji << std::endl;
        X_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(j1 m|R|i1 a')[df]")*mo_integral(L"(m j2|R|a' i2)[df]")).set_shape(ijij_ijji_shape);
//        std::cout << X_ijij_ijji << std::endl;
    }

    // compute B term
    TArray B_ijij_ijji;
    {

        utility::print_par(world, "Compute B_ijij_ijji \n");

        B_ijij_ijji("i1,j1,i2,j2") = (mo_integral(L"(Κ |dR2|i1 i2)")*mo_integral(L"(Κ|dR2|Λ)[inv]")*mo_integral(L"(Λ |dR2|j1 j2)")).set_shape(ijij_ijji_shape);

//        std::cout << B_ijij_ijji << std::endl;
        auto hJ = mo_int_.compute(L"(P' | hJ | i)[df]");
        B_ijij_ijji("i1,j1,i2,j2") += (mo_integral(L"(i1 P'|R2|j1 j2)[df]")*hJ("P',i2")).set_shape(ijij_ijji_shape);
        B_ijij_ijji("i1,j1,i2,j2") += (mo_integral(L"(j1 P'|R2|i1 i2)[df]")*hJ("P',j2")).set_shape(ijij_ijji_shape);

//        std::cout << B_ijij_ijji << std::endl;

        B_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(i1 P'|R|j1 Q')[df]")*mo_integral(L"(P'|K|R')[df]")*mo_integral(L"(R' i2|R|Q' j2)[df]")).set_shape(ijij_ijji_shape);
        B_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(j1 P'|R|i1 Q')[df]")*mo_integral(L"(P'|K|R')[df]")*mo_integral(L"(R' j2|R|Q' i2)[df]")).set_shape(ijij_ijji_shape);

//        std::cout << B_ijij_ijji << std::endl;
        B_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(i1 P'|R|j1 m)[df]")*mo_integral(L"(P'|F|R')[df]")*mo_integral(L"(R' i2|R|m j2)[df]")).set_shape(ijij_ijji_shape);
        B_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(j1 P'|R|i1 m)[df]")*mo_integral(L"(P'|F|R')[df]")*mo_integral(L"(R' j2|R|m i2)[df]")).set_shape(ijij_ijji_shape);


//        std::cout << B_ijij_ijji << std::endl;
        B_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(i1 p|R|j1 a)[df]")*mo_integral(L"(p|F|r)[df]")*mo_integral(L"(i2 r|R|j2 a)[df]")).set_shape(ijij_ijji_shape);
        B_ijij_ijji("i1,j1,i2,j2") -= (mo_integral(L"(j1 p|R|i1 a)[df]")*mo_integral(L"(p|F|r)[df]")*mo_integral(L"(j2 r|R|i2 a)[df]")).set_shape(ijij_ijji_shape);


//        std::cout << B_ijij_ijji << std::endl;
        B_ijij_ijji("i1,j1,i2,j2") += (mo_integral(L"(i1 m|R|j1 b')[df]")*mo_integral(L"(m|F|n)[df]")*mo_integral(L"(n i2|R|b' j2)[df]")).set_shape(ijij_ijji_shape);
        B_ijij_ijji("i1,j1,i2,j2") += (mo_integral(L"(j1 m|R|i1 b')[df]")*mo_integral(L"(m|F|n)[df]")*mo_integral(L"(n j2|R|b' i2)[df]")).set_shape(ijij_ijji_shape);

//        std::cout << B_ijij_ijji << std::endl;

        B_ijij_ijji("i1,j1,i2,j2") -= (2.0*mo_integral(L"(i1 m|R|j1 b')[df]")*mo_integral(L"(m|F|P')[df]")*mo_integral(L"(P' i2|R|b' j2)[df]")).set_shape(ijij_ijji_shape);
        B_ijij_ijji("i1,j1,i2,j2") -= (2.0*mo_integral(L"(j1 m|R|i1 b')[df]")*mo_integral(L"(m|F|P')[df]")*mo_integral(L"(P' j2|R|b' i2)[df]")).set_shape(ijij_ijji_shape);

//        std::cout << B_ijij_ijji << std::endl;

        B_ijij_ijji("i1,j1,i2,j2") -= (2.0*mo_integral(L"(i1 p|R|j1 a)[df]")*mo_integral(L"(p|F|a')[df]")*mo_integral(L"(i2 a'|R|j2 a)[df]")).set_shape(ijij_ijji_shape);
        B_ijij_ijji("i1,j1,i2,j2") -= (2.0*mo_integral(L"(j1 p|R|i1 a)[df]")*mo_integral(L"(p|F|a')[df]")*mo_integral(L"(j2 a'|R|i2 a)[df]")).set_shape(ijij_ijji_shape);

//        std::cout << B_ijij_ijji << std::endl;
    }

    // compute B bar tem
    {

        utility::print_par(world, "Compute B_bar_ijij_ijji \n");

        auto Fij = mo_int_.compute(L"(i|F|j)[df]");
        auto Fij_eigen = array_ops::array_to_eigen(Fij);
        f12::convert_X_ijkl(X_ijij_ijji, Fij_eigen);

        auto C_bar_iajb = f12::convert_C_iajb(C_iajb, occ, orbital_energy_);

        B_ijij_ijji("i1,j1,i2,j2") = B_ijij_ijji("i1,j1,i2,j2") - X_ijij_ijji("i1,j1,i2,j2") + (C_iajb("i1,a,j1,b")*C_bar_iajb("i2,a,j2,b")).set_shape(ijij_ijji_shape);
//        std::cout << "B bar ijij_ijji Shape" << std::endl;
//        std::cout << B_ijij_ijji << std::endl;
    }

    double E22 = 0.0;
    {
        // diagonal sum
        E22 = 0.25*B_ijij_ijji("i1,j1,i2,j2").reduce(f12::DiagonalSum<TA::TensorD>());
        // off diagonal sum
        E22 += 0.0625 * (7 * B_ijij_ijji("i1,j1,i2,j2") + B_ijij_ijji("i1,j1,j2,i2")).reduce(f12::OffDiagonalSum<TA::TensorD>());
        utility::print_par(world, "E22: ", E22, "\n");
    }
    utility::print_par(world, "E_F12: ", E21+E22, "\n");
}
