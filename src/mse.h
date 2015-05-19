#ifndef _mse_h
#define _mse_h

// Mean Square Error (MSE) class
// inheriting from Similarity class
class calcMSE : public SimilarityMetric
{

private:
  Scalar mse;

public:

  calcMSE();

  // get functions
  Scalar getMSE()
  {
    return mse;
  }

  // calculates mse using openCV functions
  virtual float compare(const Mat &mat1, const Mat &mat2);

};

#endif
