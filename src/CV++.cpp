#include "../include/CV++.h"

using namespace cv;
using namespace std;
using namespace mqtt;

/* --------------------------------------------------------------------------------------------------------------------------------------------------
 * Implementation of the ellipse class
 * Stores ellipse data and performs calculations
 */

// constructors
Ellipse::Ellipse(const Point2f &newCenter, Size2f newSize, float newAngleDeg, const Scalar &newColour, int newThickness) : RotatedRect(newCenter, newSize, (float)fmod(newAngleDeg, 360.0))
{
    setColour(newColour);
    setThickness(newThickness);
}

Ellipse::Ellipse(const Ellipse &copiedEllipse) : RotatedRect(copiedEllipse.center, copiedEllipse.size, copiedEllipse.angle)
{
    setColour(copiedEllipse.getColour());
    setThickness(copiedEllipse.getThickness());
}

// operator overloads
Ellipse &Ellipse::operator=(const Ellipse &assignedEllipse)
{
    center = assignedEllipse.center;
    size = assignedEllipse.size;
    angle = assignedEllipse.angle;
    return (*this);
}

// gets
Scalar Ellipse::getColour() const
{
    return colour;
}
int Ellipse::getThickness() const
{
    return thickness;
}

float Ellipse::getMinorAxis() const
{
    return (size.height);
}

float Ellipse::getMajorAxis() const
{
    return (size.width);
}

float Ellipse::getEccentricity() const
{
    float sqrMinRad(getMinorAxis() / 2);
    sqrMinRad *= sqrMinRad;

    float sqrMajRad(getMajorAxis() / 2);
    sqrMajRad *= sqrMajRad;

    return ((float)sqrt(1 - (sqrMinRad / sqrMajRad)));
}

Point2f Ellipse::getCenter() const
{
    return (center);
}

float Ellipse::getArea() const
{
    return ((float)(CV_PI * getMinorAxis() * getMajorAxis() / 4));
}

void Ellipse::setColour(const Scalar &newColour)
{
    colour = newColour;
    // Validate and correct color values
    if (colour[0] < 0)
    {
        colour[0] = 0;
        cout << "Blue less than 0. Trimming...";
    }
    else if (colour[0] > 255)
    {
        colour[0] = 255;
        cout << "Blue more than 255. Trimming...";
    }

    if (colour[1] < 0)
    {
        colour[1] = 0;
        cout << "Green less than 0. Trimming...";
    }
    else if (colour[1] > 255)
    {
        colour[1] = 255;
        cout << "Green more than 255. Trimming...";
    }

    if (colour[2] < 0)
    {
        colour[2] = 0;
        cout << "Red less than 0. Trimming...";
    }
    else if (colour[2] > 255)
    {
        setColour(newColour);
        colour[2] = 255;
        cout << "Red more than 255. Trimming...";
    }
}

void Ellipse::setThickness(const int &newThickness)
{
    thickness = newThickness;
}

void Ellipse::setMinorAxis(float newMinorAxis)
{
    size.width = newMinorAxis;
}

void Ellipse::setMajorAxis(float newMajorAxis)
{
    size.height = newMajorAxis;
}

void Ellipse::setCenter(float newX, float newY)
{
    center.x = newX;
    center.y = newY;
}
void Ellipse::setCenterX(float newX)
{
    center.x = newX;
}
void Ellipse::setCenterY(float newY)
{
    center.y = newY;
}

bool Ellipse::isPointInside(const Point2f &point) const
{
    Rect_<float> boundingRect = boundingRect2f();
    if (!boundingRect.contains(point))
        return (false);
    float a = getMinorAxis() / 2;
    float b = getMajorAxis() / 2;
    float xDiff = point.x - center.x;
    float yDiff = point.y - center.y;

    return ((xDiff * xDiff) / (a * a) + (yDiff * yDiff) / (b * b) <= 1.0f);
}

bool Ellipse::isCircle() const
{
    return (getEccentricity() <= 0.2f);
}

void Ellipse::translate(float xTranslation, float yTranslation)
{
    center.x += xTranslation;
    center.y += yTranslation;
}
void Ellipse::operator+(float xyTranslation)
{
    translate(xyTranslation, xyTranslation);
}

void Ellipse::scale(float xScale, float yScale)
{
    size.width *= xScale;
    size.height *= yScale;
}
void Ellipse::operator*(float xyScale)
{
    scale(xyScale, xyScale);
}

void Ellipse::rotate(float angleDeg)
{
    angle += angleDeg;
}
void Ellipse::rotateRad(float angleRad)
{
    rotate((float)(angleRad * 180 / CV_PI));
}

void Ellipse::draw(const Mat &frame) const
{
    ellipse(frame, *this, colour, thickness);
}

void Ellipse::operator()(const Mat &frame) const
{
    draw(frame);
}

Ellipse ellipseAverage(Ellipse elp1, Ellipse elp2)
{
    Ellipse elp =
        Ellipse(
            Point2f(
                (elp1.getCenter().x + elp2.getCenter().x) / 2,
                (elp1.getCenter().y + elp2.getCenter().y) / 2
            ),
            Size2f(
                (elp1.getMajorAxis() + elp2.getMajorAxis()) / 2,
                (elp1.getMinorAxis() + elp2.getMinorAxis()) / 2
            ),
            (elp1.angle + elp2.angle) / 2,
            Scalar(0, 255, 0, 255),
            (elp1.getThickness() + elp2.getThickness()) / 2
        );

    return elp;
}

// contains the phase ring
const Point2f pRingCentre(60, 60);
const vector<Ellipse> phaseEllipses =
{
    // outer
    Ellipse(
        // Point2f(220, 137),
        // Point2f(260, 133),
        pRingCentre,
        Size2f(46, 46),
        0,
        Scalar(0, 255, 255, 255),
        1
    ),

    // inner
    Ellipse(
        // Point2f(220, 137),
        // Point2f(260, 133),
        pRingCentre,
        Size2f(56, 56),
        0,
        Scalar(0, 255, 255, 255),
        1
    )
};

/* This following block of code has been modified from https://github.com/siamezzze/ellipse_detection -----------------------------------------------

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

bool isEllipseFound = true;

inline double distance_square(double x1, double y1, double x2, double y2)
{
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

inline pair<int,int> compute_center(int x1, int y1, int x2, int y2)
{
    return make_pair(cvRound((x1 + x2)/2), cvRound((y1 + y2)/2));
}

inline double major_axis_length_sq(int x1, int y1, int x2, int y2)
{
    return distance_square(x1, y1, x2, y2) / 4;
}

inline double orientation(double x1, double y1, double x2, double y2)
{
    if (x1 == x2)
        return CV_PI / 2;
    return atan((y2 - y1) / (x2 - x1));
}

vector<vector<vector<short> > > store_hiearchical_pyramid(Mat& edgesM, int min_size = 32) //create hierarchical pyramid of minimized images
{
    int log2h = (int)floor(log2(edgesM.rows));
    int log2w = (int)floor(log2(edgesM.cols));
    int log2min = min(log2h, log2w);
    unsigned int max_size = pow(2, log2min);
    Mat resized = Mat(max_size,max_size,edgesM.type());
    resize(edgesM, resized, resized.size()); //resize to square

    unsigned int steps = (unsigned int)log2min - (unsigned int)floor(log2(min_size)) + 1;
    vector<vector<vector<short> > > pyramid = vector<vector<vector<short> > >(steps);
    pyramid[0] = vector<vector<short> >(max_size,vector<short>(max_size,0)); //creating the first image (original size)
    for (int i = 0; i < max_size; ++i)
    {
        for (int j = 0; j < max_size; ++j)
        {
            uchar p = resized.at<uchar>(i,j);
            if (p > 0)
                pyramid[0][i][j] = 1;
        }
    }

    unsigned int sz = max_size; //create next levels by summing pixels of previous ones.
    for (int i = 1; i < steps; i++)
    {
        sz /= 2;
        pyramid[i] = vector<vector<short> >(sz,vector<short>(sz,0));
        for (int j = 0; j < sz; j++)
        {
            for (int k = 0; k < sz; ++k)
            {
                pyramid[i][j][k] = pyramid[i-1][j*2][k*2] + pyramid[i-1][j*2+1][k*2] + pyramid[i-1][j*2][k*2+1] + pyramid[i-1][j*2+1][k*2+1];
            }
        }
    }
    return pyramid;
}

bool compr2(pair<ellipse_data, int> a, pair<ellipse_data, int> b)
{
    return a.second < b.second;
}

inline bool can_belong_to_ellipse(int x, int y, ellipse_data e, double eps = 0.1) //for cleaning the image
{
    double _x = cos(e.orient) * (x - e.x0) + sin(e.orient) * (y - e.y0);
    double _y = sin(e.orient) * (x - e.x0) - cos(e.orient) * (y - e.y0);
    double eqt = (_x * _x)/(e.a*e.a) + (_y * _y)/(e.b*e.b);
    return abs(eqt - 1) < eps;
}

void voting(vector<pair<ellipse_data, int> > &ellipses, const vector<vector<short> > &data, pair<int, int> xbounds,
            pair<int, int> ybounds, int min_dist, int min_vote, pair<int, int> p0, pair<int, int> p1)
{
    int maxlen = cvRound(sqrt(data.size() * data.size() + data[0].size() * data[0].size()));
    int acc[maxlen];
    for (int i = 0; i < maxlen; ++i)   //clear the accumulator array
    {
        acc[i] = 0;
    }
    int x1 = p0.first;
    int y1 = p0.second;
    int x2 = p1.first;
    int y2 = p1.second;

    pair<int, int> center = compute_center(x1, y1, x2, y2); //get the center point of the ellipse

    int x0 = center.first;
    int y0 = center.second;

    double a_sq = major_axis_length_sq(x1, y1, x2, y2); //major axis (half)
    double a = sqrt(a_sq);

    if (a < min_dist)
        return;

    double orient = orientation(x1, y1, x2, y2); //ellipse can be rotated


    for (int y = ybounds.first; y < ybounds.second; y++)
    {
        for (int x = xbounds.first; x < xbounds.second; x++) //point of the ellipse (x,y)
        {
            if (((x == x1) && (y == y1)) ||
                    ((x == x2) && (y == y2)) ||
                    (data[y][x] == 0)) //the same as one of the side points or not a boundary point
                continue;

            double d_sq = distance_square(x, y, x0, y0);
            double d = sqrt(d_sq); //distance from the center

            if ((d < min_dist) || (d > a)) //if it's too small - it is probably a line
                //if bigger than the major axis - not an ellipse point
                continue;


            double f_sq = min(distance_square(x, y, x1, y1),
                              distance_square(x, y, x2, y2)); //distance to the nearest side point
            double costau = (a_sq + d_sq - f_sq) / (2 * a * d);
            double costau_sq = costau * costau;
            if (costau_sq > 1)
            {
                costau_sq = 1;
            }
            if (costau_sq < 0)
            {
                costau_sq = 0;
            }
            double sintau_sq = 1 - costau_sq;
            double b_sq = (a_sq * d_sq * sintau_sq) / (a_sq - d_sq * costau_sq);
            int b = cvRound(sqrt(b_sq));
            if (b > min_dist)   //voting
            {
                acc[b] += data[y][x];
            }
        }
    }
    //here we get the most-likely minor axis length of the assumed ellipse and how many points would such an ellipse contain
    int maxlength = 0;
    int maxvote = 0;
    for (int k = 0; k < maxlen; ++k)
    {
        if (acc[k] >= maxvote)
        {
            maxvote = acc[k];
            maxlength = k;
        }
        //acc[k] = 0;
    }

    if (maxvote > min_vote)
    {
        ellipses.push_back(make_pair(ellipse_data(x0, y0, a, maxlength, orient, x1, y1, x2, y2), maxvote));
    }
}

//First search (should be made on the minimized image)
vector<pair<ellipse_data, int> > hough_transform(vector<vector<short> > &data, int min_vote = 1, int min_dist = 0)
{
    pair<int, int> xbounds = make_pair(0, (int) data.size());
    pair<int, int> ybounds = make_pair(0, (int) data[0].size());
    vector<pair<ellipse_data, int> > res = vector<pair<ellipse_data, int> >();

    for (int y1 = ybounds.first; y1 < ybounds.second; y1++)
    {
        for (int x1 = xbounds.first; x1 < xbounds.second; x1++) //point (x1, y1) - side point of major axis
        {
            if (data[y1][x1] == 0) //not a boundary point
                continue;

            for (int y2 = y1; y2 < ybounds.second; y2++)
            {
                for (int x2 = xbounds.first; x2 < xbounds.second; x2++) //point (x2, y2) - side point of major axis
                {
                    if (((y2 == y1) && (x2 <= x1)) ||
                            (data[y2][x2] == 0)) //already been selected OR not a boundary point
                        continue;

                    voting(res, data, xbounds, ybounds, min_dist, min_vote, make_pair(x1, y1), make_pair(x2, y2));
                }
            }
        }
    }
    return res;
}

ellipse_data relocate_ellipse(vector<vector<short> > &data, ellipse_data prev, int min_vote = 1, int min_dist = 0)
{
    //The area where we search for ellipse points (basically, all the image)
    pair<int, int> xbounds = make_pair(0, (int) data.size());
    pair<int, int> ybounds = make_pair(0, (int) data[0].size());
    vector<pair<ellipse_data, int> > res;

    int maxlen = cvRound(sqrt(data.size() * data.size() + data[0].size() * data[0].size()));

    int acc[maxlen];
    for (int i = 0; i < maxlen; ++i)   //clear the accumulator array
    {
        acc[i] = 0;
    }


    //This time, we search for side points only as far as +- pixel from previously found ones
    //Okay, +-2 pixels for a little bit higher accuracy (I believe).
    int sidex1_start = max(xbounds.first, 2 * prev.x1 - 2);
    int sidex1_fin = min(xbounds.second, 2 * prev.x1 + 2);
    int sidex2_start = max(xbounds.first, 2 * prev.x2 - 2);
    int sidex2_fin = min(xbounds.second, 2 * prev.x2 + 2);
    int sidey1_start = max(ybounds.first, 2 * prev.y1 - 2);
    int sidey1_fin = min(ybounds.second, 2 * prev.y1 + 2);
    int sidey2_start = max(ybounds.first, 2 * prev.y2 - 2);
    int sidey2_fin = min(ybounds.second, 2 * prev.y2 + 2);

    for (int y1 = sidey1_start; y1 <= sidey1_fin; y1++)
    {
        for (int x1 = sidex1_start; x1 <= sidex1_fin; x1++) //point (x1, y1) - side point of major axis
        {
            if (data[y1][x1] == 0)
                continue;

            for (int y2 = sidey2_start; y2 <= sidey2_fin; y2++)
            {
                for (int x2 = sidex2_start; x2 <= sidex2_fin; x2++) //point (x2, y2) - side point of major axis
                {
                    if (((y2 == y1) && x2 <= x1) || (data[y2][x2] == 0))
                        continue;

                    voting(res, data, xbounds, ybounds, min_dist, min_vote, make_pair(x1, y1), make_pair(x2, y2));
                }
            }
        }
    }
    pair<ellipse_data, int> e = *max_element(res.begin(), res.end(), compr2);
    return e.first;
}

void clear_picture(Mat& edges, ellipse_data elp)
{
    for (int x = 0; x < edges.cols; x++)
        for (int y = 0; y < edges.rows; y++)
        {
            if (can_belong_to_ellipse(x, y, elp, 0.2))
            {
                edges.at<uchar>(y,x) = 0;
            }
        }
}


ellipse_data ellipse_detection(Mat edges, int minimized_size = 64, int min_vote = 4, int min_dist = 5)
{
    vector<vector<vector<short> > > pyramid = store_hiearchical_pyramid(edges, minimized_size);

    int n = (int)pyramid.size();
    vector<pair<ellipse_data, int> > ellipses = hough_transform(pyramid[n - 1], min_vote, min_dist);

    if (ellipses.size() < 1)
    {
        // cout << "Didn't find anything" << endl;
        isEllipseFound = false;
        return ellipse_data(-1, -1, -1, -1, 0);
    }
    // cout << "Step 1 of " << n << endl;
    ellipse_data found = (*max_element(ellipses.begin(), ellipses.end(), compr2)).first;
    for (int i = n-2; i >= 0; i--)
    {
        // cout << "Step " << n - i << " of " << n << endl;
        found = relocate_ellipse(pyramid[i], found);
    }
    return found;
}

vector<ellipse_data> detect_ellipses(Mat src, int minimized_size = 64, unsigned int number_of_ellipses = 1)
{
    vector<ellipse_data> ellipses = vector<ellipse_data>(number_of_ellipses);
    Mat edges;
    Canny(src, edges, 50, 200, 3);
    //Probably some other transformation should be performed here, such as blurring
    for (int i = 0; i < number_of_ellipses; i++)
    {
        ellipse_data elp = ellipse_detection(edges, minimized_size, 5, 0);

        ellipses[i] = elp;
        cout << "Found: (x_0 = " << elp.x0 << ", y_0 = " << elp.y0 << ", a = " << elp.a << ", b = " << elp.b <<
             ", A = " << elp.orient << ") " << endl;
        clear_picture(edges, elp);
    }
    return ellipses;
}

ellipse_data::ellipse_data(int _x0, int _y0, double _a, double _b, double th) : a(_a), b(_b),
    x0(_x0), y0(_y0),
    orient(th)
{
}

ellipse_data::ellipse_data(int _x0, int _y0, double _a, double _b, double th, int _x1, int _y1, int _x2, int _y2) : a(_a), b(_b),
    x0(_x0), y0(_y0),
    x1(_x1), y1(_y1),
    x2(_x2), y2(_y2),
    orient(th)
{
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------
 * PaddedMat
 * An extended form of Mat
 * Padding is added to make square with length of power 2
 */

PaddedMat::PaddedMat(const Mat &mat) : Mat(pad(mat)) {}

PaddedMat::PaddedMat(const PaddedMat &other) : Mat(other)
{
    rowDiff = other.rowDiff;
    colDiff = other.colDiff;
}

PaddedMat &PaddedMat::operator=(const PaddedMat &other)
{
    if (this != &other)
    {
        Mat::operator=(other);
        rowDiff = other.rowDiff;
        colDiff = other.colDiff;
    }
    return *this;
}

Mat PaddedMat::pad(const Mat &mat)
{
    // finds the longer side
    unsigned int larger = std::max(mat.rows, mat.cols);

    // finds the closest power of 2 larger than the longer side
    unsigned int desire = std::pow(2, std::ceil(std::log2(larger)));
    rowDiff = desire - mat.rows;
    colDiff = desire - mat.cols;

    // create the Mat for the padded image
    Mat paddedMat = Mat::zeros(desire, desire, mat.type());

    // copy across the actual image
    mat.copyTo(paddedMat(Rect(0, 0, mat.cols, mat.rows)));

    return paddedMat;
}

/* --------------------------------------------------------------------------------------------------------------------------------------------------
 * Using above:
 * Hough Transform implementation
 * (Mohamed Ghali)
 */
vector<Ellipse> detectEllipses(Mat src, unsigned int numEllipses, int minimizedSize)
{
    const int kernelSize = mqtt::topics::cv::noiseKernel;
    isEllipseFound = true;

    // GaussianBlur(src, src, Size(kernelSize, kernelSize), 0);

    // apply noise filter to image -------------------------
    cvtColor(src, src, COLOR_BGRA2GRAY);

    // Mat kernel = getStructuringElement(MORPH_RECT, Size(kernelSize, kernelSize)); // Adjust kernel size as needed
    // morphologyEx(src, src, MORPH_OPEN, kernel);

    // Mat mask = src.clone();
    // Mat orgSrc = src.clone();
    // GaussianBlur(mask, mask, Size(31, 31), 0, 0);
    // cv::max(orgSrc,mask,src);

    Scalar avg = mean(src, src > 30);
    Mat mask(src.size(), CV_8UC1, avg[0]);
    cv::max(src,mask,src);

    // send filtered image to broker
    auto token = publishImage("cvimages/filtered", src);
    token->wait_for(std::chrono::seconds(10));

    // apply Otsu's method threshold to image --------------
    // adaptiveThreshold(src,src,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY_INV, mqtt::topics::cv::adaptiveSize,mqtt::topics::cv::threshold);
    // threshold(src, src, mqtt::topics::cv::threshold/*140*/, 255, THRESH_BINARY);
    threshold(src, src, 0, 255, THRESH_OTSU);

    // send thresholded image to broker
    token = publishImage("cvimages/threshold", src);
    token->wait_for(std::chrono::seconds(10));

    // add padding to image to make square with length of power of two
    PaddedMat padsrc(src);

    vector<Ellipse> ellipses;
    ellipses.reserve(numEllipses);

    // apply the hough transform
    auto preEllipses =
        detect_ellipses(
            padsrc,
            minimizedSize,
            numEllipses
        );

    // if no ellipses were found, send a clear array
    if (!isEllipseFound)
    {
        ellipses.clear();
    }
    // otherwise convert to an "Ellipse" object
    else
    {
        // convert to Ellipse
        for (unsigned int i = 0; i < numEllipses; i++)
        {
            auto preElp = preEllipses[i];

            auto elp =
                Ellipse(
                    Point2f(
                        preElp.x0,
                        preElp.y0),
                    Size2f(
                        2 * preElp.a,
                        2 * preElp.b
                    ),
                    preElp.orient,
                    Scalar(255, 0, 0, 255),
                    1
                );

            ellipses.push_back(elp);
        }
    }

    return ellipses;
}
