/*
 * main.cpp
 *
 */

#include <string>
#include <list>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/SimpleLayout.hh>
#include <log4cpp/Priority.hh>

#include "Config.h"
#include "Directory.h"
#include "ImageProcessor.h"
#include "KNearestOcr.h"
#include "Plausi.h"
#include "RRDatabase.h"

static int delay = 1000;

#ifndef VERSION
#define VERSION "0.9.7"
#endif

static void testOcr(ImageInput* pImageInput) {
    log4cpp::Category::getRoot().info("testOcr");

    Config config;
    config.loadConfig();
    ImageProcessor proc(config);
    proc.debugWindow();
    proc.debugDigits();

    Plausi plausi;

    KNearestOcr ocr(config);
    if (! ocr.loadTrainingData()) {
        std::cout << "Failed to load OCR training data\n";
        return;
    }
    std::cout << "OCR training data loaded.\n";
    std::cout << "<q> to quit.\n";

    while (pImageInput->nextImage()) {
        proc.setInput(pImageInput->getImage());
        proc.process();

        std::string result = ocr.recognize(proc.getOutput());
        std::cout << result;
        if (plausi.check(result, pImageInput->getTime())) {
            std::cout << "  " << std::fixed << std::setprecision(1) << plausi.getCheckedValue() << std::endl;
        } else {
            std::cout << "  -------" << std::endl;
        }
        int key = cv::waitKey(delay) & 255;

        if (key == 'q') {
            std::cout << "Quit\n";
            break;
        }
    }
}

static void learnOcr(ImageInput* pImageInput) {
    log4cpp::Category::getRoot().info("learnOcr");

    Config config;
    config.loadConfig();
    ImageProcessor proc(config);
    proc.debugWindow();

    KNearestOcr ocr(config);
    ocr.loadTrainingData();
    std::cout << "Entering OCR training mode!\n";
    std::cout << "<0>..<9> to answer digit, <space> to ignore digit, <s> to save and quit, <q> to quit without saving.\n";

    int key = 0;
    while (pImageInput->nextImage()) {
        proc.setInput(pImageInput->getImage());
        proc.process();

        key = ocr.learn(proc.getOutput());
        std::cout << std::endl;

        if (key == 'q' || key == 's') {
            std::cout << "Quit\n";
            break;
        }
    }

    if (key != 'q' && ocr.hasTrainingData()) {
        std::cout << "Saving training data\n";
        ocr.saveTrainingData();
    }
}

static void adjustCamera(ImageInput* pImageInput) {
    log4cpp::Category::getRoot().info("adjustCamera");

    Config config;
    config.loadConfig();
    ImageProcessor proc(config);
    proc.debugWindow();
    proc.debugDigits();
    //proc.debugEdges();
    //proc.debugSkew();

    std::cout << "Adjust camera.\n";
    std::cout << "<r>, <p> to select raw or processed image, <s> to save config and quit, <q> to quit without saving.\n";

    bool processImage = true;
    int key = 0;
    while (pImageInput->nextImage()) {
        proc.setInput(pImageInput->getImage());
        if (processImage) {
            proc.process();
        } else {
            proc.showImage();
        }

        key = cv::waitKey(delay) & 255;

        if (key == 'q' || key == 's') {
            std::cout << "Quit\n";
            break;
        } else if (key == 'r') {
            processImage = false;
        } else if (key == 'p') {
            processImage = true;
        }
    }
    if (key != 'q') {
        std::cout << "Saving config\n";
        config.saveConfig();
    }
}

static void capture(ImageInput* pImageInput) {
    log4cpp::Category::getRoot().info("capture");

    std::cout << "Capturing images into directory.\n";
    std::cout << "<Ctrl-C> to quit.\n";

    while (pImageInput->nextImage()) {
        usleep(delay*1000L);
    }
}

static void writeData(ImageInput* pImageInput) {
    log4cpp::Category::getRoot().info("writeData");

    Config config;
    config.loadConfig();
    ImageProcessor proc(config);

    Plausi plausi;

    RRDatabase rrd("emeter.rrd");

    struct stat st;

    KNearestOcr ocr(config);
    if (! ocr.loadTrainingData()) {
        std::cout << "Failed to load OCR training data\n";
        return;
    }
    std::cout << "OCR training data loaded.\n";
    std::cout << "<Ctrl-C> to quit.\n";

    while (pImageInput->nextImage()) {
        proc.setInput(pImageInput->getImage());
        proc.process();

        if (proc.getOutput().size() == 7) {
            std::string result = ocr.recognize(proc.getOutput());
            if (plausi.check(result, pImageInput->getTime())) {
                rrd.update(plausi.getCheckedTime(), plausi.getCheckedValue());
            }
        }
        if (0 == stat("imgdebug", &st) && S_ISDIR(st.st_mode)) {
            // write debug image
            pImageInput->setOutputDir("imgdebug");
            pImageInput->saveImage();
            pImageInput->setOutputDir("");
        }
        usleep(delay*1000L);
    }
}

static void usage(const char* progname) {
    std::cout << "Program to read and recognize the counter of an electricity meter with OpenCV.\n";
    std::cout << "Version: " << VERSION << std::endl;
    std::cout << "Usage: " << progname << " [-i <dir>|-c <cam>] [-l|-t|-a|-w|-o <dir>] [-s <delay>] [-v <level>\n";
    std::cout << "\nImage input:\n";
    std::cout << "  -i <image directory> : read image files (png) from directory.\n";
    std::cout << "  -c <camera number> : read images from camera.\n";
    std::cout << "\nOperation:\n";
    std::cout << "  -a : adjust camera.\n";
    std::cout << "  -o <directory> : capture images into directory.\n";
    std::cout << "  -l : learn OCR.\n";
    std::cout << "  -t : test OCR.\n";
    std::cout << "  -w : write OCR data to RR database. This is the normal working mode.\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -s <n> : Sleep n milliseconds after processing of each image (default=1000).\n";
    std::cout << "  -v <l> : Log level. One of DEBUG, INFO, ERROR (default).\n";
}

static void configureLogging(const std::string & priority = "INFO", bool toConsole = false) {
    log4cpp::Appender *fileAppender = new log4cpp::FileAppender("default", "emeocv.log");
    log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
    layout->setConversionPattern("%d{%d.%m.%Y %H:%M:%S} %p: %m%n");
    fileAppender->setLayout(layout);
    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.setPriority(log4cpp::Priority::getPriorityValue(priority));
    root.addAppender(fileAppender);
    if (toConsole) {
        log4cpp::Appender *consoleAppender = new log4cpp::OstreamAppender("console", &std::cout);
        consoleAppender->setLayout(new log4cpp::SimpleLayout());
        root.addAppender(consoleAppender);
    }
}

int main(int argc, char **argv) {
    int opt;
    ImageInput* pImageInput = 0;
    int inputCount = 0;
    std::string outputDir;
    std::string logLevel = "ERROR";
    char cmd = 0;
    int cmdCount = 0;

    while ((opt = getopt(argc, argv, "i:c:ltaws:o:v:h")) != -1) {
        switch (opt) {
            case 'i':
                pImageInput = new DirectoryInput(Directory(optarg, ".png"));
                inputCount++;
                break;
            case 'c':
                pImageInput = new CameraInput(atoi(optarg));
                inputCount++;
                break;
            case 'l':
            case 't':
            case 'a':
            case 'w':
                cmd = opt;
                cmdCount++;
                break;
            case 'o':
                cmd = opt;
                cmdCount++;
                outputDir = optarg;
                break;
            case 's':
                delay = atoi(optarg);
                break;
            case 'v':
                logLevel = optarg;
                break;
            case 'h':
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }
    if (inputCount != 1) {
        std::cerr << "*** You should specify exactly one camera or input directory!\n\n";
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    if (cmdCount != 1) {
        std::cerr << "*** You should specify exactly one operation!\n\n";
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    configureLogging(logLevel, cmd == 'a');

    switch (cmd) {
        case 'o':
            pImageInput->setOutputDir(outputDir);
            capture(pImageInput);
            break;
        case 'l':
            learnOcr(pImageInput);
            break;
        case 't':
            testOcr(pImageInput);
            break;
        case 'a':
            adjustCamera(pImageInput);
            break;
        case 'w':
            writeData(pImageInput);
            break;
    }

    delete pImageInput;
    exit(EXIT_SUCCESS);
}
