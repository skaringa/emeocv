/*
 * KNearestOcr.h
 *
 */

#ifndef KNEARESTOCR_H_
#define KNEARESTOCR_H_

#include <vector>
#include <list>
#include <string>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

#include "Config.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

class KNearestOcr {
public:
    KNearestOcr(const Config & config);
    virtual ~KNearestOcr();

    int learn(const cv::Mat & img);
    int learn(const std::vector<cv::Mat> & images);
    void saveTrainingData();
    bool loadTrainingData();

    char recognize(const cv::Mat & img);
    std::string recognize(const std::vector<cv::Mat> & images);

private:
    cv::Mat prepareSample(const cv::Mat & img);
    void initModel();

    cv::Mat _samples;
    cv::Mat _responses;
    Ptr <KNearest> _pModel;
    Config _config;
};

#endif /* KNEARESTOCR_H_ */
