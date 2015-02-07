/*
 * ImageProcessor.cpp
 *
 */

#include <vector>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#include "ImageProcessor.h"
#include "Config.h"

/**
 * Functor to help sorting rectangles by their x-position.
 */
class sortRectByX {
public:
    bool operator()(cv::Rect const & a, cv::Rect const & b) const {
        return a.x < b.x;
    }
};

ImageProcessor::ImageProcessor(const Config & config) :
        _config(config), _debugWindow(false), _debugSkew(false), _debugDigits(false), _debugEdges(false) {
}

/**
 * Set the input image.
 */
void ImageProcessor::setInput(cv::Mat & img) {
    _img = img;
}

/**
 * Get the vector of output images.
 * Each image contains the edges of one isolated digit.
 */
const std::vector<cv::Mat> & ImageProcessor::getOutput() {
    return _digits;
}

void ImageProcessor::debugWindow(bool bval) {
    _debugWindow = bval;
    if (_debugWindow) {
        cv::namedWindow("ImageProcessor");
    }
}

void ImageProcessor::debugSkew(bool bval) {
    _debugSkew = bval;
}

void ImageProcessor::debugEdges(bool bval) {
    _debugEdges = bval;
}

void ImageProcessor::debugDigits(bool bval) {
    _debugDigits = bval;
}

void ImageProcessor::showImage() {
    cv::imshow("ImageProcessor", _img);
    cv::waitKey(1);
}

/**
 * Main processing function.
 * Read input image and create vector of images for each digit.
 */
void ImageProcessor::process() {
    _digits.clear();

    // convert to gray
    cvtColor(_img, _imgGray, CV_BGR2GRAY);

    // initial rotation to get the digits up
    rotate(_config.getRotationDegrees());

    // detect and correct remaining skew (+- 30 deg)
    float skew_deg = detectSkew();
    rotate(skew_deg);

    // find and isolate counter digits
    findCounterDigits();

    if (_debugWindow) {
        showImage();
    }
}

/**
 * Rotate image.
 */
void ImageProcessor::rotate(double rotationDegrees) {
    cv::Mat M = cv::getRotationMatrix2D(cv::Point(_imgGray.cols / 2, _imgGray.rows / 2), rotationDegrees, 1);
    cv::Mat img_rotated;
    cv::warpAffine(_imgGray, img_rotated, M, _imgGray.size());
    _imgGray = img_rotated;
    if (_debugWindow) {
        cv::warpAffine(_img, img_rotated, M, _img.size());
        _img = img_rotated;
    }
}

/**
 * Draw lines into image.
 * For debugging purposes.
 */
void ImageProcessor::drawLines(std::vector<cv::Vec2f>& lines) {
    // draw lines
    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0];
        float theta = lines[i][1];
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
        cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
        cv::line(_img, pt1, pt2, cv::Scalar(255, 0, 0), 1);
    }
}

/**
 * Draw lines into image.
 * For debugging purposes.
 */
void ImageProcessor::drawLines(std::vector<cv::Vec4i>& lines, int xoff, int yoff) {
    for (size_t i = 0; i < lines.size(); i++) {
        cv::line(_img, cv::Point(lines[i][0] + xoff, lines[i][1] + yoff),
                cv::Point(lines[i][2] + xoff, lines[i][3] + yoff), cv::Scalar(255, 0, 0), 1);
    }
}

/**
 * Detect the skew of the image by finding almost (+- 30 deg) horizontal lines.
 */
float ImageProcessor::detectSkew() {
    log4cpp::Category& rlog = log4cpp::Category::getRoot();

    cv::Mat edges = cannyEdges();

    // find lines
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 180.f, 140);

    // filter lines by theta and compute average
    std::vector<cv::Vec2f> filteredLines;
    float theta_min = 60.f * CV_PI / 180.f;
    float theta_max = 120.f * CV_PI / 180.0f;
    float theta_avr = 0.f;
    float theta_deg = 0.f;
    for (size_t i = 0; i < lines.size(); i++) {
        float theta = lines[i][1];
        if (theta >= theta_min && theta <= theta_max) {
            filteredLines.push_back(lines[i]);
            theta_avr += theta;
        }
    }
    if (filteredLines.size() > 0) {
        theta_avr /= filteredLines.size();
        theta_deg = (theta_avr / CV_PI * 180.f) - 90;
        rlog.info("detectSkew: %.1f deg", theta_deg);
    } else {
        rlog.warn("failed to detect skew");
    }

    if (_debugSkew) {
        drawLines(filteredLines);
    }

    return theta_deg;
}

/**
 * Detect edges using Canny algorithm.
 */
cv::Mat ImageProcessor::cannyEdges() {
    cv::Mat edges;
    // detect edges
    cv::Canny(_imgGray, edges, _config.getCannyThreshold1(), _config.getCannyThreshold2());
    return edges;
}

/**
 * Find bounding boxes that are aligned at y position.
 */
void ImageProcessor::findAlignedBoxes(std::vector<cv::Rect>::const_iterator begin,
        std::vector<cv::Rect>::const_iterator end, std::vector<cv::Rect>& result) {
    std::vector<cv::Rect>::const_iterator it = begin;
    cv::Rect start = *it;
    ++it;
    result.push_back(start);

    for (; it != end; ++it) {
        if (abs(start.y - it->y) < _config.getDigitYAlignment() && abs(start.height - it->height) < 5) {
            result.push_back(*it);
        }
    }
}

/**
 * Filter contours by size of bounding rectangle.
 */
void ImageProcessor::filterContours(std::vector<std::vector<cv::Point> >& contours,
        std::vector<cv::Rect>& boundingBoxes, std::vector<std::vector<cv::Point> >& filteredContours) {
    // filter contours by bounding rect size
    for (size_t i = 0; i < contours.size(); i++) {
        cv::Rect bounds = cv::boundingRect(contours[i]);
        if (bounds.height > _config.getDigitMinHeight() && bounds.height < _config.getDigitMaxHeight()
                && bounds.width > 5 && bounds.width < bounds.height) {
            boundingBoxes.push_back(bounds);
            filteredContours.push_back(contours[i]);
        }
    }
}

/**
 * Find and isolate the digits of the counter,
 */
void ImageProcessor::findCounterDigits() {
    log4cpp::Category& rlog = log4cpp::Category::getRoot();

    // edge image
    cv::Mat edges = cannyEdges();
    if (_debugEdges) {
        cv::imshow("edges", edges);
    }

    cv::Mat img_ret = edges.clone();

    // find contours in whole image
    std::vector<std::vector<cv::Point> > contours, filteredContours;
    std::vector<cv::Rect> boundingBoxes;
    cv::findContours(edges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    // filter contours by bounding rect size
    filterContours(contours, boundingBoxes, filteredContours);

    rlog << log4cpp::Priority::INFO << "number of filtered contours: " << filteredContours.size();

    // find bounding boxes that are aligned at y position
    std::vector<cv::Rect> alignedBoundingBoxes, tmpRes;
    for (std::vector<cv::Rect>::const_iterator ib = boundingBoxes.begin(); ib != boundingBoxes.end(); ++ib) {
        tmpRes.clear();
        findAlignedBoxes(ib, boundingBoxes.end(), tmpRes);
        if (tmpRes.size() > alignedBoundingBoxes.size()) {
            alignedBoundingBoxes = tmpRes;
        }
    }
    rlog << log4cpp::Priority::INFO << "max number of alignedBoxes: " << alignedBoundingBoxes.size();

    // sort bounding boxes from left to right
    std::sort(alignedBoundingBoxes.begin(), alignedBoundingBoxes.end(), sortRectByX());

    if (_debugEdges) {
        // draw contours
        cv::Mat cont = cv::Mat::zeros(edges.rows, edges.cols, CV_8UC1);
        cv::drawContours(cont, filteredContours, -1, cv::Scalar(255));
        cv::imshow("contours", cont);
    }

    // cut out found rectangles from edged image
    for (int i = 0; i < alignedBoundingBoxes.size(); ++i) {
        cv::Rect roi = alignedBoundingBoxes[i];
        _digits.push_back(img_ret(roi));
        if (_debugDigits) {
            cv::rectangle(_img, roi, cv::Scalar(0, 255, 0), 2);
        }
    }
}
