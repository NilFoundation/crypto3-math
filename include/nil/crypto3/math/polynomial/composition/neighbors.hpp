#ifndef CRYPTO3_MATH_COMPOSITION_POLYNOMIAL_NEIGHBORS_H_
#define CRYPTO3_MATH_COMPOSITION_POLYNOMIAL_NEIGHBORS_H_

#include <utility>
#include <vector>

namespace nil {
    namespace crypto3 {
        namespace math {
            /*
              This class is used to iterate over the induced mask values of given traces.
            */
            class Neighbors {
            public:
                /*
                  Constructs a Neighbors instance.
                  trace_lde_coset and composition_trace_lde_coset should refer to one coset in the LDE of the
                  trace, and MUST be kept alive as long the iterator is alive. mask is a list of pairs (relative
                  row, column), as defined by an AIR. trace_lde_coset is a list of columns representing the LDE of
                  the trace in some coset, in a natural order.
                */
                Neighbors(gsl::span<const std::pair<int64_t, uint64_t>> mask,
                          gsl::span<const gsl::span<const BaseFieldElement>>
                              trace_lde_coset,
                          gsl::span<const gsl::span<const ExtensionFieldElement>>
                              composition_trace_lde_coset);

                // Disable copy-constructor and operator= since end_ refers to this.
                Neighbors(const Neighbors& other) = delete;
                Neighbors& operator=(const Neighbors& other) = delete;
                Neighbors(Neighbors&& other) noexcept = delete;
                Neighbors& operator=(Neighbors&& other) noexcept = delete;

                ~Neighbors() = default;

                class Iterator {
                public:
                    Iterator(const Neighbors* parent, size_t idx);

                    bool operator==(const Iterator& other) const {
                        ASSERT_DEBUG(parent_ == other.parent_,
                                     "Comparing iterators with different parent_ is not allowed.");
                        return idx_ == other.idx_;
                    }
                    bool operator!=(const Iterator& other) const {
                        return !((*this) == other);
                    }

                    Iterator& operator+=(size_t offset) {
                        idx_ += offset;
                        return *this;
                    }

                    Iterator& operator++() {
                        idx_++;
                        return *this;
                    }

                    /*
                      Returns the values of the neighbors.
                      Calling operator*() twice is not recommended as it will copy the values twice.
                      Returns spans for an internal storage of the iterator, which is invalidated once operator++()
                      is called or the iterator is destroyed.
                    */
                    std::pair<gsl::span<const BaseFieldElement>, gsl::span<const ExtensionFieldElement>> operator*();

                private:
                    /*
                      Pointer to the corresponding Neighbors instance. Used for retreiving the mask and traces.
                    */
                    const Neighbors* parent_;

                    /*
                      Index of the current point.
                    */
                    size_t idx_ = 0;

                    /*
                      Pre-allocated space for neighbor values.
                    */
                    std::vector<BaseFieldElement> neighbors_;
                    std::vector<ExtensionFieldElement> composition_neighbors_;
                };

                /*
                  Note: Destroying this instance will invalidate the iterator.
                */
                Iterator begin() const {
                    return Iterator(this, 0);
                }    // NOLINT: invalid case style.

                const Iterator& end() const {
                    return end_;
                }    // NOLINT: invalid case style.

                uint64_t CosetSize() const {
                    return coset_size_;
                }

            private:
                const std::vector<std::pair<int64_t, uint64_t>> mask_;
                const uint64_t coset_size_;

                /*
                  Precomputed value to allow computing (x % coset_size_) using an & operation.
                */
                const size_t neighbor_wraparound_mask_;
                const std::vector<gsl::span<const BaseFieldElement>> trace_lde_coset_;
                const std::vector<gsl::span<const ExtensionFieldElement>> composition_trace_lde_coset_;

                /*
                  Keep one instance of the iterator at the end, so that calls to end() will not allocate memory.
                */
                const Iterator end_;
            };

            namespace {

                size_t GetCosetSize(gsl::span<const gsl::span<const BaseFieldElement>> trace_lde_coset,
                                    gsl::span<const gsl::span<const ExtensionFieldElement>>
                                        composition_trace_lde_coset) {
                    ASSERT_RELEASE(!trace_lde_coset.empty(), "Trace must contain at least one column.");
                    size_t coset_size = trace_lde_coset[0].size();
                    for (const auto& column : trace_lde_coset) {
                        ASSERT_RELEASE(column.size() == coset_size, "All columns must have the same size.");
                    }
                    for (const auto& column : composition_trace_lde_coset) {
                        ASSERT_RELEASE(column.size() == coset_size, "All columns must have the same size.");
                    }
                    return coset_size;
                }

            }    // namespace

            Neighbors::Neighbors(gsl::span<const std::pair<int64_t, uint64_t>> mask,
                                 gsl::span<const gsl::span<const BaseFieldElement>>
                                     trace_lde_coset,
                                 gsl::span<const gsl::span<const ExtensionFieldElement>>
                                     composition_trace_lde_coset) :
                mask_(mask.begin(), mask.end()),
                coset_size_(GetCosetSize(trace_lde_coset, composition_trace_lde_coset)),
                neighbor_wraparound_mask_(coset_size_ - 1),
                trace_lde_coset_(trace_lde_coset.begin(), trace_lde_coset.end()),
                composition_trace_lde_coset_(composition_trace_lde_coset.begin(), composition_trace_lde_coset.end()),
                end_(this, coset_size_) {
                ASSERT_RELEASE(IsPowerOfTwo(coset_size_), "Coset size must be a power of 2.");
                for (const auto& mask_item : mask) {
                    ASSERT_RELEASE(mask_item.second < trace_lde_coset.size() + composition_trace_lde_coset.size(),
                                   "Too few trace LDE columns provided.");
                }
            }

            Neighbors::Iterator::Iterator(const Neighbors* parent, size_t idx) :
                parent_(parent), idx_(idx),
                // Allocating mask_.size() elements for neighbors and composition_neighbors is enough because
                // it is the total size of them together. We allocate this memory only once and pass it only
                // by reference so the memory cost is negligible.
                neighbors_(BaseFieldElement::UninitializedVector(parent->mask_.size())),
                composition_neighbors_(ExtensionFieldElement::UninitializedVector(parent->mask_.size())) {
            }

            std::pair<gsl::span<const BaseFieldElement>, gsl::span<const ExtensionFieldElement>>
                Neighbors::Iterator::operator*() {
                const auto& mask = parent_->mask_;
                const auto& trace_lde_coset = parent_->trace_lde_coset_;
                const auto& composition_trace_lde_coset = parent_->composition_trace_lde_coset_;
                const auto& neighbor_wraparound_mask = parent_->neighbor_wraparound_mask_;
                size_t neighbors_idx = 0;
                size_t compoaition_neighbors_idx = 0;
                for (const auto [row, col] : mask) {    // NOLINT: structured binding.
                    if (col < trace_lde_coset.size()) {
                        neighbors_[neighbors_idx] = trace_lde_coset.at(col).at((idx_ + row) & neighbor_wraparound_mask);
                        ++neighbors_idx;
                    } else {
                        composition_neighbors_[compoaition_neighbors_idx] =
                            composition_trace_lde_coset.at(col - trace_lde_coset.size())
                                .at((idx_ + row) & neighbor_wraparound_mask);
                        ++compoaition_neighbors_idx;
                    }
                }
                return std::pair(gsl::make_span(neighbors_).subspan(0, neighbors_idx),
                                 gsl::make_span(composition_neighbors_).subspan(0, compoaition_neighbors_idx));
            }

        }    // namespace math
    }        // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_MATH_COMPOSITION_POLYNOMIAL_NEIGHBORS_H_
