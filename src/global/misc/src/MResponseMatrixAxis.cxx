/*
 * MResponseMatrixAxis.cxx
 *
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */


// Include the header:
#include "MResponseMatrixAxis.h"

// Standard libs:
#include <algorithm>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MExceptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CLING___
ClassImp(MResponseMatrixAxis)
#endif


////////////////////////////////////////////////////////////////////////////////


//! Default constructor
MResponseMatrixAxis::MResponseMatrixAxis(const MString& Name) : m_Dimension(1), m_NumberOfBins(0), m_IsLogarithmic(false)
{
  m_Names.push_back(Name);
}


////////////////////////////////////////////////////////////////////////////////


//! Default destructor
MResponseMatrixAxis::~MResponseMatrixAxis()
{
}


////////////////////////////////////////////////////////////////////////////////


//! Clone this axis
MResponseMatrixAxis* MResponseMatrixAxis::Clone() const
{
  MResponseMatrixAxis* Axis = new MResponseMatrixAxis(m_Names[0]); // Takes care of names and dimension
  Axis->m_BinEdges = m_BinEdges;
  Axis->m_NumberOfBins = m_NumberOfBins;
  Axis->m_IsLogarithmic = m_IsLogarithmic;
  
  return Axis;
}


////////////////////////////////////////////////////////////////////////////////


//! Equality operator
bool MResponseMatrixAxis::operator==(const MResponseMatrixAxis& Axis) const
{  
  if (m_Dimension != Axis.m_Dimension) {
    return false;
  }
    
  if (m_BinEdges != Axis.m_BinEdges) {
    return false;
  }
  
  // bins and logarithmic taken care of with bin edges
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


//! Set the bin edges
void MResponseMatrixAxis::SetBinEdges(vector<double> BinEdges) 
{ 
  m_BinEdges = BinEdges;
  
  if (m_BinEdges.size() <= 1) { 
    m_NumberOfBins = 0; 
  } else { 
    m_NumberOfBins = m_BinEdges.size() - 1; 
  }
}


////////////////////////////////////////////////////////////////////////////////


//! Set the axis in linear mode
void MResponseMatrixAxis::SetLinear(unsigned long NBins, double Min, double Max, double UnderFlowMin, double OverFlowMax)
{
  if (Min >= Max) {
    throw MExceptionTestFailed("Minimum is larger or equal Maximum", Min, ">=", Max); 
  }
  if (UnderFlowMin != g_DoubleNotDefined && UnderFlowMin >= Min) {
    throw MExceptionTestFailed("The underflow must be smaller than the minimum", UnderFlowMin, ">=", Min); 
  }
  if (OverFlowMax != g_DoubleNotDefined && OverFlowMax <= Max) {
    throw MExceptionTestFailed("The overlow must be larger than the maximum", OverFlowMax, "<=", Max); 
  }
  
  m_BinEdges.clear();
  
  if (UnderFlowMin != g_DoubleNotDefined) {
    m_BinEdges.push_back(UnderFlowMin);
  }
  
  double Dist = (Max-Min)/(NBins);
  
  for (unsigned long i = 0; i < NBins+1; ++i) {
    m_BinEdges.push_back(Min+i*Dist);
  }
  
  if (OverFlowMax != g_DoubleNotDefined) {
    m_BinEdges.push_back(OverFlowMax);
  }
  
  /*
  if (Inverted == true) {
    vector<double> Temp = m_BinEdges;
    for (unsigned long i = 1; i < Temp.size()-1; ++i) {
      m_BinEdges[i] = m_BinEdges[i-1] + (Temp[Temp.size()-i]-Temp[Temp.size()-i-1]);
    }
  }
  
  for (unsigned long i = 0; i < m_BinEdges.size(); ++i) {
    m_BinEdges[i] += Offset;
  }
  */
  
  if (m_BinEdges.size() <= 1) { 
    m_NumberOfBins = 0; 
  } else { 
    m_NumberOfBins = m_BinEdges.size() - 1; 
  }
}


////////////////////////////////////////////////////////////////////////////////


//! Set the axis in logarithmic mode
void MResponseMatrixAxis::SetLogarithmic(unsigned long NBins, double Min, double Max, double UnderFlowMin, double OverFlowMax)
{
  if (Min <= 0) {
    throw MExceptionTestFailed("The minimum must be larger than 0", Min, "<=", 0); 
  }
  if (Min >= Max) {
    throw MExceptionTestFailed("Minimum is larger or equal Maximum", Min, ">=", Max); 
  }
  if (UnderFlowMin != g_DoubleNotDefined && UnderFlowMin >= Min) {
    throw MExceptionTestFailed("The underflow must be smaller than the minimum", UnderFlowMin, ">=", Min); 
  }
  if (OverFlowMax != g_DoubleNotDefined && OverFlowMax <= Max) {
    throw MExceptionTestFailed("The overlow must be larger than the maximum", OverFlowMax, "<=", Max); 
  }
  
  m_BinEdges.clear();
  
  if (UnderFlowMin != g_DoubleNotDefined) {
    m_BinEdges.push_back(UnderFlowMin);
  }
  
  Min = log(Min);
  Max = log(Max);
  double Dist = (Max-Min)/(NBins);
  
  for (unsigned long i = 0; i < NBins+1; ++i) {
    m_BinEdges.push_back(exp(Min+i*Dist));
  }
  
  if (OverFlowMax != g_DoubleNotDefined) {
    m_BinEdges.push_back(OverFlowMax);
  }

  m_IsLogarithmic = true;
  
  /*
  if (Inverted == true) {
    vector<double> Temp = Axis;
    for (unsigned long i = 1; i < Temp.size()-1; ++i) {
      Axis[i] = Axis[i-1] + (Temp[Temp.size()-i]-Temp[Temp.size()-i-1]);
    }
  }
  
  for (unsigned long i = 0; i < Axis.size(); ++i) {
    Axis[i] += Offset;
  }
  */
  
  if (m_BinEdges.size() <= 1) { 
    m_NumberOfBins = 0; 
  } else { 
    m_NumberOfBins = m_BinEdges.size() - 1; 
  }
}


////////////////////////////////////////////////////////////////////////////////


//! Return the axis bin given a axis value
unsigned long MResponseMatrixAxis::GetAxisBin(double X1, double X2) const
{
  if (m_BinEdges.size() < 2) {
    throw MExceptionArbitrary("Not enough bin edges (< 2) to determine bins");
  }
  
  if (InRange(X1, X2) == false) {
    throw MExceptionValueOutOfBounds(m_BinEdges.front(), m_BinEdges.back(), X1);
  }
  
  // C++ version
  auto UpperBound = upper_bound(m_BinEdges.begin(), m_BinEdges.end(), X1);
  unsigned long Bin = UpperBound - m_BinEdges.begin() - 1;

  // upper_bound is [) - let's take care of the case if we are at the highest bin value
  if (X1 == m_BinEdges.back()) Bin = m_BinEdges.size() - 2; // -2 since we have edges, i.e. one more than bins
  
  if (Bin >= m_BinEdges.size() - 1) {
    throw MExceptionIndexOutOfBounds(0, m_BinEdges.size() - 1, Bin);
  }
  
  return Bin;
}


////////////////////////////////////////////////////////////////////////////////


//! Return the area of the given axis bin
double MResponseMatrixAxis::GetArea(unsigned long Bin) const
{
  if (Bin >= m_BinEdges.size() - 1) {
    throw MExceptionIndexOutOfBounds(0, m_BinEdges.size() - 1, Bin);
  }
  
  return m_BinEdges[Bin+1] - m_BinEdges[Bin];
}


////////////////////////////////////////////////////////////////////////////////


//! Test if the x-value is within the range of the axis
bool MResponseMatrixAxis::InRange(double X1, double X2) const
{
  if (m_BinEdges.size() < 2) {
    return false;
  }
  
  if (X1 < m_BinEdges.front() || X1 > m_BinEdges.back()) {
    return false;
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


//! Return the axis bins for drawing --- those might be narrower than the real bins
vector<vector<double>> MResponseMatrixAxis::GetDrawingAxisBinEdges() const
{
  vector<vector<double>> AxisBinEdges;
  
  AxisBinEdges.push_back(m_BinEdges);
  
  return AxisBinEdges;
}


////////////////////////////////////////////////////////////////////////////////


//! Return the minimum axis values
vector<double> MResponseMatrixAxis::GetMinima() const
{
  if (m_BinEdges.size() == 0) {
    throw MExceptionArbitrary("Empty bin edge vector --- cannot determine minima");
  }
  
  return { m_BinEdges[0] };
}


////////////////////////////////////////////////////////////////////////////////


//! Return the minimum axis values
vector<double> MResponseMatrixAxis::GetMaxima() const
{
  if (m_BinEdges.size() == 0) {
    throw MExceptionArbitrary("Empty bin edge vector --- cannot determine minima");
  }
  
  return { m_BinEdges.back() };
  
}


////////////////////////////////////////////////////////////////////////////////


//! Return the bin center(s) of the given axis bin
//! Can throw: MExceptionIndexOutOfBounds
vector<double> MResponseMatrixAxis::GetBinCenters(unsigned long Bin) const
{
  if (Bin >= m_BinEdges.size() - 1) {
    throw MExceptionIndexOutOfBounds(0, m_BinEdges.size() - 1, Bin);
  }
  
  return vector<double> { 0.5 *(m_BinEdges[Bin] + m_BinEdges[Bin+1]) };
}


////////////////////////////////////////////////////////////////////////////////


//! Write the content to a stream
void MResponseMatrixAxis::Write(ostringstream& out)
{
  out<<"# Axis name"<<endl;
  out<<"AN \""<<m_Names[0]<<"\""<<endl;
  out<<"# Axis type"<<endl;
  out<<"AT 1D BinEdges"<<endl;
  out<<"# Axis data"<<endl;
  out<<"AD ";
  for (unsigned long b = 0; b < m_BinEdges.size(); ++b) {
    out<<m_BinEdges[b]<<" ";
  }
  out<<endl;
}


// MResponseMatrixAxis.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
