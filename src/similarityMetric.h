#ifndef _similarityMetric_h
#define _similarityMetric_h

// Image SimilarityMetric Base Class
class SimilarityMetric
{

public:

  // virtual functions compare, returns CvScalar value and has separate implementation for each of the algorithms
  virtual float compare(const Mat &src1, const Mat &src2) = 0;

  // function to convert image into different colorspaces based upon the enum Colorspace

};


#endif
