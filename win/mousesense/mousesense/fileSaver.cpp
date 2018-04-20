//
//  fileSaver.cpp
//  mousesense
//
//  Created by Winthrop Gillis on 4/20/18.
//  Copyright © 2018 Winthrop Gillis. All rights reserved.
//

#include <string>
#include <iomanip>
#include "fileSaver.hpp"
//#include <librealsense2/rs.hpp>

FileSaver::FileSaver(string filename, bool isBinary) {
    if (isBinary) {
        file.open(filename, ofstream::binary | ofstream::out);
    } else {
        file.open(filename, ofstream::out);
    }
}

void FileSaver::writeFrameNumber(unsigned long frameNumber) {
    file << frameNumber;
    file << "\n";
}

void FileSaver::write(double timeStamp) {
    // set to 4 bc realsense doesn't go below 3 decimal places
    file << fixed << setprecision(4) << timeStamp;
    file << "\n";
}

void FileSaver::write(rs2::video_frame depth) {
    // write these binary data to file
    file.write((char *)depth.get_data(), depth.get_height() * depth.get_stride_in_bytes());
}
