#ifndef CRYPTO3_MATH_COMPOSITION_POLYNOMIAL_PERIODIC_COLUMN_H_
#define CRYPTO3_MATH_COMPOSITION_POLYNOMIAL_PERIODIC_COLUMN_H_

#include <utility>
#include <vector>

namespace nil {
    namespace crypto3 {
        namespace math {

            /*
              Represents a polynomial whose evaluation on a given coset is periodic with a given period.
              This can be used to simulate public columns (known both to the prover and the verifier) where the
              data of the column is periodic with a relatively small period. For example, round constants that
              appear in a hash function and repeat every invocation.

              Example usage:
                PeriodicColumn p = ...;
                auto coset_eval = p.GetCoset(...);
                ParallelFor(..., {
                  auto it = coset_eval.begin();
                  // Do stuff with iterator, safely
                }).
            */
            class PeriodicColumn {
            public:
                /*
                  Constructs a PeriodicColumn whose evaluation on the trace domain is composed of repetitions of
                  the given values. Namely, f(trace_generator^i) = values[i % values.size()].
                */
                PeriodicColumn(gsl::span<const BaseFieldElement> values, uint64_t trace_size);

                /*
                  Returns the evaluation of the interpolation polynomial at a given point.
                */
                template<typename FieldElementT>
                FieldElementT EvalAtPoint(const FieldElementT& x) const;

                // Forward declaration.
                class CosetEvaluation;

                /*
                  Returns an iterator that computes the polynomial on a coset of the same size as the trace.
                */
                CosetEvaluation GetCoset(const BaseFieldElement& start_point, size_t coset_size) const;

            private:
                /*
                  The period of the column with respect to the trace.
                  Note that
                    period_in_trace_ = values.size().
                */
                const uint64_t period_in_trace_;

                /*
                  The size of the coset divided by the length of the period.
                */
                const size_t n_copies_;

                /*
                  The lde_manager of the column. This should be treated as a polynomial in x^{n_copies}.
                */
                LdeManager<BaseFieldElement> lde_manager_;
            };

            /*
              Represents an efficient evaluation of the periodic column on a coset. Can spawn thin iterators to
              the evaluation, which are thread-safe.
            */
            class PeriodicColumn::CosetEvaluation {
            public:
                explicit CosetEvaluation(std::vector<BaseFieldElement> values) :
                    values_(std::move(values)), index_mask_(values_.size() - 1) {
                    ASSERT_RELEASE(IsPowerOfTwo(values_.size()), "values must be of size which is a power of two.");
                }

                class Iterator {
                public:
                    Iterator(const CosetEvaluation* parent, uint64_t index, const uint64_t index_mask) :
                        parent_(parent), index_(index), index_mask_(index_mask) {
                    }

                    Iterator& operator++() {
                        index_ = (index_ + 1) & index_mask_;
                        return *this;
                    }

                    Iterator operator+(uint64_t offset) const {
                        return Iterator(parent_, (index_ + offset) & index_mask_, index_mask_);
                    }

                    BaseFieldElement operator*() const {
                        return parent_->values_[index_];
                    }

                private:
                    const CosetEvaluation* parent_;
                    uint64_t index_;
                    const uint64_t index_mask_;
                };

                Iterator begin() const {
                    return Iterator(this, 0, index_mask_);
                }    // NOLINT

            private:
                const std::vector<BaseFieldElement> values_;
                const uint64_t index_mask_;
            };

            template<typename FieldElementT>
            FieldElementT PeriodicColumn::EvalAtPoint(const FieldElementT& x) const {
                const FieldElementT point = Pow(x, n_copies_);
                FieldElementT output = FieldElementT::Uninitialized();
                lde_manager_.EvalAtPoints<FieldElementT>(0, gsl::make_span(&point, 1), gsl::make_span(&output, 1));

                return output;
            }

            PeriodicColumn::PeriodicColumn(gsl::span<const BaseFieldElement> values, uint64_t trace_size) :
                period_in_trace_(values.size()), n_copies_(SafeDiv(trace_size, period_in_trace_)),
                lde_manager_(Coset(values.size(), BaseFieldElement::One()),
                             /*eval_in_natural_order=*/true) {
                lde_manager_.AddEvaluation(values);
            }

            auto PeriodicColumn::GetCoset(const BaseFieldElement& start_point, const size_t coset_size) const
                -> CosetEvaluation {
                const BaseFieldElement offset = Pow(start_point, n_copies_);
                ASSERT_RELEASE(coset_size == n_copies_ * period_in_trace_,
                               "coset_size must be the same as the size of the coset that was used to create the "
                               "PeriodicColumn.");

                // Allocate storage for the LDE computation.
                std::vector<BaseFieldElement> period_on_coset = BaseFieldElement::UninitializedVector(period_in_trace_);
                std::array<gsl::span<BaseFieldElement>, 1> output_spans {gsl::make_span(period_on_coset)};
                lde_manager_.EvalOnCoset(offset, output_spans);
                return CosetEvaluation(std::move(period_on_coset));
            }

        }    // namespace math
    }        // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_MATH_COMPOSITION_POLYNOMIAL_PERIODIC_COLUMN_H_
