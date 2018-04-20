//
//  fileSaver.hpp
//  mousesense
//
//  Created by Winthrop Gillis on 4/20/18.
//  Copyright © 2018 Winthrop Gillis. All rights reserved.
//

#ifndef fileSaver_hpp
#define fileSaver_hpp

using namespace std;
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <librealsense2/rs.hpp>

class FileSaver {
    ofstream file;
public:
    FileSaver(string, bool);
    virtual void writeFrameNumber(unsigned long);
    virtual void write(double);
    virtual void write(rs2::video_frame);
};

#endif /* fileSaver_hpp */
