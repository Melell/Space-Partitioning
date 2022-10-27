#pragma once
#include <gtest/gtest.h>
#include "geometry.hpp"
#include "debug.hpp"

namespace testing {
    namespace internal {
        /**
		 * @brief
		 * 	Override of double near for vectors, calls double nears
		 * @param expr1
		 * @param expr2
		 * @param abs_error_expr
		 * @param val1
		 * @param val2
		 * @param abs_error
		 * @return
		 */
        AssertionResult DoubleNearPredFormat(const char*      expr1,
                                             const char*      expr2,
                                             const char*      abs_error_expr,
                                             glm::vec3 const& val1,
                                             glm::vec3 const& val2,
                                             double           abs_error)
        {
            auto x_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.x, val2.x, abs_error);
            auto y_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.y, val2.y, abs_error);
            auto z_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.z, val2.z, abs_error);

            if (!x_res || !y_res || !z_res) {
                return AssertionFailure()
                       << "The difference between " << expr1 << " and " << expr2
                       << " exceeds " << abs_error_expr << ", where\n"
                       << expr1 << " evaluates to " << val1 << ",\n"
                       << expr2 << " evaluates to " << val2 << ", and\n"
                       << abs_error_expr << " evaluates to " << abs_error << ".";
            }

            return AssertionSuccess();
        }

        /**
		 * @brief
		 * 	Override of double near for vectors, calls double nears
		 * @param expr1
		 * @param expr2
		 * @param abs_error_expr
		 * @param val1
		 * @param val2
		 * @param abs_error
		 * @return
		 */
        AssertionResult DoubleNearPredFormat(const char*      expr1,
                                             const char*      expr2,
                                             const char*      abs_error_expr,
                                             glm::vec2 const& val1,
                                             glm::vec2 const& val2,
                                             double           abs_error)
        {
            auto x_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.x, val2.x, abs_error);
            auto y_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1.y, val2.y, abs_error);

            if (!x_res || !y_res) {
                return AssertionFailure()
                       << "The difference between " << expr1 << " and " << expr2
                       << " exceeds " << abs_error_expr << ", where\n"
                       << expr1 << " evaluates to " << val1 << ",\n"
                       << expr2 << " evaluates to " << val2 << ", and\n"
                       << abs_error_expr << " evaluates to " << abs_error << ".";
            }

            return AssertionSuccess();
        }

        /**
		 * @brief
		 * 	Override of double near for segments, calls vector nears
		 * @param expr1
		 * @param expr2
		 * @param abs_error_expr
		 * @param val1
		 * @param val2
		 * @param abs_error
		 * @return
		 */
        AssertionResult DoubleNearPredFormat(const char*           expr1,
                                             const char*           expr2,
                                             const char*           abs_error_expr,
                                             cs350::segment const& val1,
                                             cs350::segment const& val2,
                                             double                abs_error)
        {
            auto start_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1[0], val2[0], abs_error);
            auto end_res = DoubleNearPredFormat(expr1, expr2, abs_error_expr, val1[1], val2[1], abs_error);

            if (!start_res || !end_res) {
                return AssertionFailure()
                       << "The difference between " << expr1 << " and " << expr2
                       << " exceeds " << abs_error_expr << ", where\n"
                       << expr1 << " evaluates to " << val1 << ",\n"
                       << expr2 << " evaluates to " << val2 << ", and\n"
                       << abs_error_expr << " evaluates to " << abs_error << ".";
            }

            return AssertionSuccess();
        }
    }
}