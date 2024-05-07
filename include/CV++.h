#ifndef COMPUTER_VISION_HPP
#define COMPUTER_VISION_HPP

#include "MQTT++.h"
#include <opencv2/opencv.hpp>
#include <fstream>

/* --------------------------------------------------------------------------------------------------------------------------------------------------
 * Declaration of the ellipse class
 * Stores ellipse data and performs calculations
 */

class Ellipse : public cv::RotatedRect
{
private:
        // public variables to store the colour and thickness of the ellipse
        cv::Scalar colour;
        int thickness;

public:
        // constructors
        Ellipse(const cv::Point2f &center, cv::Size2f newSize, float angle, const cv::Scalar &newColour = cv::Scalar(255, 255, 255, 1), int newThickness = 1);
        Ellipse(const Ellipse &newEllipse);

        // destructor
        ~Ellipse() = default;

        // operator overloads
        Ellipse &operator=(const Ellipse &assignedEllipse);

        // gets
        cv::Scalar getColour() const;
        int getThickness() const;
        float getMinorAxis() const;
        float getMajorAxis() const;
        cv::Point2f getCenter() const;
        float getEccentricity() const;
        float getArea() const;

        // sets
        void setColour(const cv::Scalar &newColour);
        void setThickness(const int &newThickness);
        void setMinorAxis(float newMinorAxis);
        void setMajorAxis(float newMajorAxis);
        void setCenter(float newX, float newY);
        void setCenterX(float newX);
        void setCenterY(float newX);

        bool isPointInside(const cv::Point2f &point) const;
        bool isCircle() const;

        void translate(float xTranslation, float yTranslation);
        void operator+(float xyTranslation);

        void scale(float xScale, float yScale);
        void operator*(float xyScale);

        void rotate(float angleDeg);
        void rotateRad(float angleRad);

        void draw(const cv::Mat &frame) const;
        void operator()(const cv::Mat &frame) const;
};

// finds the average of two ellipses
Ellipse ellipseAverage(Ellipse elp1, Ellipse elp2);

extern const std::vector<Ellipse> phaseEllipses;

/* This code has been modified from https://github.com/siamezzze/ellipse_detection

The MIT License (MIT)

Copyright (c) 2015 Maria Gluhova

Permission is hereby granted, free of charge, to any person obtaining a copy
        of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
        to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
        copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

// is there a new ellipse which was just found?
extern bool isEllipseFound;

struct ellipse_data
{

        ellipse_data() : x0(-1), y1(-1){};

        ellipse_data(int _x0, int _y0, double _a, double _b, double th);

        ellipse_data(int _x0, int _y0, double _a, double _b, double th, int _x1, int _y1, int _x2, int _y2);

        double a, b;
        int x0, y0;
        int x1, y1, x2, y2;

        double orient;
};

/* --------------------------------------------------------------------------------------------------------------------------------------------------
 * PaddedMat
 * An extended form of Mat
 * Padding is added to make square with length of power 2
 */

class PaddedMat : public cv::Mat {
public:
    unsigned int rowDiff, colDiff;

    PaddedMat(const cv::Mat& mat);
    PaddedMat(const PaddedMat& other);
    PaddedMat& operator=(const PaddedMat& other);

private:
    cv::Mat pad(const cv::Mat& mat);
};

// Mohamed Ghali's modifications to the Hough Transform for purpose
std::vector<Ellipse> detectEllipses(cv::Mat src, unsigned int numEllipses = 2, int minimizedSize = 64);

#endif // COMPUTER_VISION_HPP
