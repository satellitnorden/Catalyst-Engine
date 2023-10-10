/**
 * @file methods/lars/lars.hpp
 * @author Nishant Mehta (niche)
 *
 * Definition of the LARS class, which performs Least Angle Regression and the
 * LASSO.
 *
 * Only minor modifications of LARS are necessary to handle the constrained
 * version of the problem:
 *
 * \f[
 * \min_{\beta} 0.5 || X \beta - y ||_2^2 + 0.5 \lambda_2 || \beta ||_2^2
 * \f]
 * subject to \f$ ||\beta||_1 <= \tau \f$
 *
 * Although this option currently is not implemented, it will be implemented
 * very soon.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_LARS_LARS_HPP
#define MLPACK_METHODS_LARS_LARS_HPP

#include <mlpack/core.hpp>

namespace mlpack {

// beta is the estimator
// yHat is the prediction from the current estimator

/**
 * An implementation of LARS, a stage-wise homotopy-based algorithm for
 * l1-regularized linear regression (LASSO) and l1+l2 regularized linear
 * regression (Elastic Net).
 *
 * Let \f$ X \f$ be a matrix where each row is a point and each column is a
 * dimension and let \f$ y \f$ be a vector of responses.
 *
 * The Elastic Net problem is to solve
 *
 * \f[ \min_{\beta} 0.5 || X \beta - y ||_2^2 + \lambda_1 || \beta ||_1 +
 *     0.5 \lambda_2 || \beta ||_2^2 \f]
 *
 * where \f$ \beta \f$ is the vector of regression coefficients.
 *
 * If \f$ \lambda_1 > 0 \f$ and \f$ \lambda_2 = 0 \f$, the problem is the LASSO.
 * If \f$ \lambda_1 > 0 \f$ and \f$ \lambda_2 > 0 \f$, the problem is the
 *   elastic net.
 * If \f$ \lambda_1 = 0 \f$ and \f$ \lambda_2 > 0 \f$, the problem is ridge
 *   regression.
 * If \f$ \lambda_1 = 0 \f$ and \f$ \lambda_2 = 0 \f$, the problem is
 *   unregularized linear regression.
 *
 * Note: This algorithm is not recommended for use (in terms of efficiency)
 * when \f$ \lambda_1 \f$ = 0.
 *
 * For more details, see the following papers:
 *
 * @code
 * @article{efron2004least,
 *   title={Least angle regression},
 *   author={Efron, B. and Hastie, T. and Johnstone, I. and Tibshirani, R.},
 *   journal={The Annals of statistics},
 *   volume={32},
 *   number={2},
 *   pages={407--499},
 *   year={2004},
 *   publisher={Institute of Mathematical Statistics}
 * }
 * @endcode
 *
 * @code
 * @article{zou2005regularization,
 *   title={Regularization and variable selection via the elastic net},
 *   author={Zou, H. and Hastie, T.},
 *   journal={Journal of the Royal Statistical Society Series B},
 *   volume={67},
 *   number={2},
 *   pages={301--320},
 *   year={2005},
 *   publisher={Royal Statistical Society}
 * }
 * @endcode
 */
class LARS
{
 public:
  /**
   * Set the parameters to LARS.  Both lambda1 and lambda2 default to 0.
   *
   * @param useCholesky Whether or not to use Cholesky decomposition when
   *    solving linear system (as opposed to using the full Gram matrix).
   * @param lambda1 Regularization parameter for l1-norm penalty.
   * @param lambda2 Regularization parameter for l2-norm penalty.
   * @param tolerance Run until the maximum correlation of elements in (X^T y)
   *     is less than this.
   * @param intercept If true, fit an intercept in the model.
   * @param normalize If true, normalize all features to have unit variance for
   *     training.
   * @param fitIntercept If true, fit an intercept in the model.
   * @param normalizeData If true, normalize all features to have unit variance
   * for training.
   */
  LARS(const bool useCholesky = false,
       const double lambda1 = 0.0,
       const double lambda2 = 0.0,
       const double tolerance = 1e-16,
       const bool fitIntercept = true,
       const bool normalizeData = true);

  /**
   * Set the parameters to LARS, and pass in a precalculated Gram matrix.  Both
   * lambda1 and lambda2 default to 0.
   *
   * Note that the precalculated Gram matrix must match the settings of
   * `fitIntercept` and `normalizeData` (which both default to `true`): so, this
   * means that by default, the Gram matrix should be computed on mean-centered
   * data whose features are normalized to have unit variance.
   *
   * @param useCholesky Whether or not to use Cholesky decomposition when
   *    solving linear system (as opposed to using the full Gram matrix).
   * @param gramMatrix Gram matrix.
   * @param lambda1 Regularization parameter for l1-norm penalty.
   * @param lambda2 Regularization parameter for l2-norm penalty.
   * @param tolerance Run until the maximum correlation of elements in (X^T y)
   *     is less than this.
   * @param intercept If true, fit an intercept in the model.
   * @param normalize If true, normalize all features to have unit variance for
   *     training.
   * @param fitIntercept If true, fit an intercept in the model.
   * @param normalizeData If true, normalize all features to have unit variance
   * for training.
   */
  LARS(const bool useCholesky,
       const arma::mat& gramMatrix,
       const double lambda1 = 0.0,
       const double lambda2 = 0.0,
       const double tolerance = 1e-16,
       const bool fitIntercept = true,
       const bool normalizeData = true);

  /**
   * Set the parameters to LARS and run training. Both lambda1 and lambda2
   * are set by default to 0.
   *
   * @param data Input data.
   * @param responses A vector of targets.
   * @param transposeData Should be true if the input data is column-major and
   *     false otherwise.
   * @param useCholesky Whether or not to use Cholesky decomposition when
   *     solving linear system (as opposed to using the full Gram matrix).
   * @param lambda1 Regularization parameter for l1-norm penalty.
   * @param lambda2 Regularization parameter for l2-norm penalty.
   * @param tolerance Run until the maximum correlation of elements in (X^T y)
   *     is less than this.
   * @param fitIntercept If true, fit an intercept in the model.
   * @param normalizeData If true, normalize all features to have unit variance
   * for training.
   */
  LARS(const arma::mat& data,
       const arma::rowvec& responses,
       const bool transposeData = true,
       const bool useCholesky = false,
       const double lambda1 = 0.0,
       const double lambda2 = 0.0,
       const double tolerance = 1e-16,
       const bool fitIntercept = true,
       const bool normalizeData = true);

  /**
   * Set the parameters to LARS, pass in a precalculated Gram matrix, and run
   * training. Both lambda1 and lambda2 are set by default to 0.
   *
   * Note that the precalculated Gram matrix must match the settings of
   * `fitIntercept` and `normalizeData` (which both default to `true`): so, this
   * means that by default, the Gram matrix should be computed on mean-centered
   * data whose features are normalized to have unit variance.
   *
   * @param data Input data.
   * @param responses A vector of targets.
   * @param transposeData Should be true if the input data is column-major and
   *     false otherwise.
   * @param useCholesky Whether or not to use Cholesky decomposition when
   *     solving linear system (as opposed to using the full Gram matrix).
   * @param gramMatrix Gram matrix.
   * @param lambda1 Regularization parameter for l1-norm penalty.
   * @param lambda2 Regularization parameter for l2-norm penalty.
   * @param tolerance Run until the maximum correlation of elements in (X^T y)
   *     is less than this.
   * @param fitIntercept If true, fit an intercept in the model.
   * @param normalizeData If true, normalize all features to have unit variance
   * for training.
   */
  LARS(const arma::mat& data,
       const arma::rowvec& responses,
       const bool transposeData,
       const bool useCholesky,
       const arma::mat& gramMatrix,
       const double lambda1 = 0.0,
       const double lambda2 = 0.0,
       const double tolerance = 1e-16,
       const bool fitIntercept = true,
       const bool normalizeData = true);

  /**
   * Construct the LARS object by copying the given LARS object.
   *
   * @param other LARS object to copy.
   */
  LARS(const LARS& other);

  /**
   * Construct the LARS object by taking ownership of the given LARS object.
   *
   * @param other LARS object to take ownership of.
   */
  LARS(LARS&& other);

  /**
   * Copy the given LARS object.
   *
   * @param other LARS object to copy.
   */
  LARS& operator=(const LARS& other);

  /**
   * Take ownership of the given LARS object.
   *
   * @param other LARS object to take ownership of.
   */
  LARS& operator=(LARS&& other);

  /**
   * Run LARS.  The input matrix (like all mlpack matrices) should be
   * column-major -- each column is an observation and each row is a dimension.
   * However, because LARS is more efficient on a row-major matrix, this method
   * will (internally) transpose the matrix.  If this transposition is not
   * necessary (i.e., you want to pass in a row-major matrix), pass 'false' for
   * the transposeData parameter.
   *
   * @param data Column-major input data (or row-major input data if rowMajor =
   *     true).
   * @param responses A vector of targets.
   * @param beta Vector to store the solution (the coefficients) in.
   * @param transposeData Set to false if the data is row-major.
   * @return minimum cost error(||y-beta*X||2 is used to calculate error).
   */
  double Train(const arma::mat& data,
               const arma::rowvec& responses,
               arma::vec& beta,
               const bool transposeData = true);

  /**
   * Run LARS.  The input matrix (like all mlpack matrices) should be
   * column-major -- each column is an observation and each row is a dimension.
   * However, because LARS is more efficient on a row-major matrix, this method
   * will (internally) transpose the matrix.  If this transposition is not
   * necessary (i.e., you want to pass in a row-major matrix), pass 'false' for
   * the transposeData parameter.
   *
   * @param data Input data.
   * @param responses A vector of targets.
   * @param transposeData Should be true if the input data is column-major and
   *     false otherwise.
   * @return minimum cost error(||y-beta*X||2 is used to calculate error).
   */
  double Train(const arma::mat& data,
               const arma::rowvec& responses,
               const bool transposeData = true);

  /**
   * Predict y_i for each data point in the given data matrix using the
   * currently-trained LARS model.
   *
   * @param points The data points to regress on.
   * @param predictions y, which will contained calculated values on completion.
   * @param rowMajor Should be true if the data points matrix is row-major and
   *     false otherwise.
   */
  void Predict(const arma::mat& points,
               arma::rowvec& predictions,
               const bool rowMajor = false) const;

  //! Get the L1 regularization coefficient.
  double Lambda1() const { return lambda1; }
  //! Modify the L1 regularization coefficient.
  double& Lambda1() { return lambda1; }

  //! Get the L2 regularization coefficient.
  double Lambda2() const { return lambda2; }
  //! Modify the L2 regularization coefficient.
  double& Lambda2() { return lambda2; }

  //! Get whether to use the Cholesky decomposition.
  bool UseCholesky() const { return useCholesky; }
  //! Modify whether to use the Cholesky decomposition.
  bool& UseCholesky() { return useCholesky; }

  //! Get the tolerance for maximum correlation during training.
  double Tolerance() const { return tolerance; }
  //! Modify the tolerance for maximum correlation during training.
  double& Tolerance() { return tolerance; }

  //! Get whether or not to fit an intercept.
  bool FitIntercept() const { return fitIntercept; }
  //! Modify whether or not to fit an intercept.
  void FitIntercept(const bool newFitIntercept)
  {
    // If we are storing a Gram matrix internally, but now will be normalizing
    // data, then the Gram matrix we have computed is incorrect and needs to be
    // recomputed.
    if (fitIntercept != newFitIntercept)
    {
      if (matGram != &matGramInternal)
      {
        throw std::invalid_argument("LARS::FitIntercept(): cannot change value "
            "when an external Gram matrix was specified!");
      }

      fitIntercept = newFitIntercept;
      matGramInternal.clear();
    }
  }

  //! Get whether or not to normalize data during training.
  bool NormalizeData() const { return normalizeData; }
  //! Modify whether or not to normalize data during training.
  void NormalizeData(const bool newNormalizeData)
  {
    // If we are storing a Gram matrix internally, but now will be normalizing
    // data, then the Gram matrix we have computed is incorrect and needs to be
    // recomputed.
    if (normalizeData != newNormalizeData)
    {
      if (matGram != &matGramInternal)
      {
        throw std::invalid_argument("LARS::NormalizeData(): cannot change value "
            "when an external Gram matrix was specified!");
      }

      normalizeData = newNormalizeData;
      matGramInternal.clear();
    }
  }

  //! Access the set of active dimensions.
  const std::vector<size_t>& ActiveSet() const { return activeSet; }

  //! Access the set of coefficients after each iteration; the solution is the
  //! last element.
  const std::vector<arma::vec>& BetaPath() const { return betaPath; }

  //! Access the solution coefficients
  const arma::vec& Beta() const { return betaPath.back(); }

  //! Access the set of values for lambda1 after each iteration; the solution is
  //! the last element.
  const std::vector<double>& LambdaPath() const { return lambdaPath; }

  //! Return the intercept (if fitted, otherwise 0).
  double Intercept() const { return interceptPath.back(); }

  //! Return the intercept path (the intercept for every model).
  const std::vector<double>& InterceptPath() const { return interceptPath; }

  //! Access the upper triangular cholesky factor.
  const arma::mat& MatUtriCholFactor() const { return matUtriCholFactor; }

  /**
   * Serialize the LARS model.
   */
  template<typename Archive>
  void serialize(Archive& ar, const uint32_t /* version */);

  /**
   * Compute cost error of the given data matrix using the
   * currently-trained LARS model. Only ||y-beta*X||2 is used to calculate
   * cost error.
   *
   * @param matX Column-major input data (or row-major input data if rowMajor =
   *     true).
   * @param y responses A vector of targets.
   * @param rowMajor Should be true if the data points matrix is row-major and
   *   false otherwise.
   * @return The minimum cost error.
   */
  double ComputeError(const arma::mat& matX,
                      const arma::rowvec& y,
                      const bool rowMajor = false);

 private:
  //! Gram matrix.
  arma::mat matGramInternal;

  //! Pointer to the Gram matrix we will use.
  const arma::mat* matGram;

  //! Upper triangular cholesky factor; initially 0x0 matrix.
  arma::mat matUtriCholFactor;

  //! Whether or not to use Cholesky decomposition when solving linear system.
  bool useCholesky;

  //! True if this is the LASSO problem.
  bool lasso;
  //! Regularization parameter for l1 penalty.
  double lambda1;

  //! True if this is the elastic net problem.
  bool elasticNet;
  //! Regularization parameter for l2 penalty.
  double lambda2;

  //! Tolerance for main loop.
  double tolerance;

  //! Whether or not to fit an intercept.
  bool fitIntercept;

  //! Whether or not to normalize data during training (i.e. make each feature
  //! have unit variance for training).
  bool normalizeData;

  //! Solution path.
  std::vector<arma::vec> betaPath;

  //! Value of lambda_1 for each solution in solution path.
  std::vector<double> lambdaPath;

  //! Intercept (only if fitIntercept is true).
  std::vector<double> interceptPath;

  //! Active set of dimensions.
  std::vector<size_t> activeSet;

  //! Active set membership indicator (for each dimension).
  std::vector<bool> isActive;

  // Set of variables that are ignored (if any).

  //! Set of ignored variables (for dimensions in span{active set dimensions}).
  std::vector<size_t> ignoreSet;

  //! Membership indicator for set of ignored variables.
  std::vector<bool> isIgnored;

  /**
   * Remove activeVarInd'th element from active set.
   *
   * @param activeVarInd Index of element to remove from active set.
   */
  void Deactivate(const size_t activeVarInd);

  /**
   * Add dimension varInd to active set.
   *
   * @param varInd Dimension to add to active set.
   */
  void Activate(const size_t varInd);

  /**
   * Add dimension varInd to ignores set (never removed).
   *
   * @param varInd Dimension to add to ignores set.
   */
  void Ignore(const size_t varInd);

  // compute "equiangular" direction in output space
  void ComputeYHatDirection(const arma::mat& matX,
                            const arma::vec& betaDirection,
                            arma::vec& yHatDirection);

  // interpolate to compute last solution vector
  void InterpolateBeta();

  void CholeskyInsert(const arma::vec& newX, const arma::mat& X);

  void CholeskyInsert(double sqNormNewX, const arma::vec& newGramCol);

  void GivensRotate(const arma::vec::fixed<2>& x,
                    arma::vec::fixed<2>& rotatedX,
                    arma::mat& G);

  void CholeskyDelete(const size_t colToKill);
};

} // namespace mlpack

// Include implementation of serialize().
#include "lars_impl.hpp"

#endif
