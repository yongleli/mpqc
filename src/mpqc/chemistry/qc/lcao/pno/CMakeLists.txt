set(sources
    ccsd_pno.h
    ccsd_pno.cpp
    ccsd_pno_impl.h
    linkage.h
    )

add_mpqc_library(lcao_pno sources sources "MPQClcao_scf;MPQCcc" "mpqc/chemistry/qc/lcao/pno")
