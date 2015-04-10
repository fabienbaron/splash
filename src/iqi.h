#ifndef _iqi_h
#define _iqi_h

// Image Quality Index class
// inheriting SimilarityMetric
class calcQualityIndex : public SimilarityMetric {

  private:
    int B; // blur block size value for filtering
    Mat image_quality_map;
    Scalar image_quality_value; // image_quality_index value

  public:

    calcQualityIndex();
    
    ~calcQualityIndex();

    // get and setfunctions    
    void setB(int val) { B = val; }

    Scalar getImageQuality() { return image_quality_value; }
    Mat getImageQuality_map() { return image_quality_map; }

    void releaseImageQuality_map();

    // prints the index_map
    int print_map();
    
    // returns the iqi_value using openCV functions
    virtual Scalar compare(Mat& source1, Mat& source2);

};

#endif
