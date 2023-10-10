/**
 * @file methods/local_coordinate_coding/lcc_impl.hpp
 * @author Nishant Mehta
 *
 * Implementation of Local Coordinate Coding
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_LOCAL_COORDINATE_CODING_LCC_IMPL_HPP
#define MLPACK_METHODS_LOCAL_COORDINATE_CODING_LCC_IMPL_HPP

// In case it hasn't been included yet.
#include "lcc.hpp"

namespace mlpack {

template<typename DictionaryInitializer>
LocalCoordinateCoding::LocalCoordinateCoding(
    const arma::mat& data,
    const size_t atoms,
    const double lambda,
    const size_t maxIterations,
    const double tolerance,
    const DictionaryInitializer& initializer) :
    atoms(atoms),
    lambda(lambda),
    maxIterations(maxIterations),
    tolerance(tolerance)
{
  // Train the model.
  Train(data, initializer);
}

inline LocalCoordinateCoding::LocalCoordinateCoding(
    const size_t atoms,
    const double lambda,
    const size_t maxIterations,
    const double tolerance) :
    atoms(atoms),
    lambda(lambda),
    maxIterations(maxIterations),
    tolerance(tolerance)
{
  // Nothing to do.
}

template<typename DictionaryInitializer>
double LocalCoordinateCoding::Train(
    const arma::mat& data,
    const DictionaryInitializer& initializer)
{
  // Initialize the dictionary.
  initializer.Initialize(data, atoms, dictionary);

  double lastObjVal = DBL_MAX;

  // Take the initial coding step, which has to happen before entering the main
  // loop.
  Log::Info << "Initial Coding Step." << std::endl;

  arma::mat codes;
  Encode(data, codes);
  arma::uvec adjacencies = find(codes);

  Log::Info << "  Sparsity level: " << 100.0 * ((double)(adjacencies.n_elem)) /
      ((double)(atoms * data.n_cols)) << "%.\n";
  Log::Info << "  Objective value: " << Objective(data, codes, adjacencies)
      << "." << std::endl;

  for (size_t t = 1; t != maxIterations; t++)
  {
    Log::Info << "Iteration " << t << " of " << maxIterations << "."
        << std::endl;

    // First step: optimize the dictionary.
    Log::Info << "Performing dictionary step..." << std::endl;
    OptimizeDictionary(data, codes, adjacencies);
    double dsObjVal = Objective(data, codes, adjacencies);
    Log::Info << "  Objective value: " << dsObjVal << "." << std::endl;

    // Second step: perform the coding.
    Log::Info << "Performing coding step..." << std::endl;
    Encode(data, codes);
    adjacencies = find(codes);
    Log::Info << "  Sparsity level: " << 100.0 * ((double) (adjacencies.n_elem))
        / ((double)(atoms * data.n_cols)) << "%.\n";

    // Terminate if the objective increased in the coding step.
    double curObjVal = Objective(data, codes, adjacencies);
    if (curObjVal > dsObjVal)
    {
      Log::Warn << "Objective increased in coding step!  Terminating."
          << std::endl;
      break;
    }

    // Find the new objective value and improvement so we can check for
    // convergence.
    double improvement = lastObjVal - curObjVal;
    Log::Info << "Objective value: " << curObjVal << " (improvement "
        << std::scientific << improvement << ")." << std::endl;

    if (improvement < tolerance)
    {
      Log::Info << "Converged within tolerance " << tolerance << ".\n";
      break;
    }

    lastObjVal = curObjVal;
  }

  return lastObjVal;
}

inline void LocalCoordinateCoding::Encode(const arma::mat& data,
                                          arma::mat& codes)
{
  arma::mat invSqDists = 1.0 / (repmat(trans(sum(square(dictionary))), 1,
      data.n_cols) + repmat(sum(square(data)), atoms, 1) - 2 * trans(dictionary)
      * data);

  arma::mat dictGram = trans(dictionary) * dictionary;
  arma::mat dictGramTD(dictGram.n_rows, dictGram.n_cols);

  codes.set_size(atoms, data.n_cols);
  for (size_t i = 0; i < data.n_cols; ++i)
  {
    // Report progress.
    if ((i % 100) == 0)
    {
      Log::Debug << "Optimization at point " << i << "." << std::endl;
    }

    arma::vec invW = invSqDists.unsafe_col(i);
    arma::mat dictPrime = dictionary * diagmat(invW);

    arma::mat dictGramTD = diagmat(invW) * dictGram * diagmat(invW);

    bool useCholesky = false;
    // Normalization and fitting and intercept are disabled.
    LARS lars(useCholesky, dictGramTD, 0.5 * lambda, 0,
        1e-16 /* default tolerance */, false, false);

    // Run LARS for this point, by making an alias of the point and passing
    // that.
    arma::vec beta = codes.unsafe_col(i);
    arma::rowvec responses = data.unsafe_col(i).t();
    lars.Train(dictPrime, responses, beta, false);
    beta %= invW; // Remember, beta is an alias of codes.col(i).
  }
}

inline void LocalCoordinateCoding::OptimizeDictionary(
    const arma::mat& data,
    const arma::mat& codes,
    const arma::uvec& adjacencies)
{
  // Count number of atomic neighbors for each point x^i.
  arma::uvec neighborCounts = arma::zeros<arma::uvec>(data.n_cols, 1);
  if (adjacencies.n_elem > 0)
  {
    // This gets the column index.  Intentional integer division.
    size_t curPointInd = (size_t) (adjacencies(0) / atoms);
    ++neighborCounts(curPointInd);

    size_t nextColIndex = (curPointInd + 1) * atoms;
    for (size_t l = 1; l < adjacencies.n_elem; l++)
    {
      // If l no longer refers to an element in this column, advance the column
      // number accordingly.
      if (adjacencies(l) >= nextColIndex)
      {
        curPointInd = (size_t) (adjacencies(l) / atoms);
        nextColIndex = (curPointInd + 1) * atoms;
      }

      ++neighborCounts(curPointInd);
    }
  }

  // Build dataPrime := [X x^1 ... x^1 ... x^n ... x^n]
  // where each x^i is repeated for the number of neighbors x^i has.
  arma::mat dataPrime = arma::zeros(data.n_rows,
      data.n_cols + adjacencies.n_elem);

  dataPrime(arma::span::all, arma::span(0, data.n_cols - 1)) = data;

  size_t curCol = data.n_cols;
  for (size_t i = 0; i < data.n_cols; ++i)
  {
    if (neighborCounts(i) > 0)
    {
      dataPrime(arma::span::all, arma::span(curCol, curCol + neighborCounts(i)
          - 1)) = repmat(data.col(i), 1, neighborCounts(i));
    }
    curCol += neighborCounts(i);
  }

  // Handle the case of inactive atoms (atoms not used in the given coding).
  std::vector<size_t> inactiveAtoms;
  for (size_t j = 0; j < atoms; ++j)
    if (accu(codes.row(j) != 0) == 0)
      inactiveAtoms.push_back(j);

  const size_t nInactiveAtoms = inactiveAtoms.size();
  const size_t nActiveAtoms = atoms - nInactiveAtoms;

  // Efficient construction of codes restricted to active atoms.
  arma::mat codesPrime = arma::zeros(nActiveAtoms, data.n_cols +
      adjacencies.n_elem);
  arma::vec wSquared = arma::ones(data.n_cols + adjacencies.n_elem, 1);

  if (nInactiveAtoms > 0)
  {
    Log::Warn << "There are " << nInactiveAtoms
        << " inactive atoms.  They will be re-initialized randomly.\n";

    // Create matrix holding only active codes.
    arma::mat activeCodes;
    RemoveRows(codes, inactiveAtoms, activeCodes);

    // Create reverse atom lookup for active atoms.
    arma::uvec atomReverseLookup(atoms);
    size_t inactiveOffset = 0;
    for (size_t i = 0; i < atoms; ++i)
    {
      if (inactiveAtoms[inactiveOffset] == i)
        ++inactiveOffset;
      else
        atomReverseLookup(i - inactiveOffset) = i;
    }

    codesPrime(arma::span::all, arma::span(0, data.n_cols - 1)) = activeCodes;

    // Fill the rest of codesPrime.
    for (size_t l = 0; l < adjacencies.n_elem; ++l)
    {
      // Recover the location in the codes matrix that this adjacency refers to.
      size_t atomInd = adjacencies(l) % atoms;
      size_t pointInd = (size_t) (adjacencies(l) / atoms);

      // Fill matrix.
      codesPrime(atomReverseLookup(atomInd), data.n_cols + l) = 1.0;
      wSquared(data.n_cols + l) = codes(atomInd, pointInd);
    }
  }
  else
  {
    // All atoms are active.
    codesPrime(arma::span::all, arma::span(0, data.n_cols - 1)) = codes;

    for (size_t l = 0; l < adjacencies.n_elem; ++l)
    {
      // Recover the location in the codes matrix that this adjacency refers to.
      size_t atomInd = adjacencies(l) % atoms;
      size_t pointInd = (size_t) (adjacencies(l) / atoms);

      // Fill matrix.
      codesPrime(atomInd, data.n_cols + l) = 1.0;
      wSquared(data.n_cols + l) = codes(atomInd, pointInd);
    }
  }

  wSquared.subvec(data.n_cols, wSquared.n_elem - 1) = lambda *
      abs(wSquared.subvec(data.n_cols, wSquared.n_elem - 1));

  // Solve system.
  if (nInactiveAtoms == 0)
  {
    // No inactive atoms.  We can solve directly.
    arma::mat A = codesPrime * diagmat(wSquared) * trans(codesPrime);
    arma::mat B = codesPrime * diagmat(wSquared) * trans(dataPrime);

    dictionary = trans(solve(A, B));
    /*
    dictionary = trans(solve(codesPrime * diagmat(wSquared) * trans(codesPrime),
        codesPrime * diagmat(wSquared) * trans(dataPrime)));
    */
  }
  else
  {
    // Inactive atoms must be reinitialized randomly, so we cannot solve
    // directly for the entire dictionary estimate.
    arma::mat dictionaryActive =
      trans(solve(codesPrime * diagmat(wSquared) * trans(codesPrime),
                  codesPrime * diagmat(wSquared) * trans(dataPrime)));

    // Update all atoms.
    size_t currentInactiveIndex = 0;
    for (size_t i = 0; i < atoms; ++i)
    {
      if (inactiveAtoms[currentInactiveIndex] == i)
      {
        // This atom is inactive.  Reinitialize it randomly.
        dictionary.col(i) = (data.col(RandInt(data.n_cols)) +
                             data.col(RandInt(data.n_cols)) +
                             data.col(RandInt(data.n_cols)));

        // Now normalize the atom.
        dictionary.col(i) /= norm(dictionary.col(i), 2);

        // Increment inactive atom counter.
        ++currentInactiveIndex;
      }
      else
      {
        // Update estimate.
        dictionary.col(i) = dictionaryActive.col(i - currentInactiveIndex);
      }
    }
  }
}

inline double LocalCoordinateCoding::Objective(
    const arma::mat& data,
    const arma::mat& codes,
    const arma::uvec& adjacencies) const
{
  double weightedL1NormZ = 0;

  for (size_t l = 0; l < adjacencies.n_elem; l++)
  {
    // Map adjacency back to its location in the codes matrix.
    const size_t atomInd = adjacencies(l) % atoms;
    const size_t pointInd = (size_t) (adjacencies(l) / atoms);

    weightedL1NormZ += fabs(codes(atomInd, pointInd)) * arma::as_scalar(
        arma::sum(arma::square(dictionary.col(atomInd) - data.col(pointInd))));
  }

  double froNormResidual = norm(data - dictionary * codes, "fro");
  return std::pow(froNormResidual, 2.0) + lambda * weightedL1NormZ;
}

template<typename Archive>
void LocalCoordinateCoding::serialize(Archive& ar,
                                      const uint32_t /* version */)
{
  ar(CEREAL_NVP(atoms));
  ar(CEREAL_NVP(dictionary));
  ar(CEREAL_NVP(lambda));
  ar(CEREAL_NVP(maxIterations));
  ar(CEREAL_NVP(tolerance));
}

} // namespace mlpack

#endif
