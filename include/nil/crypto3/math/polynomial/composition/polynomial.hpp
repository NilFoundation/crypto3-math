#ifndef CRYPTO3_MATH_COMPOSITION_POLYNOMIAL_COMPOSITION_POLYNOMIAL_H_
#define CRYPTO3_MATH_COMPOSITION_POLYNOMIAL_COMPOSITION_POLYNOMIAL_H_

#include <memory>
#include <vector>

#include <nil/crypto3/math/polynomial/composition/neighbors.hpp>
#include <nil/crypto3/math/polynomial/composition/periodic_column.hpp>

namespace nil {
    namespace crypto3 {
        namespace math {

            /*
              Represents a polynomial of the form:

              F(x, y_1, y_2, ... , y_k) =
              \sum_i (c_{2*i} + c_{2*i+1} * x^{n_i}) * f_i(x, y_0, y_1, ... , y_k, p_0, ... , p_m) *
              P_i(x)/Q_i(x).

              Where:

              - The term (c_{2*i} + c_{2*i+1} * x^{n_i}) is used for both logical 'AND' over all the
                constraints, and degree adjustment of each constraint.
              - The sequence (p_0, ... , p_m) consists of the evaluations of the periodic public columns.
              - The term f_i(y_0, y_1, ... , y_k, p_0, ... , p_m) represents a constraint.
              - The term P_i(x)/Q_i(x) is a rational function such that Q_i(x)/P(i) is a polynomial with only
                simple roots, and its roots are exactly the locations in which the constraint f_i has to be
                satisfied.

              Parameters deduction:

              - (c_0, c_1, ...) are the random coefficients chosen by the verifier.
              - The values of (n_0, n_1,...) are computed so that the degree bound of the resulting polynomial
                will be air->GetCompositionPolynomialDegreeBound().
              - The functions (f_0, f_1,...) are induced by air.ConstraintsEval().
              - The mask (for evaluations on entire cosets) is obtained from air.GetMask().

              This class is used both to evaluate F(x, y_0, y_1, ...) on a single point, and on entire cosets
              using optimizations improving the (amortized) computation time for each point in the coset.
            */
            class CompositionPolynomial {
            public:
                virtual ~CompositionPolynomial() = default;

                /*
                  Evaluates the composition polynomial at a single point. The neighbors are the values obtained
                  from the trace's low degree extension, using the AIR's mask.
                */
                virtual ExtensionFieldElement
                    EvalAtPoint(const BaseFieldElement& point, gsl::span<const BaseFieldElement> neighbors,
                                gsl::span<const ExtensionFieldElement> composition_neighbors) const = 0;

                virtual ExtensionFieldElement
                    EvalAtPoint(const ExtensionFieldElement& point, gsl::span<const ExtensionFieldElement> neighbors,
                                gsl::span<const ExtensionFieldElement> composition_neighbors) const = 0;

                /*
                  Evaluates the composition polynomial on the coset coset_offset*<group_generator>, which must be
                  of size coset_size. The evaluation is split into different tasks of size task_size each.
                  The evaluation is written to 'out_evaluation', in bit-reversed order: out_evaluation[i] contains
                  the evaluation on the point coset_offset*(group_generator^{bit_reverse(i)}).
                */
                virtual void EvalOnCosetBitReversedOutput(
                    const BaseFieldElement& coset_offset, gsl::span<const gsl::span<const BaseFieldElement>> trace_lde,
                    gsl::span<const gsl::span<const ExtensionFieldElement>> composition_trace_lde,
                    gsl::span<ExtensionFieldElement> out_evaluation, uint64_t task_size) const = 0;

                /*
                  Returns the degree bound of the composition polynomial.
                */
                virtual uint64_t GetDegreeBound() const = 0;
            };

            template<typename AirT>
            class CompositionPolynomialImpl : public CompositionPolynomial {
            public:
                class Builder {
                public:
                    explicit Builder(uint64_t num_periodic_columns) : periodic_columns_(num_periodic_columns) {
                    }

                    void AddPeriodicColumn(PeriodicColumn column, size_t periodic_column_index);

                    /*
                      Builds an instance of CompositionPolynomialImpl.
                      Note that once Build or BuildUniquePtr are used, the periodic columns that were added
                      previously are consumed and the Builder goes back to a clean slate state.
                    */
                    CompositionPolynomialImpl Build(MaybeOwnedPtr<const AirT> air,
                                                    const BaseFieldElement& trace_generator, uint64_t coset_size,
                                                    gsl::span<const ExtensionFieldElement> random_coefficients,
                                                    gsl::span<const uint64_t> point_exponents,
                                                    gsl::span<const BaseFieldElement> shifts);

                    std::unique_ptr<CompositionPolynomialImpl<AirT>>
                        BuildUniquePtr(MaybeOwnedPtr<const AirT> air, const BaseFieldElement& trace_generator,
                                       uint64_t coset_size, gsl::span<const ExtensionFieldElement> random_coefficients,
                                       gsl::span<const uint64_t> point_exponents,
                                       gsl::span<const BaseFieldElement> shifts);

                private:
                    std::vector<std::optional<PeriodicColumn>> periodic_columns_;
                };

                /*
                  For performance reasons we have separate functions for evaluating at a BaseFieldElement point
                  and an ExtensionFieldElement point, although we could use the ExtensionFieldElement version for
                  both cases by casting.
                */
                ExtensionFieldElement
                    EvalAtPoint(const BaseFieldElement& point, gsl::span<const BaseFieldElement> neighbors,
                                gsl::span<const ExtensionFieldElement> composition_neighbors) const override {
                    return EvalAtPointImpl(point, neighbors, composition_neighbors);
                }

                ExtensionFieldElement EvalAtPoint(const ExtensionFieldElement& point,
                                                  gsl::span<const ExtensionFieldElement>
                                                      neighbors,
                                                  gsl::span<const ExtensionFieldElement>
                                                      composition_neighbors) const override {
                    return EvalAtPointImpl(point, neighbors, composition_neighbors);
                }

                template<typename FieldElementT>
                ExtensionFieldElement
                    EvalAtPointImpl(const FieldElementT& point, gsl::span<const FieldElementT> neighbors,
                                    gsl::span<const ExtensionFieldElement> composition_neighbors) const;

                void EvalOnCosetBitReversedOutput(const BaseFieldElement& coset_offset,
                                                  gsl::span<const gsl::span<const BaseFieldElement>>
                                                      trace_lde,
                                                  gsl::span<const gsl::span<const ExtensionFieldElement>>
                                                      composition_trace_lde,
                                                  gsl::span<ExtensionFieldElement>
                                                      out_evaluation,
                                                  uint64_t task_size) const override;

                /*
                  Same as above where neighbors are the values obtained from the trace low degree extension, using
                  the AIR's mask.
                */
                void EvalOnCosetBitReversedOutput(const BaseFieldElement& coset_offset, const Neighbors& neighbors,
                                                  gsl::span<ExtensionFieldElement> out_evaluation,
                                                  uint64_t task_size) const;

                uint64_t GetDegreeBound() const override {
                    return air_->GetCompositionPolynomialDegreeBound();
                }

            private:
                /*
                  The constructor is private.
                  Users should use the Builder class to build an instance of this class.
                */
                CompositionPolynomialImpl(MaybeOwnedPtr<const AirT> air, BaseFieldElement trace_generator,
                                          uint64_t coset_size, std::vector<PeriodicColumn> periodic_columns,
                                          gsl::span<const ExtensionFieldElement> coefficients,
                                          gsl::span<const uint64_t> point_exponents,
                                          gsl::span<const BaseFieldElement> shifts);

                MaybeOwnedPtr<const AirT> air_;
                const BaseFieldElement trace_generator_;
                const uint64_t coset_size_;
                const std::vector<PeriodicColumn> periodic_columns_;
                const std::vector<ExtensionFieldElement> coefficients_;
                // Exponents of the point powers that are needed for the evaluation of the composition polynomial.
                const std::vector<uint64_t> point_exponents_;
                // Powers of the generator needed for the evaluation of the composition polynomial.
                const std::vector<BaseFieldElement> shifts_;
            };

            template<typename AirT>
            void CompositionPolynomialImpl<AirT>::Builder::AddPeriodicColumn(PeriodicColumn column,
                                                                             size_t periodic_column_index) {
                ASSERT_RELEASE(!periodic_columns_[periodic_column_index].has_value(),
                               "Cannot set periodic column twice.");
                periodic_columns_[periodic_column_index].emplace(std::move(column));
            }

            template<typename AirT>
            CompositionPolynomialImpl<AirT> CompositionPolynomialImpl<AirT>::Builder::Build(
                MaybeOwnedPtr<const AirT> air, const BaseFieldElement& trace_generator, const uint64_t coset_size,
                gsl::span<const ExtensionFieldElement> random_coefficients, gsl::span<const uint64_t> point_exponents,
                gsl::span<const BaseFieldElement> shifts) {
                std::vector<PeriodicColumn> periodic_columns;
                periodic_columns.reserve(periodic_columns_.size());
                for (size_t i = 0; i < periodic_columns_.size(); ++i) {
                    ASSERT_RELEASE(periodic_columns_[i].has_value(),
                                   "Uninitialized periodic column at index " + std::to_string(i) + ".");
                    periodic_columns.push_back(std::move(periodic_columns_[i].value()));
                }
                return CompositionPolynomialImpl(std::move(air), trace_generator, coset_size, periodic_columns,
                                                 random_coefficients, point_exponents, shifts);
            }

            template<typename AirT>
            std::unique_ptr<CompositionPolynomialImpl<AirT>> CompositionPolynomialImpl<AirT>::Builder::BuildUniquePtr(
                MaybeOwnedPtr<const AirT> air, const BaseFieldElement& trace_generator, const uint64_t coset_size,
                gsl::span<const ExtensionFieldElement> random_coefficients, gsl::span<const uint64_t> point_exponents,
                gsl::span<const BaseFieldElement> shifts) {
                return std::make_unique<CompositionPolynomialImpl<AirT>>(
                    Build(std::move(air), trace_generator, coset_size, random_coefficients, point_exponents, shifts));
            }

            template<typename AirT>
            CompositionPolynomialImpl<AirT>::CompositionPolynomialImpl(
                MaybeOwnedPtr<const AirT> air, BaseFieldElement trace_generator, uint64_t coset_size,
                std::vector<PeriodicColumn> periodic_columns, gsl::span<const ExtensionFieldElement> coefficients,
                gsl::span<const uint64_t> point_exponents, gsl::span<const BaseFieldElement> shifts) :
                air_(std::move(air)),
                trace_generator_(trace_generator), coset_size_(coset_size),
                periodic_columns_(std::move(periodic_columns)), coefficients_(coefficients.begin(), coefficients.end()),
                point_exponents_(point_exponents.begin(), point_exponents.end()),
                shifts_(shifts.begin(), shifts.end()) {
                ASSERT_RELEASE(coefficients.size() == air_->NumRandomCoefficients(), "Wrong number of coefficients.");
                ASSERT_RELEASE(IsPowerOfTwo(coset_size_), "Only cosets of size which is a power of two are supported.");
                ASSERT_RELEASE(Pow(trace_generator_, coset_size_) == BaseFieldElement::One(),
                               "The provided generator does not generate a group of the expected size.");
            }

            template<typename AirT>
            template<typename FieldElementT>
            ExtensionFieldElement CompositionPolynomialImpl<AirT>::EvalAtPointImpl(
                const FieldElementT& point, gsl::span<const FieldElementT> neighbors,
                gsl::span<const ExtensionFieldElement> composition_neighbors) const {
                std::vector<FieldElementT> periodic_column_vals;
                periodic_column_vals.reserve(periodic_columns_.size());
                for (const PeriodicColumn& column : periodic_columns_) {
                    periodic_column_vals.push_back(column.EvalAtPoint(point));
                }

                std::vector<FieldElementT> point_powers =
                    FieldElementT::UninitializedVector(1 + point_exponents_.size());
                point_powers[0] = point;
                BatchPow(point, point_exponents_, gsl::make_span(point_powers).subspan(1));

                return air_->template ConstraintsEval<FieldElementT>(
                    neighbors, composition_neighbors, periodic_column_vals, coefficients_, point_powers, shifts_);
            }

            template<typename AirT>
            void CompositionPolynomialImpl<AirT>::EvalOnCosetBitReversedOutput(
                const BaseFieldElement& coset_offset, gsl::span<const gsl::span<const BaseFieldElement>> trace_lde,
                gsl::span<const gsl::span<const ExtensionFieldElement>> composition_trace_lde,
                gsl::span<ExtensionFieldElement> out_evaluation, uint64_t task_size) const {
                Neighbors neighbors(air_->GetMask(), trace_lde, composition_trace_lde);
                EvalOnCosetBitReversedOutput(coset_offset, neighbors, out_evaluation, task_size);
            }

            namespace composition_polynomial {
                namespace details {

                    class CompositionPolynomialImplWorkerMemory {
                        using PeriodicColumnIterator = typename PeriodicColumn::CosetEvaluation::Iterator;

                    public:
                        CompositionPolynomialImplWorkerMemory(size_t periodic_columns_size, size_t point_powers_size) :
                            periodic_column_vals(BaseFieldElement::UninitializedVector(periodic_columns_size)),
                            point_powers(BaseFieldElement::UninitializedVector(point_powers_size)) {
                            periodic_columns_iter.reserve(periodic_columns_size);
                        }

                        // Iterators for the evaluations of the periodic columns on a coset.
                        std::vector<PeriodicColumnIterator> periodic_columns_iter;
                        // Pre-allocated space for periodic column results.
                        std::vector<BaseFieldElement> periodic_column_vals;
                        // Pre-allocated space for the powers of the evaluation point.
                        std::vector<BaseFieldElement> point_powers;
                    };

                }    // namespace details
            }        // namespace composition_polynomial

            template<typename AirT>
            void CompositionPolynomialImpl<AirT>::EvalOnCosetBitReversedOutput(const BaseFieldElement& coset_offset,
                                                                               const Neighbors& neighbors,
                                                                               gsl::span<ExtensionFieldElement>
                                                                                   out_evaluation,
                                                                               uint64_t task_size) const {
                // Input verification.
                ASSERT_RELEASE(out_evaluation.size() == coset_size_, "Output span size does not match the coset size.");

                ASSERT_RELEASE(neighbors.CosetSize() == coset_size_,
                               "Given neighbors iterator is not of the expected length.");

                // Precompute useful constants.
                const size_t log_coset_size = SafeLog2(coset_size_);
                const uint_fast64_t n_tasks = DivCeil(coset_size_, task_size);

                // Prepare offset for each task.
                std::vector<BaseFieldElement> algebraic_offsets;
                algebraic_offsets.reserve(n_tasks);
                BaseFieldElement point = coset_offset;
                const BaseFieldElement point_multiplier = Pow(trace_generator_, task_size);
                for (uint64_t i = 0; i < n_tasks; ++i) {
                    algebraic_offsets.push_back(point);
                    point *= point_multiplier;
                }

                // Prepare #threads workers.
                using WorkerMemoryT = composition_polynomial::details::CompositionPolynomialImplWorkerMemory;
                TaskManager& task_manager = TaskManager::GetInstance();
                std::vector<WorkerMemoryT> worker_mem;
                worker_mem.reserve(task_manager.GetNumThreads());
                for (size_t i = 0; i < task_manager.GetNumThreads(); ++i) {
                    worker_mem.emplace_back(periodic_columns_.size(), 1 + point_exponents_.size());
                }

                // Prepare iterator for each periodic column.
                std::vector<typename PeriodicColumn::CosetEvaluation> periodic_column_cosets;
                periodic_column_cosets.reserve(periodic_columns_.size());
                for (const PeriodicColumn& column : periodic_columns_) {
                    periodic_column_cosets.emplace_back(column.GetCoset(coset_offset, coset_size_));
                }

                // Evaluate on coset.
                const std::vector<BaseFieldElement> gen_powers = BatchPow(trace_generator_, point_exponents_);
                task_manager.ParallelFor(n_tasks, [this, &algebraic_offsets, &periodic_column_cosets, &gen_powers,
                                                   &worker_mem, &neighbors, &out_evaluation, log_coset_size,
                                                   task_size](const TaskInfo& task_info) {
                    const uint64_t initial_point_idx = task_size * task_info.start_idx;
                    BaseFieldElement point = algebraic_offsets[task_info.start_idx];
                    WorkerMemoryT& wm = worker_mem[TaskManager::GetWorkerId()];

                    // Compute point powers.
                    wm.point_powers[0] = point;
                    BatchPow(point, point_exponents_, gsl::make_span(wm.point_powers).subspan(1));

                    // Initialize periodic columns interators.
                    wm.periodic_columns_iter.clear();
                    for (const auto& column_coset : periodic_column_cosets) {
                        wm.periodic_columns_iter.push_back(column_coset.begin() + initial_point_idx);
                    }

                    typename Neighbors::Iterator neighbors_iter = neighbors.begin();
                    neighbors_iter += static_cast<size_t>(initial_point_idx);

                    const size_t actual_task_size = std::min(task_size, coset_size_ - initial_point_idx);
                    const size_t end_of_coset_index = initial_point_idx + actual_task_size;

                    for (size_t point_idx = initial_point_idx; point_idx < end_of_coset_index; ++point_idx) {
                        ASSERT_RELEASE(neighbors_iter != neighbors.end(),
                                       "neighbors_iter reached the end of the iterator unexpectedly.");
                        // Evaluate periodic columns.
                        for (size_t i = 0; i < periodic_columns_.size(); ++i) {
                            wm.periodic_column_vals[i] = *wm.periodic_columns_iter[i];
                            ++wm.periodic_columns_iter[i];
                        }

                        // Evaluate on a single point.
                        auto [neighbors_vals, composition_neighbors] = *neighbors_iter;    // NOLINT
                        out_evaluation[BitReverse(point_idx, log_coset_size)] =
                            air_->template ConstraintsEval<BaseFieldElement>(neighbors_vals, composition_neighbors,
                                                                             wm.periodic_column_vals, coefficients_,
                                                                             wm.point_powers, shifts_);

                        // On the last iteration, skip the preperations for the next iterations.
                        if (point_idx + 1 < end_of_coset_index) {
                            // Advance evaluation point.
                            point *= trace_generator_;
                            wm.point_powers[0] = point;
                            for (size_t i = 0; i < gen_powers.size(); ++i) {
                                // Shift point_powers, x^k -> (g*x)^k.
                                wm.point_powers[i + 1] *= gen_powers[i];
                            }

                            ++neighbors_iter;
                        }
                    }
                });
            }
        }    // namespace math
    }        // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_MATH_COMPOSITION_POLYNOMIAL_COMPOSITION_POLYNOMIAL_H_
