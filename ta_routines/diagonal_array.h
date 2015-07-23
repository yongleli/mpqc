#pragma once
#ifndef TCC_PUREIFICATION_DIAGONALARRAY_H
#define TCC_PUREIFICATION_DIAGONALARRAY_H

#include "../include/tiledarray.h"
#include "../tensor/tcc_tile.h"
#include "../tensor/decomposed_tensor.h"

namespace tcc {
namespace array {

template <typename T>
void make_diagonal_tile(TiledArray::Tensor<T> &tile, T val) {
    auto const extent = tile.range().extent();
    auto map = TiledArray::eigen_map(tile, extent[0], extent[1]);
    for (auto i = 0ul; i < extent[0]; ++i) {
        map(i, i) = val;
    }
}

template <typename T>
void make_diagonal_tile(tensor::Tile<tensor::DecomposedTensor<T>> &tile,
                        T val) {
    assert(tile.tile().ndecomp() == 1);
    auto &tensor = tile.tile().tensor(0);
    auto const extent = tensor.range().extent();
    auto map = TiledArray::eigen_map(tensor, extent[0], extent[1]);
    for (auto i = 0ul; i < extent[0]; ++i) {
        map(i, i) = val;
    }
}

template <typename T, unsigned int N, typename Tile>
TiledArray::Array<T, N, Tile, TiledArray::SparsePolicy> create_diagonal_matrix(
      TiledArray::Array<T, N, Tile, TiledArray::SparsePolicy> const &model,
      double val) {

    using Array = TiledArray::Array<T, N, Tile, TiledArray::SparsePolicy>;

    TiledArray::Tensor<float> tile_shape(model.trange().tiles(), 0.0);

    auto pmap = model.get_pmap();

    auto pmap_end = pmap->end();
    for (auto it = pmap->begin(); it != pmap_end; ++it) {
        auto idx = model.trange().tiles().idx(*it);
        if (idx[0] == idx[1]) {
            tile_shape[*it] = val;
        }
    }

    TiledArray::SparseShape<float> shape(model.get_world(), tile_shape,
                                         model.trange());

    Array diag(model.get_world(), model.trange(), shape);

    auto const &trange = diag.trange();
    pmap = diag.get_pmap();
    auto end = pmap->end();
    for (auto it = pmap->begin(); it != end; ++it) {
        const auto ord = *it;

        auto idx = trange.tiles().idx(ord);
        auto diagonal_tile
              = std::all_of(idx.begin(), idx.end(),
                            [&](typename Array::size_type const &x) {
                  return x == idx.front();
              });

        using TileType = typename Array::value_type;
        if (diagonal_tile && !diag.is_zero(ord)) {
            auto tile = TileType(trange.make_tile_range(ord), 0.0);
            make_diagonal_tile(tile, val);
            diag.set(ord, std::move(tile));
        }
    }

    return diag;
}
/*!
 * \breif takes a TiledArray::TiledRange and a value and returns a diagonal
 *matrix.
 *
 * This function creates a diagonal matrix given a TiledArray::TiledRange and
 * a value.  The template parameters must be a numeric type followed by a
 * tile type. Finally there must be a create_diagonal_tile overload for the tile
 * type that gets passed in.
 *
 * By default this function will create the array using the default
 * madness::World, but you can pass in a world as an optional third parameter.
 *
 * \todo Finish diagonal matrix this is a little trickier than previous identity
 *functions because it needs to gracefully handle non symmetric tiling.
 */
template <typename T, typename Tile>
TiledArray::Array<T, 2, Tile, TiledArray::SparsePolicy>
diagonal_matrix(TiledArray::TiledRange const &trange, double val,
                madness::World &world = madness::World::get_default()) {

    using Array = TiledArray::Array<T, 2, Tile, TiledArray::SparsePolicy>;

    // TODO initialize tile_norms
    TiledArray::Tensor<float> tile_norms(trange.tiles(), 0.0);

    TiledArray::SparseShape<float> shape(world, tile_norms, trange);

    Array diag(world, trange, shape);
    // TODO assign to diagonal tiles.

    return diag;
}

template <typename T, unsigned int N, typename Tile>
TiledArray::Array<T, N, Tile, TiledArray::DensePolicy> create_diagonal_matrix(
      TiledArray::Array<T, N, Tile, TiledArray::DensePolicy> const &model,
      double val) {

    using Array = TiledArray::Array<T, N, Tile, TiledArray::DensePolicy>;

    Array diag(model.get_world(), model.trange());

    auto pmap_ptr = diag.get_pmap();
    const auto end = pmap_ptr->end();
    for (auto it = pmap_ptr->begin(); it != end; ++it) {

        const auto ord = *it;
        auto const &idx = diag.trange().tiles().idx(ord);
        auto tile = Tile{diag.trange().make_tile_range(ord)};
        auto const extent = tile.range().extent();
        auto map = TiledArray::eigen_map(tile, extent[0], extent[1]);

        if (idx[0] == idx[1]) {
            for (auto i = 0ul; i < extent[0]; ++i) {
                for (auto j = 0ul; j < extent[1]; ++j) {
                    if (i != j) {
                        map(i, j) = 0;
                    } else {
                        map(i, i) = val;
                    }
                }
            }
        } else {
            for (auto i = 0ul; i < extent[0]; ++i) {
                for (auto j = 0ul; j < extent[1]; ++j) {
                    map(i, j) = 0;
                }
            }
        }

        diag.set(ord, std::move(tile));
    }

    return diag;
}

} // namespace pure
} // namespace tcc

#endif // TCC_PUREIFICATION_DIAGONALARRAY_H