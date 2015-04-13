#include "../include/tiledarray.h"

#include "../common/namespaces.h"
#include "../common/typedefs.h"

#include "../utility/parallel_break_point.h"

#include "../tensor/decomposed_tensor_nonintrusive_interface.h"
#include "../tensor/tcc_tile.h"
#include "../tensor/density_tensor.h"
#include "../utility/time.h"

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

using namespace tcc;
using namespace tcc::tensor;
using namespace tcc_time;

TA::Tensor<double> random_matrix(std::size_t dim) {
    TA::Range r{dim, dim};
    TA::Tensor<double> M(r);
    auto map_M = TA::eigen_map(M, dim, dim);
    map_M = TA::EigenMatrixXd::Random(dim, dim);

    return M;
}

TA::Tensor<double>
lr_ta_tensor(std::size_t df_dim, std::size_t bs_dim, std::size_t rank) {
    TA::Range r{df_dim, bs_dim, bs_dim};
    TA::Tensor<double> M(r);
    auto L = TA::EigenMatrixXd::Random(df_dim, rank);
    auto R = TA::EigenMatrixXd::Random(rank, bs_dim * bs_dim);
    auto map_M = TA::eigen_map(M, df_dim, bs_dim * bs_dim);
    map_M = L * R;

    return M;
}

class LowRankTensors {
    std::vector<DecomposedTensor<double>> low_rank_;
    std::vector<DecomposedTensor<double>> full_rank_;
    DecomposedTensor<double> density_matrix_;
    std::vector<DecomposedTensor<double>> almost_full_;
    std::vector<DecomposedTensor<double>> full_almost_full_;
    TA::math::GemmHelper gh;
    static constexpr madness::cblas::CBLAS_TRANSPOSE NoT
          = madness::cblas::CBLAS_TRANSPOSE::NoTrans;

  public:
    LowRankTensors(std::size_t df_dim, std::size_t bs_dim,
                   std::vector<std::size_t> const &ranks)
            : low_rank_{},
              full_rank_{},
              density_matrix_(1e-7, random_matrix(bs_dim)),
              almost_full_(2),
              gh(NoT, NoT, 3, 3, 2) {
        const auto size = ranks.size();
        low_rank_.reserve(size);
        full_rank_.reserve(size);

        // Store the desired tensors
        auto max_rank = 0.5 * std::min(df_dim, bs_dim * bs_dim);
        for (auto rank : ranks) {
            if (rank > max_rank) {
                throw;
            }

            full_rank_.emplace_back(1e-7, lr_ta_tensor(df_dim, bs_dim, rank));
            low_rank_.push_back(
                  algebra::two_way_decomposition(full_rank_.back()));
        }

        // Do this twice
        full_almost_full_.emplace_back(1e-7, lr_ta_tensor(df_dim, bs_dim,
                                                          max_rank - 1));
        almost_full_.push_back(
              algebra::two_way_decomposition(full_almost_full_.back()));
        full_almost_full_.emplace_back(1e-7, lr_ta_tensor(df_dim, bs_dim,
                                                          max_rank - 1));
        almost_full_.push_back(
              algebra::two_way_decomposition(full_almost_full_.back()));
    }

    std::size_t Ntensors() const { return low_rank_.size(); }
    DecomposedTensor<double> low_rank(std::size_t n) const {
        return clone(low_rank_[n]);
    }
    DecomposedTensor<double> full_rank(std::size_t n) const {
        return clone(full_rank_[n]);
    }
    DecomposedTensor<double> almost_full_rank(std::size_t n) const {
        return clone(almost_full_[n]);
    }
    DecomposedTensor<double> full_almost_full_rank(std::size_t n) const {
        return clone(full_almost_full_[n]);
    }
    DecomposedTensor<double> density() const { return clone(density_matrix_); }
    TA::math::GemmHelper gemm_helper() const { return gh; }
};

void gemm_test(LowRankTensors const &l) {
    const auto size = l.Ntensors();
    auto D = l.density();
    auto D_tensor = D.tensor(0).clone();

    auto gh = l.gemm_helper();
    std::vector<double> ta_times(l.Ntensors());
    std::vector<double> full_times(l.Ntensors());
    std::vector<double> low_times(l.Ntensors());
    std::vector<double> full_accuracy(l.Ntensors());
    std::vector<double> low_accuracy(l.Ntensors());
    for (auto i = 0ul; i < size; ++i) {
        auto full = l.full_rank(i);
        auto low = l.low_rank(i);
        auto ta_tensor = full.tensor(0).clone();

        // Dummy to load functions
        auto dummy = ta_tensor.gemm(D_tensor, 2.0, gh);
        auto time_ta0 = now();
        auto correct_answer = ta_tensor.gemm(D_tensor, 1.0, gh);
        auto time_ta1 = now();
        ta_times[i] = duration_in_s(time_ta0, time_ta1);

        auto time_full0 = now();
        auto my_full = gemm(full, D, 1.0, gh);
        auto time_full1 = now();
        full_times[i] = duration_in_s(time_full0, time_full1);
        full_accuracy[i] = correct_answer.subt(algebra::combine(my_full)).norm();

        auto my_lr = gemm(low, D, 1.0, gh);
        low_accuracy[i] = correct_answer.subt(algebra::combine(my_lr)).norm();
        auto total_time_low = 0.0;
        for (auto j = 0ul; j < size; ++j) {
            auto time_low0 = now();
            auto my_lr2 = gemm(low, D, 1.0, gh);
            auto time_low1 = now();
            total_time_low += duration_in_s(time_low0, time_low1);
        }
        low_times[i] = total_time_low / double(size);
    }

    auto average_time_ta = std::accumulate(ta_times.begin(), ta_times.end(),
                                           0.0) / double(size);
    auto average_time_full
          = std::accumulate(full_times.begin(), full_times.end(), 0.0)
            / double(size);
    auto average_accuracy_full
          = std::accumulate(full_accuracy.begin(), full_accuracy.end(), 0.0)
            / double(size);

    auto const &extent = l.full_rank(0).tensor(0).range().size();
    auto full_rank = std::min(extent[0], extent[1] * extent[2]);
    std::cout << "Gemm Test: DF dimension " << extent[0] << ", BS dimension "
              << extent[1] << "\n";
    std::cout << "\tTA::Tensor average time " << average_time_ta
              << " s, DecomposedTensor Full average time " << average_time_full
              << " s, Full average accuracy " << average_accuracy_full << "\n";

    for (auto i = 0ul; i < size; ++i) {
        const auto rank = l.low_rank(i).rank();
        std::cout << "\t\tLow rank " << rank << ", rank percent of full "
                  << 100 * double(rank) / double(full_rank) << ", average time "
                  << low_times[i] << " s, speed up is "
                  << average_time_ta / low_times[i] << ", accuracy "
                  << low_accuracy[i] << "\n";
    }
}

void gemm_to_full_full_test(LowRankTensors const &l) {
    const auto size = l.Ntensors();

    auto my_c = l.full_rank(0);
    auto ta_c = my_c.tensor(0).clone();
    auto D = l.density();
    auto ta_d = D.tensor(0).clone();
    auto gh = l.gemm_helper();
    std::vector<double> ta_time;
    std::vector<double> my_time;
    std::vector<double> my_accuracy;
    for (auto i = 1ul; i < size; ++i) {
        auto my_c_i = clone(my_c);
        auto ta_c_i = ta_c.clone();

        auto my_a = l.full_rank(i);
        auto ta_a = my_a.tensor(0).clone();

        auto time_ta0 = now();
        ta_c_i.gemm(ta_a, ta_d, 1.0, gh);
        auto time_ta1 = now();
        ta_time.push_back(duration_in_s(time_ta0, time_ta1));

        auto time_me0 = now();
        gemm(my_c_i, my_a, D, 1.0, gh);
        auto time_me1 = now();
        my_time.push_back(duration_in_s(time_me0, time_me1));

        my_accuracy.push_back(ta_c_i.subt(algebra::combine(my_c_i)).norm());
    }

    auto avg_ta_time = std::accumulate(ta_time.begin(), ta_time.end(), 0.0)
                       / double(size - 1);
    auto avg_me_time = std::accumulate(my_time.begin(), my_time.end(), 0.0)
                       / double(size - 1);
    auto avg_accur = std::accumulate(my_accuracy.begin(), my_accuracy.end(),
                                     0.0) / double(size - 1);

    std::cout << "\tFull Full gemm_to ta average time = " << avg_ta_time
              << " s, my average time " << avg_me_time << " s, my avg accuracy "
              << avg_accur << "\n";
}

void gemm_to_full_low_test(LowRankTensors const &l) {
    const auto size = l.Ntensors();

    auto my_c = l.full_rank(0);
    auto ta_c = my_c.tensor(0).clone();
    auto D = l.density();
    auto ta_d = D.tensor(0).clone();
    auto gh = l.gemm_helper();

    std::vector<double> ta_time;
    std::vector<double> my_time;
    std::vector<double> my_accuracy;

    for (auto i = 0ul; i < size; ++i) {
        auto my_c_i = clone(my_c);
        auto ta_c_i = ta_c.clone();

        auto my_a = l.low_rank(i);
        auto ta_a = algebra::combine(my_a);

        auto time_ta0 = now();
        ta_c_i.gemm(ta_a, ta_d, 1.0, gh);
        auto time_ta1 = now();
        ta_time.push_back(duration_in_s(time_ta0, time_ta1));

        auto total_time = 0.0;
        for (auto j = 0ul; j < size; ++j) {
            auto my_c_ij = clone(my_c_i);
            auto time_me0 = now();
            gemm(my_c_ij, my_a, D, 1.0, gh);
            auto time_me1 = now();
            total_time += duration_in_s(time_me0, time_me1);
        }
        my_time.push_back(total_time / double(size));

        // For accuracy reasons.
        gemm(my_c_i, my_a, D, 1.0, gh);
        my_accuracy.push_back(ta_c_i.subt(algebra::combine(my_c_i)).norm());
    }

    auto avg_ta_time = std::accumulate(ta_time.begin(), ta_time.end(), 0.0)
                       / double(size - 1);

    std::cout << "\tFull Low gemm_to ta average time = " << avg_ta_time
              << " s\n";

    auto const &extent = l.full_rank(0).tensor(0).range().size();
    auto full_rank = std::min(extent[0], extent[1] * extent[2]);
    for (auto i = 0ul; i < size; ++i) {
        auto rank = l.low_rank(i).rank();
        std::cout << "\t\trank " << rank << ", rank percent "
                  << 100 * double(rank) / double(full_rank) << ", my avg time "
                  << my_time[i] << ", speed up " << avg_ta_time / my_time[i]
                  << ", my accuracy " << my_accuracy[i] << "\n";
    }
}

void gemm_to_low_full_test(LowRankTensors const &l) {
    const auto size = l.Ntensors();

    auto my_c = l.low_rank(0);
    auto ta_c = algebra::combine(my_c);
    auto D = l.density();
    auto ta_d = D.tensor(0).clone();
    auto gh = l.gemm_helper();

    std::vector<double> ta_time;
    std::vector<double> my_time;
    std::vector<double> my_accuracy;

    for (auto i = 0ul; i < size; ++i) {
        auto my_c_i = clone(my_c);
        auto ta_c_i = ta_c.clone();

        auto my_a = l.full_rank(i);
        auto ta_a = my_a.tensor(0);

        auto time_ta0 = now();
        ta_c_i.gemm(ta_a, ta_d, 1.0, gh);
        auto time_ta1 = now();
        ta_time.push_back(duration_in_s(time_ta0, time_ta1));

        auto total_time = 0.0;
        for (auto j = 0ul; j < size; ++j) {
            auto my_c_ij = clone(my_c_i);
            auto time_me0 = now();
            gemm(my_c_ij, my_a, D, 1.0, gh);
            auto time_me1 = now();
            total_time += duration_in_s(time_me0, time_me1);
        }
        my_time.push_back(total_time / double(size));

        // For accuracy reasons.
        gemm(my_c_i, my_a, D, 1.0, gh);
        my_accuracy.push_back(ta_c_i.subt(algebra::combine(my_c_i)).norm());
    }

    auto avg_ta_time = std::accumulate(ta_time.begin(), ta_time.end(), 0.0)
                       / double(size);

    std::cout << "\tLow Full gemm_to ta average time = " << avg_ta_time
              << " s\n";

    auto const &extent = l.full_rank(0).tensor(0).range().size();
    auto full_rank = std::min(extent[0], extent[1] * extent[2]);
    for (auto i = 0ul; i < size; ++i) {
        auto rank = l.low_rank(i).rank();
        std::cout << "\t\trank " << rank << ", rank percent "
                  << 100 * double(rank) / double(full_rank) << ", my avg time "
                  << my_time[i] << ", speed up " << avg_ta_time / my_time[i]
                  << ", my accuracy " << my_accuracy[i] << "\n";
    }
}

void gemm_to_low_low_test(LowRankTensors const &l) {
    const auto size = l.Ntensors();
    auto D = l.density();
    auto ta_d = algebra::combine(D);
    auto gh = l.gemm_helper();

    std::vector<double> ta_time;
    std::vector<double> my_time;
    std::vector<double> my_accuracy;

    auto counter = 0ul;
    for (auto i = 0ul; i < size; ++i) {
        for (auto j = 0ul; j <= i; ++j, ++counter) {
            auto my_c_i = l.low_rank(i);
            auto ta_c_i = algebra::combine(my_c_i);

            auto my_a = l.low_rank(j);
            auto ta_a = algebra::combine(my_a);

            auto time_ta0 = now();
            ta_c_i.gemm(ta_a, ta_d, 1.0, gh);
            auto time_ta1 = now();
            ta_time.push_back(duration_in_s(time_ta0, time_ta1));

            auto time_me0 = now();
            gemm(my_c_i, my_a, D, 1.0, gh);
            auto time_me1 = now();
            my_time.push_back(duration_in_s(time_me0, time_me1));

            // For accuracy reasons.
            my_accuracy.push_back(ta_c_i.subt(algebra::combine(my_c_i)).norm());
        }
    }

    auto avg_ta_time = std::accumulate(ta_time.begin(), ta_time.end(), 0.0)
                       / double(counter);

    std::cout << "\tLow Low gemm_to ta average time = " << avg_ta_time
              << " s\n";

    auto const &extent = l.full_rank(0).tensor(0).range().size();
    auto full_rank = std::min(extent[0], extent[1] * extent[2]);
    counter = 0;
    for (auto i = 0ul; i < size; ++i) {
        for (auto j = 0ul; j <= i; ++j, ++counter) {
            auto rank_i = l.low_rank(i).rank();
            auto rank_j = l.low_rank(j).rank();
            std::cout << "\t\trank c " << rank_i << ", percent "
                      << 100 * double(rank_i) / double(full_rank) << ", rank a "
                      << rank_j << ",  percent "
                      << 100 * double(rank_j) / double(full_rank)
                      << ", my time " << my_time[counter] << ", speed up "
                      << avg_ta_time / my_time[counter] << ", my accuracy "
                      << my_accuracy[counter] << "\n";
        }
    }
}

void gemm_to_test(LowRankTensors const &l) {
    auto const &extent = l.full_rank(0).tensor(0).range().size();
    std::cout << "Gemm To DF dimension " << extent[0] << " BS dimension "
              << extent[1] << "\n";
    gemm_to_full_full_test(l);
    gemm_to_full_low_test(l);
    gemm_to_low_full_test(l);
    gemm_to_low_low_test(l);
}

int main(int argc, char **argv) {
    auto df_dim = std::stoul(argv[1]);
    auto bs_dim = std::stoul(argv[2]);
    auto max_allowed_rank = std::min(df_dim, bs_dim * bs_dim);
    auto max_rank = std::min(max_allowed_rank, std::stoul(argv[3]));
    auto rank_step = std::stoul(argv[4]);

    std::vector<decltype(rank_step)> ranks;
    for (auto i = 1ul; i < max_rank; i += rank_step) {
        ranks.push_back(i);
    }

    LowRankTensors tensors(df_dim, bs_dim, ranks);

    gemm_test(tensors);
    gemm_to_test(tensors);
    return 0;
}
