/************************************************************************/
/*                                                                      */
/*       Copyright 2007 by F. Heinrich, B. Seppke, Ullrich Koethe       */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*    This file is part of the VIGRA computer vision library.           */
/*    The VIGRA Website is                                              */
/*        http://kogs-www.informatik.uni-hamburg.de/~koethe/vigra/      */
/*    Please direct questions, bug reports, and contributions to        */
/*        ullrich.koethe@iwr.uni-heidelberg.de    or                    */
/*        vigra@informatik.uni-hamburg.de                               */
/*                                                                      */
/*    Permission is hereby granted, free of charge, to any person       */
/*    obtaining a copy of this software and associated documentation    */
/*    files (the "Software"), to deal in the Software without           */
/*    restriction, including without limitation the rights to use,      */
/*    copy, modify, merge, publish, distribute, sublicense, and/or      */
/*    sell copies of the Software, and to permit persons to whom the    */
/*    Software is furnished to do so, subject to the following          */
/*    conditions:                                                       */
/*                                                                      */
/*    The above copyright notice and this permission notice shall be    */
/*    included in all copies or substantial portions of the             */
/*    Software.                                                         */
/*                                                                      */
/*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND    */
/*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES   */
/*    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND          */
/*    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT       */
/*    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,      */
/*    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      */
/*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR     */
/*    OTHER DEALINGS IN THE SOFTWARE.                                   */                
/*                                                                      */
/************************************************************************/

#include <iostream>
#include <functional>
#include <cmath>
#include <list>
#include "unittest.hxx"

#include "vigra/multi_distance.hxx"
#include "vigra/distancetransform.hxx"

using namespace vigra;

struct MultiDistanceTest
{
    typedef vigra::MultiArray<3,int> IntVolume;
    typedef vigra::MultiArray<3,double> DoubleVolume; 
    typedef vigra::MultiArray<2,double> Double2DArray;
    typedef vigra::DImage Image;
    typedef vigra::MultiArrayView<2,Image::value_type> ImageView;
    typedef vigra::TinyVector<int,3> IntVec;

#if 1
    enum { WIDTH    =   15,  // 
           HEIGHT   =   15,  // Volume-Dimensionen
           DEPTH    =   15}; //
#else
    enum { WIDTH    =   4,  // 
           HEIGHT   =   4,  // Volume-Dimensionen
           DEPTH    =   1}; //
#endif

    std::list<std::list<IntVec> > pointslists;
    std::vector<Image> images;
    Double2DArray img2;
    DoubleVolume volume;
    IntVolume shouldVol;

    MultiDistanceTest()
    : images(3, Image(7,7)), img2(Double2DArray::difference_type(7,1)),
      volume(IntVolume::difference_type(WIDTH,HEIGHT,DEPTH)),
      shouldVol(IntVolume::difference_type(WIDTH,HEIGHT,DEPTH))
    {
        std::list<IntVec> temp;
        temp.push_back(IntVec(      0,        0,       0));
        temp.push_back(IntVec(WIDTH-1,        0,       0));
        temp.push_back(IntVec(      0, HEIGHT-1,       0));
        temp.push_back(IntVec(WIDTH-1, HEIGHT-1,       0));
        temp.push_back(IntVec(      0,        0, DEPTH-1));
        temp.push_back(IntVec(WIDTH-1,        0, DEPTH-1));
        temp.push_back(IntVec(      0, HEIGHT-1, DEPTH-1));
        temp.push_back(IntVec(WIDTH-1, HEIGHT-1, DEPTH-1));
        pointslists.push_back(temp);

        temp.clear();
        temp.push_back(IntVec(      0, HEIGHT/2, DEPTH/2));
        temp.push_back(IntVec(WIDTH/2, HEIGHT/2,       0));
        temp.push_back(IntVec(WIDTH/2,        0, DEPTH/2));
        temp.push_back(IntVec(WIDTH-1, HEIGHT/2, DEPTH/2));
        temp.push_back(IntVec(WIDTH/2, HEIGHT/2, DEPTH-1));
        temp.push_back(IntVec(WIDTH/2, HEIGHT-1, DEPTH/2));
        pointslists.push_back(temp);
    

        static const double in[] = {
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

        {
            Image::ScanOrderIterator i = images[0].begin();
            Image::ScanOrderIterator end = images[0].end();
            Image::Accessor acc = images[0].accessor();
            const double * p = in;

            for(; i != end; ++i, ++p)
            {
                acc.set(*p, i);
            }
        }
        
        static const unsigned char in2[] = {
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1};

        {
            Image::ScanOrderIterator i = images[1].begin();
            Image::ScanOrderIterator end = images[1].end();
            Image::Accessor acc = images[1].accessor();
            const unsigned char * p = in2;

            for(; i != end; ++i, ++p)
            {
                acc.set(*p, i);
            }
        }

        static const unsigned char in3[] = {
            1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 0, 1,
            1, 1, 1, 1, 1, 1, 1};

        {
            Image::ScanOrderIterator i = images[2].begin();
            Image::ScanOrderIterator end = images[2].end();
            Image::Accessor acc = images[2].accessor();
            const unsigned char * p = in3;

            for(; i != end; ++i, ++p)
            {
                acc.set(*p, i);
            }
        }

        static const double in2d[] = {0, 0, 0, 1, 0, 0, 0};
        const double * p=in2d;
        for(Double2DArray::iterator iter=img2.begin(); iter!=img2.end(); ++iter, ++p){
            *iter=*p;
        }
    }

    void testDistanceVolumes()
    {    
        DoubleVolume desired(volume);
        for(std::list<std::list<IntVec> >::iterator list_iter=pointslists.begin(); list_iter!=pointslists.end(); ++list_iter){
            IntVec temp;
            for(int z=0; z<DEPTH; ++z)
                for(int y=0; y<HEIGHT; ++y)
                    for(int x=0; x<WIDTH; ++x){
                        temp = IntVec(x,y,z);
                        int tempVal=10000000;
                        for(std::list<IntVec>::iterator iter=(*list_iter).begin(); iter!=(*list_iter).end(); ++iter){
                            if((temp-*iter).squaredMagnitude()<tempVal){
                                tempVal = (temp-*iter).squaredMagnitude();
                            }
                        }
                        desired(x,y,z)=tempVal;
                    }

            for(DoubleVolume::iterator vol_iter = volume.begin(); vol_iter != volume.end(); ++vol_iter)
                *vol_iter=0;
            for(std::list<IntVec>::iterator iter=(*list_iter).begin(); iter!=(*list_iter).end(); ++iter){
                *(volume.traverser_begin()+*iter)=1;
            }

            separableMultiDistSquared(srcMultiArrayRange(volume),
                                      destMultiArray(volume),
                                      true);
            shouldEqualSequence(volume.begin(),volume.end(),desired.begin());
        }
    }

    void testDistanceVolumesAnisoptopic()
    {    
        double epsilon = 1e-14;
        TinyVector<double, 3> pixelPitch(1.2, 1.0, 2.4);
        
        DoubleVolume desired(volume);
        for(std::list<std::list<IntVec> >::iterator list_iter=pointslists.begin(); list_iter!=pointslists.end(); ++list_iter){

            for(DoubleVolume::iterator vol_iter = volume.begin(); vol_iter != volume.end(); ++vol_iter)
                *vol_iter=0;
            for(std::list<IntVec>::iterator iter=(*list_iter).begin(); iter!=(*list_iter).end(); ++iter)
                *(volume.traverser_begin()+*iter)=1;

            IntVec temp;
            for(int z=0; z<DEPTH; ++z)
                for(int y=0; y<HEIGHT; ++y)
                {
                    for(int x=0; x<WIDTH; ++x)
                    {
                        temp = IntVec(x,y,z);
                        double tempVal=10000000.0;
                        for(std::list<IntVec>::iterator iter=(*list_iter).begin(); iter!=(*list_iter).end(); ++iter){
                            double squaredMag = (pixelPitch*(temp-*iter)).squaredMagnitude();
                            if(squaredMag<tempVal){
                                tempVal = squaredMag;
                            }
                        }
                        desired(x,y,z)=tempVal;
                    }
                }


            separableMultiDistSquared(srcMultiArrayRange(volume),
                                      destMultiArray(volume),
                                      true, pixelPitch);
            shouldEqualSequenceTolerance(volume.begin(),volume.end(),desired.begin(), epsilon);
        }
    }

    void distanceTransform2DCompare()
    {
        for(unsigned int k=0; k<images.size(); ++k)
        {
            Image res(images[k]);
            ImageView img_array(ImageView::difference_type(images[k].width(), images[k].height()), &images[k](0,0));

            distanceTransform(srcImageRange(images[k]), destImage(res), 0.0, 2);

            separableMultiDistance(srcMultiArrayRange(img_array),
                                   destMultiArray(img_array),
                                   true);

            Image::Iterator i = res.upperLeft();
            Image::Accessor acc = res.accessor();

            int x,y;

            for(y=0; y<7; ++y)
            {
                for(x=0; x<7; ++x)
                {
                    double dist_old = acc(i, vigra::Diff2D(x,y));

                    shouldEqualTolerance(dist_old, img_array(x,y), 1e-7);
                }
            }
        }
    }

    void distanceTest1D()
    {
        vigra::MultiArray<2,double> res(img2);
        
        static const int desired[] = {3, 2, 1, 0, 1, 2, 3};
        separableMultiDistance(srcMultiArrayRange(img2), destMultiArray(res), true);
        shouldEqualSequence(res.begin(), res.end(), desired);
    }
};



struct SimpleAnalysisTestSuite
: public vigra::test_suite
{
    SimpleAnalysisTestSuite()
    : vigra::test_suite("SimpleAnalysisTestSuite")
    {
        add( testCase( &MultiDistanceTest::testDistanceVolumes));
        add( testCase( &MultiDistanceTest::testDistanceVolumesAnisoptopic));
        add( testCase( &MultiDistanceTest::distanceTransform2DCompare));
        add( testCase( &MultiDistanceTest::distanceTest1D));
    }
};

int main(int argc, char ** argv)
{
    SimpleAnalysisTestSuite test;

    int failed = test.run(vigra::testsToBeExecuted(argc, argv));

    std::cout << test.report() << std::endl;
    return (failed != 0);
}

