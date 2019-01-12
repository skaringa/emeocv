/*
 * Config.cpp
 *
 */

#include <opencv2/highgui/highgui.hpp>

#include "Config.h"

Config::Config() :
        _rotationDegrees(0), _ocrMaxDist(5e5), _digitMinHeight(20), _digitMaxHeight(
                90), _digitYAlignment(10), _cannyThreshold1(100), _cannyThreshold2(
                200), _trainingDataFilename("trainctr.yml") ,_digitNum(6),
                _divisionNum(1), _coordX(0), _coordY(0), _coordW(480), _coordH(640),
                _imgW(480), _imgH(640), _oneFile(0), _negativeNum(0), _rollAvgInt(10) {
}

void Config::saveConfig() {
    cv::FileStorage fs("config.yml", cv::FileStorage::WRITE);
    fs << "rotationDegrees" << _rotationDegrees;
    fs << "cannyThreshold1" << _cannyThreshold1;
    fs << "cannyThreshold2" << _cannyThreshold2;
    fs << "digitMinHeight" << _digitMinHeight;
    fs << "digitMaxHeight" << _digitMaxHeight;
    fs << "digitYAlignment" << _digitYAlignment;
    fs << "ocrMaxDist" << _ocrMaxDist;
    fs << "trainingDataFilename" << _trainingDataFilename;
    fs << "digitNum" << _digitNum;
    fs << "divisionNum" << _divisionNum;
    fs << "coordX" << _coordX;
    fs << "coordY" << _coordY;
    fs << "coordW" << _coordW;
    fs << "coordH" << _coordH;
    fs << "imgW"   << _imgW;
    fs << "imgH"   << _imgH;
    fs << "oneFile" << _oneFile;
    fs << "negativeNum" << _negativeNum;
    fs << "rollAvgInt" << _rollAvgInt;
    fs.release();
}

void Config::loadConfig() {
    cv::FileStorage fs("config.yml", cv::FileStorage::READ);
    if (fs.isOpened()) {
        fs["rotationDegrees"] >> _rotationDegrees;
        fs["cannyThreshold1"] >> _cannyThreshold1;
        fs["cannyThreshold2"] >> _cannyThreshold2;
        fs["digitMinHeight"] >> _digitMinHeight;
        fs["digitMaxHeight"] >> _digitMaxHeight;
        fs["digitYAlignment"] >> _digitYAlignment;
        fs["ocrMaxDist"] >> _ocrMaxDist;
        fs["trainingDataFilename"] >> _trainingDataFilename;
        fs["digitNum"] >>_digitNum;
        fs["divisionNum"] >> _divisionNum;
        fs["coordX"] >> _coordX;
        fs["coordY"] >> _coordY;
        fs["coordW"] >> _coordW;
        fs["coordH"] >> _coordH;
        fs["imgW"] >> _imgW;
        fs["imgH"] >> _imgH;
        fs["oneFile"] >> _oneFile;
        fs["negativeNum"] >> _negativeNum;
        fs["rollAvgInt"] >> _rollAvgInt;
        fs.release();
    } else {
        // no config file - create an initial one with default values
        saveConfig();
    }
}

