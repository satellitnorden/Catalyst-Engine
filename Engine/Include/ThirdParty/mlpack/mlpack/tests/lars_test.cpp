/**
 * @file tests/lars_test.cpp
 * @author Nishant Mehta
 *
 * Test for LARS.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#include <mlpack/core.hpp>
#include <mlpack/methods/lars.hpp>

#include "catch.hpp"
#include "test_catch_tools.hpp"

using namespace mlpack;

void GenerateProblem(
    arma::mat& X, arma::rowvec& y, size_t nPoints, size_t nDims)
{
  X = arma::randn(nDims, nPoints);
  arma::vec beta = arma::randn(nDims, 1);
  y = beta.t() * X;
}

void LARSVerifyCorrectness(arma::vec beta, arma::vec errCorr, double lambda)
{
  size_t nDims = beta.n_elem;
  const double tol = 1e-10;
  for (size_t j = 0; j < nDims; ++j)
  {
    if (beta(j) == 0)
    {
      // Make sure that |errCorr(j)| <= lambda.
      REQUIRE(std::max(fabs(errCorr(j)) - lambda, 0.0) ==
          Approx(0.0).margin(tol));
    }
    else if (beta(j) < 0)
    {
      // Make sure that errCorr(j) == lambda.
      REQUIRE(errCorr(j) - lambda == Approx(0.0).margin(tol));
    }
    else // beta(j) > 0
    {
      // Make sure that errCorr(j) == -lambda.
      REQUIRE(errCorr(j) + lambda == Approx(0.0).margin(tol));
    }
  }
}

void LassoTest(size_t nPoints, size_t nDims, bool elasticNet, bool useCholesky,
               bool fitIntercept, bool normalizeData)
{
  arma::mat X;
  arma::rowvec y;

  for (size_t i = 0; i < 100; ++i)
  {
    GenerateProblem(X, y, nPoints, nDims);

    // Armadillo's median is broken, so...
    arma::vec sortedAbsCorr = sort(abs(X * y.t()));
    double lambda1 = sortedAbsCorr(nDims / 2);
    double lambda2;
    if (elasticNet)
      lambda2 = lambda1 / 2;
    else
      lambda2 = 0;

    LARS lars(useCholesky, lambda1, lambda2);
    lars.FitIntercept(fitIntercept);
    lars.NormalizeData(normalizeData);
    arma::vec betaOpt;
    lars.Train(X, y, betaOpt);

    if (fitIntercept)
    {
      y -= arma::mean(y);
      X.each_col() -= arma::mean(X, 1);
    }

    if (normalizeData)
    {
      arma::vec stds = arma::stddev(X, 0, 1);
      stds.replace(0.0, 1.0);

      X.each_col() /= stds;
      betaOpt %= stds; // recover solution in normalized space
    }

    arma::vec errCorr = (X * trans(X) + lambda2 *
        arma::eye(nDims, nDims)) * betaOpt - X * y.t();

    LARSVerifyCorrectness(betaOpt, errCorr, lambda1);
  }
}

TEST_CASE("LARSTestLassoCholesky", "[LARSTest]")
{
  LassoTest(100, 10, false, true, false, false);
  LassoTest(100, 10, false, true, true, false);
  LassoTest(100, 10, false, true, false, true);
  LassoTest(100, 10, false, true, true, true);
}


TEST_CASE("LARSTestLassoGram", "[LARSTest]")
{
  LassoTest(100, 10, false, false, false, false);
  LassoTest(100, 10, false, false, true, false);
  LassoTest(100, 10, false, false, false, true);
  LassoTest(100, 10, false, false, true, true);
}

TEST_CASE("LARSTestElasticNetCholesky", "[LARSTest]")
{
  LassoTest(100, 10, true, true, false, false);
  LassoTest(100, 10, true, true, true, false);
  LassoTest(100, 10, true, true, false, true);
  LassoTest(100, 10, true, true, true, true);
}

TEST_CASE("LARSTestElasticNetGram", "[LARSTest]")
{
  LassoTest(100, 10, true, false, false, false);
  LassoTest(100, 10, true, false, true, false);
  LassoTest(100, 10, true, false, false, true);
  LassoTest(100, 10, true, false, true, true);
}

// Ensure that LARS doesn't crash when the data has linearly dependent features
// (meaning that there is a singularity).  This test uses the Cholesky
// factorization.
TEST_CASE("CholeskySingularityTest", "[LARSTest]")
{
  arma::mat X;
  arma::mat Y;

  if (!data::Load("lars_dependent_x.csv", X))
    FAIL("Cannot load dataset lars_dependent_x.csv");
  if (!data::Load("lars_dependent_y.csv", Y))
    FAIL("Cannot load dataset lars_dependent_y.csv");

  arma::rowvec y = Y.row(0);

  // Test for a couple values of lambda1.
  for (double lambda1 = 0.0; lambda1 < 1.0; lambda1 += 0.1)
  {
    LARS lars(true, lambda1, 0.0);
    lars.FitIntercept(false);
    lars.NormalizeData(false);
    arma::vec betaOpt;
    lars.Train(X, y, betaOpt);

    arma::vec errCorr = (X * X.t()) * betaOpt - X * y.t();

    LARSVerifyCorrectness(betaOpt, errCorr, lambda1);
  }
}

// Same as the above test but with no cholesky factorization.
TEST_CASE("NoCholeskySingularityTest", "[LARSTest]")
{
  arma::mat X;
  arma::mat Y;

  if (!data::Load("lars_dependent_x.csv", X))
    FAIL("Cannot load dataset lars_dependent_x.csv");
  if (!data::Load("lars_dependent_y.csv", Y))
    FAIL("Cannot load dataset lars_dependent_y.csv");

  arma::rowvec y = Y.row(0);

  // Test for a couple values of lambda1.
  for (double lambda1 = 0.0; lambda1 < 1.0; lambda1 += 0.1)
  {
    LARS lars(false, lambda1, 0.0);
    lars.FitIntercept(false);
    lars.NormalizeData(false);
    arma::vec betaOpt;
    lars.Train(X, y, betaOpt);

    arma::vec errCorr = (X * X.t()) * betaOpt - X * y.t();

    LARSVerifyCorrectness(betaOpt, errCorr, lambda1);
  }
}

// Make sure that Predict() provides reasonable enough solutions.
TEST_CASE("PredictTest", "[LARSTest]")
{
  for (size_t i = 0; i < 2; ++i)
  {
    // Run with both true and false.
    bool useCholesky = bool(i);

    arma::mat X;
    arma::rowvec y;

    GenerateProblem(X, y, 1000, 100);

    for (double lambda1 = 0.0; lambda1 < 1.0; lambda1 += 0.2)
    {
      for (double lambda2 = 0.0; lambda2 < 1.0; lambda2 += 0.2)
      {
        LARS lars(useCholesky, lambda1, lambda2);
        lars.FitIntercept(false);
        lars.NormalizeData(false);
        arma::vec betaOpt;
        lars.Train(X, y, betaOpt);

        // Calculate what the actual error should be with these regression
        // parameters.
        arma::vec betaOptPred = (X * X.t()) * betaOpt;
        arma::rowvec predictions;
        lars.Predict(X, predictions);
        arma::vec adjPred = X * predictions.t();

        REQUIRE(predictions.n_elem == 1000);
        for (size_t i = 0; i < betaOptPred.n_elem; ++i)
        {
          if (std::abs(betaOptPred[i]) < 1e-5)
            REQUIRE(adjPred[i] == Approx(0.0).margin(1e-5));
          else
            REQUIRE(adjPred[i] == Approx(betaOptPred[i]).epsilon(1e-7));
        }
      }
    }
  }
}

TEST_CASE("PredictRowMajorTest", "[LARSTest]")
{
  arma::mat X;
  arma::rowvec y;
  GenerateProblem(X, y, 1000, 100);

  // Set lambdas to 0.

  LARS lars(false, 0, 0);
  lars.FitIntercept(false);
  lars.NormalizeData(false);
  arma::vec betaOpt;
  lars.Train(X, y, betaOpt);

  // Get both row-major and column-major predictions.  Make sure they are the
  // same.
  arma::rowvec rowMajorPred, colMajorPred;

  lars.Predict(X, colMajorPred);
  lars.Predict(X.t(), rowMajorPred, true);

  REQUIRE(colMajorPred.n_elem == rowMajorPred.n_elem);
  for (size_t i = 0; i < colMajorPred.n_elem; ++i)
  {
    if (std::abs(colMajorPred[i]) < 1e-5)
      REQUIRE(rowMajorPred[i] == Approx(0.0).margin(1e-5));
    else
      REQUIRE(colMajorPred[i] == Approx(rowMajorPred[i]).epsilon(1e-7));
  }
}

/**
 * Make sure that if we train twice, there is no issue.
 */
TEST_CASE("LARSRetrainTest", "[LARSTest]")
{
  arma::mat origX;
  arma::rowvec origY;
  GenerateProblem(origX, origY, 1000, 50);

  arma::mat newX;
  arma::rowvec newY;
  GenerateProblem(newX, newY, 750, 75);

  LARS lars(false, 0.1, 0.1);
  lars.FitIntercept(false);
  lars.NormalizeData(false);
  arma::vec betaOpt;
  lars.Train(origX, origY, betaOpt);

  // Now train on new data.
  lars.Train(newX, newY, betaOpt);

  arma::vec errCorr = (newX * trans(newX) + 0.1 *
        arma::eye(75, 75)) * betaOpt - newX * newY.t();

  LARSVerifyCorrectness(betaOpt, errCorr, 0.1);
}

/**
 * Make sure if we train twice using the Cholesky decomposition, there is no
 * issue.
 */
TEST_CASE("RetrainCholeskyTest", "[LARSTest]")
{
  arma::mat origX;
  arma::rowvec origY;
  GenerateProblem(origX, origY, 1000, 50);

  arma::mat newX;
  arma::rowvec newY;
  GenerateProblem(newX, newY, 750, 75);

  LARS lars(true, 0.1, 0.1);
  lars.FitIntercept(false);
  lars.NormalizeData(false);
  arma::vec betaOpt;
  lars.Train(origX, origY, betaOpt);

  // Now train on new data.
  lars.Train(newX, newY, betaOpt);

  arma::vec errCorr = (newX * trans(newX) + 0.1 *
        arma::eye(75, 75)) * betaOpt - newX * newY.t();

  LARSVerifyCorrectness(betaOpt, errCorr, 0.1);
}

/**
 * Make sure that we get correct solution coefficients when running training
 * and accessing solution coefficients separately.
 */
TEST_CASE("TrainingAndAccessingBetaTest", "[LARSTest]")
{
  arma::mat X;
  arma::rowvec y;

  GenerateProblem(X, y, 1000, 100);

  LARS lars1;
  arma::vec beta;
  lars1.Train(X, y, beta);

  LARS lars2;
  lars2.Train(X, y);

  REQUIRE(beta.n_elem == lars2.Beta().n_elem);
  for (size_t i = 0; i < beta.n_elem; ++i)
    REQUIRE(beta[i] == Approx(lars2.Beta()[i]).epsilon(1e-7));
}

/**
 * Make sure that we learn the same when running training separately and through
 * constructor. Test it with default parameters.
 */
TEST_CASE("TrainingConstructorWithDefaultsTest", "[LARSTest]")
{
  arma::mat X;
  arma::rowvec y;

  GenerateProblem(X, y, 1000, 100);

  LARS lars1;
  arma::vec beta;
  lars1.Train(X, y, beta);

  LARS lars2(X, y);

  REQUIRE(beta.n_elem == lars2.Beta().n_elem);
  for (size_t i = 0; i < beta.n_elem; ++i)
    REQUIRE(beta[i] == Approx(lars2.Beta()[i]).epsilon(1e-7));
}

/**
 * Make sure that we learn the same when running training separately and through
 * constructor. Test it with non default parameters.
 */
TEST_CASE("TrainingConstructorWithNonDefaultsTest", "[LARSTest]")
{
  arma::mat X;
  arma::rowvec y;

  GenerateProblem(X, y, 1000, 100);

  bool transposeData = true;
  bool useCholesky = true;
  double lambda1 = 0.2;
  double lambda2 = 0.4;

  LARS lars1(useCholesky, lambda1, lambda2);
  arma::vec beta;
  lars1.Train(X, y, beta);

  LARS lars2(X, y, transposeData, useCholesky, lambda1, lambda2);

  REQUIRE(beta.n_elem == lars2.Beta().n_elem);
  for (size_t i = 0; i < beta.n_elem; ++i)
    REQUIRE(beta[i] == Approx(lars2.Beta()[i]).epsilon(1e-7));
}

/**
 * Test that LARS::Train() returns finite error value.
 */
TEST_CASE("LARSTrainReturnCorrelation", "[LARSTest]")
{
  arma::mat X;
  arma::mat Y;

  if (!data::Load("lars_dependent_x.csv", X))
    FAIL("Cannot load dataset lars_dependent_x.csv");
  if (!data::Load("lars_dependent_y.csv", Y))
    FAIL("Cannot load dataset lars_dependent_y.csv");

  arma::rowvec y = Y.row(0);

  double lambda1 = 0.1;
  double lambda2 = 0.1;

  // Test with Cholesky decomposition and with lasso.
  LARS lars1(true, lambda1, 0.0);
  arma::vec betaOpt1;
  double error = lars1.Train(X, y, betaOpt1);

  REQUIRE(std::isfinite(error) == true);

  // Test without Cholesky decomposition and with lasso.
  LARS lars2(false, lambda1, 0.0);
  arma::vec betaOpt2;
  error = lars2.Train(X, y, betaOpt2);

  REQUIRE(std::isfinite(error) == true);

  // Test with Cholesky decomposition and with elasticnet.
  LARS lars3(true, lambda1, lambda2);
  arma::vec betaOpt3;
  error = lars3.Train(X, y, betaOpt3);

  REQUIRE(std::isfinite(error) == true);

  // Test without Cholesky decomposition and with elasticnet.
  LARS lars4(false, lambda1, lambda2);
  arma::vec betaOpt4;
  error = lars4.Train(X, y, betaOpt4);

  REQUIRE(std::isfinite(error) == true);
}

/**
 * Test that LARS::ComputeError() returns error value less than 1
 * and greater than 0.
 */
TEST_CASE("LARSTestComputeError", "[LARSTest]")
{
  arma::mat X;
  arma::mat Y;

  if (!data::Load("lars_dependent_x.csv", X))
    FAIL("Cannot load dataset lars_dependent_x.csv");
  if (!data::Load("lars_dependent_y.csv", Y))
    FAIL("Cannot load dataset lars_dependent_y.csv");

  arma::rowvec y = Y.row(0);

  LARS lars1(true, 0.1, 0.0);
  lars1.FitIntercept(false);
  lars1.NormalizeData(false);
  arma::vec betaOpt1;
  double train1 = lars1.Train(X, y, betaOpt1);
  double cost = lars1.ComputeError(X, y);

  REQUIRE(cost <= 1);
  REQUIRE(cost >= 0);
  REQUIRE(cost == train1);
}

/**
 * Simple test for LARS copy constructor.
 */
TEST_CASE("LARSCopyConstructorTest", "[LARSTest]")
{
  arma::mat features, Y;
  arma::rowvec targets;

  // Load training input and predictions for testing.
  if (!data::Load("lars_dependent_x.csv", features))
    FAIL("Cannot load dataset lars_dependent_x.csv");
  if (!data::Load("lars_dependent_y.csv", Y))
    FAIL("Cannot load dataset lars_dependent_y.csv");
  targets = Y.row(0);

  // Check if the copy is accessible even after deleting the pointer to the
  // object.
  LARS* glm1 = new LARS(false, .1, .1);
  arma::rowvec predictions, predictionsFromCopiedModel;
  std::vector<LARS> models;
  glm1->Train(features, targets);
  glm1->Predict(features, predictions);
  models.emplace_back(*glm1); // Call the copy constructor.
  delete glm1; // Free LARS internal memory.
  models[0].Predict(features, predictionsFromCopiedModel);
  // The output of both models should be the same.
  CheckMatrices(predictions, predictionsFromCopiedModel);
  // Check if we can train the model again.
  REQUIRE_NOTHROW(models[0].Train(features, targets));

  // Check if we can train the copied model.
  LARS glm2(false, 0.1, 0.1);
  models.emplace_back(glm2); // Call the copy constructor.
  REQUIRE_NOTHROW(glm2.Train(features, targets));
  REQUIRE_NOTHROW(models[1].Train(features, targets));

  // Create a copy using assignment operator.
  LARS glm3 = glm2;
  models[1].Predict(features, predictions);
  glm3.Predict(features, predictionsFromCopiedModel);
  // The output of both models should be the same.
  CheckMatrices(predictions, predictionsFromCopiedModel);
}

/**
 * Test that fitting an intercept is the same as scaling data.
 */
TEST_CASE("LARSFitInterceptTest", "[LARSTest]")
{
  arma::mat features = arma::randu<arma::mat>(10, 100);
  arma::rowvec responses = arma::randu<arma::rowvec>(100);

  arma::mat centeredFeatures = features.each_col() - arma::mean(features, 1);
  arma::rowvec centeredResponses = responses - arma::mean(responses);

  LARS l1(features, responses, true, true, 0.001, 0.001, 1e-16, true, false);
  LARS l2(centeredFeatures, centeredResponses, true, true, 0.001, 0.001, 1e-16, false, false);

  // The weights learned should be the same.
  REQUIRE(l1.Beta().n_elem == l2.Beta().n_elem);
  CheckMatrices(l1.Beta(), l2.Beta());
  REQUIRE(l1.Intercept() == Approx(arma::mean(responses) -
      arma::dot(arma::mean(features, 1), l1.Beta())));
}

// Make sure that Predict() provides reasonable enough solutions when we are
// fitting an intercept.
TEST_CASE("PredictFitInterceptTest", "[LARSTest]")
{
  for (size_t i = 0; i < 2; ++i)
  {
    // Run with both true and false.
    bool useCholesky = bool(i);

    arma::mat X;
    arma::rowvec y;

    GenerateProblem(X, y, 1000, 100);

    for (double lambda1 = 0.0; lambda1 < 1.0; lambda1 += 0.2)
    {
      for (double lambda2 = 0.0; lambda2 < 1.0; lambda2 += 0.2)
      {
        LARS lars(useCholesky, lambda1, lambda2);
        lars.FitIntercept(true);
        lars.NormalizeData(false);
        arma::vec betaOpt;
        lars.Train(X, y, betaOpt);
        const double intercept = arma::mean(y) -
            arma::dot(arma::mean(X, 1), betaOpt);

        // Calculate what the actual error should be with these regression
        // parameters.
        arma::vec betaOptPred = X.t() * betaOpt + intercept;
        arma::rowvec predictions;
        lars.Predict(X, predictions);
        arma::vec adjPred = predictions.t();

        REQUIRE(predictions.n_elem == 1000);
        for (size_t i = 0; i < betaOptPred.n_elem; ++i)
        {
          if (std::abs(betaOptPred[i]) < 1e-5)
            REQUIRE(adjPred[i] == Approx(0.0).margin(1e-5));
          else
            REQUIRE(adjPred[i] == Approx(betaOptPred[i]).epsilon(1e-7));
        }
      }
    }
  }
}

// Make sure that Predict() provides reasonable enough solutions when we are
// normalizing data.
TEST_CASE("PredictNormalizeDataTest", "[LARSTest]")
{
  for (size_t i = 0; i < 2; ++i)
  {
    // Run with both true and false.
    bool useCholesky = bool(i);

    arma::mat X;
    arma::rowvec y;

    GenerateProblem(X, y, 1000, 100);

    for (double lambda1 = 0.0; lambda1 < 1.0; lambda1 += 0.2)
    {
      for (double lambda2 = 0.0; lambda2 < 1.0; lambda2 += 0.2)
      {
        LARS lars(useCholesky, lambda1, lambda2);
        lars.FitIntercept(false);
        lars.NormalizeData(true);
        arma::vec betaOpt;
        lars.Train(X, y, betaOpt);

        // Calculate what the actual error should be with these regression
        // parameters.
        arma::vec betaOptPred = (X * X.t()) * betaOpt;
        arma::rowvec predictions;
        lars.Predict(X, predictions);
        arma::vec adjPred = X * predictions.t();

        REQUIRE(predictions.n_elem == 1000);
        for (size_t i = 0; i < betaOptPred.n_elem; ++i)
        {
          if (std::abs(betaOptPred[i]) < 1e-5)
            REQUIRE(adjPred[i] == Approx(0.0).margin(1e-5));
          else
            REQUIRE(adjPred[i] == Approx(betaOptPred[i]).epsilon(1e-7));
        }
      }
    }
  }
}

// Make sure that Predict() provides reasonable enough solutions when we are
// fitting an intercept and normalizing data.
TEST_CASE("PredictFitInterceptNormalizeDataTest", "[LARSTest]")
{
  for (size_t i = 0; i < 2; ++i)
  {
    // Run with both true and false.
    bool useCholesky = bool(i);

    arma::mat X;
    arma::rowvec y;

    GenerateProblem(X, y, 1000, 100);

    for (double lambda1 = 0.0; lambda1 < 1.0; lambda1 += 0.2)
    {
      for (double lambda2 = 0.0; lambda2 < 1.0; lambda2 += 0.2)
      {
        LARS lars(useCholesky, lambda1, lambda2);
        lars.FitIntercept(true);
        lars.NormalizeData(true);
        arma::vec betaOpt;
        lars.Train(X, y, betaOpt);
        const double intercept = arma::mean(y) -
            arma::dot(arma::mean(X, 1), betaOpt);

        // Calculate what the actual error should be with these regression
        // parameters.
        arma::vec betaOptPred = X.t() * betaOpt + intercept;
        arma::rowvec predictions;
        lars.Predict(X, predictions);
        arma::vec adjPred = predictions.t();

        REQUIRE(predictions.n_elem == 1000);
        for (size_t i = 0; i < betaOptPred.n_elem; ++i)
        {
          if (std::abs(betaOptPred[i]) < 1e-5)
            REQUIRE(adjPred[i] == Approx(0.0).margin(1e-5));
          else
            REQUIRE(adjPred[i] == Approx(betaOptPred[i]).epsilon(1e-7));
        }
      }
    }
  }
}

/**
 * Verify that KKT conditions are satisfied for a solution of the standard Lasso
 * problem (with X, y and lambda) (own version)
 *
 * A solution \bar{beta} of the standard Lasso problem verifies
 *
 * \bar{beta} = argmin 1/2* || X.beta-y||^2 + lambda*|beta|_1
 *
 * @param beta vector of double values, computed solution of the standard Lasso problem
 * @param X matrix of the standard Lasso problem
 * @param y vector of doubles, from the standard Lasso problem, as an armadillo rowvec
 * @param lambda double, parameter in the standard Lasso problem
 */
void CheckKKT(const arma::vec& beta,
              const arma::mat& X,
              const arma::rowvec& y,
              const double lambda)
{
  const double epsilon = 1e-10; // For numerical precision.

  arma::vec v = X.t() * X * beta - X.t() * y.t() + lambda * arma::sign(beta);
  // Active set indices with global numbering: could be empty.
  arma::uvec ia = arma::find(arma::abs(beta) > epsilon);
  // Zero indices with global numbering: could be empty.
  arma::uvec iz = arma::find(arma::abs(beta) <= epsilon);

  // Should be zero if beta is the solution.
  const double crit = arma::dot(beta, v);
  REQUIRE(std::abs(crit) < epsilon);

  // v should be zero at the Active Set ia.
  REQUIRE(arma::all(arma::abs(v(ia)) <= epsilon));

  // We should have abs(v) <= lambda at zero values Iz
  REQUIRE(arma::all(v(iz) >= -lambda));
  REQUIRE(arma::all(v(iz) <= lambda));
}

TEST_CASE("LARSTestKKT", "[LARSTest]")
{
  // Each row of F corresponds to a test.
  //
  // For each test i,
  //    F(0, i) is the matrix of covariates X, and
  //    F(1, i) is the matrix (vector) of responses/observations y.
  arma::field<arma::mat> F;
  F.load("lars_kkt.bin");

  arma::vec beta;
  bool useCholesky = true;
  LARS lars(useCholesky, 1.0, 0.0);

  for (size_t i = 0; i < F.n_cols; i++)
  {
    arma::mat X = std::move(F(0, i));
    arma::rowvec y = std::move(F(1, i));
    const arma::rowvec xMean = arma::mean(X, 0);
    arma::rowvec xStds = arma::stddev(X, 0, 0);
    xStds.replace(0.0, 1.0);
    const double yMean = arma::mean(y);

    lars.FitIntercept(false);
    lars.NormalizeData(false);
    lars.Train(X, y, beta, false);
    CheckKKT(beta, X, y, 1.0);

    // Now try when we fit an intercept too.
    lars.FitIntercept(true);
    lars.NormalizeData(false);
    lars.Train(X, y, beta, false);

    // Now mean-center data before the check.
    X.each_row() -= xMean;
    y -= yMean;

    CheckKKT(beta, X, y, 1.0);

    X.each_row() += xMean;
    y += yMean;

    // Now try when we normalize the data.
    lars.FitIntercept(false);
    lars.NormalizeData(true);
    lars.Train(X, y, beta, false);

    X.each_row() /= xStds;
    beta %= xStds.t();

    CheckKKT(beta, X, y, 1.0);

    X.each_row() %= xStds;

    lars.FitIntercept(true);
    lars.NormalizeData(true);
    lars.Train(X, y, beta, false);

    X.each_row() -= xMean;
    X.each_row() /= xStds;
    beta %= xStds.t();
    y -= yMean;

    CheckKKT(beta, X, y, 1.0);
  }
}
